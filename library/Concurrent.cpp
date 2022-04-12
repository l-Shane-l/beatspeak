#include "Concurrent.hpp"
using namespace cv;
promise<bool> prms;
future<bool> ftr = prms.get_future();
std::mutex mutexA, mutexB, mutexC, mutexD;
std::condition_variable condA, condB, condC, condD;

int run(int argc, char *argv[])
{

  if (argc < 2)
  {
    std::cerr << "enter the command like " << endl
              << argv[0] << " cam"
              << std::endl
              << "for webcam or " << std::endl
              << argv[0]
              << " demo " << std::endl
              << "for demo of software without camera"
              << std::endl;
    return 1;
  }

  shared_ptr<WebCam> camInputRef(new WebCam(argv[1]));
  shared_ptr<WebCam> camOutputRef = camInputRef;
  shared_ptr<WebCam> camTrackRef = camInputRef;
  unique_ptr<HeadTracker> tracker(new HeadTracker());
  shared_ptr<Raw_Data_List> rawData(new Raw_Data_List());

  std::thread t1(getInput, ref(camInputRef));
  std::thread t2(trackHead, ref(camInputRef), ref(tracker), ref(rawData));
  std::thread t3(sendOutput, ref(camInputRef));
  std::thread t4(batchData, ref(rawData));
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  cout << "End" << std::endl;
  return 0;
}

void getInput(shared_ptr<WebCam> &cam)
{
  bool run = cam->running;
  std::unique_lock<std::mutex> lck(mutexA);
  while (run)
  {
    condB.notify_one();
    condA.wait(lck);
    run = cam->running;
    cam->updateFrame();
  }
}

void trackHead(shared_ptr<WebCam> &cam, unique_ptr<HeadTracker> &tracker, shared_ptr<Raw_Data_List> &rawData)
{
  bool run = cam->running;
  std::unique_lock<std::mutex> lck(mutexB);
  while (run)
  {
    condC.notify_one();
    condD.notify_one();
    condB.wait(lck);
    run = cam->running;
    tracker->trackHeadInFrame(cam->getframeRGB(), cam->getframeGray(), rawData);
    string hr = "HeartRate 72";
    putText(cam->getframeRGB(), hr, Point(30, 30), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 255), 1);
  }
}

void sendOutput(shared_ptr<WebCam> &cam)
{
  bool run = cam->running;
  std::unique_lock<std::mutex> lck(mutexC);
  while (run)
  {
    condA.notify_one();
    condC.wait(lck);
    if (!cam->getframeRGB().empty())
    {

      imshow("Beatspeak", cam->getframeRGB());
      char c = (char)waitKey(25);
      if (c == 27)
      {
        cam->toggleRunning();
        condA.notify_one();
        condB.notify_one();
        run = cam->running;
        break;
      };
    }
  }
}

void batchData(shared_ptr<Raw_Data_List> &rawData)
{
  std::unique_lock<std::mutex> lck(mutexD);
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(5)); // spawns a process every 5 seconds
    condD.wait(lck);
    std::thread run(processData, ref(rawData));
    run.detach();
  }
}

void processData(shared_ptr<Raw_Data_List> &rawData)
{
  // rawData->Modal_Dist_Filter();   // this causes a crash
  cout << "batched" << endl;
  // run raw_data_list functions here
  // add data is called from HeadTracker
}
