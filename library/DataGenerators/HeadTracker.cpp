#include "../../include/DataGenerators/HeadTracker.hpp"

void HeadTracker::trackHeadInFrame(Mat &FrameRGB, Mat &FrameGray) {

  trackerData->frameGray = FrameGray;
  trackerData->frameRGB = FrameRGB;
  if (points.size() <
      numOfPoints) {          // check to see if enough points are being tracked
    faceReq->detectAndDraw(); // generate more points
    if (trackerData->faces.size() >
        0) { // if the number of points is greater than zero track them
      pointsGen->trackPoints(points, true);
    }
  } else if (points.size() ==
             numOfPoints) { // if there are enough points simply track without
                            // generating new ones
    pointsGen->trackPoints(points, false);
  }
}
