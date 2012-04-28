#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

template <typename T> T
max(const Mat& m)
{
  T result = m.at<T>(0, 0);
  for (int i = 0; i < m.rows; ++i)
  {
    for (int j = 0; j < m.cols; ++j)
    {
      if (result < m.at<T>(i, j))
        result = m.at<T>(i, j);
    }
  }
  return result;
}

int main (int argc, char** argv)
{
  if (argc != 3)
  {
    cerr << "useage: program <image file name> <points file name>\n";
    return 1;
  }
  Mat image = imread(argv[1]);

  // read points in
  ifstream points_file(argv[2]);
  vector<Point2d> points;
  while (points_file.good())
  {
    Point2d p;
    points_file >> p.x >> p.y;
    points.push_back(p);
  }

  Mat point_image = Mat::ones(image.size(), CV_8UC1);
  for (auto it = points.begin(); it != points.end(); ++it)
    point_image.at<unsigned char>(it->y, it->x) = 0;
  Mat dist_image;
  distanceTransform(point_image, dist_image, CV_DIST_L2, CV_DIST_MASK_PRECISE);
  cout << max<float>(dist_image) << endl;
  dist_image /= max<float>(dist_image);
  //threshold(dist_image, dist_image, 0.1, 1, THRESH_BINARY);

  string window_name = "distance transform";
  /*namedWindow(window_name, CV_WINDOW_NORMAL);
  imshow(window_name, dist_image);
  waitKey();*/
  imwrite("dt.jpg", dist_image * 255);

  return 0;
}

