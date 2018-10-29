/*===================== begin_copyright_notice ==================================

Copyright (c) 2017 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


======================= end_copyright_notice ==================================*/

#include <tuple>

#include "ifcvt.h"
#include "common.h"
#include "BuildIR.h"

//#define DEBUG_VERBOSE_ON
#if defined(DEBUG_VERBOSE_ON)
#define DEBUG(X) do { X; } while (0)
#else
#define DEBUG(X) do { } while (0)
#endif

using namespace vISA;

namespace {

    const unsigned FullyConvertibleMaxInsts = 5;
    const unsigned PartialConvertibleMaxInsts = 3;

    enum IfConvertKind {
        FullConvert,
        // Both 'if' and 'else' (if present) branches could be predicated.
        PartialIfConvert,
        // If both 'if' and 'else' branches are present, but only 'if' branch
        // could be predicated.
        PartialElseConvert
        // If both 'if' and 'else' branches are present, but only 'else' branch
        // could be predicated.

        // For the later two cases, it's potentially beneficial to convert the
        // original 'if-else-fi' into 'if-fi' following predicated the other
        // part. For example,
        //
        //  if (pred) {
        //      BB1;
        //  } else {
        //      BB2;
        //  } endif
        //
        // If BB2 cannot be converted (PartialIfConverted), we could convert it
        // into
        //
        //  (pred) BB1;
        //  if (-pred) {
        //      BB2;
        //  } endif
        //
        // Ofc, BB1 may need to be very smaller to really improve the final
        // performance.
    };

    // If-convertible.
    struct IfConvertible {
        IfConvertKind kind;
        G4_Predicate *pred;
        G4_BB *head;
        G4_BB *succIf;
        G4_BB *succElse;
        G4_BB *tail;

        IfConvertible(IfConvertKind k, G4_Predicate *p, G4_BB *h,
                      G4_BB *s0, G4_BB *s1, G4_BB *t)
            : kind(k), pred(p), head(h), succIf(s0), succElse(s1), tail(t) {}
    };

    // Trivial if-conversion.
    class IfConverter {
        FlowGraph &fg;

        /// getSinglePredecessor - Get the single predecessor or null
        /// otherwise.
        G4_BB *getSinglePredecessor(G4_BB *BB, G4_BB *If) const {
            if (BB->Preds.size() != 1) {
                if (BB->Preds.size() == 2) {
                    if (BB->Preds.front() == If)
                        return BB->Preds.back();
                    if (BB->Preds.back() == If)
                        return BB->Preds.front();
                }
                return nullptr;
            }
            return BB->Preds.front();
        }

        /// getSingleSuccessor - Get the single successor or null
        /// otherwise.
        G4_BB *getSingleSuccessor(G4_BB *BB, G4_BB *Else) const {
            if (BB->Succs.size() != 1) {
                if (BB->Succs.size() == 2) {
                    if (BB->Succs.front() == Else)
                        return BB->Succs.back();
                    if (BB->Succs.back() == Else)
                        return BB->Succs.front();
                }
                return nullptr;
            }
            return BB->Succs.front();
        }

        /// getEMaskBits() -
        unsigned getEMaskBits(unsigned maskOpt, unsigned execSize) const {
            uint64_t Val = ((uint64_t)1 << execSize) - 1;
            switch (maskOpt) {
            case InstOpt_NoOpt:
                return unsigned(Val);
            case InstOpt_M0:
                return unsigned(Val << 0);
            case InstOpt_M4:
                return unsigned(Val << 4);
            case InstOpt_M8:
                return unsigned(Val<< 8);
            case InstOpt_M12:
                return unsigned(Val << 12);
            case InstOpt_M16:
                return unsigned(Val << 16);
            case InstOpt_M20:
                return unsigned(Val << 20);
            case InstOpt_M24:
                return unsigned(Val << 24);
            case InstOpt_M28:
                return unsigned(Val << 28);
            }
            ASSERT_USER(false, "Invalid mask option!");
            return 0;
        }

