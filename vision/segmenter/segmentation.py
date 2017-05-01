import numpy as np
import cv2
import sys

def process_frame(original_frame):
    frame = original_frame.copy()

    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    frame = cv2.bitwise_not(frame)
    blur = cv2.GaussianBlur(frame, (15, 15), 0)
    thresh = cv2.adaptiveThreshold(blur, 255,
            cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 11, 1)

    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE)

    good_contours = 0
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

        cv2.rectangle(original_frame, (x, y), (x + w, y + h), \
                (0, 255, 0), 2)
        cv2.rectangle(frame, (x, y), (x + w, y + h), \
                (0, 0, 0), 2)

        good_contours += 1

    print str(good_contours) + " good contours were found (" \
            + str(len(contours)) + " were filtered out)"

    cv2.imshow("Original Frame", original_frame)
    cv2.imshow("Adaptive Thresholding", thresh)
    cv2.imshow('Contours', frame)

def use_webcam_stream():

    cap = cv2.VideoCapture(0)
    cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 720)
    while(True):
        ret, frame = cap.read()
        cv2.imshow('image', frame)
        process_frame(frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cap.release()

def use_given_image():
    frame = cv2.imread(sys.argv[1], cv2.CV_LOAD_IMAGE_COLOR)
    cv2.imshow('image', frame)
    grey_scale = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    process_frame(frame);
    cv2.waitKey(0)

def run_main():
    if len(sys.argv) < 2:
        use_webcam_stream()
    else:
        use_given_image()

    cv2.destroyAllWindows()

if __name__ == "__main__":
    run_main()
