#ifndef FacialRecogniser_hpp
#define FacialRecogniser_hpp

#include "TrackerData.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include <iostream>
#include <memory>

using namespace std;
using namespace cv;

struct FacialRecogniser {
    FacialRecogniser(shared_ptr<TrackerData> data) {
        Data = data;
        face.load("../BeatReq/cascade/"
                  "haarcascade_frontalface_alt.xml");
        eyes.load("../BeatReq/cascade/haarcascade_eye.xml");
        mouth.load("../BeatReq/cascade/haarcascade_smile.xml");
    }

    CascadeClassifier face, eyes, mouth;
    Mat faceArea;
    shared_ptr<TrackerData> Data;
    string cascadeName, nestedCascadeName;
    void detectAndDraw();
    int faceDetectCooldownTime = 5;
    bool faceDetectCooldown = false;

  private:
};

#endif
