load("@bazel_skylib//rules:build_test.bzl", "build_test")
load("@llvm-project//mlir:tblgen.bzl", "gentbl_cc_library")
load("//tensorflow:tensorflow.bzl", "tf_cc_test")
load("//tensorflow:tensorflow.default.bzl", "get_compatible_with_portable")

# MLIR passes for DTensor support.
load("//tensorflow/core/platform:rules_cc.bzl", "cc_library")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = [
        "//tensorflow/dtensor:dtensor-internal",
        # Allow visibility from the mlir language server.
        "//learning/brain/mlir/mlir_lsp_server:__pkg__",
        "//third_party/py/tensorflow_addons/custom_ops/dtensor:__subpackages__",
    ],
    licenses = ["notice"],
)

gentbl_cc_library(
    name = "tensorflow_dtensor_ops_inc_gen",
    compatible_with = get_compatible_with_portable(),
    tbl_outs = {
        "ir/tf_dtensor.h.inc": ["-gen-op-decls"],
        "ir/tf_dtensor.cc.inc": ["-gen-op-defs"],
    },
    tblgen = "@llvm-project//mlir:mlir-tblgen",
    td_file = "ir/tf_dtensor.td",
    td_srcs = [
        "//tensorflow/compiler/mlir/tensorflow:ir/tf_op_base.td",
        "//tensorflow/compiler/mlir/tensorflow:ir/tf_op_interfaces.td",
    ],
    deps = [
        "@llvm-project//mlir:CallInterfacesTdFiles",
        "@llvm-project//mlir:FuncTdFiles",
        "@llvm-project//mlir:InferTypeOpInterfaceTdFiles",
        "@llvm-project//mlir:OpBaseTdFiles",
        "@llvm-project//mlir:SideEffectInterfacesTdFiles",
    ],
)

gentbl_cc_library(
    name = "dtensor_passes_inc_gen",
    compatible_with = get_compatible_with_portable(),
    tbl_outs = {"dtensor_passes.h.inc": [
        "-gen-pass-decls",
        "-name=DTensor",
    ]},
    tblgen = "@llvm-project//mlir:mlir-tblgen",
    td_file = "Passes.td",
    deps = ["@llvm-project//mlir:PassBaseTdFiles"],
)

cc_library(
    name = "tf_dtensor_dialect",
    srcs = ["ir/tf_dtensor.cc"],
    hdrs = ["ir/tf_dtensor.h"],
    includes = ["include"],
    deps = [
        ":tensorflow_dtensor_ops_inc_gen",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_attributes",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_op_interfaces",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_side_effects",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_traits",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_types",
        "//tensorflow/dtensor/mlir/dtensor_dialect:ir/dtensor_attributes",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:DerivedAttributeOpInterface",
        "@llvm-project//mlir:Dialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:InferTypeOpInterface",
        "@llvm-project//mlir:SideEffectInterfaces",
        "@llvm-project//mlir:Support",
    ],
    alwayslink = 1,
)

