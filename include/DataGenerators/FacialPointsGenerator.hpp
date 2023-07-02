#ifndef FacialPointsGenerator_hpp
#define FacialPointsGenerator_hpp

#include "TrackerData.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/features2d.hpp"
#include <iostream>
#include <memory>

using namespace std;
using namespace cv;

struct FacialPointsGenerator {
  FacialPointsGenerator(shared_ptr<TrackerData> data) : Data(move(data)) {}

public:
  void trackPoints(vector<Point2f> &newPoints, bool update);
  void setNumberOfPoints(int num) { numOfPoints = num; }

private:
  void generateMask();
  void generateTrackingPoints();
  void updateTrackedPoints();
  void drawPoints();
  Mat maskedFrame, errors;
  vector<Point2f> prevPoints, currentPoints, generatedPoints;
  vector<Point2f> none{'0'};
  vector<Point> faceRegion, eyeRegion;
  vector<uchar> trackingStatus;
  uchar notFound{'0'};
  int numOfPoints;
  shared_ptr<TrackerData> Data;
  float scaleFactor = 1.2f;
int numLevels = 8;
double edgeThreshold = 0.001;
int firstLevel = 0;
int scoreType = cv::ORB::HARRIS_SCORE;
  //TODO
  // Below is 25 which is the number of points, it was not created properly this is a quickfix
  Ptr<cv::ORB> orb = cv::ORB::create(25, scaleFactor, numLevels, edgeThreshold, firstLevel, 2, scoreType, 31, 20);
  Mat descriptors;
};

#endif
