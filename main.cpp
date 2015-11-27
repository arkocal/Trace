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
const Vec3b YELLOW(55,170,130);
const double THRESHOLD = 30;
const int STEP=10;

class Tracer{
private:
  int frames;
  Mat cameraFrame;
  VideoCapture stream;
  // Map in which frame the pixel was yellow for the last time.
  int* matchesMap;
public:
  Tracer();
  void mainloop(bool);
};


Tracer::Tracer()
{
  // Setup video stream
  stream = VideoCapture("input.webm");

  if (!stream.isOpened()) {
    cout << "Can not access input.webm" << endl;
    exit(EXIT_FAILURE);
  }
  stream.read(cameraFrame);
  frames = 0;

  matchesMap = new int[WIDTH*HEIGHT];
  for (int i=0; i<WIDTH*HEIGHT; i++)
	matchesMap[i] = -1;
}

void Tracer::mainloop(bool displayFrames) {
  while (true) {
    if (!stream.read(cameraFrame))
      break;
    frames++;

    int countMatches = 0;
    int matches[WIDTH*HEIGHT][2];

    for (int i=0; i<WIDTH; i+=10) {
      for (int j=0; j<HEIGHT; j+=10) {
	Vec3b &c = cameraFrame.at<Vec3b>(Point(i,j));
	if (norm(YELLOW-c)<THRESHOLD) {
	    matches[countMatches][0] = i;
	    matches[countMatches][1] = j;
	    countMatches++;
	    matchesMap[i+WIDTH*j] = frames;
	}
      }
    }

    if (displayFrames) {
      // Mark frames
      for (int i=0; i<countMatches; i++) {
	int x = matches[i][0];
	int y = matches[i][1];
	Vec3b &color = cameraFrame.at<Vec3b>(Point(x,y));
	color[0] = 0;
	color[1] = 0;
	color[2] = 255;
      }
      imshow("Camera", cameraFrame);
      if (waitKey(40) >= 0)
	break;
    }
  }
  cout << "Frames:" << frames << endl;
}

int main(int argc, char *argv[]) {
  Tracer* t = new Tracer();
  bool displayFrames;
  for (int i=0; i<argc; i++) {
    if(strcmp("show", argv[i])==0){
      displayFrames = true;
      break;
    }
  }
  t->mainloop(displayFrames);
  return 0;
}
