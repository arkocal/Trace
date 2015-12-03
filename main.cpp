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
#include <vector>
#include "constants.hpp"
#include "utils.hpp"

using namespace cv;
using namespace std;
using namespace constants;

struct MPoint{
  int x;
  int y;
};

class Tracer{
private:
  int frames;
  Mat cameraFrame;
  VideoCapture stream;
  // Map in which frame the pixel was yellow for the last time.
  int* matchesMap;
  int stepMatches(int[][2]);
  int matchNeighbours(int, int[][2]);
  Point matchRing(vector<Point>, int, int);
  bool render(Point, vector<Point>, int[][2], int);
public:
  Tracer(char*);
  void mainloop(bool);
};


Tracer::Tracer(char* filename)
{
  // Setup video stream
  stream = VideoCapture(filename);

  cout << "Hey" << endl;
  if (!stream.isOpened()) {
    cout << "Can not access input" << endl;
    exit(EXIT_FAILURE);
  }
  stream.read(cameraFrame);
  frames = 0;

  matchesMap = new int[WIDTH*HEIGHT];
  for (int i=0; i<WIDTH*HEIGHT; i++)
	matchesMap[i] = -1;
}

/*
 * Find matches iterating with STEP and return number of matches found
 * \param matches Adress of array to save match coordinates.
 */
int Tracer::stepMatches(int matches[][2]) {
  int countMatches = 0;

  for (int i=0; i<WIDTH; i+=10) {
    for (int j=0; j<HEIGHT; j+=10) {
      Vec3b &c = cameraFrame.at<Vec3b>(Point(i,j));
      if (c[0]<160 && c[1]>200 && c[2]>200) {
	matches[countMatches][0] = i;
	matches[countMatches][1] = j;
	countMatches++;
	matchesMap[i+WIDTH*j] = frames;
      }
    }
  }
  return countMatches;
}

/*
 * Match the given ring with pixels found. Search around
 * x_orig and y_orig, return center of the ring with most
 * matches.
 */
Point Tracer::matchRing(vector<Point> r, int x_orig, int y_orig){
  Point best = Point(0,0);
  int best_score = 0;
  //TODO Replace magic numbers with constants
  for (int x=x_orig-16; x<x_orig+16; x++) {
    for (int y=y_orig-16; y<y_orig+16 ; y++) {
      int count = 0;
      for (int i=0; i<r.size(); i++) {
	Point p = r.at(i);
	if (p.x+x<0 || p.x+x>=WIDTH || p.y+y<0 || p.y+y>=HEIGHT)
	  continue;
	if (matchesMap[x+p.x+(y+p.y)*WIDTH]==frames)
	  count++;
      }
      if (count > best_score){
	best = Point(x, y);
	best_score = count;
      }
    }
  }
  return best;
}

/*
 * Mark the match ring and color matches. Return if interrupted by key.
 */
bool Tracer::render(Point center, vector<Point> ring, int matches[][2], int countMatches){
  for (int i=0; i<ring.size(); i++) {
    Vec3b &c = cameraFrame.at<Vec3b>(center+ring.at(i));
    c[0] = 255;
    c[1] = 0;
    c[2] = 0;
  }

  for (int i=0; i<countMatches; i++) {
    int x = matches[i][0];
    int y = matches[i][1];
    Vec3b &color = cameraFrame.at<Vec3b>(Point(x,y));
    color[0] = 0;
    color[1] = 0;
    color[2] = 255;
  }
  imshow("Camera", cameraFrame);
  return (waitKey(40) >= 0);
}

/*
 * Match neighbours of pixels found in stepMatches and return number of matches found.
 * \param countMatches How many matches have been found already.
 * \param matches Adress of array to save match coordinates.
 */
int Tracer::matchNeighbours(int countMatches, int matches[][2]) {
  // Neighbour matches
  for (int i=0; i<countMatches; i++) {
    int x = matches[i][0];
    int y = matches[i][1];
    for (int ox=-1; ox<=1; ox+=2) {
      if (x+ox < 0 || x+ox >= WIDTH)
	continue;
      for (int oy=-1; oy<=1; oy+=2) {
	if (y+oy < 0 || y+oy >= HEIGHT)
	  continue;
	Vec3b &color = cameraFrame.at<Vec3b>(Point(x+ox,y+oy));
	if (matchesMap[x+ox+(y+oy)*WIDTH]==frames)
	  continue;
	if (color[0]<160 && color[1]>200 && color[2]>200) {
	  matches[countMatches][0] = x+ox;
	  matches[countMatches][1] = y+oy;
	  matchesMap[x+ox+WIDTH*(y+oy)] = frames;
	  countMatches++;
	}
      }
    }
  }
  return countMatches;
}


void Tracer::mainloop(bool displayFrames) {
  vector<Point> r = utils::ring(RING_SIZE_INNER,RING_SIZE_OUTER);
  while (true) {
    if (!stream.read(cameraFrame))
      break;
    frames++;

    int matches[WIDTH*HEIGHT][2];
    int countMatches = stepMatches(matches);
    countMatches = matchNeighbours(countMatches, matches);
    Point center = matchRing(r, matches[0][0], matches[0][1]);

    if (displayFrames) {
      if(render(center, r, matches, countMatches))
	break;
    }
  }
  cout << "Frames:" << frames << endl;
}

int main(int argc, char *argv[]) {
  bool displayFrames=false;
  for (int i=0; i<argc; i++) {
    if(strcmp("show", argv[i])==0){
      displayFrames = true;
      break;
    }
  }
  Tracer* t = new Tracer(argv[1]);
  t->mainloop(displayFrames);
  return 0;
}
