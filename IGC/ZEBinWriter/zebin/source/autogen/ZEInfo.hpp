/*========================== begin_copyright_notice ============================

Copyright (C) 2020-2022 Intel Corporation

SPDX-License-Identifier: MIT

============================= end_copyright_notice ===========================*/

//===- ZEInfo.hpp -----------------------------------------------*- C++ -*-===//
// ZE Binary Utilitis
//
// \file
// This file declares the struct representation of .ze.info section
//===----------------------------------------------------------------------===//

// ******************** DO NOT MODIFY DIRECTLY *********************************
// This file is auto-generated by ZEAutoTool/fileparser.py

#ifndef ZE_INFO_HPP
#define ZE_INFO_HPP

#include <string>
#include <vector>

namespace zebin {

typedef int64_t     zeinfo_int64_t;
typedef int32_t     zeinfo_int32_t;
typedef bool        zeinfo_bool_t;
typedef std::string zeinfo_str_t;
struct zeInfoUserAttribute
{
    bool operator==(const zeInfoUserAttribute& other) const
    {
        return intel_reqd_sub_group_size == other.intel_reqd_sub_group_size && intel_reqd_workgroup_walk_order == other.intel_reqd_workgroup_walk_order && invalid_kernel == other.invalid_kernel && reqd_work_group_size == other.reqd_work_group_size && vec_type_hint == other.vec_type_hint && work_group_size_hint == other.work_group_size_hint;
    }
    zeinfo_int32_t intel_reqd_sub_group_size = 0;
    std::vector<zeinfo_int32_t> intel_reqd_workgroup_walk_order;
    zeinfo_str_t invalid_kernel;
    std::vector<zeinfo_int32_t> reqd_work_group_size;
    zeinfo_str_t vec_type_hint;
    std::vector<zeinfo_int32_t> work_group_size_hint;
};
struct zeInfoExecutionEnv
{
    bool operator==(const zeInfoExecutionEnv& other) const
    {
        return barrier_count == other.barrier_count && disable_mid_thread_preemption == other.disable_mid_thread_preemption && grf_count == other.grf_count && has_4gb_buffers == other.has_4gb_buffers && has_device_enqueue == other.has_device_enqueue && has_dpas == other.has_dpas && has_fence_for_image_access == other.has_fence_for_image_access && has_global_atomics == other.has_global_atomics && has_multi_scratch_spaces == other.has_multi_scratch_spaces && has_no_stateless_write == other.has_no_stateless_write && has_stack_calls == other.has_stack_calls && require_disable_eufusion == other.require_disable_eufusion && indirect_stateless_count == other.indirect_stateless_count && inline_data_payload_size == other.inline_data_payload_size && offset_to_skip_per_thread_data_load == other.offset_to_skip_per_thread_data_load && offset_to_skip_set_ffid_gp == other.offset_to_skip_set_ffid_gp && required_sub_group_size == other.required_sub_group_size && required_work_group_size == other.required_work_group_size && simd_size == other.simd_size && slm_size == other.slm_size && subgroup_independent_forward_progress == other.subgroup_independent_forward_progress && thread_scheduling_mode == other.thread_scheduling_mode && work_group_walk_order_dimensions == other.work_group_walk_order_dimensions;
    }
    zeinfo_int32_t barrier_count = 0;
    zeinfo_bool_t disable_mid_thread_preemption = false;
    zeinfo_int32_t grf_count = 0;
    zeinfo_bool_t has_4gb_buffers = false;
    zeinfo_bool_t has_device_enqueue = false;
    zeinfo_bool_t has_dpas = false;
    zeinfo_bool_t has_fence_for_image_access = false;
    zeinfo_bool_t has_global_atomics = false;
    zeinfo_bool_t has_multi_scratch_spaces = false;
    zeinfo_bool_t has_no_stateless_write = false;
    zeinfo_bool_t has_stack_calls = false;
    zeinfo_bool_t require_disable_eufusion = false;
    zeinfo_int32_t indirect_stateless_count = 0;
    zeinfo_int32_t inline_data_payload_size = 0;
    zeinfo_int32_t offset_to_skip_per_thread_data_load = 0;
    zeinfo_int32_t offset_to_skip_set_ffid_gp = 0;
    zeinfo_int32_t required_sub_group_size = 0;
    std::vector<zeinfo_int32_t> required_work_group_size;
    zeinfo_int32_t simd_size = 0;
    zeinfo_int32_t slm_size = 0;
    zeinfo_bool_t subgroup_independent_forward_progress = false;
    zeinfo_str_t thread_scheduling_mode;
    std::vector<zeinfo_int32_t> work_group_walk_order_dimensions;
};
struct zeInfoPayloadArgument
{
    bool operator==(const zeInfoPayloadArgument& other) const
    {
        return arg_type == other.arg_type && offset == other.offset && size == other.size && arg_index == other.arg_index && addrmode == other.addrmode && addrspace == other.addrspace && access_type == other.access_type && sampler_index == other.sampler_index && source_offset == other.source_offset && slm_alignment == other.slm_alignment && image_type == other.image_type && image_transformable == other.image_transformable && sampler_type == other.sampler_type && is_pipe == other.is_pipe;
    }
    zeinfo_str_t arg_type;
    zeinfo_int32_t offset = 0;
    zeinfo_int32_t size = 0;
    zeinfo_int32_t arg_index = -1;
    zeinfo_str_t addrmode;
    zeinfo_str_t addrspace;
    zeinfo_str_t access_type;
    zeinfo_int32_t sampler_index = -1;
    zeinfo_int32_t source_offset = -1;
    zeinfo_int32_t slm_alignment = 0;
    zeinfo_str_t image_type;
    zeinfo_bool_t image_transformable = false;
    zeinfo_str_t sampler_type;
    zeinfo_bool_t is_pipe = false;
};
struct zeInfoPerThreadPayloadArgument
{
    bool operator==(const zeInfoPerThreadPayloadArgument& other) const
    {
        return arg_type == other.arg_type && offset == other.offset && size == other.size;
    }
    zeinfo_str_t arg_type;
    zeinfo_int32_t offset = 0;
    zeinfo_int32_t size = 0;
};
struct zeInfoBindingTableIndex
{
    bool operator==(const zeInfoBindingTableIndex& other) const
    {
        return bti_value == other.bti_value && arg_index == other.arg_index;
    }
    zeinfo_int32_t bti_value = 0;
    zeinfo_int32_t arg_index = 0;
};
struct zeInfoPerThreadMemoryBuffer
{
    bool operator==(const zeInfoPerThreadMemoryBuffer& other) const
    {
        return type == other.type && usage == other.usage && size == other.size && slot == other.slot && is_simt_thread == other.is_simt_thread;
    }
    zeinfo_str_t type;
    zeinfo_str_t usage;
    zeinfo_int32_t size = 0;
    zeinfo_int32_t slot = 0;
    zeinfo_bool_t is_simt_thread = false;
};
struct zeInfoInlineSampler
{
    bool operator==(const zeInfoInlineSampler& other) const
    {
        return sampler_index == other.sampler_index && addrmode == other.addrmode && filtermode == other.filtermode && normalized == other.normalized;
    }
    zeinfo_int32_t sampler_index = 0;
    zeinfo_str_t addrmode;
    zeinfo_str_t filtermode;
    zeinfo_bool_t normalized = false;
};
struct zeInfoExperimentalProperties
{
    bool operator==(const zeInfoExperimentalProperties& other) const
    {
        return has_non_kernel_arg_load == other.has_non_kernel_arg_load && has_non_kernel_arg_store == other.has_non_kernel_arg_store && has_non_kernel_arg_atomic == other.has_non_kernel_arg_atomic;
    }
    zeinfo_int32_t has_non_kernel_arg_load = -1;
    zeinfo_int32_t has_non_kernel_arg_store = -1;
    zeinfo_int32_t has_non_kernel_arg_atomic = -1;
};
struct zeInfoDebugEnv
{
    bool operator==(const zeInfoDebugEnv& other) const
    {
        return sip_surface_bti == other.sip_surface_bti && sip_surface_offset == other.sip_surface_offset;
    }
    zeinfo_int32_t sip_surface_bti = -1;
    zeinfo_int32_t sip_surface_offset = -1;
};
struct zeInfoHostAccess
{
    bool operator==(const zeInfoHostAccess& other) const
    {
        return device_name == other.device_name && host_name == other.host_name;
    }
    zeinfo_str_t device_name;
    zeinfo_str_t host_name;
};
struct zeInfoArgInfo
{
    bool operator==(const zeInfoArgInfo& other) const
    {
        return index == other.index && name == other.name && address_qualifier == other.address_qualifier && access_qualifier == other.access_qualifier && type_name == other.type_name && type_qualifiers == other.type_qualifiers;
    }
    zeinfo_int32_t index = 0;
    zeinfo_str_t name;
    zeinfo_str_t address_qualifier;
    zeinfo_str_t access_qualifier;
    zeinfo_str_t type_name;
    zeinfo_str_t type_qualifiers;
};
typedef std::vector<zeInfoPayloadArgument> PayloadArgumentsTy;
typedef std::vector<zeInfoPerThreadPayloadArgument> PerThreadPayloadArgumentsTy;
typedef std::vector<zeInfoBindingTableIndex> BindingTableIndicesTy;
typedef std::vector<zeInfoPerThreadMemoryBuffer> PerThreadMemoryBuffersTy;
typedef std::vector<zeInfoInlineSampler> InlineSamplersTy;
struct zeInfoKernel
{
    bool operator==(const zeInfoKernel& other) const
    {
        return name == other.name && user_attributes == other.user_attributes && execution_env == other.execution_env && payload_arguments == other.payload_arguments && per_thread_payload_arguments == other.per_thread_payload_arguments && binding_table_indices == other.binding_table_indices && per_thread_memory_buffers == other.per_thread_memory_buffers && inline_samplers == other.inline_samplers && experimental_properties == other.experimental_properties && debug_env == other.debug_env;
    }
    zeinfo_str_t name;
    zeInfoUserAttribute user_attributes;
    zeInfoExecutionEnv execution_env;
    PayloadArgumentsTy payload_arguments;
    PerThreadPayloadArgumentsTy per_thread_payload_arguments;
    BindingTableIndicesTy binding_table_indices;
    PerThreadMemoryBuffersTy per_thread_memory_buffers;
    InlineSamplersTy inline_samplers;
    zeInfoExperimentalProperties experimental_properties;
    zeInfoDebugEnv debug_env;
};
struct zeInfoFunction
{
    bool operator==(const zeInfoFunction& other) const
    {
        return name == other.name && execution_env == other.execution_env;
    }
    zeinfo_str_t name;
    zeInfoExecutionEnv execution_env;
};
typedef std::vector<zeInfoArgInfo> ArgsInfoTy;
struct zeInfoKernelMiscInfo
{
    bool operator==(const zeInfoKernelMiscInfo& other) const
    {
        return name == other.name && args_info == other.args_info;
    }
    zeinfo_str_t name;
    ArgsInfoTy args_info;
};
typedef std::vector<zeInfoKernel> KernelsTy;
typedef std::vector<zeInfoFunction> FunctionsTy;
typedef std::vector<zeInfoHostAccess> HostAccessesTy;
typedef std::vector<zeInfoKernelMiscInfo> KernelsMiscInfoTy;
struct zeInfoContainer
{
    bool operator==(const zeInfoContainer& other) const
    {
        return version == other.version && kernels == other.kernels && functions == other.functions && global_host_access_table == other.global_host_access_table && kernels_misc_info == other.kernels_misc_info;
    }
    zeinfo_str_t version;
    KernelsTy kernels;
    FunctionsTy functions;
    HostAccessesTy global_host_access_table;
    KernelsMiscInfoTy kernels_misc_info;
};
struct PreDefinedAttrGetter{
    static zeinfo_str_t getVersionNumber() { return "1.22"; }

