package(default_visibility = ['//visibility:public'])

cc_library(
  name = 'license_lib',
  visibility = ['//visibility:public'],
  hdrs = [
    'util/license.h',
  ],
)

cc_binary(
  name = 'license',
  visibility = ['//visibility:public'],
  srcs = [
    'license.cpp',
  ],

  deps = [
    ':license_lib',
  ],
)
