import numpy as np
import cv2
import sys

def cut_from_image(frame, padding, x, y, w, h):
    x_min = max(0, x - padding)
    x_max = min(np.size(frame, 1), x + w + padding * 2)
    y_min = max(0, y - padding)
    y_max = min(np.size(frame, 1), y + h + padding * 2)

    return frame[y_min:y_max, x_min:x_max]

def process_frame(original_frame):
    bounding_box_highlight_frame = original_frame.copy()
    frame = original_frame.copy()

    # Apply various filters to the image to make the targets stand out.
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    frame = cv2.bitwise_not(frame)
    blur = cv2.GaussianBlur(frame, (15, 15), 0)
    thresh = cv2.adaptiveThreshold(blur, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
             cv2.THRESH_BINARY_INV, 11, 1)

    # Generate a list of contours that stand out from the image after the above
    # filters were applied.
    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE)

    possible_shapes = list()
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

        extracted_contour = cut_from_image(original_frame, 10, x, y, w, h)
        possible_shapes.append(extracted_contour)

        cv2.rectangle(bounding_box_highlight_frame, (x, y), (x + w, y + h), \
                (255, 0, 0), 1)
        cv2.imshow("Contour Locations", bounding_box_highlight_frame)

    print str(len(possible_shapes)) + " good contours were found! (" \
            + str(len(contours)) + " were filtered out)"

def use_webcam_stream():
    cap = cv2.VideoCapture(0)
    cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 720)

    while(True):
        ret, frame = cap.read()
        process_frame(frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

def use_given_image():
    frame = cv2.imread(sys.argv[1], cv2.CV_LOAD_IMAGE_COLOR)
    if frame is None:
        print "Image given has no data to analyze."
        sys.exit(1)

    grey_scale = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    process_frame(frame);
    cv2.waitKey(0)
    cv2.destroyAllWindows()

def main():
    if len(sys.argv) < 2:
        # No arguments given, so feed in sample data from the webcam stream.
        use_webcam_stream()
    else:
        # Use the first image path given as an argument to the program.
        use_given_image()

if __name__ == "__main__":
    main()
