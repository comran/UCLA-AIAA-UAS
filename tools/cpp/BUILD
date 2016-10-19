package(default_visibility = ['//visibility:public'])

cc_library(
  name = 'empty_main',
  srcs = [ 'empty_main.c' ],
)

#cc_library(
#  name = 'malloc',
#  deps = select({
#    '//tools:has_asan': [],
#    '//tools:has_tsan': [],
#    '//conditions:default': ['//third_party/gperftools:tcmalloc'],
#  }),
#)

cc_library(
  name = 'stl',
)

filegroup(
  name = 'empty',
  srcs = [],
)

# This is the entry point for --crosstool_top.  Toolchains are found
# by lopping off the name of --crosstool_top and searching for
# 'cc-compiler-${CPU}' in this BUILD file, where CPU is the target CPU
# specified in --cpu.
#
# This file group should include
#   * all cc_toolchain targets supported
#   * all file groups that said cc_toolchain might refer to,
# including the default_grte_top setting in the CROSSTOOL
# protobuf.
filegroup(
  name = 'toolchain',
  srcs = [
    ':cc-compiler-k8',
    ':cc-compiler-raspi',
    ':flags_compiler_inputs',
    '@gcc_linaro_arm_linux_gnueabihf_raspbian_repo//:compiler_components',
    ':linaro-gcc-files',
  ],
)

# Compiler inputs given by --copt etc in //tools:bazel.rc.
filegroup(
  name = 'flags_compiler_inputs',
  srcs = select({
    '//tools:has_asan': [
      'asan-blacklist',
    ],
    '//tools:has_ubsan': [
      'ubsan-blacklist',
    ],
    '//conditions:default': [],
  }),
)

cc_toolchain(
  name = 'cc-compiler-k8',
  all_files = ':flags_compiler_inputs',
  compiler_files = ':flags_compiler_inputs',
  cpu = 'local',
  dwp_files = ':empty',
  dynamic_runtime_libs = [':empty'],
  linker_files = ':empty',
  objcopy_files = ':empty',
  static_runtime_libs = [':empty'],
  strip_files = ':empty',
  supports_param_files = 1,
)

filegroup(
  name = 'raspi-gcc-files',
  srcs = [
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:clang-symlinks',
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:tool-wrappers',
    '@gcc_linaro_arm_linux_gnueabihf_raspbian_repo//:compiler_pieces',
  ],
)

filegroup(
  name = 'linaro_linux_linker_files',
  srcs = [
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:gcc',
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:ld',
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:ar',
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:clang-ld',
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:clang',
    '@gcc_linaro_arm_linux_gnueabihf_raspbian_repo//:compiler_pieces',
  ],
)

filegroup(
  name = 'linaro_linux_compiler_files',
  srcs = [
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:gcc',
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:ld',
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:clang',
    '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:as',
  ],
)

cc_toolchain(
  name = 'cc-compiler-raspi',
  all_files = ':raspi-gcc-files',
  compiler_files = ':linaro_linux_compiler_files',
  cpu = 'raspi',
  dwp_files = ':empty',
  dynamic_runtime_libs = [':empty'],
  linker_files = ':linaro_linux_linker_files',
  objcopy_files = '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:objcopy',
  static_runtime_libs = [':empty'],
  strip_files = '//tools/cpp/gcc_linaro_arm_linux_gnueabihf_raspbian:strip',
  supports_param_files = 1,
)
