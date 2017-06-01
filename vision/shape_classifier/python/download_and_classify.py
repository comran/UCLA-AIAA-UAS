import classifier

import os
import sys
import thread
import time
from datetime import datetime
import subprocess
from collections import deque

def form_directory_structure():
    root = "./"
    if len(sys.argv) > 1:
        root = sys.argv[1]

    parent_directory = root + "captures"
    if os.path.isdir(parent_directory) is False:
        os.makedirs(parent_directory)
        print "Created captures directory at " + parent_directory

    return parent_directory

def add_raw_output_to_deque(raw, segment_deque):
    new_files = raw.splitlines()

    for new_file in new_files:
        if new_file.find("captures") != -1 and new_file.find(".jpg") != -1:
            segment_deque.append(new_file)
            print new_file


def main():
    remote = "pi@192.168.1.2"
    remote_captures_directory = '/home/pi/vision/captures/segments/'

    os.system("ssh-add ../../python/ucla-aiaa-uas-raspi")
    segments = deque()
    captures_path = form_directory_structure()

    segment_classifier = classifier.Classifier()
    thread.start_new_thread(segment_classifier.use_deque_stream, ( \
            segments, False, ))

    proc = subprocess.Popen("ssh " + remote + " find " + \
            remote_captures_directory, \
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    new_files_raw = proc.stdout.read()
    new_files_raw = new_files_raw.replace( \
            "/home/pi/vision/captures/segments/", "captures/")
    print new_files_raw
    add_raw_output_to_deque(new_files_raw, segments)

    while True:
        time.sleep(1)
        print str(datetime.now()) + " Attempting sync..."

        proc = subprocess.Popen("rsync -avz --partial " \
                + "--out-format='captures/%n' " + remote + ":" + \
                remote_captures_directory + " ./captures", \
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)

        new_files_raw = proc.stdout.read()
        add_raw_output_to_deque(new_files_raw, segments)

if __name__ == "__main__":
    main()
