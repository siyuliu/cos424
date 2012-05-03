#ifndef _FURRY_LASER_CUT_
#define _FURRY_LASER_CUT_

#include <opencv2/core/core.hpp>

namespace furry
{

enum
  {
    LC_BGD    = 0,  //!< background
    LC_FGD    = 1,  //!< foreground
    LC_PR_BGD = 2,  //!< most probably background
    LC_PR_FGD = 3   //!< most probably foreground 
  };


//! GrabCut algorithm flags
enum
  {

    LC_INIT_WITH_RECT  = 0,
    LC_INIT_WITH_MASK  = 1,
    LC_EVAL            = 2
  };


//! segments the image using GrabCut algorithm
void lasercut(cv::InputArray img, cv::InputOutputArray mask, cv::Rect rect, 
              cv::InputOutputArray bgdModel, cv::InputOutputArray fgdModel,
              int iterCount, int mode = LC_EVAL );

} // furry

#endif
