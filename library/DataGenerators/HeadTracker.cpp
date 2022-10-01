#include "../../include/DataGenerators/HeadTracker.hpp"

void HeadTracker::trackHeadInFrame(Mat &FrameRGB, Mat &FrameGray) {

  trackerData->frameGray = FrameGray;
  trackerData->frameRGB = FrameRGB;
  if (points.size() < numOfPoints) {
    faceReq->detectAndDraw();
    if (trackerData->faces.size() > 0) {
      pointsGen->trackPoints(points, true);
    }
  }
  if (points.size() == numOfPoints) {
    pointsGen->trackPoints(points, false);
  }
}