        /// getnnerMostIf - If the given BB is the head of an innermost IF
        /// block, return its condition, 'if' branch and 'else' branch (if any)
        /// and tail, i.e.,
        ///
        ///         H                  H
        ///        / \                / |
        ///     'if' 'else'   or   'if' |
        ///        \ /                \ |
        ///         T                  T
        /// Otherwise, return all null pointers.
        ///
        /// TODO: Add 'goto' support as CFG structurization is currently all or
        /// nothing.
        std::tuple<G4_INST * /* last instruction in head, i.e. 'if' */,
                   G4_BB * /* if */, G4_BB * /* else */, G4_BB * /* tail */>
        getInnermostIfBlock(G4_BB *BB) const {
            // Such BB should already be recognized as structural IF statement.
            if (BB->empty())
                return std::make_tuple(nullptr, nullptr, nullptr, nullptr);

            G4_INST *last = BB->back();

            // Skip if there's 'NoMask' on that (possible) conditional branch.
            if (last->getMaskOption() & InstOpt_WriteEnable)
                return std::make_tuple(nullptr, nullptr, nullptr, nullptr);

            // Check whether it's 'if' or 'goto'.
            G4_opcode op = last->opcode();
            if (op != G4_if) {
                if (op != G4_goto)
                    return std::make_tuple(nullptr, nullptr, nullptr, nullptr);

                // Extra checks for 'goto'.

                // Skip if there's no predicate.
                if (!last->getPredicate())
                    return std::make_tuple(nullptr, nullptr, nullptr, nullptr);

                // Skip backward goto.
                if (last->isFlowControl() && last->asCFInst()->isBackward())
                    return std::make_tuple(nullptr, nullptr, nullptr, nullptr);

                // Skip if there's no exactly 2 successors.
                if (BB->Succs.size() != 2)
                    return std::make_tuple(nullptr, nullptr, nullptr, nullptr);
            }

            ASSERT_USER(BB->Succs.size() == 2,
                        "'if' should have exactly two successors!");
            ASSERT_USER(last->getPredicate(),
                        "'if' or 'goto' should be conditional!");

            G4_BB *s0 = BB->Succs.front();  // if-block
            G4_BB *s1 = BB->Succs.back();   // else-block

            G4_BB *t0 = getSingleSuccessor(s0, s1);
            if (!t0) {
                // The innermost 'if' branch should have only one
                // successor.
                return std::make_tuple(nullptr, nullptr, nullptr, nullptr);
            }
            // It also needs to have a single predecessor.
            if (!getSinglePredecessor(s0, s1))
                return std::make_tuple(nullptr, nullptr, nullptr, nullptr);

            if (t0 == s1) {
                // 'if-fi'
                DEBUG(std::cerr << "Found an innermost if-fi block at"
                                << " BB" << BB->getId()
                                << " with branch BB" << s0->getId()
                                << " and tail BB" << t0->getId() << '\n');
                return std::make_tuple(last, s0, nullptr, t0);
            }

            G4_BB *t1 = getSingleSuccessor(s1, s0);
            if (!t1 || t0 != t1) {
                // The innermost 'else' branch should have only one common
                // successor from the 'if' branch.
                return std::make_tuple(nullptr, nullptr, nullptr, nullptr);
            }
            // It also needs to have a single predecessor.
            if (!getSinglePredecessor(s1, s0))
                return std::make_tuple(nullptr, nullptr, nullptr, nullptr);

            DEBUG(std::cerr << "Found an innermost if-else-fi block at"
                            << " BB" << BB->getId()
                            << " with branches {"
                            << "BB" << s0->getId() << ", BB" << s1->getId()
                            << "} and tail BB" << t0->getId() << '\n');

            // 'if-else-fi'
            return std::make_tuple(last, s0, s1, t0);
        }

