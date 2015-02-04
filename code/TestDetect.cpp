#define private public

#include <iostream>
#include "Preprocess.h"
#include "Detect.h"
#include "gtest/gtest.h"

using namespace cv;

TEST(Detect, euclideanDist) {
    Detect d;
    Point p(2, -1);
    Point q(-2, 2);
    float actual = d.euclideanDist(p, q);
    float expected = 5.0;
    ASSERT_EQ(expected, actual);
}


TEST(Detect, euclideanDist2) {
    Detect d;
    Point p(2, 5);
    Point q(-2, 5);
    float actual = d.euclideanDist(p, q);
    float expected = 4.0;
    ASSERT_EQ(expected, actual);
}

TEST(Detect, euclideanDist3) {
    Detect d;
    Point p(9, -2);
    Point q(0, 10);
    float actual = d.euclideanDist(p, q);
    float expected = 15.0;
    ASSERT_EQ(expected, actual);
}

TEST(Detect, getAngle) {
    Detect d;
    Point a(2, 3);
    Point b(3, 4);
    Point c(4, 2);
    double actual = d.getAngle(a, b, c);
    double expected = 36.8699;
    ASSERT_TRUE(abs(expected - actual) < .1);
}

TEST(Detect, getAngle1) {
    Detect d;
    Point a(2, 4);
    Point b(3, 5);
    Point c(4, 8);
    double actual = d.getAngle(a, b, c);
    double expected = 8.1303;
    ASSERT_TRUE(abs(expected - actual) < .1);
}

TEST(Detect, getAngle2) {
    Detect d;
    Point a(3, 3);
    Point b(2, 4);
    Point c(8, 2);
    double actual = d.getAngle(a, b, c);
    double expected = 7.1276;
    ASSERT_TRUE(abs(expected - actual) < .1);
}

TEST(Detect, getPolyCurves) {
    Mat frame = imread("TestHand.png");
    cvtColor(frame, frame, CV_BGR2GRAY);
    Detect d;
    vector<vector<Point>> a = d.getPolyCurves(frame);
    vector<vector<Point>> b;
     for(int i = 0; i < b.size(); ++i) {
        for(int j = 0; j < b[0].size(); ++j){
            Point& actual   = a[i][j];
            Point& expected = b[i][j];
            ASSERT_EQ(expected.x, actual.x);
            ASSERT_EQ(expected.y, actual.y);            
        }
    }
}

TEST(Detect, getConvexHulls) {
    vector<vector<Point>> polyCurves;
    Detect d;
    vector<vector<int>> a = d.getConvexHulls(polyCurves);
    vector<vector<int>> b;
    for(int i = 0; i < a.size(); ++i) {
        for(int j = 0; j < a[0].size(); ++j){
            int expected = b[i][j];
            int actual   = a[i][j];
            ASSERT_EQ(expected, actual);
        }
    }
}

TEST(Detect, findMaxInscribedCircle) {
    vector<vector<Point>> polyCurves;
    Mat frame = imread("TestHand.png");
    Detect d;
    std::pair<Point, double> actual = d.findMaxInscribedCircle(polyCurves, frame);
    std::pair<Point, double> expected;
    ASSERT_EQ(expected.first.x, actual.first.x);
    ASSERT_EQ(expected.first.y, actual.first.y);
    ASSERT_EQ(expected.second, actual.second);

}

TEST(Detect, findMinEnclosingCircle) {
    vector<vector<Point>> polyCurves;
    Detect d;
    std::pair<Point2f, float> actual = d.findMinEnclosingCircle(polyCurves);
    std::pair<Point2f, float> expected;
    ASSERT_EQ(expected.first.x, actual.first.x);
    ASSERT_EQ(expected.first.y, actual.first.y);
    ASSERT_EQ(expected.second, actual.second);
}

TEST(Detect, findFingerTips) {
    vector<Point> defectEnds;
    std::pair<Point, double> maxCircle;
    Mat frame = imread("TestHand.png");
    Detect d;
    vector<Point> a = d.findFingerTips(defectEnds, maxCircle, frame);
    vector<Point> b;
    for(int i = 0; i < b.size(); ++i) {
        Point actual = a[i];
        Point expected = b[i];
        ASSERT_EQ(expected.x, actual.x);
        ASSERT_EQ(expected.y, actual.y);
    }
}

