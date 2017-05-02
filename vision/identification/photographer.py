from pysony import SonyAPI, payload_header
import time
import urllib
import os, glob
import subprocess
import sys

class Photographer:
    def take_pictures(self):
        # Take pictures forever.
        camera = SonyAPI()
        camera.QX_ADDR = "http://192.168.122.1:8080"

        # TODO(comran): Try/catch the below command to detect if we are actually
        #               connected to the camera.
        camera.actTakePicture()

        print "Starting to take pictures..."
        i = 0
        while 1:
            url = camera.actTakePicture()['result'][0][0]
            url = url.replace("\/", "/")
            url = url.replace("Scn", "Origin")

            captured_image_path = directory_string + "/" + str(i) + ".jpg"
            urllib.urlretrieve(url, captured_image_path)

            i = i + 1;
            print "Picture #" + str(i) + " stored at " + captured_image_path
