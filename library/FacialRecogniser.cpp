#include "../include/FacialRecogniser.hpp"

void FacialRecogniser::detectAndDraw() {
  face.detectMultiScale(Data->frameGray, Data->faces, 1.1, 8,
                        0 | CASCADE_SCALE_IMAGE, Size(15, 15));
  if (Data->faces.size() != 0) {
    Data->face = Data->faces[0];
    Data->faceCenter.x = cvRound((Data->face.x + Data->face.width * 0.5));
    Data->faceCenter.y = cvRound((Data->face.y + Data->face.height * 0.5));
    Data->radius = Size(Data->face.width * 0.7, Data->face.height);
    // ellipse( Data->frameRGB, Data->faceCenter,Data->radius,0, 0, 360,
    // Data->trackerColor, 3, 8, 0 );

    faceArea = Data->frameGray(Data->face);

    eyes.detectMultiScale(faceArea, Data->eyes, 1.1, 3, 0 | CASCADE_SCALE_IMAGE,
                          Size(3, 3));

    for (int i = 0; i < Data->eyes.size(); i++) {
      Point tmp{0, 0};
      Data->eyeCenter.push_back(tmp);
      Data->eyeCenter[i].x =
          cvRound((Data->face.x + Data->eyes[i].x + Data->eyes[i].width * 0.5));
      Data->eyeCenter[i].y = cvRound(
          (Data->face.y + Data->eyes[i].y + Data->eyes[i].height * 0.5));
      Data->eyeRadius.push_back(Data->eyes[i].width * 0.7);
      // circle( Data->frameRGB, Data->eyeCenter[i], Data->eyeRadius[i],
      // Data->trackerColor, 3, 8, 0 );
    }
  }
}
