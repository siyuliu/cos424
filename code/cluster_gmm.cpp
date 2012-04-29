#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <opencv2/ml/ml.hpp>
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
  if (argc != 5)
  {
    cerr << "useage: program <image file name> <dst_transform file name> <threshold on mask in [0,1]> <number of clusters N>\n";
    return 1;
  }

  // read in image
  Mat image = imread(argv[1]);
  // read distance image and get binary imagei
  cout << image.channels() << "-channel image" << endl;
  Mat dt = imread(argv[2]);
  float thres = atof(argv[3]);
  int N = atoi(argv[4]);
 
  Mat mask;
  cvtColor(dt, mask, CV_RGB2GRAY);
  thres *= max<uchar>(mask);
  mask = mask >= thres;
  int nsample = mask.size().width * mask.size().height;
  int height = mask.size().height;
  mask = mask.reshape(0, nsample);
  int pixels = countNonZero(mask);
  cout << "Number of pixels selected = " << pixels << " (" << ((pixels+0.0)/nsample) << ")" << endl;
  // cout << mask.rowRange(0,5) << endl; 
  // split channels
  vector<Mat> rgbs;
  split(image, rgbs);
  Mat sample = Mat::zeros(0, nsample, rgbs[0].type());
  Mat label;
  // preprocess cluster matrix
  for (int i = 0; i < 3; i++) {
    rgbs[i] = rgbs[i].reshape(0, 1);
    sample.push_back(rgbs[i].row(0));
  }
  sample = sample.t();
  sample.convertTo(sample, CV_32FC1);

  // run EM
  CvEM gmm;
  CvEMParams params;
  
  // initialize model parameters,
  params.covs = NULL;
  params.means = NULL;
  params.weights = NULL;
  params.probs = NULL;
  params.nclusters = N;
  params.cov_mat_type = CvEM::COV_MAT_SPHERICAL;
  params.start_step = CvEM::START_AUTO_STEP;
  params.term_crit.max_iter = 10;
  params.term_crit.epsilon = 0.1;
  params.term_crit.type = CV_TERMCRIT_ITER|CV_TERMCRIT_EPS;

  // train GMM
  gmm.train(sample, mask, params);

  Mat means = gmm.getMeans();
  means.convertTo(means, rgbs[0].type());  
  cout << "Cluster means = " << endl;
  cout << means << endl;
  
  Mat p(1, 3, CV_32FC1);
  Mat rep = Mat::zeros(0, 3, rgbs[0].type());
  Mat z = Mat::zeros(1, 3, means.type());
  // get labels
  for (int i = 0; i < nsample; i++) {
    if (mask.at<uchar>(i) == 0) {
      rep.push_back(z.row(0));
      continue;
    }
    p.at<float>(0) = sample.at<float>(i,0);
    p.at<float>(1) = sample.at<float>(i,1);
    p.at<float>(2) = sample.at<float>(i,2);
    int resp = round(gmm.predict(p));
    rep.push_back(means.row(resp));
  }

  // merge channels back
  rep = rep.t();
  for (int i = 0; i < 3; i++) {
    rgbs[i] = rep.row(i);
    rgbs[i] = rgbs[i].reshape(0, image.size().height);
  }
  merge(rgbs, image);
  // image.convertTo(image, CV_8UC1);
  cout << "Output image size = " << rgbs[0].size().height << "x" << rgbs[0].size().width << endl;

  // show and write out image
  imwrite("cluster.jpg", image);
  string window_name = "clustering result";
  namedWindow(window_name, CV_WINDOW_NORMAL);
  imshow(window_name, image);
  waitKey();

  return 0;
}

