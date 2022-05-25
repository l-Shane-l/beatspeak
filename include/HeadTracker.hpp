#ifndef HeadTracker_hpp
#define HeadTracker_hpp

#include <unordered_set>
#include "WebCam.hpp"
#include "FacialRecogniser.hpp"
#include "FacialPointsGenerator.hpp"
#include "TrackerData.hpp"
#include "RawDataList.hpp"

using namespace std;


struct HeadTracker {

public:
    HeadTracker(){
        shared_ptr<TrackerData> newTrackerData(new TrackerData());
        unique_ptr<FacialPointsGenerator> newPointsGen(new FacialPointsGenerator(newTrackerData));
        unique_ptr<FacialRecogniser> newFaceReq(new FacialRecogniser(newTrackerData));
        faceReq = move(newFaceReq);
        trackerData = newTrackerData;
        pointsGen = move(newPointsGen);
        pointsGen->setNumberOfPoints(numOfPoints);
        // shared_ptr<HeadMovementData> headPos(new HeadMovementData());
    }

        void trackHeadInFrame(Mat& img, Mat& gray, shared_ptr<Raw_Data_List> &rawdata );
    void test(){
        cout << "Tracker test" << endl;
    }
    unique_ptr<FacialRecogniser> faceReq;
    unique_ptr<FacialPointsGenerator> pointsGen;
    shared_ptr<TrackerData> trackerData;
    Mat frameRGB, frameGray, oldFrame;

private:


    int numOfPoints{50};
    vector<Point2f> points;
};

#endif