        /// isPredictable - Check whether the given instruction 'I' could be
        /// predicated using the predicate from the specified 'if' instruction.
        bool isPredictable(G4_INST *I, G4_INST *ifInst) const {
            // Already predicated.
            if (I->getPredicate()) {
                // NOTE: It's not the responsibility of this routine to check
                // special cases where an already predicated instruction could be
                // predicated again.
                return false;
            }

            // With cond modifier.
            if (I->getCondMod()) {
                // When condition modifier is present, we cannot predicate due to
                // 1) the flag register is used as both predicate and condmod flag;
                // 2) the update to flag register is guarded by EMask only. The
                //    behavior is different from regular predicated insts.
                return false;
            }

            G4_opcode op = I->opcode();
            switch (G4_Inst_Table[op].instType) {
            case InstTypeMov:
                switch (op) {
                case G4_mov:
                case G4_movi:
                case G4_smov:
                    break;
                case G4_sel:
                case G4_csel:
                default:
                    return false;
                }
                break;
            case InstTypeArith:
            case InstTypeLogic:
            case InstTypeVector:
                break;
            case InstTypeCompare:
            case InstTypeFlow:
            case InstTypeMask:
            case InstTypeMisc:
                // TODO: G4_send, G4_sendc, G4_sends, and G4_sendsc need
                // investigating whether they are profitable to be predicated.
            case InstTypePseudoLogic:
            case InstTypeReserved:
            default:
                return false;
            }

            unsigned maskOpt = I->getMaskOption();

            // Skip 'NoMask' so far as it requires further investigation.
            // TODO: When 'NoMask' is present, we could convert them to
            // (+P.any) or (-P.all) depending on 'if' or 'else' branch. E.g.,
            //
            //  (+P) if (16)                    (-P) if (16)
            //      mov (1) V0 V1 {NoMask}  or      mov (1) V0 V1 {NoMask}
            //  endif (16)                      endif (16)
            //
            //
            // could be converted into
            //
            //  (+P.any) mov (1) V0 V1 {NoMask} or
            //  (-P.all) mov (1) V0 V1 {NoMask}
            if (maskOpt & InstOpt_WriteEnable)
                return false;

            unsigned ifMaskOpt = ifInst->getMaskOption();
            ASSERT_USER((ifMaskOpt & InstOpt_WriteEnable) == 0,
                        "Unexpected 'NoMask' in 'if' emask.");

            unsigned maskBits =
                getEMaskBits(maskOpt & InstOpt_QuarterMasks, I->getExecSize());
            unsigned ifMaskBits =
                getEMaskBits(ifMaskOpt & InstOpt_QuarterMasks,
                             ifInst->getExecSize());
            // Skip if emask bits in 'if' cannot cover the one from the given
            // instruction.
            if ((~ifMaskBits) & maskBits)
                return false;

            return true;
        }

        // isFlagClearingFollowedByGoto - Check if the current instruction is
        // the flag clearing instruction followed by a goto using that flag.
        bool isFlagClearingFollowedByGoto(G4_INST *I, G4_BB *BB) const {
            // Skip if it's not the second to the last instruction, which
            // should be a 'goto' with predicate.
            if (BB->size() <= 1)
                return false;
            auto iter = BB->rbegin();
            G4_INST *last = *iter++;
            if (I != *iter)
                return false;
            if (last->opcode() != G4_goto)
                return false;
            G4_Predicate *pred = last->getPredicate();
            if (!pred)
                return false;

            // Skip non-mov instructions.
            if (I->opcode() != G4_mov)
                return false;

            // Source should be zero.
            G4_Operand *src = I->getSrc(0);
            if (!src->isImm() || !src->asImm()->isZero())
                return false;

            // Dst should be a flag register.
            G4_Operand *dst = I->getDst();
            if (!dst->isFlag())
                return false;

            if (dst->getTopDcl() != pred->getTopDcl())
                return false;

            // Dst should be used in that goto instruction as the predicate.
            return true;
        }

