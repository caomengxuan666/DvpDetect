#pragma once
#include <opencv2/core/mat.hpp>

#include "FrameProcessor.hpp"  //NOLINT

namespace DvpUtils {
inline cv::Mat CapturedFrame2Mat(const CapturedFrame& frame) {
  // 可选：校验数据大小（根据 CV_8UC3 假设）
  // assert(frame.data.size() ==
  //        static_cast<size_t>(frame.width() * frame.height() * 3));

  return cv::Mat(frame.height(), frame.width(), CV_8UC3,
                 const_cast<uint8_t*>(frame.data.data()));
}
}  // namespace DvpUtils
