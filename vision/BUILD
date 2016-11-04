package(default_visibility = ['//visibility:public'])

cc_library(
  name = 'target_spotter_lib',
  srcs = ['target_spotter.cc',],
  hdrs = ['target_spotter.h',],

  deps = [
    '//vision/util:shape_detector_lib',
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
