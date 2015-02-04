#include <iostream>

#include "Detect.h"
#include "Preprocess.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

bool makeMovies = FALSE;

/**
 * gets the background of the video
 *
 * this runs first our procedure
 * the function itself is implemented using a simple rolling average
 */

Mat getBackground(VideoCapture& cap){
    // initailize the count, and the frames
    int i = 1;
    Mat frame, frame2, sum;

    cap >> frame;
    frame2 = frame;

    // the main loop (termiantes when a key is pressed);
    while (true) {
        cap >> frame;
        if (i > 1) {
            frame2 = sum;
        }

        double beta = 1.0 / (i + 1);
        double alpha = 1.0 - beta; 
        addWeighted(frame2, alpha, frame, beta, 0.0, sum);

        imshow("background", sum);
        if(waitKey(30) >= 0) break;
        ++i;
    }

    destroyWindow("background");
    return sum;
}

/**
 * the keyboard handler function
 * this is called after each frame is displayed in main
 */

void keyboardHandler() 
{
    int key = waitKey(30);
    switch (key) {
        case 'q':
            destroyAllWindows();
            exit(0);
            break;
    }
}

/**
 * the main function
 * 
 * there are several options avaible here, one of which is to record the frame
 * as a video
 */

int main(int argc, char **argv)
{
    cvNamedWindow("HandMade");

    // Setup capture object
    cv::VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    int frameWidth = 640;
    int frameHeight = 480;

    // Setup preprocessor and detector
    Preprocess p(getBackground(cap));
    Detect d;

    Mat frame;

    // variables to draw
    Point prev;
    prev.x = -1;
    int count = 0;
    Mat whiteboard = Mat::zeros(frameHeight, frameWidth, CV_32F);

    // the main input loop

    while (true) {
        cap >> frame;

        Mat raw = frame.clone();

        frame = p(frame);
        imshow("processed", frame);

        // make the processed movie if necessary
        if(makeMovies) { 
	        char buffer[20];
	        sprintf(buffer, "processed/processed_%03d.jpg", count);
	        imwrite(buffer, frame);
	    }
            
        // call the detection module
        std::pair<vector<Point>, std::pair<Point, double>> handInfo = d(frame, raw, count);

        // get where the finger tips are from the detection module
        vector<Point>& tips = handInfo.first;
        std::pair<Point, double>& maxCircle = handInfo.second;

        // all five fingers are present, so erase
        if(tips.size() >= 10) {
            maxCircle.first.x = frameWidth - maxCircle.first.x;
            circle(whiteboard, maxCircle.first, 3.5*maxCircle.second, Scalar(0, 0, 0), -1);
        }

        // else, draw the first finger
        else if(tips.size() > 0) {
            if(prev.x != -1) {
                tips[0].x = frameWidth - tips[0].x;
                if(d.euclideanDist(prev, tips[0]) < 200)
                    line(whiteboard, prev, tips[0], Scalar(255, 255, 255), 3);
            }
            prev = tips[0];
        }

        
        imshow("HandMade", whiteboard);

        // output the board movie (if the flag is set)
        if(makeMovies) {
	        char buffer2[20];
	        sprintf(buffer2, "board/board_%03d.jpg", count);
			imwrite(buffer2, whiteboard);
    	}

        keyboardHandler();

        ++count;
    }
}

