package(default_visibility = ['//visibility:public'])

load('/tools/build_rules/select', 'cpu_select', 'compiler_select')

cc_library(
  name = 'shape_detector_lib',
  srcs = [
    'shape_detector.cc',
  ],
  hdrs = [
    'shape_detector.h',
  ],
  deps = cpu_select({
    'amd64': [
      '//external:opencv'
    ],
    'raspi': [
      '//external:opencv'
    ],
  }),
  copts = [
    '-Wno-cast-align',
  ],
)

cc_binary(
  name = 'target_spotter',
  visibility = ['//visibility:public'],
  srcs = [
    'target_spotter.cc',
  ],
  deps = [
    ':shape_detector_lib',
  ],
)
