#include "Preprocess.h"

/**
 * The constructor for Preprocess
 *
 * Only takes in a Mat representing the background
 *
 * Note that we do not attempt to find the background dynamically
 * Rather, we initialize the background before hand and rely on that throughout
 * the code
 */

Preprocess::Preprocess(const Mat& bg) :
    _bg(bg),
    min_YCrCb(Scalar(0, 133, 77)),
    max_YCrCb(Scalar(255, 173, 127))
{
   if( !face_cascade.load( "frontal_face.xml" ) ){ printf("--(!)Error loading\n");};
}

/**
 * The destructor for Preprocess
 *
 * As of now this does nothing
 */

Preprocess::~Preprocess() 
{}

/**
 * Thresholds the image and filters out noise
 * This function assumes that the frame passed in is of YCrCb values
 *
 * We accomplish this by using Otus's method on each of the three channels and
 * then running pixel erosion and dilation morphologyEx on them as well
 */

Mat Preprocess::thresholdFilter(Mat& frame)
{
    Mat frameChannels[3];
    split(frame, frameChannels);

    // Chooses how we erode and dilate
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));

    for (int i = 0; i < 3; i++) {
        // Applies a threshold using Otsu's method
        threshold(frameChannels[i], frameChannels[i], 0, 255, THRESH_BINARY | THRESH_OTSU);

        // Runs pixel erosion and dilation for three iterations
        morphologyEx(frameChannels[i], frameChannels[i], MORPH_OPEN, element, Point(-1, -1), 3);
    }

    // Merge the channels back together
    Mat merged;
    merge(frameChannels, 3, merged);

    return merged;
}

/**
 * The main interface to the Preprocess class
 * 
 * This takes in an image, most likely a frame from a video and preprocesses that image 
 * and outputs a binary image (hopefully) containing only the hand
 *
 * We accomplish this by a mix of procedures, 
 * such as thresholding, bitwise and, pixel erosion and dilation, and more
 *
 * A description of why each method was selected is provided in our report, as well as it's inspiration
 */

Mat Preprocess::operator() (Mat& frame)
{
	// detect faces
    vector<Rect> faces = faceDetection(frame);

	cvtColor(frame, frame, CV_BGR2YCrCb);

    // Copy the raw image for debugging purposes
    Mat raw = frame.clone();

    GaussianBlur(_bg, _bg, Size(7,7), 1.8, 1.8);
    GaussianBlur(frame, frame, Size(7,7), 1.8, 1.8);

    frame = frame - _bg;

    frame = thresholdFilter(frame);

    // Run another pixel erosion and dilation on this function
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(frame, frame, MORPH_OPEN, element, Point(-1, -1), 3);

    // And the thresholded image with the original
    bitwise_and(frame, raw, frame);

    // Convert the frame to grayscale
    cvtColor(frame, frame, CV_YCrCb2BGR);
    cvtColor(frame, frame, CV_BGR2GRAY);

    // Threshold the grayscaled image with Otsu once more
    threshold(frame, frame, 120, 255, THRESH_BINARY_INV);

    // Skin extraction from our member variables
    Mat skinRegion; 
    inRange(raw, min_YCrCb, max_YCrCb, skinRegion);

    compare(frame, skinRegion, frame, CMP_EQ);

    // draw a black rectange over each face.
    for(int i = 0; i < faces.size(); i++) {
        rectangle(frame, faces[i], Scalar(0, 0, 0), -1, 0, 0);
    }

    // Finally run a GaussianFilter
    GaussianBlur(frame, frame, Size(7,7), 1.8, 1.8);

    return frame;
}

/**
 * Finds faces using detectMultiScale
 * Returns a vector of Rects representing location and size of face.
 */

vector<Rect> Preprocess::faceDetection(Mat& frame) {
    vector<Rect> faces;
    Mat frame_gray;
    cvtColor( frame, frame_gray, CV_BGR2GRAY );

    face_cascade.detectMultiScale( frame_gray, faces, 1.3, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

    return faces;
}

