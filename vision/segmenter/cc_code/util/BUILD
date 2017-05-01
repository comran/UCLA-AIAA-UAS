package(default_visibility = ['//visibility:public'])
load("/tools/build_rules/select", "cpu_select")

cc_library(
  name = 'shape_detector_lib',
  srcs = ['shape_detector.cc',],
  hdrs = ['shape_detector.h',],

  deps = cpu_select({
    "amd64": [
      '//external:opencv',
    ],
    "raspi": [
      '//external:opencv_raspi',
    ],
  }),
  copts = [
    '-Wno-cast-align',
  ],
)
