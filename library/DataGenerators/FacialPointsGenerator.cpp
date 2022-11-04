#include "../../include/DataGenerators/FacialPointsGenerator.hpp"
#include "spdlog/spdlog.h"

// logic to setup tracking
void FacialPointsGenerator::generateMask() {
  maskedFrame = Mat::zeros(Data->frameGray.rows, Data->frameGray.cols, CV_8UC1);
  ellipse2Poly(Data->faceCenter, (Data->radius) / 2, 0, 0, 360, 10, faceRegion);
  fillConvexPoly(maskedFrame, faceRegion, (255));
  for (int i = 0; i < Data->eyes.size(); i++) {
    ellipse2Poly(Data->eyeCenter[i],
                 (Size((Data->radius.width) * 3, Data->eyeRadius[i])) / 2, 0, 0,
                 360, 10, eyeRegion);
    fillConvexPoly(maskedFrame, eyeRegion, (0));
  }
}

void FacialPointsGenerator::generateTrackingPoints() {
  goodFeaturesToTrack(Data->frameGray, generatedPoints, numOfPoints, 0.001, 10,
                      maskedFrame, 3, false, 0.04);
  prevPoints = generatedPoints;
}

void FacialPointsGenerator::trackPoints(vector<Point2f> &newPoints,
                                        bool update) {
  if (update) {
    generateMask();
    generateTrackingPoints();
  }
  spdlog::info("Tracking Points");
  updateTrackedPoints();
  newPoints = currentPoints;
}

void FacialPointsGenerator::updateTrackedPoints() {
  if (Data->frameGray.rows == Data->oldFrame.rows &&
      Data->frameGray.cols == Data->oldFrame.cols) {
    TermCriteria criteria =
        TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
    try {
      calcOpticalFlowPyrLK(Data->oldFrame, Data->frameGray, prevPoints,
                           currentPoints, trackingStatus, errors, Size(15, 15),
                           3, criteria);
    } catch (const exception &e) {
      cout << e.what() << endl;
    }
    prevPoints = currentPoints;
    Data->oldFrame = Data->frameGray.clone();
    drawPoints();
  } else {
    Data->oldFrame = Data->frameGray.clone();
    spdlog::info("size miss match");
    spdlog::info(Data->frameGray.rows);
    spdlog::info(Data->oldFrame.rows);
  }
}

void FacialPointsGenerator::drawPoints() {
  Scalar pointColor{(0)}, lineColor{(0)};
  for (int i = 0; i < currentPoints.size(); i++) {
    circle(Data->frameRGB, currentPoints[i], 5, pointColor, 1);
    line(Data->frameRGB, Point(currentPoints[i].x - 5, currentPoints[i].y),
         Point(currentPoints[i].x + 5, currentPoints[i].y), lineColor, 1);
    line(Data->frameRGB, Point(currentPoints[i].x, currentPoints[i].y - 5),
         Point(currentPoints[i].x, currentPoints[i].y + 5), lineColor, 1);
  }
}
