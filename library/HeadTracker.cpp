#include "../include/HeadTracker.hpp"

void HeadTracker::trackHeadInFrame(Mat& FrameRGB, Mat& FrameGray, shared_ptr<Raw_Data_List> &rawdata){

    trackerData->frameGray = FrameGray;
    trackerData->frameRGB = FrameRGB;
    if(points.size() < numOfPoints){
        faceReq->detectAndDraw();
        if(trackerData->faces.size() > 0){
                pointsGen->trackPoints(points, rawdata, true);
        }
    }
    if(points.size() == numOfPoints){
            pointsGen->trackPoints(points, rawdata, false);
    }

}
