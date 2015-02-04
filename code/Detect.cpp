#include "Detect.h"
#include <iostream>
#include <math.h>

// global bool to makeMovies;
bool makeMoviesD = FALSE;

/**
 * the constructor
 *
 * this does nothing
 */

Detect::Detect()
{}

/**
 * the destructor
 *
 * this does nothing
 */

Detect::~Detect()
{}

/**
 * Calculates the euclideanDist between the two points
 *
 * This computes it by the simple pythagorean formula, 
 * which is efficient and simple
 */

float Detect::euclideanDist(const Point& p, const Point& q) 
{
    Point diff = p - q;
    return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

/**
 * Calculates the angle between the three points specified
 *
 * This works by using the inverse cosine function provided by math.h
 */

double Detect::getAngle(const Point& a, 
                const Point& b, 
                const Point& c) 
{
    float sideA = euclideanDist(a, c);
    float sideB = euclideanDist(b, c);
    float sideC = euclideanDist(a, b);

    double angle = acos((sideA*sideA + sideB*sideB - sideC*sideC) / (2* sideA * sideB)) * 180.0/3.14159; 

    return angle;
}

/**
 * finds the polynomial curves in the image
 *
 * this first finds the contours in the image, filters them out, and finds
 * matching polynomial curves
 *
 * this function returns a vector<vector<Point>>
 */

vector<vector<Point>> Detect::getPolyCurves(Mat& frame)
{
    // First find the contours in the image
    vector<vector<Point>> contours;
    findContours(frame, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // Filter contours by area
    vector<vector<Point>> goodContours;

    for (int i = 0; i < contours.size(); ++i) {
        // an arbitrary amount for now
        if (contourArea(contours[i]) > 5000) {
            goodContours.push_back(contours[i]);
        }
    }

    vector<vector<Point>> polyCurves;
    
    // Get poly curves for every contour
    for (int i = 0; i < goodContours.size(); ++i) {
        vector<Point> current;
        approxPolyDP(goodContours[i], current, 2.0, false);

        if(current.size() > 0){
            polyCurves.push_back(current);
        }
    }

    return polyCurves;
}

/** 
 * finds the convex hulls of polynomial curves
 *
 * we do this by the OpenCV call convexHull
 * note that this function returns the indices of the hull in the polynomial
 * curve, not the actual points themselves
 */

vector<vector<int>> Detect::getConvexHulls(vector<vector<Point>>& polyCurves)
{
    vector<vector<int>> hullIndices;

    for (int i = 0; i < polyCurves.size(); ++i) {
        vector<int> hullIndex;
        if (polyCurves[i].size() > 0) {
            convexHull(polyCurves[i], hullIndex);
            if(hullIndex.size() > 2){
                hullIndices.push_back(hullIndex);  
            }
            else {
                polyCurves.erase(polyCurves.begin() + i);
                --i;
            }
        }
    }

    return hullIndices;
}

/**
 * finds the max inscribed circle in the polyCurves we have
 *
 * this works by iterating through the entire image and seeing if the current
 * pixel is the greatest that we have so far
 * doing this, we are able to determine the greatest inscribed circle
 */

std::pair<Point, double> Detect::findMaxInscribedCircle(
        const vector<vector<Point>>& polyCurves, 
        const Mat& frame)
{
    std::pair<Point, double> c;
    double dist    = -1;
    double maxdist = -1;

    if (polyCurves.size() > 0) {
        for (int i = 0; i < frame.cols; i+=10) {
            for (int j = 0; j < frame.rows; j+=10) {
                dist = pointPolygonTest(polyCurves[0], Point(i,j), true);
                if (dist > maxdist) {
                    maxdist = dist;
                    c.first = Point(i,j);
                }
            }
        }
        c.second = maxdist;
    }

    return c;
}


/**
 * finds the minimum enclosing circle of the curve
 *
 * this is rather trivial to write, since we are simply finding the bounding
 * circle
 */

std::pair<Point2f, float> Detect::findMinEnclosingCircle(const vector<vector<Point>>& goodPolyCurve)
{
    std::pair<Point2f, float> c;
    
    if(goodPolyCurve.size() > 0)
        minEnclosingCircle(goodPolyCurve[0], c.first, c.second);

    return c;
}

/**
 * gets the region of interest of the curve
 *
 * we do this by truncating contours that are not within 3.5 times of the max
 * inscribed circle's radius.
 * 
 * We've found this to be a rather accurate heuristic in general
 */

void Detect::getRegionOfInterest(
        vector<vector<Point>>& goodPolyCurves, 
        const vector<vector<Point>>& polyCurves, 
        const std::pair<Point, double>& maxCircle)
{
    for (int i = 0; i < polyCurves.size(); i++) {
        vector<Point> goodContour;
        for(int j = 0; j < polyCurves[i].size(); j++) {
            Point current = polyCurves[i][j];
            if (euclideanDist(current, maxCircle.first) < 3.5 * maxCircle.second) {
                goodContour.push_back(current);
            }
        }

        // DO NOT ADD POLY CURVE IF FEWER THAN 3 POINTS SINCE CANNOT HAVE HULL INDEX
        if(goodContour.size() > 3){
            goodPolyCurves.push_back(goodContour);
        }
    }
}

/**
 * The main interface call to Detect
 *
 * This function takes in a preprocessed frame and outputs information about the hand in the image
 *
 * This involves:
 *   * The min enclosing circle
 *   * The max inscribed circle
 *   * The location of the finger points
 *
 * Returns a pair with a vector of finger tip Points and another Pair describing the max inscribed circle
 */

std::pair<vector<Point>, std::pair<Point,double>> Detect::operator() (Mat& frame, Mat& raw, int count)
{
    vector<Point> tips;

    // first find the curves in the image
    vector<vector<Point>> polyCurves = getPolyCurves(frame);

    //std::cout << polyCurves.size() << std::endl;

    // Find max inscribed circle for the 0th polycurve and draw it.
    std::pair<Point, double> maxCircle = findMaxInscribedCircle(polyCurves, raw);

    circle(raw, maxCircle.first, maxCircle.second, cv::Scalar(220,75,20), 1, CV_AA);

    // Good PolyCurve is with 3.5 * max inscribed radius
    vector<vector<Point>> goodPolyCurves;
    getRegionOfInterest(goodPolyCurves, polyCurves, maxCircle);

    // draw good poly curve
    drawContours(raw,goodPolyCurves, -1 , cv::Scalar(0,255,0), 2);

    // Find min enclosing circle on goodPolyCurve and draw it
    std::pair<Point2f, double> minCircle = findMinEnclosingCircle(goodPolyCurves);
    circle(raw, minCircle.first, minCircle.second, cv::Scalar(220,75,20), 1, CV_AA);

    // now find the convex hull for each polyCurve
    if (goodPolyCurves.size() < 1) {
        return std::pair<vector<Point>, std::pair<Point, double>>(tips, maxCircle);
    }

    // Get convex hulls
    vector<vector<int>> hullIndices = getConvexHulls(goodPolyCurves);

    vector<vector<Point>> hullPoints;
    for(int i = 0; i < goodPolyCurves.size(); i++) {
        if (goodPolyCurves[i].size() > 0) {
            vector<Point> hullPoint;
            convexHull(goodPolyCurves[i], hullPoint);
            hullPoints.push_back(hullPoint);
        }
    }

    // Draw the convex hulls
    drawContours(raw, hullPoints, -1, cv::Scalar(255, 0, 0), 2);

    
    for (int i = 0; i < 1; ++i)
    {
        vector<Vec4i> defects;
        
        // find convexity defects for each poly curve and draw them
        convexityDefects(goodPolyCurves[i], hullIndices[i], defects);

        defects = filterDefects(defects);
        vector<Point> defectEnds;

        for (int j = 0; j < defects.size(); ++j) {
            Vec4i& defect = defects[j];

            int startIdx = defect[0];
            int endIdx = defect[1];
            int farIdx = defect[2];

            Point start = goodPolyCurves[i][startIdx];
            Point end = goodPolyCurves[i][endIdx];
            Point far = goodPolyCurves[i][farIdx];
            if(euclideanDist(far, start) > maxCircle.second) {
                defectEnds.push_back(start);
                defectEnds.push_back(end);
            }
        }

        tips = findFingerTips(defectEnds, maxCircle, raw);
    }

    flip(raw, raw, 1);
    imshow("hand", raw);

    // movie code (we should return the frame to HandMade and put movie code there with the others.)
    if(makeMoviesD) {
	    char buffer[30];
	    sprintf(buffer, "detected/detected_%03d.jpg", count++);
	    imwrite(buffer, raw);
	}

    return std::pair<vector<Point>, std::pair<Point, double>>(tips, maxCircle);
}

/**
 *  Takes a vector of convexity defect endpoints and matches them based on the first match under
 * a length proportionate to the palm radius. Then draws the point on the frame and adds to the return vector
 *  
 * Also filters based on distance from the palm to elimated weird contours on the edge of the arms. This should
 * be moved to the filterDefects method.
 *
 * Needs to be improved upon a lot. Currently returns two points for every match which results in 10 return points
 * for 5 fingers. Also it should find the best match (nearest neighbor) instead of the first under some distance.
 */

vector<Point> Detect::findFingerTips(vector<Point> defectEnds, std::pair<Point, double> maxCircle, Mat& raw)
{
    vector<Point> tips;
    for(int k = 0; k < defectEnds.size(); k++)
    {
        for(int l = 0; l < defectEnds.size(); l++)
        {
            if( k != l) {
                float distance = euclideanDist(defectEnds[k], defectEnds[l]);
                if (distance < maxCircle.second / 2 ){

                    Point tip = (defectEnds[k] + defectEnds[l])*.5;

                    float palmDist = euclideanDist(tip, maxCircle.first);
                    if(palmDist > 2 * maxCircle.second) {
                        tips.push_back(tip);
                        circle(raw, tip, 10, Scalar(255, 255, 255), 2);
                        break;
                    }
                }
            }

        }
    }
    return tips;
}

/**
 *  Filters defects based on the depth of the convexity.
 *  Defects between fingers have a large depth. Defects on the between end fingers and other parts
 *  of the hand dont however, and this can cause issues with detection.
 */

vector<Vec4i> Detect::filterDefects(const vector<Vec4i> & defects)
{
    vector<Vec4i> filteredDefects;
    for(int i = 0; i < defects.size(); ++i)
    {
        Vec4i defect = defects[i];
        float depth = defect[3] / 256;

        if(depth > 20) {
            filteredDefects.push_back(defect);
        }
    }
    return filteredDefects;
}