TEST(Detect, filterDefects) {
    vector<Vec4i> defects;
    Detect d;
    vector<Vec4i> a = d.filterDefects(defects);
    vector<Vec4i> b;
    for(int i = 0; i < b.size(); ++i) {
        Vec4i actual = a[i];
        Vec4i expected = b[i];
        ASSERT_EQ(expected[0], actual[0]);
        ASSERT_EQ(expected[1], actual[1]);
        ASSERT_EQ(expected[2], actual[2]);
        ASSERT_EQ(expected[3], actual[3]);
    }
}


TEST(Detect, getPolyCurves1) {
    Mat frame = imread("TestHand1.png");
    cvtColor(frame, frame, CV_BGR2GRAY);
    Detect d;
    vector<vector<Point>> a = d.getPolyCurves(frame);
    vector<vector<Point>> b;
     for(int i = 0; i < b.size(); ++i) {
        for(int j = 0; j < b[0].size(); ++j){
            Point& actual   = a[i][j];
            Point& expected = b[i][j];
            ASSERT_EQ(expected.x, actual.x);
            ASSERT_EQ(expected.y, actual.y);            
        }
    }
}

TEST(Detect, getConvexHulls1) {
    vector<vector<Point>> polyCurves;
    Detect d;
    vector<vector<int>> a = d.getConvexHulls(polyCurves);
    vector<vector<int>> b;
    for(int i = 0; i < a.size(); ++i) {
        for(int j = 0; j < a[0].size(); ++j){
            int expected = b[i][j];
            int actual   = a[i][j];
            ASSERT_EQ(expected, actual);
        }
    }
}

TEST(Detect, findMaxInscribedCircle1) {
    vector<vector<Point>> polyCurves;
    Mat frame = imread("TestHand1.png");
    Detect d;
    std::pair<Point, double> actual = d.findMaxInscribedCircle(polyCurves, frame);
    std::pair<Point, double> expected;
    ASSERT_EQ(expected.first.x, actual.first.x);
    ASSERT_EQ(expected.first.y, actual.first.y);
    ASSERT_EQ(expected.second, actual.second);

}

TEST(Detect, findMinEnclosingCircle1) {
    vector<vector<Point>> polyCurves;
    Detect d;
    std::pair<Point2f, float> actual = d.findMinEnclosingCircle(polyCurves);
    std::pair<Point2f, float> expected;
    ASSERT_EQ(expected.first.x, actual.first.x);
    ASSERT_EQ(expected.first.y, actual.first.y);
    ASSERT_EQ(expected.second, actual.second);
}

TEST(Detect, findFingerTips1) {
    vector<Point> defectEnds;
    std::pair<Point, double> maxCircle;
    Mat frame = imread("TestHand1.png");
    Detect d;
    vector<Point> a = d.findFingerTips(defectEnds, maxCircle, frame);
    vector<Point> b;
    for(int i = 0; i < b.size(); ++i) {
        Point actual = a[i];
        Point expected = b[i];
        ASSERT_EQ(expected.x, actual.x);
        ASSERT_EQ(expected.y, actual.y);
    }
}

TEST(Detect, filterDefects1) {
    vector<Vec4i> defects;
    Detect d;
    vector<Vec4i> a = d.filterDefects(defects);
    vector<Vec4i> b;
    for(int i = 0; i < b.size(); ++i) {
        Vec4i actual = a[i];
        Vec4i expected = b[i];
        ASSERT_EQ(expected[0], actual[0]);
        ASSERT_EQ(expected[1], actual[1]);
        ASSERT_EQ(expected[2], actual[2]);
        ASSERT_EQ(expected[3], actual[3]);
    }
}


// Constructor of preprocess
TEST(Preprocess, preprocess) {
}

TEST(Preprocess, thresholdFilter) {
}

TEST(Preprocess, faceDetection) {
}

TEST(HandMade, getBackground) {
}
