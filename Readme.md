# In Development

## Setup

check the scripts folder

## TODO

### Switch from Lucas Kanade to KLT
Tracking currently uses Lucas Kanade which is sensitive to light changes which is heavily effected by the dynamic lighting in a lot of videos and web cams. Lucas Kanade runs in real time comparing current frame to last. The system will greatly benefit from switching the tracking to a batch mode where every 10 seconds or so a KLT algorithm is run on a batch of frames. KLT uses all the frames to track the points and is a lot more robust.

Unfortunately this is take a lot of work to change the code use a batch of the points instead of a real time, additionally it will put a lot of load on the cpu at set intervals instead of spreading it out as it is now.

### Compare HR from wearable movesense device with value found here

### Implement a way to store the wearable data with the video to use for training

### Move key parameters to a config file for training

### Using a learning algorithm to tune the parameters of the system to get the best result

### Investigate switching from PCA analysis to a Fourier Transform
Fourier Transform might perform a lot better and would work well on the 30 hz signals heartrates will never be any more than 4 beats a second so fourier should be good up to 15 a second with a sample rate of 30 a second. 

### Fix start up bug

### Add noise detection features and an accuracy estimate
It should be possible to pre process and analsys the video to determine how noisy it is.

### Add respiration rate detection

### Add skin temperature sensing

### Investigate Blood pressure detection

### Add system to flutter app using flutter FFI

