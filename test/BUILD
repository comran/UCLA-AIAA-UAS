package(default_visibility = ['//visibility:public'])
load("/tools/build_rules/select", "cpu_select")

cc_binary(
  name = 'opencv_test',
  visibility = ['//visibility:public'],
  srcs = [
    'opencv_test.cc',
  ],

  deps = cpu_select({
    "amd64": [
      '//external:opencv',
    ],
    "raspi": [
      '//external:opencv_raspi',
    ],
  }),
)

cc_binary(
  name = 'opencv_test2',
  visibility = ['//visibility:public'],
  srcs = [
    'opencv_test2.cc',
  ],

  deps = cpu_select({
    "amd64": [
      '//external:opencv',
    ],
    "raspi": [
      '//external:opencv_raspi',
    ],
  }),
)
