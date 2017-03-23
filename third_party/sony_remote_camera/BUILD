licenses(['notice'])

cc_library(
  name = 'sony_remote_camera',
  visibility = ['//visibility:public'],
  srcs = glob(['**/*.cpp']),
  hdrs = glob([
    '**/*.h',
  ]),
  includes = ['.'],
  copts = [
    '-Wno-cast-align',
    '-Wno-cast-qual',

    '-Wno-switch-enum',
    '-Wno-format-nonliteral',
    '-Wno-unused-parameter',
    '-Wno-ignored-attributes',
    '-Wno-reorder',
    '-Wno-unused-variable',
  ],
)
