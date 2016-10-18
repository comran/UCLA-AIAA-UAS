licenses(['restricted'])

cc_library(
    name = "opencv",
    srcs = glob(["lib/*.so*"]),
    hdrs = glob([
        "include/**/*.hpp",
        "include/**/*.h",
        "modules/**/*.hpp",
        "modules/core/include/**/*.h",
        "modules/imgproc/include/**/*.h",
        "modules/highgui/include/**/*.h",
        "modules/imgcodecs/include/**/*.h",
        "modules/videoio/include/**/*.h",
        "modules/photo/include/**/*.h",
        "modules/video/include/**/*.h",
        "modules/objdetect/include/**/*.h",
    ]),
    includes = [
        "include",
        "modules/core/include",
        "modules/imgproc/include",
        "modules/highgui/include",
        "modules/imgcodecs/include",
        "modules/videoio/include",
        "modules/photo/include",
        "modules/video/include",
        "modules/objdetect/include",
    ],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)
