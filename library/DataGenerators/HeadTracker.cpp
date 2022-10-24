#include "../../include/DataGenerators/HeadTracker.hpp"
#include "spdlog/spdlog.h"

void HeadTracker::trackHeadInFrame(Mat &FrameRGB, Mat &FrameGray) {

  trackerData->frameGray = FrameGray;
  trackerData->frameRGB = FrameRGB;
  if (points.size() <
      numOfPoints) { // check to see if enough points are being tracked
    spdlog::info("Generating Points");
    faceReq->detectAndDraw(); // generate more points
    if (trackerData->faces.size() >
        0) { // if the number of points is greater than zero track them
      for_each(points.begin(),
               points.end(), // push the points to the output queue
               [&](Point2f x) { output_queue->push(x); });
      pointsGen->trackPoints(points, true);
    }
  } else if (points.size() ==
             numOfPoints) { // if there are enough points simply track without
                            // generating new ones
    for_each(points.begin(),
             points.end(), // push the points to the output queue
             [&](Point2f x) { output_queue->push(x); });
    pointsGen->trackPoints(points, false);
  }
}