    enum class ArgThreadSchedulingMode {
        age_based,
        round_robin,
        round_robin_stall
    };
    enum class ArgType {
        packed_local_ids,
        local_id,
        local_size,
        group_count,
        work_dimensions,
        global_size,
        enqueued_local_size,
        global_id_offset,
        private_base_stateless,
        buffer_address,
        buffer_offset,
        printf_buffer,
        implicit_arg_buffer,
        sync_buffer,
        arg_byvalue,
        arg_bypointer,
        image_height,
        image_width,
        image_depth,
        image_num_mip_levels,
        image_channel_data_type,
        image_channel_order,
        image_srgb_channel_order,
        image_array_size,
        image_num_samples,
        flat_image_baseoffset,
        flat_image_height,
        flat_image_width,
        flat_image_pitch,
        sampler_address,
        sampler_normalized,
        sampler_snap_wa
    };
    enum class ArgAddrMode {
        stateless,
        stateful,
        bindless,
        slm
    };
    enum class ArgAddrSpace {
        global,
        local,
        constant,
        image,
        sampler
    };
    enum class ArgAccessType {
        readonly,
        writeonly,
        readwrite
    };
    enum class ArgImageType {
        image_buffer,
        image_1d,
        image_1d_array,
        image_2d,
        image_2d_array,
        image_3d,
        image_cube,
        image_cube_array,
        image_2d_depth,
        image_2d_array_depth,
        image_2d_msaa,
        image_2d_msaa_depth,
        image_2d_array_msaa,
        image_2d_array_msaa_depth,
        image_2d_media,
        image_2d_media_block
    };
    enum class ArgSamplerType {
        texture,
        sample_8x8,
        sample_8x8_2dconvolve,
        sample_8x8_erode,
        sample_8x8_dilate,
        sample_8x8_minmaxfilter,
        sample_8x8_minmax,
        sample_8x8_centroid,
        sample_8x8_bool_centroid,
        sample_8x8_bool_sum
    };
    enum class MemBufferType {
        global,
        scratch,
        slm
    };
    enum class MemBufferUsage {
        private_space,
        spill_fill_space,
        single_space
    };
    enum class ArgSamplerAddrMode {
        none,
        clamp_border,
        clamp_edge,
        repeat,
        mirror
    };
    enum class ArgSamplerFilterMode {
        nearest,
        linear
    };
    static zeinfo_str_t get(ArgThreadSchedulingMode val) {
        switch(val) {
        case ArgThreadSchedulingMode::age_based:
            return "age_based";
        case ArgThreadSchedulingMode::round_robin:
            return "round_robin";
        case ArgThreadSchedulingMode::round_robin_stall:
            return "round_robin_stall";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(ArgType val) {
        switch(val) {
        case ArgType::packed_local_ids:
            return "packed_local_ids";
        case ArgType::local_id:
            return "local_id";
        case ArgType::local_size:
            return "local_size";
        case ArgType::group_count:
            return "group_count";
        case ArgType::work_dimensions:
            return "work_dimensions";
        case ArgType::global_size:
            return "global_size";
        case ArgType::enqueued_local_size:
            return "enqueued_local_size";
        case ArgType::global_id_offset:
            return "global_id_offset";
        case ArgType::private_base_stateless:
            return "private_base_stateless";
        case ArgType::buffer_address:
            return "buffer_address";
        case ArgType::buffer_offset:
            return "buffer_offset";
        case ArgType::printf_buffer:
            return "printf_buffer";
        case ArgType::implicit_arg_buffer:
            return "implicit_arg_buffer";
        case ArgType::sync_buffer:
            return "sync_buffer";
        case ArgType::arg_byvalue:
            return "arg_byvalue";
        case ArgType::arg_bypointer:
            return "arg_bypointer";
        case ArgType::image_height:
            return "image_height";
        case ArgType::image_width:
            return "image_width";
        case ArgType::image_depth:
            return "image_depth";
        case ArgType::image_num_mip_levels:
            return "image_num_mip_levels";
        case ArgType::image_channel_data_type:
            return "image_channel_data_type";
        case ArgType::image_channel_order:
            return "image_channel_order";
        case ArgType::image_srgb_channel_order:
            return "image_srgb_channel_order";
        case ArgType::image_array_size:
            return "image_array_size";
        case ArgType::image_num_samples:
            return "image_num_samples";
        case ArgType::flat_image_baseoffset:
            return "flat_image_baseoffset";
        case ArgType::flat_image_height:
            return "flat_image_height";
        case ArgType::flat_image_width:
            return "flat_image_width";
        case ArgType::flat_image_pitch:
            return "flat_image_pitch";
        case ArgType::sampler_address:
            return "sampler_address";
        case ArgType::sampler_normalized:
            return "sampler_normalized";
        case ArgType::sampler_snap_wa:
            return "sampler_snap_wa";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(ArgAddrMode val) {
        switch(val) {
        case ArgAddrMode::stateless:
            return "stateless";
        case ArgAddrMode::stateful:
            return "stateful";
        case ArgAddrMode::bindless:
            return "bindless";
        case ArgAddrMode::slm:
            return "slm";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(ArgAddrSpace val) {
        switch(val) {
        case ArgAddrSpace::global:
            return "global";
        case ArgAddrSpace::local:
            return "local";
        case ArgAddrSpace::constant:
            return "constant";
        case ArgAddrSpace::image:
            return "image";
        case ArgAddrSpace::sampler:
            return "sampler";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(ArgAccessType val) {
        switch(val) {
        case ArgAccessType::readonly:
            return "readonly";
        case ArgAccessType::writeonly:
            return "writeonly";
        case ArgAccessType::readwrite:
            return "readwrite";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(ArgImageType val) {
        switch(val) {
        case ArgImageType::image_buffer:
            return "image_buffer";
        case ArgImageType::image_1d:
            return "image_1d";
        case ArgImageType::image_1d_array:
            return "image_1d_array";
        case ArgImageType::image_2d:
            return "image_2d";
        case ArgImageType::image_2d_array:
            return "image_2d_array";
        case ArgImageType::image_3d:
            return "image_3d";
        case ArgImageType::image_cube:
            return "image_cube";
        case ArgImageType::image_cube_array:
            return "image_cube_array";
        case ArgImageType::image_2d_depth:
            return "image_2d_depth";
        case ArgImageType::image_2d_array_depth:
            return "image_2d_array_depth";
        case ArgImageType::image_2d_msaa:
            return "image_2d_msaa";
        case ArgImageType::image_2d_msaa_depth:
            return "image_2d_msaa_depth";
        case ArgImageType::image_2d_array_msaa:
            return "image_2d_array_msaa";
        case ArgImageType::image_2d_array_msaa_depth:
            return "image_2d_array_msaa_depth";
        case ArgImageType::image_2d_media:
            return "image_2d_media";
        case ArgImageType::image_2d_media_block:
            return "image_2d_media_block";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(ArgSamplerType val) {
        switch(val) {
        case ArgSamplerType::texture:
            return "texture";
        case ArgSamplerType::sample_8x8:
            return "sample_8x8";
        case ArgSamplerType::sample_8x8_2dconvolve:
            return "sample_8x8_2dconvolve";
        case ArgSamplerType::sample_8x8_erode:
            return "sample_8x8_erode";
        case ArgSamplerType::sample_8x8_dilate:
            return "sample_8x8_dilate";
        case ArgSamplerType::sample_8x8_minmaxfilter:
            return "sample_8x8_minmaxfilter";
        case ArgSamplerType::sample_8x8_minmax:
            return "sample_8x8_minmax";
        case ArgSamplerType::sample_8x8_centroid:
            return "sample_8x8_centroid";
        case ArgSamplerType::sample_8x8_bool_centroid:
            return "sample_8x8_bool_centroid";
        case ArgSamplerType::sample_8x8_bool_sum:
            return "sample_8x8_bool_sum";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(MemBufferType val) {
        switch(val) {
        case MemBufferType::global:
            return "global";
        case MemBufferType::scratch:
            return "scratch";
        case MemBufferType::slm:
            return "slm";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(MemBufferUsage val) {
        switch(val) {
        case MemBufferUsage::private_space:
            return "private_space";
        case MemBufferUsage::spill_fill_space:
            return "spill_fill_space";
        case MemBufferUsage::single_space:
            return "single_space";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(ArgSamplerAddrMode val) {
        switch(val) {
        case ArgSamplerAddrMode::none:
            return "none";
        case ArgSamplerAddrMode::clamp_border:
            return "clamp_border";
        case ArgSamplerAddrMode::clamp_edge:
            return "clamp_edge";
        case ArgSamplerAddrMode::repeat:
            return "repeat";
        case ArgSamplerAddrMode::mirror:
            return "mirror";
        default:
            break;
        }
        return "";
    }
    static zeinfo_str_t get(ArgSamplerFilterMode val) {
        switch(val) {
        case ArgSamplerFilterMode::nearest:
            return "nearest";
        case ArgSamplerFilterMode::linear:
            return "linear";
        default:
            break;
        }
        return "";
    }
};
}
#endif
