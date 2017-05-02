from pysony import SonyAPI, payload_header
import time
import urllib
import os, glob
import subprocess
import sys

def main():
    # Create a neat directory structure to store all of the image that our
    # image identification system will generate.
    root = ""
    if len(sys.argv) > 1:
        root = sys.argv[1]

    parent_directory = root + "captures"
    if os.path.isdir(parent_directory) is False:
        os.makedirs(parent_directory)

    raw_directory = parent_directory + "/raw"
    if os.path.isdir(raw_directory) is False:
        os.makedirs(raw_directory)

    segments_directory = parent_directory + "/segments"
    if os.path.isdir(segments_directory) is False:
        os.makedirs(segments_directory)

    directory = 0
    while os.path.isdir(raw_directory + "/" + str(directory).zfill(5)):
        directory = directory + 1;

    directory_string = raw_directory + "/" + str(directory).zfill(5)
    os.makedirs(directory_string)

    # Take pictures forever.
    camera = SonyAPI()
    camera.QX_ADDR = "http://192.168.122.1:8080"

#   print "Starting to take pictures..."
#   i = 0

#   while 1:
#       url = camera.actTakePicture()['result'][0][0]
#       url = url.replace("\/", "/")
#       print url
#       urllib.urlretrieve(url, "/home/pi/vision/captures/" + str(directory) \
#               + "/" + str(i) + ".jpg")
#       i = i + 1;

if __name__ == "__main__":
    main()
