package(default_visibility = ['//visibility:public'])

load('/tools/build_rules/select', 'cpu_select', 'compiler_select')

cc_binary(
  name = 'original',
  visibility = ['//visibility:public'],
  srcs = [
    'original.cc',
  ],
  deps = cpu_select({
    'amd64': [
      '//external:opencv'
    ],
    'raspi': [
      '//external:opencv'
    ],
  }),
)

cc_binary(
  name = 'color_tracking',
  visibility = ['//visibility:public'],
  srcs = [
    'color_tracking.cc',
  ],
  deps = cpu_select({
    'amd64': [
      '//external:opencv'
    ],
    'raspi': [
      '//external:opencv'
    ],
  }),
)

cc_binary(
  name = 'shape_detection',
  visibility = ['//visibility:public'],
  srcs = [
    'shape_detection.cc',
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
  name = 'streaming_shape_detection',
  visibility = ['//visibility:public'],
  srcs = [
    'streaming_shape_detection.cc',
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
