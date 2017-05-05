import numpy as np
from matplotlib import pyplot as plt
import cv
import cv2
import sys
import time
import os

class Segmenter:
    def cut_from_image(self, frame, padding, x, y, w, h):
        x_min = max(0, x - padding)
        x_max = min(np.size(frame, 1), x + w + padding * 2)
        y_min = max(0, y - padding)
        y_max = min(np.size(frame, 1), y + h + padding * 2)

        return frame[y_min:y_max, x_min:x_max]

    def process_frame(self, original_frame):
        bounding_box_highlight_frame = original_frame.copy()
        frame = original_frame.copy()

        # Apply various filters to the image to make the targets stand out.
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        frame = cv2.bitwise_not(frame)
        blur = cv2.GaussianBlur(frame, (15, 15), 0)
        thresh = cv2.adaptiveThreshold(blur, 255,
                 cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 11, 1)

        # Generate a list of contours that stand out from the image after the
        # above filters were applied.
        contours, hierarchy = cv2.findContours(thresh, cv2.RETR_EXTERNAL,
            cv2.CHAIN_APPROX_SIMPLE)

        possible_shapes = list()
        i = 0
        for cnt in contours:
            if len(cnt) < 5:
                continue

            x, y, w, h = cv2.boundingRect(cnt)
            area = w * h
            aspect_ratio = w / float(h)
            if area < 200 or area > 1000:
                continue

            aspect_ratio_tolerance = 0.1
            if abs(aspect_ratio - 1) > aspect_ratio_tolerance:
                continue

            print "Image " + str(i)

            extracted_contour = self.cut_from_image(original_frame, 10, x, y, \
                    w, h)
            possible_shapes.append(extracted_contour)

            cv2.rectangle(bounding_box_highlight_frame, (x, y), \
                    (x + w, y + h), (255, 0, 0), 1)

            cv2.imshow("Contour Locations", bounding_box_highlight_frame)
            cv2.moveWindow("Contour Locations", 300, 0)
            cv2.imshow("contour", extracted_contour)

            img = extracted_contour
            h = np.zeros((300,256,3))
            bins = np.arange(256).reshape(256,1)
            color = [ (255,0,0),(0,255,0),(0,0,255) ]

            histogram_reject = True
            for ch, col in enumerate(color):
                hist_item = cv2.calcHist([img],[ch],None,[256],[0,255])
#               cv2.normalize(hist_item,hist_item,0,255,cv2.NORM_MINMAX)
                hist=np.int32(np.around(hist_item))
                print hist_item.max()
                if hist_item.max() > 100:
                    histogram_reject = False

                pts = np.column_stack((bins,hist))
                cv2.polylines(h,[pts],False,col)

            if histogram_reject is False:
                print "Good contour!"
                i = i + 1

            h = np.flipud(h)

            cv2.imshow('colorhist',h)
            cv2.moveWindow("colorhist", 80, 0)
            cv2.waitKey(0)

        print str(len(possible_shapes)) \
                + " good contours were found! (" \
                + str(len(contours)) \
                + " were filtered out)"

        return possible_shapes

    def use_webcam_stream(self):
        cap = cv2.VideoCapture(0)
        cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 1280)
        cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 720)

        while True:
            ret, frame = cap.read()
            process_frame(frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        cap.release()
        cv2.destroyAllWindows()

    def use_given_image(self):
        frame = cv2.imread(sys.argv[1], cv2.CV_LOAD_IMAGE_COLOR)
        if frame is None:
            print "Image given has no data to analyze."
            sys.exit(1)
        grey_scale = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        self.process_frame(frame);
        cv2.waitKey(0)
        cv2.destroyAllWindows()

    def use_deque_stream(self, save_directory, photos):
        while True:
            if len(photos) == 0:
                print "No photos available to segment."
                time.sleep(1)
                continue

            raw_image_path = photos.pop()
            frame = cv2.imread(raw_image_path, cv2.CV_LOAD_IMAGE_COLOR)
            grey_scale = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            possible_shapes = self.process_frame(frame);

            # Make a new folder just for this image's segments.
            image_folder_name = os.path.basename(raw_image_path)
            image_folder_name = image_folder_name.replace(".jpg", "")
            image_folder_name = save_directory + "/" + image_folder_name
            print image_folder_name
            if os.path.isdir(image_folder_name) is True:
                print "Segments directory out of sync."
                sys.exit(1)
            os.makedirs(image_folder_name)

            # Output the images.
            i = 0
            for shape in possible_shapes:
                image_path = image_folder_name + "/" + str(i).zfill(5) + ".jpg"
                cv2.imwrite(image_path, shape)
                i = i + 1

def main():
    if len(sys.argv) > 1:
        segmenter = Segmenter()
        segmenter.use_given_image()

if __name__ == "__main__":
    main()
