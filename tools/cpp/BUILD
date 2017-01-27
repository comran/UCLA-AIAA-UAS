package(default_visibility = ['//visibility:public'])

cc_library(
  name = 'empty_main',
  srcs = [ 'empty_main.c' ],
)

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
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:clang-symlinks',
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:tool-wrappers',
    '@gcc_linaro_arm_linux_gnueabihf_raspbian_repo//:compiler_pieces',
  ],
)

filegroup(
  name = 'linaro_linux_linker_files',
  srcs = [
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:gcc',
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:ld',
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:ar',
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:clang-ld',
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:clang',
    '@gcc_linaro_arm_linux_gnueabihf_raspbian_repo//:compiler_pieces',
  ],
)

filegroup(
  name = 'linaro_linux_compiler_files',
  srcs = [
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:gcc',
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:ld',
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:clang',
    '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:as',
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
  objcopy_files = '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:objcopy',
  static_runtime_libs = [':empty'],
  strip_files = '//tools/cpp/arm-rpi-4.9.3-linux-gnueabihf:strip',
  supports_param_files = 1,
)