        /// getPredictableInsts - Return the total number of instructions if
        /// all instruction in the given BB is predictable. Otherwise, return
        /// 0.
        unsigned getPredictableInsts(G4_BB *BB, G4_INST *ifInst) const {
            ASSERT_USER(ifInst->opcode() == G4_if ||
                        ifInst->opcode() == G4_goto,
                        "Either 'if' or 'goto' is expected!");

            bool isGoto = (ifInst->opcode() == G4_goto);
            unsigned sum = 0;

            for (auto *I : *BB) {
                G4_opcode op = I->opcode();
                // Ignore G4_label
                if (op == G4_label) {
                    ASSERT_USER(I == BB->front(),
                                "'label' should be the first instruction!");
                    continue;
                }
                // Ignore G4_else
                if (isGoto) {
                    if (op == G4_join) {
                        ASSERT_USER(BB->size() > 1 &&
                                    I == (*++BB->begin()),
                                    "'join' should be the second instruction!");
                        continue;
                    }
                    if (op == G4_goto) {
                        ASSERT_USER(I == BB->back(),
                                    "'goto' should be the last instruction!");
                        continue;
                    }
                    if (isFlagClearingFollowedByGoto(I, BB)) {
                        ASSERT_USER(BB->size() > 1 &&
                                    I == (*++BB->rbegin()),
                                    "flag clearing should be the second to last"
                                    " instruction!");
                        continue;
                    }
                } else {
                    if (op == G4_else) {
                        ASSERT_USER(I == BB->back(),
                                    "'else' should be the last instruction!");
                        continue;
                    }
                }
                if (!isPredictable(I, ifInst)) {
                    return 0;
                }
                ++sum;
            }

            return sum;
        }

        /// reversePredicate - Reverse the predicate state.
        void reversePredicate(G4_Predicate *pred) const {
            G4_PredState state = pred->getState();
            switch (state) {
            case PredState_Plus:
                state = PredState_Minus;
                break;
            case PredState_Minus:
                state = PredState_Plus;
                break;
            default:
                break;
            }
            pred->setState(state);
        }

        /// An alternative to c++11 standard 'std::to_string' but does not
        /// require c++11. 'T' should be restricted to integer types by using
        /// 'enable_if' to avoid breaking broken c++ support on some platforms.
        template<typename T>
        std::string toString(T v) const {
            std::ostringstream oss;
            oss << v;
            return oss.str();
        }

        /// markEmptyBB - Mark the given BB as empty.
        void markEmptyBB(IR_Builder *IRB, G4_BB *BB) const {
            ASSERT_USER(BB->empty(),
                        "BB to be marked empty is not empty!");

            std::string id = "LABEL__EMPTYBB__" + toString(BB->getId());
            G4_Label *label = IRB->createLabel(id, LABEL_BLOCK);
            G4_INST *inst =
                IRB->createInternalInst(nullptr, G4_label, nullptr, false,
                                UNDEFINED_EXEC_SIZE, nullptr,
                                label, nullptr, 0);
            BB->push_back(inst);
        }

        void fullConvert(IfConvertible &);
        void partialConvert(IfConvertible &);

    public:
        IfConverter(FlowGraph &g) : fg(g) {}

        void analyze(std::vector<IfConvertible> &);

        void convert(IfConvertible &IC) {
            switch (IC.kind) {
            case FullConvert:
                fullConvert(IC);
                break;
            default:
                partialConvert(IC);
                break;
            }
        }
    };

} // End anonymous namespace

void IfConverter::analyze(std::vector<IfConvertible> &list) {
    for (auto *BB : fg.BBs) {
        G4_INST *ifInst;
        G4_BB *s0, *s1, *t;
        std::tie(ifInst, s0, s1, t) = getInnermostIfBlock(BB);

        if (!ifInst) {
            // Skip non-innermost if.
            continue;
        }

        G4_Predicate *pred = ifInst->getPredicate();

        unsigned n0 = getPredictableInsts(s0, ifInst);
        unsigned n1 = s1 ? getPredictableInsts(s1, ifInst) : 0;

        if (s0 && s1) {
            if (((n0 > 0) && (n0 < FullyConvertibleMaxInsts)) &&
                ((n1 > 0) && (n1 < FullyConvertibleMaxInsts))) {
                // Both 'if' and 'else' are profitable to be if-converted.
                list.push_back(
                    IfConvertible(FullConvert, pred, BB, s0, s1, t));
            } else if ((n0 > 0) && (n0 < PartialConvertibleMaxInsts)) {
                // Only 'if' is profitable to be converted.
                list.push_back(
                    IfConvertible(PartialIfConvert, pred, BB, s0, s1, t));
            } else if ((n1 > 0) && (n1 < PartialConvertibleMaxInsts)) {
                // Only 'else' is profitable to be converted.
                list.push_back(
                    IfConvertible(PartialElseConvert, pred, BB, s0, s1, t));
            }
        } else if ((n0 > 0) && (n0 < FullyConvertibleMaxInsts)) {
            list.push_back(
                IfConvertible(FullConvert, pred, BB, s0, nullptr, t));
        }
    }
}

