#ifndef HeadTracker_hpp
#define HeadTracker_hpp

#include "../IO/WebCam.hpp"
#include "../Utilities/LockFreeStack.hpp"
#include "FacialPointsGenerator.hpp"
#include "FacialRecogniser.hpp"
#include "TrackerData.hpp"
#include <unordered_set>

using namespace std;

struct HeadTracker {

public:
  HeadTracker(shared_ptr<lock_free_queue<Point2f>> queue)
      : output_queue(move(queue)) {
    shared_ptr<TrackerData> newTrackerData(new TrackerData());
    unique_ptr<FacialPointsGenerator> newPointsGen(
        new FacialPointsGenerator(newTrackerData));
    unique_ptr<FacialRecogniser> newFaceReq(
        new FacialRecogniser(newTrackerData));
    faceReq = move(newFaceReq);
    trackerData = newTrackerData;
    pointsGen = move(newPointsGen);
    pointsGen->setNumberOfPoints(numOfPoints);
  }

  void trackHeadInFrame(Mat &img, Mat &gray);
  void test() { cout << "Tracker test" << endl; }
  unique_ptr<FacialRecogniser> faceReq;
  unique_ptr<FacialPointsGenerator> pointsGen;
  shared_ptr<TrackerData> trackerData;
  Mat frameRGB, frameGray, oldFrame;

private:
  shared_ptr<lock_free_queue<Point2f>> output_queue;
  int numOfPoints{50};
  vector<Point2f> points;
};

#endif
