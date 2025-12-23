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
 *  - File: example.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include <utility>
#include <vector>

#include "DvpCameraBuilder.hpp"
#include "DvpCameraManager.hpp"
#include "DvpEventManager.hpp"
#include "FrameProcessor.hpp"
#include "MultiCameraCoordinator.hpp"
#include "algo/AlgoBase.hpp"
#include "algo/HoleDetection.hpp"
#include "config/ConfigManager.hpp"
// for cv2
#include <opencv2/opencv.hpp>

// // ── Simple DEMO ────────────────────────────────────────────────────────

// /**/
// // We do not support use simple function as FrameProcessor directly,
// // We had better use a Class with operator() to process frame.
// void defect_processor(const CapturedFrame& frame) {
//   // cv::Mat img(frame.height, frame.width, CV_8UC3, frame.data);
//   // auto defects = detect_holes(img);
//   // std::cout << "[ALGO] Processed frame with size: " << frame.size << "\n";
// }

// int main() {
//   auto& config_manager = config::ConfigManager::instance();
//   config_manager.start();
//   auto initial_config = config_manager.get_current_config();
//   // we create some algorithms here bro.

//   // we automatically bind the observer with the up-to-date Algo' config.
//   // In this @create_algorithm function, we extract the config and then
//   // extract the config and then create the algorithm.
//   auto holeDetection =
//   config_manager.create_algorithm<algo::HoleDetection>();
//   // some parameters configuration

//   // holeDetection->configure("threshold", "0.7");
//   // edgeDetection->configure("lowThreshold", "100");
//   // edgeDetection->configure("ratio", "2.5");

//   // Solution 1: Using function  (I have used a template function and class
//   to
//   // wrap the function into a class)
//   // You can check this function above for more detail
//   // @make_function_processor
//   /*
//   auto capture1 =
//       DvpCameraBuilder::fromUserId("MyCamera01")
//           .exposure(10000.0)
//           .gain(1.5f)
//           .roi(0, 0, 1920, 1080)
//           .hardwareIsp(true)
//           .triggerMode(false)
//           .autoExposure(true)             // 启用自动曝光
//           .autoGain(true)                 // 启用自动增益
//           .aeTargetBrightness(120)        // 设置自动曝光目标亮度
//           .antiFlickerMode(1)             // 设置抗频闪模式 (1=50Hz, 2=60Hz)
//           .exposureRange(100.0, 50000.0)  // 设置曝光时间范围
//           .gainRange(1.0f, 16.0f)         // 设置增益范围
//           .contrast(10)                   // 设置对比度
//           .gamma(1)                    // 设置伽马值
//           .saturation(120)                // 设置饱和度
//           .sharpnessEnable(true)          // 启用锐度
//           .sharpness(50)                  // 设置锐度值
//           .inverseState(true)             // 启用负片效果
//           .flipHorizontalState(true)      // 启用横向翻转
//           .flipVerticalState(false)       // 禁用纵向翻转
//           .rotateState(true)              // 启用旋转
//           .rotateOpposite(false)          // 顺时针旋转
//           .blackLevel(0.1f)              // 设置黑电平
//           .colorTemperature(5000)         // 设置色温为5000K
//           .flatFieldState(true)           // 启用平场校正
//           .defectFixState(true)           // 启用缺陷像素校正
//           .monoState(false)               // 禁用去色
//           .aeRoi(100, 100, 800, 600)      // 设置自动曝光ROI
//           .awbRoi(100, 100, 800, 600)     // 设置自动白平衡ROI
//           .coolerState(true)              // 启用制冷器
//           .bufferQueueSize(10)            // 设置缓存队列大小为10
//           .linkTimeout(5000)              // 设置连接超时时间为5000毫秒
//           .awbOperation(1)                // 启用自动白平衡
//           .triggerActivation(0)           // 设置触发激活方式为上升沿触发
//           .triggerCount(1)                // 设置触发计数为1
//           .triggerDebouncer(100.0)        // 设置触发消抖时间为100微秒
//           .strobeSource(1)                // 设置Strobe信号源为低电平
//           .strobeDelay(50.0)              // 设置Strobe信号延迟为50微秒
//           .strobeDuration(200.0)          // 设置Strobe信号持续时间为200微秒
//           .lineTrigEnable(false)          // 禁用线扫触发
//           .acquisitionFrameRate(30.0)     // 设置帧率为30fps
//           .acquisitionFrameRateEnable(true) // 启用帧率控制
//           .flatFieldEnable(true)          // 启用平场
//           .onFrame(make_function_processor(defect_processor))
//           .onEvent(DvpEventType::FrameLost,
//                    [](const DvpEventContext&) { std::cerr << "Frame lost!\n";
//                    })
//           .onEvent(
//               DvpEventType::Reconnected,
//               [](const DvpEventContext&) { std::cout << "Reconnected!\n"; })
//           .onEvent(
//               DvpEventType::FrameTimeout,
//               [](const DvpEventContext&) { std::cout << "Frame timeout!\n";
//               })
//           .onEvent(
//               DvpEventType::StreamStarted,
//               [](const DvpEventContext&) { std::cout << "Stream started!\n";
//               })
//           .onEvent(
//               DvpEventType::StreamStopped,
//               [](const DvpEventContext&) { std::cout << "Stream stopped!\n";
//               })
//           .onEvent(
//               DvpEventType::FrameStart,
//               [](const DvpEventContext&) { std::cout << "Frame start!\n"; })
//           .onEvent(DvpEventType::FrameEnd,
//                    [](const DvpEventContext&) { std::cout << "Frame end!\n";
//                    })
//           .build();
//           */