// Combining GCC 4.9.0 and libcxx 11.01, incorrect code is generated on two
// consecutive `pop_front()` on std::list. Add `volatile` to prevent incorrect
// code generation during over optimization.
#if (defined(ANDROID) && !(defined(__clang__) || defined(__INTEL_COMPILER)) && \
     (defined(__GNUC__) || defined(__GNUG__)) && \
     __GNUC__ == 4 && __GNUC_MINOR__ == 9 && __GNUC_PATCHLEVEL__ == 0 && \
     defined(_LIBCPP_VERSION) && _LIBCPP_VERSION == 1101)
#define ANDROID_WORKAROUND volatile
#else
#define ANDROID_WORKAROUND
#endif

void IfConverter::fullConvert(IfConvertible &IC) {
    G4_Predicate &pred = *IC.pred;
    G4_BB *head = IC.head;
    G4_BB * ANDROID_WORKAROUND tail = IC.tail;
    G4_BB *s0 = IC.succIf;
    G4_BB *s1 = IC.succElse;

    INST_LIST_ITER pos = std::prev(head->end());
    G4_opcode op = (*pos)->opcode();
    ASSERT_USER(op == G4_if || op == G4_goto,
                "Convertible if is not started with 'if' or 'goto'!");
    bool isGoto = (op == G4_goto);

    // forward goto's behavior is platform dependent
    bool needReversePredicateForGoto = (isGoto && fg.builder->gotoJumpOnTrue());
    // Merge predicated 'if' into header.
    for (/* EMPTY */; !s0->empty(); s0->pop_front()) {
        auto I = s0->front();
        G4_opcode op = I->opcode();
        if (op == G4_label)
            continue;
        if (isGoto && s1) {
            if (op == G4_goto)
                continue;
            if (isFlagClearingFollowedByGoto(I, s0))
                continue;
        } else {
            if (op == G4_else)
                continue;
        }
        /* Predicate instructions if it's not goto-style or it's not
         * neither goto nor its flag clearing instruction */
        if (!isGoto ||
            !(op == G4_goto || isFlagClearingFollowedByGoto(I, s0))) {
            // Negative predicate instructions if needed.
            if (needReversePredicateForGoto) {
                G4_Predicate *negPred = fg.builder->createPredicate(pred);
                reversePredicate(negPred);
                I->setPredicate(negPred);
            } else {
                I->setPredicate(fg.builder->createPredicate(pred));
            }
        }
        head->insert(pos, I);
    }
    markEmptyBB(fg.builder, s0);
    // Merge predicated 'else' into header.
    if (s1) {
        // Reverse the flag controling whether the predicate needs reversing.
        needReversePredicateForGoto = !needReversePredicateForGoto;
        for (/* EMPTY */; !s1->empty(); s1->pop_front()) {
            auto I = s1->front();
            G4_opcode op = I->opcode();
            if (op == G4_label)
                continue;
            if (op == G4_join)
                continue;
            /* Predicate instructions if it's not goto-style or it's not
             * neither goto nor its flag clearing instruction */
            if (!isGoto ||
                !(op == G4_goto || isFlagClearingFollowedByGoto(I, s1))) {
                // Negative predicate instructions if needed.
                if (needReversePredicateForGoto) {
                    G4_Predicate *negPred = fg.builder->createPredicate(pred);
                    reversePredicate(negPred);
                    I->setPredicate(negPred);
                } else {
                    I->setPredicate(fg.builder->createPredicate(pred));
                }
            }
            head->insert(pos, I);
        }
        markEmptyBB(fg.builder, s1);
    }

    // Remove 'if' instruction in head.
    head->erase(pos);

    // Skip tail merging if tail has other incoming edge(s).
    if (tail->Preds.size() != 2)
        return;

    // Remove 'label' and 'endif'/'join' instructions in tail.
    ASSERT_USER(tail->front()->opcode() == G4_label,
                "BB is not started with 'label'!");
    tail->pop_front();
    ASSERT_USER(tail->front()->opcode() == G4_endif ||
                tail->front()->opcode() == G4_join,
                "Convertible if is not ended with 'endif'!");
    tail->pop_front();
    // Merge head and tail to get more code scheduling chance.
    head->splice(head->end(), tail);
    markEmptyBB(fg.builder, tail);
}

