#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include "lasercut.hpp"

using namespace std;
using namespace cv;
using namespace furry;

int main (int argc, char** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: program <original image> <distance transform image>\n";
    return 1;
  }
  auto image = imread(argv[1]);
  auto dt_image = imread(argv[2], 0);
  Mat mask(image.size(), CV_8UC1);
  // Mark probably background with 0 and probably forground with 1
  threshold(dt_image, mask, 200, 1, THRESH_BINARY_INV);
  // Mark forground with 2
  rectangle(mask, Rect(0, 1200, 1936, 100), Scalar(2), -1);
  vector<unsigned char> lut(256);
  lut[0] = GC_BGD;
  lut[1] = GC_PR_FGD;
  lut[2] = GC_FGD;
  LUT(mask, lut, mask);

  Mat bgd_model, fgd_model;
  lasercut(image, mask, Rect(), bgd_model, fgd_model, 0, GC_INIT_WITH_MASK);

  for (int i = 0; i < 20; ++i)
  {
    grabCut(image, mask, Rect(), bgd_model, fgd_model, 1, GC_EVAL);
    stringstream filename;
    filename << "mask_" << i << ".jpg";
    cout << filename.str() << endl;
    imwrite(filename.str(), mask * 63);
  }

  Mat overlay, mask_rgb;
  cvtColor(mask, mask_rgb, CV_GRAY2BGR);
  mask_rgb *= 63;
  addWeighted(image, 0.5, mask_rgb, 0.5, 0.0, overlay);
  string window_name = "mask";
  namedWindow(window_name, CV_WINDOW_NORMAL);
  imshow(window_name, overlay);
  waitKey();

  return 0;
}