cc_library(
    name = "collectives",
    srcs = ["collectives.cc"],
    hdrs = ["collectives.h"],
    deps = [
        ":collectives_common",
        ":dtensor_location",
        ":layout_parsing",
        ":shape_utils",
        ":sparse_expander_common",
        ":spmd_expander_common",
        ":tf_dtensor_dialect",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow/transforms:tensorflow_passes",
        "//tensorflow/core:lib",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:dtensor_utils",
        "//tensorflow/dtensor/cc:tensor_layout",
        "//tensorflow/dtensor/mlir/dtensor_dialect:ir/dtensor_attributes",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
)

cc_library(
    name = "collectives_common",
    srcs = ["collectives_common.cc"],
    hdrs = ["collectives_common.h"],
    deps = [
        "//tensorflow/core:portable_gif_internal",
        "//tensorflow/core/platform:errors",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:tensor_layout",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_library(
    name = "device_utils",
    srcs = ["device_utils.cc"],
    hdrs = ["device_utils.h"],
    deps = [
        "//tensorflow/core/platform:errors",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:tensor_layout",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
    alwayslink = True,
)

cc_library(
    name = "dtensor_location",
    srcs = ["dtensor_location.cc"],
    hdrs = ["dtensor_location.h"],
    deps = [
        "//tensorflow/compiler/mlir/utils:name_utils",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
)

cc_library(
    name = "create_dtensor_mlir_passes",
    hdrs = [
        "create_dtensor_mlir_passes.h",
    ],
    deps = [
        ":device_utils",
        ":dtensor_passes_inc_gen",
        ":dtensor_send_recv",
        ":layout_parsing",
        ":op_utils",
        ":shape_utils",
        ":sparse_expander",
        ":spmd_expander",
        ":spmd_expander_common",
        ":tf_dtensor_dialect",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow/transforms:tensorflow_passes",
        "//tensorflow/core:core_cpu_base",
        "//tensorflow/core:lib",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Pass",
    ],
    alwayslink = 1,
)

cc_library(
    name = "dtensor_mlir_passes",
    srcs = [
        "annotate_global_shape.cc",
        "cluster_function_conversion.cc",
        "constant_folding.cc",
        "dce.cc",
        "designate_resource_handle_mesh.cc",
        "device_mesh_cluster_coarsening.cc",
        "dtensor_allreduce_combine_optimization.cc",
        "dtensor_allreduce_scatter_optimization.cc",
        "dtensor_allreduce_sum_optimization.cc",
        "dtensor_collective_type_lowering.cc",
        "dtensor_layout_to_xla_sharding_op.cc",
        "dtensor_mixed_precision_reduce.cc",
        "dtensor_mlir_passes.cc",
        "dtensor_multi_device_expansion.cc",
        "dtensor_remove_dtensorlayout.cc",
        "dtensor_replace_auxiliary_layout_op.cc",
        "dtensor_replace_relayout_with_identity.cc",
        "dtensor_set_hlo_sharding.cc",
        "function_renaming.cc",
        "handle_cross_cluster_dependencies.cc",
        "handle_sparsetensors.cc",
        "layout_propagation_v2.cc",
        "lower_send_recv.cc",
        "merge_clusters.cc",
        "mesh_propagation.cc",
        "move_compilation_to_host.cc",
        "op_to_device_cluster.cc",
        "propagate_default_layout.cc",
        "propagate_device_id_to_function_args.cc",
        "restore_shape_inference.cc",
        "set_default_sharding.cc",
        "sparse_expansion.cc",
        "spmd_expansion.cc",
        "tpu_add_resource_device_attribute.cc",
        "tpu_integration.cc",
        "undo_merge_const_across_mesh.cc",
    ],
    hdrs = ["dtensor_mlir_passes.h"],
    deps = [
        ":collectives_common",
        ":create_dtensor_mlir_passes",
        ":device_utils",
        ":dtensor_location",
        ":dtensor_passes_inc_gen",
        ":dtensor_send_recv",
        ":layout_parsing",
        ":op_utils",
        ":shape_utils",
        ":sparse_expander",
        ":spmd_expander",
        ":spmd_expander_common",
        ":tf_dtensor_dialect",
        ":topological_iterator",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:attribute_utils",
        "//tensorflow/compiler/mlir/tensorflow:bridge_logger",
        "//tensorflow/compiler/mlir/tensorflow:convert_tensor",
        "//tensorflow/compiler/mlir/tensorflow:dump_mlir_util",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_analysis",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_attributes",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_ops",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_types",
        "//tensorflow/compiler/mlir/tensorflow:tpu_rewrite_device_util",
        "//tensorflow/compiler/mlir/tensorflow/transforms:tensorflow_passes",
        "//tensorflow/compiler/mlir/tensorflow/transforms/host_runtime:runtime_passes",
        "//tensorflow/compiler/mlir/utils:name_utils",
        "//tensorflow/core:core_cpu_base",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/dtensor/cc:constants",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:dtensor_utils",
        "//tensorflow/dtensor/cc:layout_to_xla_sharding",
        "//tensorflow/dtensor/cc:tensor_layout",
        "//tensorflow/dtensor/mlir/dtensor_dialect:ir/dtensor_attributes",
        "//tensorflow/dtensor/mlir/utils:dtensor_mlir_passes_internal",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:Analysis",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:FunctionInterfaces",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Pass",
        "@llvm-project//mlir:SideEffectInterfaces",
        "@llvm-project//mlir:SparseTensorDialect",
        "@llvm-project//mlir:Support",
        "@llvm-project//mlir:TransformUtils",
        "@llvm-project//mlir:Transforms",
        "@local_xla//xla:xla_data_proto_cc",
        "@local_xla//xla/hlo/builder:sharding_builder",
        "@local_xla//xla/tsl/platform:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "dtensor_send_recv",
    srcs = ["dtensor_send_recv.cc"],
    hdrs = ["dtensor_send_recv.h"],
    deps = [
        ":collectives",
        ":device_utils",
        ":layout_parsing",
        ":op_utils",
        ":shape_utils",
        ":spmd_expander_common",
        ":tf_dtensor_dialect",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:convert_tensor",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_attributes",
        "//tensorflow/compiler/mlir/tensorflow/transforms:tensorflow_passes",
        "//tensorflow/core/platform:errors",
        "//tensorflow/dtensor/cc:constants",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:tensor_layout",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:span",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
    alwayslink = True,
)

cc_library(
    name = "layout_parsing",
    srcs = [
        "layout_parsing.cc",
    ],
    hdrs = ["layout_parsing.h"],
    deps = [
        ":tf_dtensor_dialect",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/core:lib",
        "//tensorflow/dtensor/cc:constants",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:tensor_layout",
        "//tensorflow/dtensor/proto:layout_proto_cc",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
    alwayslink = 1,
)

cc_library(
    name = "op_utils",
    srcs = ["op_utils.cc"],
    hdrs = ["op_utils.h"],
    deps = [
        ":tf_dtensor_dialect",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/dtensor/cc:constants",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:CallOpInterfaces",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
    alwayslink = True,
)

cc_library(
    name = "shape_utils",
    srcs = ["shape_utils.cc"],
    hdrs = ["shape_utils.h"],
    deps = [
        ":tf_dtensor_dialect",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow:shape_inference_utils",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_attributes",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_types",
        "//tensorflow/core:framework",
        "//tensorflow/core/platform:errors",
        "//tensorflow/core/platform:status",
        "//tensorflow/dtensor/cc:constants",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:tensor_layout",
        "@com_google_absl//absl/status",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:DerivedAttributeOpInterface",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:InferTypeOpInterface",
        "@llvm-project//mlir:Support",
    ],
    alwayslink = True,
)

cc_library(
    name = "sparse_expander",
    srcs = [
        "sparse_expanders.cc",
    ] + glob([
        "*sparse_expander.cc",
        "sparse_expansions/*sparse_expander.cc",
    ]),
    hdrs = glob([
        "*sparse_expander.h",
        "sparse_expansions/*sparse_expander.h",
    ]),
    deps = [
        ":op_utils",
        ":sparse_expander_common",
        ":tf_dtensor_dialect",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_ops",
        "//tensorflow/compiler/mlir/tensorflow/transforms:tensorflow_passes",
        "//tensorflow/core:framework",
        "//tensorflow/core/platform:errors",
        "//tensorflow/core/platform:status",
        "//tensorflow/dtensor/cc:dstatus",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
    alwayslink = 1,
)

cc_library(
    name = "sparse_expander_common",
    srcs = ["sparse_expander_common.cc"],
    hdrs = ["sparse_expander_common.h"],
    deps = [
        ":tf_dtensor_dialect",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_ops",
        "//tensorflow/core/platform:errors",
        "//tensorflow/dtensor/cc:dstatus",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/types:optional",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:BytecodeOpInterface",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
    ],
    alwayslink = True,
)

cc_library(
    name = "spmd_expander_common",
    srcs = ["spmd_expander_common.cc"],
    hdrs = ["spmd_expander_common.h"],
    deps = [
        ":device_utils",
        ":layout_parsing",
        ":op_utils",
        ":shape_utils",
        ":tf_dtensor_dialect",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_ops",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_types",
        "//tensorflow/core:lib",
        "//tensorflow/dtensor/cc:constants",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:tensor_layout",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:Dialect",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
    alwayslink = True,
)

cc_library(
    name = "spmd_expander",
    srcs = [
        "spmd_expanders.cc",
    ] + glob([
        "*spmd_expander.cc",
        "expansions/*spmd_expander.cc",
        "expansions/*spmd_expander_v2.cc",
    ]),
    hdrs = glob([
        "*spmd_expander.h",
        "expansions/*spmd_expander.h",
        "expansions/*spmd_expander_v2.h",
    ]),
    deps = [
        ":collectives",
        ":device_utils",
        ":dtensor_location",
        ":dtensor_send_recv",
        ":layout_parsing",
        ":op_utils",
        ":shape_utils",
        ":spmd_expander_common",
        ":tf_dtensor_dialect",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_attributes",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_ops",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_types",
        "//tensorflow/compiler/mlir/tensorflow/transforms:tensorflow_passes",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/dtensor/cc:constants",
        "//tensorflow/dtensor/cc:dstatus",
        "//tensorflow/dtensor/cc:dtensor_utils",
        "//tensorflow/dtensor/cc:save_restore_util",
        "//tensorflow/dtensor/cc:slice_util",
        "//tensorflow/dtensor/cc:tensor_layout",
        "//tensorflow/dtensor/mlir/dtensor_dialect:ir/dtensor_attributes",
        "//tensorflow/dtensor/proto:layout_proto_cc",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/numeric:bits",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:BytecodeOpInterface",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
        "@local_xla//xla/mlir_hlo:convert_op_folder",
    ],
    alwayslink = 1,
)

cc_library(
    name = "value_utils",
    srcs = ["value_utils.cc"],
    hdrs = ["value_utils.h"],
    deps = [
        ":op_utils",
        ":tf_dtensor_dialect",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:dynamic_shape_utils",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_types",
        "//tensorflow/compiler/mlir/tensorflow/transforms:tensorflow_passes",
        "//tensorflow/core:lib",
        "//tensorflow/dtensor/cc:dstatus",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
        "@local_xla//xla/tsl/protobuf:error_codes_proto_impl_cc",
    ],
    alwayslink = True,
)

cc_library(
    name = "topological_iterator",
    srcs = ["topological_iterator.cc"],
    hdrs = ["topological_iterator.h"],
    deps = [
        ":device_utils",
        ":layout_parsing",
        ":op_utils",
        ":shape_utils",
        ":tf_dtensor_dialect",
        ":value_utils",
        "//tensorflow/compiler/mlir/tensorflow",
        "//tensorflow/compiler/mlir/tensorflow:tensorflow_ops",
        "@llvm-project//llvm:Support",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
    ],
)

tf_cc_test(
    name = "dtensor_location_test",
    srcs = ["dtensor_location_test.cc"],
    deps = [
        ":dtensor_location",
        "//tensorflow/compiler/mlir/utils:name_utils",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Support",
    ],
)

build_test(
    name = "mlir_build_test",
    targets = [
        ":tf_dtensor_dialect",
        ":tensorflow_dtensor_ops_inc_gen",
    ],
)
