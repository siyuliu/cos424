#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <gflags/gflags.h>

using namespace std;
using namespace cv;

DEFINE_string(points_file_name, "../data/points.txt",
              "the file path containing points");
DEFINE_string(result_file_name, "result.jpg",
              "the file path the save the result image");
DEFINE_double(sample_ratio, 2,
              "image sample ratio");
DEFINE_int32(image_width, 1936, "");
DEFINE_int32(image_height, 2592, "");


int main(int argc, char** argv)
{
  google::SetUsageMessage("");
  google::ParseCommandLineFlags(&argc, &argv, true);

  string points_file_name = FLAGS_points_file_name;
  Size image_size(FLAGS_image_width, FLAGS_image_height);
  double sample_ratio = FLAGS_sample_ratio;

  vector<Point2f> points;
  ifstream ifs(points_file_name.c_str());
  if (!ifs.good())
  {
    cerr << "Error: can not open file " << points_file_name << endl;
    return 1;
  }
  while(ifs.good())
  {
    Point2f tmp;
    ifs >> tmp.x >> tmp.y;
    points.push_back(tmp);
  }

  Mat point_image(image_size.height / sample_ratio + 0.5,
                  image_size.width / sample_ratio + 0.5,
                  CV_64FC1);
  for (auto it = points.begin(); it != points.end(); ++it)
  {
    if (it->y >= 0 && it->y < image_size.height &&
        it->x >= 0 && it->x < image_size.width)
      point_image.at<double>(it->y / sample_ratio + 0.5,
                             it->x / sample_ratio + 0.5) = 1;
  }

  Mat sobel_x_image, sobel_y_image;
  /*point_image = imread("../data/image_00_00.jpg", 0);
  point_image.convertTo(point_image, CV_64FC1);
  point_image /= 255;*/
  Sobel(point_image, sobel_x_image, -1, 1, 0, 3);
  Sobel(point_image, sobel_y_image, -1, 0, 1, 3);
  double max_value = 0;
  minMaxLoc(sobel_x_image, 0, &max_value);
  cout << "sobel x max value: " << max_value << endl;
  minMaxLoc(sobel_y_image, 0, &max_value);
  cout << "sobel y max value: " << max_value << endl;
  //sobel_image /= max_value;
  Mat sobel_image_s(point_image.size(), CV_64FC1);
  sqrt(sobel_x_image.mul(sobel_x_image) + sobel_y_image.mul(sobel_x_image),
       sobel_image_s);
  Mat sobel_image;
  resize(sobel_image_s, sobel_image, image_size, 0, 0, INTER_AREA);
  GaussianBlur(sobel_image, sobel_image, Size(7, 7), 0);
  minMaxLoc(sobel_image, 0, &max_value);
  cout << "sobel max value: " << max_value << endl;
  cout << "sobel image size: " << sobel_image.size().width << " "
       << sobel_image.size().height << endl;
  sobel_image /= max_value;

  imwrite(FLAGS_result_file_name, sobel_image * 255);

  namedWindow("points", CV_WINDOW_NORMAL);
  imshow("points", point_image);
  namedWindow("sobel", CV_WINDOW_NORMAL);
  imshow("sobel", sobel_image);
  waitKey(0);

  return 0;
}
