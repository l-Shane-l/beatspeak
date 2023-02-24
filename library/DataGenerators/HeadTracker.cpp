#include "../../include/DataGenerators/HeadTracker.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <thread>

void HeadTracker::trackHeadInFrame(Mat &FrameRGB, Mat &FrameGray) {
  Mat blurredGray;
  GaussianBlur(FrameGray, blurredGray, Size(5, 5), 0); // add Gaussian blur with kernel size of 5x5
  equalizeHist(FrameGray, FrameGray);
  Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
erode(FrameGray, FrameGray, element);
dilate(FrameGray, FrameGray, element);
  // Apply adaptive thresholding to the grayscale image
  // adaptiveThreshold(FrameGray, FrameGray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, 5);
  trackerData->frameGray = blurredGray; // use the blurred image for processing
  trackerData->frameRGB = FrameRGB;

  trackerData->frameGray = FrameGray;
  trackerData->frameRGB = FrameRGB;
  if (points.size() <
      numOfPoints) {          // check to see if enough points are being tracked
    faceReq->detectAndDraw(); // generate more points
    if (trackerData->faces.size() >
        0) { // if the number of points is greater than zero track them
      output_queue->push(points);
      pointsGen->trackPoints(points, true);
    }
  } else if (points.size() ==
             numOfPoints) { // if there are enough points simply track without
                            // generating new ones
    output_queue->push(points);
    pointsGen->trackPoints(points, false);
  }
}
