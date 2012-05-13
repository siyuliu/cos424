#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <gflags/gflags.h>

#include "lasercut.hpp"

using namespace std;
using namespace cv;
using namespace furry;

DEFINE_string(data_path, "../../data/", "");
DEFINE_string(image_name, "image_00_00.jpg", "");
DEFINE_string(dt_image_name, "dt_00_00.jpg",
              "distance transfrom image file name");
DEFINE_string(dist_image_name, "sobel_00_00.png", "");

int main (int argc, char** argv)
{
  google::SetUsageMessage("");
  google::ParseCommandLineFlags(&argc, &argv, true);

  cout << "version: 10\n";

  auto image = imread(FLAGS_data_path + "/" + FLAGS_image_name);
  auto dt_image = imread(FLAGS_data_path + "/" + FLAGS_dt_image_name, 0);
  auto dist_image = imread(FLAGS_data_path + "/" + FLAGS_dist_image_name, 0);
  dist_image.convertTo(dist_image, CV_32F);
  dist_image /= 255;
  dist_image = dist_image * 0.8 + 0.2;
  //threshold(dist_image, dist_image, 0, 1, THRESH_TOZERO);

  /*namedWindow("sobel", CV_WINDOW_NORMAL);
  imshow("sobel", dist_image);
  waitKey(0);*/
  //dist_image = dist_image;
  dist_image = Mat::ones(dist_image.size(), CV_64F);

  Mat mask(image.size(), CV_8UC1);
  // Mark probably background with 0 and probably forground with 1
  threshold(dt_image, mask, 60, 1, THRESH_BINARY_INV);
  // Mark forground with 2
  //rectangle(mask, Rect(0, 1200, 1936, 100), Scalar(2), -1);
  vector<unsigned char> lut(256);
  lut[0] = GC_PR_BGD;
  lut[1] = GC_PR_FGD;
  lut[2] = GC_FGD;
  LUT(mask, lut, mask);

  Mat bgd_model, fgd_model;
  lasercut(image, dist_image,
           mask, Rect(), bgd_model, fgd_model, 0, GC_INIT_WITH_MASK);

  for (int i = 0; i < 20; ++i)
  {
    lasercut(image, dist_image,
             mask, Rect(), bgd_model, fgd_model, 1, GC_EVAL);
    stringstream filename;
    filename << "mask_" << i << ".jpg";
    cout << filename.str() << endl;
    imwrite(filename.str(), mask * 63);
  }

  Mat overlay, mask_rgb;
  lut = vector<unsigned char>(256, 0);
  lut[GC_PR_BGD] = 0;
  lut[GC_PR_FGD] = 1;
  LUT(mask, lut, mask);
  mask *= 255;
  vector<Mat> ms(3);
  ms[0] = ms[1] = Mat::zeros(mask.size(), mask.type());
  ms[2] = mask;
  merge(ms, mask_rgb);
  image.copyTo(mask_rgb, mask);
  //cvtColor(mask, mask_rgb, CV_GRAY2BGR);
  //mask_rgb *= 255;
  addWeighted(image, 0.5, mask_rgb, 0.5, 0.0, overlay);

  imwrite("overlay.jpg", overlay);

  string window_name = "mask";
  namedWindow(window_name, CV_WINDOW_NORMAL);
  imshow(window_name, overlay);
  waitKey();

  return 0;
}