//   // Solution 2: Using algorithm plugin with trigger mode
//   auto capture2 =
//       DvpCameraBuilder::fromUserId("MyCamera02")
//           .exposure(15000.0)
//           .gain(2.0f)
//           .roi(0, 0, 1920, 1080)
//           .hardwareIsp(true)
//           .triggerMode(true)                       // 启用触发模式
//           .triggerSource(TRIGGER_SOURCE_SOFTWARE)  // 软触发
//           .triggerDelay(100.0)                     // 触发延迟100微秒
//           .acquisitionMode(2)                      // 多帧采集模式
//           .autoExposure(false)         //
//           禁用自动曝光（触发模式下通常手动控制） .autoGain(false) //
//           禁用自动增益 .inverseState(false)         // 禁用负片效果
//           .flipHorizontalState(false)  // 禁用横向翻转
//           .flipVerticalState(true)     // 启用纵向翻转
//           .rotateState(false)          // 禁用旋转
//           .blackLevel(0.05f)           // 设置黑电平
//           .colorTemperature(6500)      // 设置色温为6500K(默认)
//           .flatFieldState(false)       // 禁用平场校正
//           .defectFixState(false)       // 禁用缺陷像素校正
//           .monoState(true)             // 启用去色
//           .aeRoi(200, 200, 600, 400)   // 设置自动曝光ROI
//           .awbRoi(200, 200, 600, 400)  // 设置自动白平衡ROI
//           .coolerState(false)          // 禁用制冷器
//           .bufferQueueSize(5)          // 设置缓存队列大小为5
//           .linkTimeout(3000)           // 设置连接超时时间为3000毫秒
//           .awbOperation(0)             // 禁用自动白平衡(手动)
//           .triggerActivation(1)        // 设置触发激活方式为下降沿触发
//           .triggerCount(3)             // 设置触发计数为3
//           .triggerDebouncer(50.0)      // 设置触发消抖时间为50微秒
//           .strobeSource(2)             // 设置Strobe信号源为高电平
//           .strobeDelay(25.0)           // 设置Strobe信号延迟为25微秒
//           .strobeDuration(100.0)       // 设置Strobe信号持续时间为100微秒
//           .lineTrigEnable(true)        // 启用线扫触发
//           .lineTrigSource(1)           // 设置线扫触发源为Line1
//           .lineTrigFilter(0.1)         // 设置线扫触发过滤为0.1
//           .lineTrigEdgeSel(0)          // 设置线扫触发边沿选择为默认
//           .lineTrigDelay(10.0)         // 设置线扫触发延时为10微秒
//           .lineTrigDebouncer(5.0)      // 设置线扫触发消抖为5微秒
//           .acquisitionFrameRate(60.0)  // 设置帧率为60fps
//           .acquisitionFrameRateEnable(true)  // 启用帧率控制
//           .flatFieldEnable(false)            // 禁用平场
//           .onFrame(algo::AlgoAdapter(holeDetection))
//           .onEvent(DvpEventType::FrameLost,
//                    [](const DvpEventContext&) { std::cerr << "Frame lost!\n";
//                    })
//           .onEvent(
//               DvpEventType::Reconnected,
//               [](const DvpEventContext&) { std::cout << "Reconnected!\n"; })
//           .onEvent(
//               DvpEventType::FrameTimeout,
//               [](const DvpEventContext&) { std::cout << "Frame timeout!\n";
//               })
//           .onEvent(
//               DvpEventType::StreamStarted,
//               [](const DvpEventContext&) { std::cout << "Stream started!\n";
//               })
//           .onEvent(
//               DvpEventType::StreamStopped,
//               [](const DvpEventContext&) { std::cout << "Stream stopped!\n";
//               })
//           .onEvent(
//               DvpEventType::FrameStart,
//               [](const DvpEventContext&) { std::cout << "Frame start!\n"; })
//           .onEvent(DvpEventType::FrameEnd,
//                    [](const DvpEventContext&) { std::cout << "Frame end!\n";
//                    })
//           .build();

