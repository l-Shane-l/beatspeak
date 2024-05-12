#include "../../include/DataGenerators/HeadTracker.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <thread>

void HeadTracker::trackHeadInFrame(Mat& FrameRGB, Mat& FrameGray) {
  // Apply preprocessing to the image
  cv::GaussianBlur(FrameGray, FrameGray, cv::Size(7, 7), 0);
  cv::equalizeHist(FrameGray, FrameGray);
  // cv::threshold(FrameGray, FrameGray, 100, 255, cv::THRESH_BINARY);
  
  trackerData->frameGray = FrameGray;
  trackerData->frameRGB = FrameRGB;
  
  if (points.size() < numOfPoints) { // check if enough points are being tracked
    faceReq->detectAndDraw(); // generate more points
    if (trackerData->faces.size() > 0) { // if number of points > 0 track them
      vector<Point2f> verticalPoints;
      for (const auto& point : points) {
        verticalPoints.push_back(Point2f(0, point.y));
      }
      output_queue->push(verticalPoints);
      pointsGen->trackPoints(points, true);
    }
  } else if (points.size() == numOfPoints) { // if there are enough points simply track without generating new ones
    vector<Point2f> verticalPoints;
    for (const auto& point : points) {
      verticalPoints.push_back(Point2f(0, point.y));
    }
    output_queue->push(verticalPoints);
    pointsGen->trackPoints(points, false);
  }
}

