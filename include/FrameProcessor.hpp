/*
 *  Copyright © 2025 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: FrameProcessor.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#pragma once

#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "DvpCamera.h"

// 帧数据结构,无关于相机类型
struct CapturedFrame {
  std::vector<uint8_t> data;  // 图像数据
  // TODO 未来需要用union来存储来自不同相机的元信息
  dvpFrame meta;  // 完整元信息（宽/高/格式/曝光等）

  // 便捷访问
  int width() const { return meta.iWidth; }
  int height() const { return meta.iHeight; }
  int format() const { return meta.format; }
  double exposure_us() const { return meta.fExposure; }
  double timestamp_us() const { return static_cast<double>(meta.uTimestamp); }
};
// 帧处理器接口
class FrameProcessor {
 public:
  virtual ~FrameProcessor() = default;

  virtual void process(const CapturedFrame& frame) {
    // 默认空实现
  }

  // 添加默认构造函数以允许赋值
  FrameProcessor() = default;
  // 添加拷贝构造函数和赋值操作符
  FrameProcessor(const FrameProcessor&) = default;
  FrameProcessor& operator=(const FrameProcessor&) = default;
};

// 函数对象包装器，允许使用函数指针或lambda表达式
template <typename Func>
class FunctionFrameProcessor : public FrameProcessor {
 public:
  explicit FunctionFrameProcessor(Func func) : func_(std::move(func)) {}

  void process(const CapturedFrame& frame) override { func_(frame); }

 private:
  Func func_;
};

// 辅助函数，创建函数对象帧处理器，这个只有函数才需要用这个
template <typename Func>
FunctionFrameProcessor<Func> make_function_processor(Func func) {
  return FunctionFrameProcessor<Func>(std::move(func));
}
