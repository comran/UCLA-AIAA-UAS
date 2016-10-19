#include "opencv/cv.h"
#include "opencv2/highgui/highgui_c.h"

int main()
{
    int c;
    IplImage* color_img;
    CvCapture* cv_cap = cvCaptureFromCAM(-1); // -1 = only one cam or doesn't matter
    cvNamedWindow("Video",1); // create window
    for(;;) {
        color_img = cvQueryFrame(cv_cap); // get frame
        if(color_img != 0)
            cvShowImage("Video", color_img); // show frame
        c = cvWaitKey(10); // wait 10 ms or for key stroke
        if(c == 27)
            break; // if ESC, break and quit
    }
    cvReleaseCapture( &cv_cap );
    cvDestroyWindow("Video");

    return 0;
}
