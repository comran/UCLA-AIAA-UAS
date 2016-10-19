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
