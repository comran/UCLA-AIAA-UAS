package(default_visibility = ['//visibility:public'])

cc_library(
  name = 'target_spotter_lib',
  hdrs = ['target_spotter.h',],

  deps = [
    '//vision/segmentation/cc_code/util:shape_detector_lib',
  ],
)

cc_binary(
  name = 'target_spotter',
  visibility = ['//visibility:public'],
  srcs = ['target_spotter.cc',],

  deps = [
    ':target_spotter_lib',
  ],
)

cc_library(
  name = 'output_to_file_lib',
  hdrs = ['output_to_file.h',],

  deps = [
    '//vision/segmentation/cc_code/util:shape_detector_lib',
  ],
)

cc_binary(
  name = 'output_to_file',
  visibility = ['//visibility:public'],
  srcs = ['output_to_file.cc',],

  deps = [
    ':output_to_file_lib',
  ],
)

cc_library(
  name = 'image_stream_lib',
  hdrs = ['image_stream.h',],

  deps = [
    '//vision/segmentation/cc_code/util:shape_detector_lib',
  ],
)

cc_binary(
  name = 'image_stream',
  visibility = ['//visibility:public'],
  srcs = ['image_stream.cc',],

  deps = [
    ':image_stream_lib',
  ],
)


