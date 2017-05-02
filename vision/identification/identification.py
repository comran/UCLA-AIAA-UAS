import photographer

import sys
import os

def form_directory_structure():
    # Create a neat directory structure to store all of the image that our
    # image identification system will generate.
    root = "./"
    if len(sys.argv) > 1:
        root = sys.argv[1]

    parent_directory = root + "captures"
    if os.path.isdir(parent_directory) is False:
        os.makedirs(parent_directory)
        print "Created captures directory at " + parent_directory

    raw_directory = parent_directory + "/raw"
    if os.path.isdir(raw_directory) is False:
        os.makedirs(raw_directory)
        print "Created raw image directory at " + raw_directory

    segments_directory = parent_directory + "/segments"
    if os.path.isdir(segments_directory) is False:
        os.makedirs(segments_directory)
        print "Created segment image directory at " + segments_directory

    directory = 0
    while os.path.isdir(raw_directory + "/" + str(directory).zfill(5)):
        directory = directory + 1;
    directory_string = raw_directory + "/" + str(directory).zfill(5)
    print "Will store raw captured images in " + directory_string
    os.makedirs(directory_string)
    return directory_string

def main():
    camera = photographer.Photographer()

    form_directory_structure()
    camera.take_pictures()

if __name__ == "__main__":
    main()
