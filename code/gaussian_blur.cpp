#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;

const Size image_size(1936, 2592);

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    cerr << "usage: program <points txt> <result image>\n";
    return 1;
  }

  string points_file_name = argv[1];
  string blur_image_file_name = argv[2];

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

  double sample_rate = 8;
  Mat image(image_size.height / sample_rate,
            image_size.width / sample_rate,
            CV_64F);
  for (auto it = points.begin(); it != points.end(); ++it)
  {
    if (it->y >= 0 && it->y < image_size.height &&
        it->x >= 0 && it->x < image_size.width)
      image.at<double>(it->y / sample_rate + 0.5,
                       it->x / sample_rate + 0.5) = 1;
  }

  Mat blur_image;
  Size ksize(3, 3);
  double sigma_x = 0.3*((ksize.width-1)*0.5 - 1) + 0.8;
  double sigma_y = 0.3*((ksize.height-1)*0.5 - 1) + 0.8;
  Mat kernel_x = getGaussianKernel(ksize.width, 0, CV_64F);
  Mat kernel_y = getGaussianKernel(ksize.height, 0, CV_64F);
  GaussianBlur(image, blur_image, ksize, 0);

  cout << "*** sigma x: " << sigma_x << endl;
  cout << "*** sigma y: " << sigma_y << endl;
  cout << "*** kernel x: ";
  for (int i = 0; i < kernel_x.rows; ++i)
    cout << kernel_x.at<double>(i);
  cout << "\n*** kernel y: ";
  for (int i = 0; i < kernel_y.rows; ++i)
    cout << kernel_y.at<double>(i);
  cout << "\n*** kernel: \n";
  Mat kernel = kernel_y * kernel_x.t();
  for (int r = 0; r < kernel.rows; ++r)
  {
    for (int c = 0; c < kernel.cols; ++c)
    {
      cout << kernel.at<double>(r, c) << " ";
    }
    cout << endl;
  }

  imwrite(blur_image_file_name, blur_image);

  string window_name = "blur";
  namedWindow(window_name, CV_WINDOW_NORMAL);
  imshow(window_name, blur_image);
  namedWindow("image", CV_WINDOW_NORMAL);
  imshow("image", image);
  waitKey(0);

  return 0;
}
