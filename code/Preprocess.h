#ifndef PREPROCESS_H
#define PREPROCESS_H

// ========
// includes
// ========

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

/** 
 * The Preprocessing class
 *
 * This takes care of all the preprocessing images we do on a frame of a video
 * 
 * This includes filtering for noise, skin extraction, pixel erosion and
 * dilation, and more. We describe all of this in detail in our proposals
 *
 * The main interface to this function is simply providing it an image
 * Any variables that we use among multiple images (such as the background) are
 * kept as member fields
 */

using namespace cv;

class Preprocess
{
    private:
        Mat _bg;
        const Scalar min_YCrCb;
        const Scalar max_YCrCb;

        CascadeClassifier face_cascade;


    public:
        Preprocess(const Mat&);
        ~Preprocess();

        // thresholds the call by YCrCb channels and erodes and dilates
        Mat thresholdFilter(Mat&);

        // the main interface call to Preprocess
        Mat operator()(Mat&);

        // face detection
        vector<Rect> faceDetection(Mat& frame);
};

#endif // PREPROCESS_H
