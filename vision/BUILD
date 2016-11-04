package(default_visibility = ['//visibility:public'])

cc_library(
  name = 'shape_detector_lib',
  srcs = ['shape_detector.cc',],
  hdrs = ['shape_detector.h',],

  deps = [
    '//external:opencv'
  ],
  copts = [
    '-Wno-cast-align',
  ],
)

cc_library(
  name = 'target_spotter_lib',
  srcs = ['target_spotter.cc',],
  hdrs = ['target_spotter.h',],

  deps = [
    ':shape_detector_lib',
  ],
)

cc_binary(
  name = 'target_spotter',
  visibility = ['//visibility:public'],
  srcs = ['target_spotter_main.cc',],

  deps = [
    ':target_spotter_lib',
  ],
)
