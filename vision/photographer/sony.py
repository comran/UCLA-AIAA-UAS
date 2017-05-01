from pysony import SonyAPI, payload_header
import time
import urllib
import os, glob
import subprocess

camera = SonyAPI()
camera.QX_ADDR = "http://192.168.122.1:8080"


directory = 0

if os.path.isdir("/home/pi/vision/captures") is False:
    os.makedirs("/home/pi/vision/captures")

while os.path.isdir("/home/pi/vision/captures/" + str(directory).zfill(5)):
    directory = directory + 1;
directory_string = "/home/pi/vision/captures/" + str(directory).zfill(5) 
os.makedirs(directory_string)

subprocess.Popen(["/home/pi/vision/image_stream", directory_string])

print "Starting to take pictures..."
i = 0

while 1:
#   if i % 2 == 0:
#       camera.actZoom(["out", "start"]);
#       camera.actZoom(["out", "start"]);
#       camera.actZoom(["out", "start"]);
#   else:
#       camera.actZoom(["in", "start"]);
#       camera.actZoom(["in", "start"]);
#       camera.actZoom(["in", "start"]);

#   time.sleep(5)
    url = camera.actTakePicture()['result'][0][0]
    url = url.replace("\/", "/")
    print url
    urllib.urlretrieve(url, "/home/pi/vision/captures/" + str(directory) + "/" + str(i) + ".jpg")
    i = i + 1;
