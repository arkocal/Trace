/*
Copyright (c) 2015 Ali Rasim Kocal <arkocal@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;

class Tracer{
private:
  int frames;
  Mat cameraFrame;
  VideoCapture stream;

public:
  Tracer();
  void mainloop();
};


Tracer::Tracer()
{
  // Setup video stream
  stream = VideoCapture(0);
  stream.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
  stream.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);

  if (!stream.isOpened()) {
    cout << "Can not access camera at 0" << endl;
    exit(EXIT_FAILURE);
  }
  stream.read(cameraFrame);
  frames = 0;
}

void Tracer::mainloop() {
  while (true) {
    stream.read(cameraFrame);
    frames++;

    for (int i=0; i<100; i++) {
      for (int j=0; j<100; j++) {
	Vec3b &c = cameraFrame.at<Vec3b>(Point(i,j));
	c[0] = 0;
	c[1] = 0;
	c[2] = 255;
      }
    }

    imshow("Camera", cameraFrame);
    if (waitKey(1) >= 0)
      break;
  }
  cout << "Frames:" << frames << endl;
}

int main() {
  Tracer* t = new Tracer();
  t->mainloop();
  return 0;
}