void IfConverter::partialConvert(IfConvertible &IC) {
    // TODO: Add partial if-conversion support.
}

void runIfCvt(FlowGraph &fg) {
    IfConverter converter(fg);

    std::vector<IfConvertible> ifList;
    converter.analyze(ifList);

    // FIXME: The convertible 'if's are traversed with assumption that BBs are
    // already ordered in topological order so that, once we merge head & tail
    // blocks, we won't break the remaining convertible 'if's to be converted.
    for (auto II = ifList.rbegin(), IE = ifList.rend(); II != IE; ++II) {
        converter.convert(*II);
    }

    // Run additional transforms from 'sel' to 'mov' if one of the source
    // operands is equal to the destination.
    for (G4_BB *BB: fg.BBs) {
        for (auto BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {
            G4_INST *I = *BI;
            if (I->opcode() != G4_sel || !I->getPredicate() || I->getCondMod())
                continue;

            auto compareOperand =
                [](G4_DstRegRegion *A, G4_Operand *B, unsigned ExecSize)
                -> G4_CmpRelation {
                G4_CmpRelation Res = A->compareOperand(B);
                if (Res != Rel_interfere)
                    return Res;
                if (A->getRegAccess() != IndirGRF ||
                    B->getRegAccess() != IndirGRF)
                    return Res;
                if (A->getHorzStride() != 1)
                    return Res;
                // Extra check if both are indirect register accesses.
                G4_VarBase *BaseA = A->getBase();
                G4_VarBase *BaseB = B->getBase();
                if (!BaseA || !BaseB || BaseA != BaseB || !BaseA->isRegVar())
                    return Res;
                if (!B->isSrcRegRegion())
                    return Res;
                G4_SrcRegRegion *S = B->asSrcRegRegion();
                if (!S->getRegion()->isContiguous(ExecSize))
                    return Res;
                if (A->getRegOff() != S->getRegOff() ||
                    A->getSubRegOff() != S->getSubRegOff())
                    return Res;
                if (A->getAddrImm() != S->getAddrImm())
                    return Res;
                return Rel_eq;
            };

            unsigned ExSz = I->getExecSize();
            G4_DstRegRegion *Dst = I->getDst();
            G4_Operand *Src0 = I->getSrc(0);
            G4_Operand *Src1 = I->getSrc(1);
            int OpndIdx = -1;
            if (compareOperand(Dst, Src0, ExSz) == Rel_eq &&
                Src0->isSrcRegRegion() &&
                Src0->asSrcRegRegion()->getModifier() == Mod_src_undef)
                OpndIdx = 0;
            else if (compareOperand(Dst, Src1, ExSz) == Rel_eq &&
                     Src1->isSrcRegRegion() &&
                     Src1->asSrcRegRegion()->getModifier() == Mod_src_undef)
                OpndIdx = 1;
            if (OpndIdx >= 0) {
                // If dst is equal to one of operands of 'sel', that
                // 'sel' could be transformed into a predicated 'mov',
                // i.e.,
                //
                // transforms
                //
                //  (+p) sel dst, src0, src1
                //
                // into
                //
                //  (+p) mov dst, src0   if dst == src1
                //
                // or
                //
                //  (-p) mov dst, src1   if dst == src0
                //
                if (OpndIdx == 0) {
                    // Inverse predicate.
                    G4_Predicate *Pred = I->getPredicate();
                    G4_PredState State = Pred->getState();
                    State = (State == PredState_Plus) ? PredState_Minus
                                                      : PredState_Plus;
                    Pred->setState(State);
                    // Swap source operands.
                    I->setSrc(Src1, 0);
                    I->setSrc(Src0, 1);
                }
                I->setOpcode(G4_mov);
                I->setSrc(nullptr, 1);
            }
        }
    }
}

// vim:ts=4:sw=4:et:
