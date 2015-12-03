#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "utils.hpp"

using namespace std;
using namespace cv;

vector<Point> utils::ring(int si, int so) {
  vector<Point> r = *(new vector<Point>());
  for (int i=-so+1; i<=so; i++) {
    for (int j=-so+1; j<=so; j++) {
      if (si*si<=i*i+j*j && so*so>=i*i+j*j){
	r.push_back(Point(i,j));
      }
    }
  }
  return r;
}