//   /*
// // Solution 3: Using hardware trigger
// auto capture3 =
// DvpCameraBuilder::fromUserId("MyCamera03")
//   .exposure(8000.0)
//   .gain(1.0f)
//   .roi(0, 0, 1280, 720)
//   .hardwareIsp(true)
//   .triggerMode(true)                    // 启用触发模式
//   .triggerSource(TRIGGER_SOURCE_LINE1)  // 硬件触发 (Line1)
//   .triggerDelay(50.0)                   // 触发延迟50微秒
//   .antiFlickerMode(2)                   // 60Hz抗频闪
//   .inverseState(true)                   // 启用负片效果
//   .flipHorizontalState(false)           // 禁用横向翻转
//   .flipVerticalState(false)             // 禁用纵向翻转
//   .rotateState(true)                    // 启用旋转
//   .rotateOpposite(true)                 // 逆时针旋转
//   .monoState(false)                     // 禁用去色
//   .aeRoi(150, 150, 500, 300)            // 设置自动曝光ROI
//   .awbRoi(150, 150, 500, 300)           // 设置自动白平衡ROI
//   .coolerState(true)                    // 启用制冷器
//   .bufferQueueSize(8)                   // 设置缓存队列大小为8
//   .linkTimeout(4000)                    // 设置连接超时时间为4000毫秒
//   .awbOperation(1)                      // 启用自动白平衡
//   .triggerActivation(0)                 // 设置触发激活方式为上升沿触发
//   .triggerCount(1)                      // 设置触发计数为1
//   .triggerDebouncer(100.0)              // 设置触发消抖时间为100微秒
//   .strobeSource(1)                      // 设置Strobe信号源为低电平
//   .strobeDelay(50.0)                    // 设置Strobe信号延迟为50微秒
//   .strobeDuration(200.0)                // 设置Strobe信号持续时间为200微秒
//   .lineTrigEnable(false)                // 禁用线扫触发
//   .acquisitionFrameRate(30.0)           // 设置帧率为30fps
//   .acquisitionFrameRateEnable(true)     // 启用帧率控制
//   .flatFieldEnable(true)                // 启用平场
//   .onFrame(algo::AlgoAdapter(edgeDetection))
//   .onEvent(DvpEventType::FrameLost,
//            [](const DvpEventContext&) { std::cerr << "Frame lost!\n"; })
//   .build();
// */
//   /*
//     if (!capture1 && !capture2 && !capture3) {
//       std::cerr << "Failed to start any camera!\n";
//       return -1;
//     }
//   */
//   if (!capture2) {
//     std::cerr << "Failed to start camera!\n";
//     return -1;
//   }

//   std::cout << "Running... Press Enter to exit.\n";
//   std::cin.get();
//   return 0;
// }

//
// ─────────────Using DvpCameraManager with multiple cameras─────────────

int main() {
  auto& config_manager = config::ConfigManager::instance();
  config_manager.start();
  auto initial_config = config_manager.get_current_config();

  // === 1. 创建融合策略（外部传入）===
  auto fusion_strategy =
      [](const std::vector<CapturedFrame>& frames) -> CapturedFrame {
    // TODO(cmx): 后期要根据具体情况实现图像的拼接、去重、对齐
    // 这里A就可以不用写策略类接口，而是只需创建一个临时LAMBDA函数
    // 示例：简单拼接（左右排列）
    if (frames.size() != 2) {
      return {};
    }

    cv::Mat img0(frames[0].height(), frames[0].width(), CV_8UC1,
                 (void*)frames[0].data.data());  // NOLINT
    cv::Mat img1(frames[1].height(), frames[1].width(), CV_8UC1,
                 (void*)frames[1].data.data());  // NOLINT

    cv::Mat fused;
    cv::hconcat(img0, img1, fused);  // OpenCV 拼接

    CapturedFrame result;
    result.data.assign(fused.data,
                       fused.data + fused.total() * fused.elemSize());
    result.meta = frames[0].meta;  // 继承元信息
    // 可更新 width 等
    return result;
  };
  // 设置融合后的处理（例如 HoleDetection）
  auto holeDetection = std::make_shared<algo::HoleDetection>();

  // === 2. 创建协同器 ===
  // 直接栈对象，无需 shared_ptr,因为只要main不退出,coordinator对象就是有效的
  MultiCameraCoordinator coordinator(2, fusion_strategy);

  // 将我们的算法传入融合器中作为预处理之后的下游处理器
  coordinator.set_downstream_processor(
      std::make_unique<algo::AlgoAdapter>(holeDetection));

  // === 3. 构建相机，绑定协同器的处理器 ===
  auto cam1 = DvpCameraBuilder::fromFriendlyName("cam1")
                  .onFrame(coordinator[0])
                  .triggerMode(true)
                  .triggerSource(TRIGGER_SOURCE_SOFTWARE)
                  .exposure(10000.0)
                  .build();

  auto cam2 = DvpCameraBuilder::fromFriendlyName("cam2")
                  .onFrame(coordinator[1])
                  .triggerMode(true)
                  .triggerSource(TRIGGER_SOURCE_SOFTWARE)
                  .exposure(10000.0)
                  .build();

  if (!cam1 || !cam2) {
    std::cerr << "Failed to build cameras!\n";
    return -1;
  }

  // === 4. 启动 ===
  DvpCameraManager manager{2};
  manager.add_camera(std::move(cam1));
  manager.add_camera(std::move(cam2));
  manager.start_all();

  std::cout << "Running... Press Enter to exit.\n";
  std::cin.get();
  return 0;
}
