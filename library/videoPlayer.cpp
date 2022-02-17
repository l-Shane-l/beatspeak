#include "videoPlayer.h"

int runVideo()
{
  VideoCapture cap("../../video/chaplin.mp4");

  // Check if camera opened successfully
  if (!cap.isOpened())
  {
    cout << "Error opening video stream or file" << endl;
    return -1;
  }
  int frame_width = cap.get(CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
  VideoWriter video("beatspeakTest.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));
  while (1)
  {

    Mat frame;
    // Capture frame-by-frame

    // If the frame is empty, break immediately
    while (frame.empty())
    {
      cap >> frame;
    }

    // Display the resulting frame
    cout << "this far";
    imshow("Frame", frame);
    cout << "this far also";
    char c = (char)waitKey(25);
    if (c == 27)
      break;

    // Press  ESC on keyboard to exit
  }

  // When everything done, release the video capture object
  cap.release();
  video.release();
  // Closes all the frames
  destroyAllWindows();
  return 0;
}