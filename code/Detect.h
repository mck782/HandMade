#ifndef DETECT_H
#define DETECT_H

// ========
// includes
// ========

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv/cv.h>

#include <stdexcept>

/**
 * The Detect class
 *
 * This represents the main motion detection in our program
 *
 * Constructed with a OpenCV VideoCapture object, this class is responsible for
 * detecting the Hand Detects and returns some kind of data type (not decided
 * yet) that represents the hand gesture in the current frame
 *
 * As of now, it simply displays the video stream's edges with Canny
 */

using namespace cv;

class Detect
{
    private:
        
        // Calculates the angle between the triangle specified
        double getAngle(const Point&, const Point&, const Point&);

    public:
        Detect();
        ~Detect();

        // Calculates the euclideanDist between the two points
        float euclideanDist(const Point&, const Point&);

        // finds the polynomial curves in the image
        vector<vector<Point>> getPolyCurves(Mat&);

        // finds the convex hulls of polynomial curves
        vector<vector<int>> getConvexHulls(vector<vector<Point>>&);

        // finds the max inscribed circle
        std::pair<Point, double> findMaxInscribedCircle(const vector<vector<Point>>&, const Mat&);

        // finds the minimum enclosing circle
        std::pair<Point2f, float> findMinEnclosingCircle(const vector<vector<Point>>&);

        // determines the region of interest based on the polyCurves passed in
        void getRegionOfInterest(vector<vector<Point>>&, const vector<vector<Point>>&, const std::pair<Point, double>&);

        // the main call to Detect 
        std::pair<vector<Point>, std::pair<Point,double>> operator() (Mat&, Mat&, int);

        // filter defects by depth
        vector<Vec4i> filterDefects(const vector<Vec4i>& defects);

        // match defect end points and return as finger tip points
        vector<Point> findFingerTips(vector<Point> defectEnds, std::pair<Point, double> maxCircle, Mat& raw);

};
#endif
