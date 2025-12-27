// tests/mock/TestDvpCamera.h
#pragma once
#include <memory>

#include "DvpCameraCapture.hpp"
#include "DvpConfig.hpp"
#include "IDVPCamera.h"

class TestDvpCamera : public DvpCameraCapture {
 public:
  explicit TestDvpCamera(std::shared_ptr<IDVPCamera> mock_camera)
      : DvpCameraCapture(0),  // 传递虚拟句柄
        mock_camera_(mock_camera) {
    // 初始化模拟相机
  }

  virtual ~TestDvpCamera() {
    if (is_running()) {
      stop();
    }
    // 在析构时调用disconnect确保资源释放
    if (mock_camera_) {
      mock_camera_->disconnect();
    }
  }

  // 重写关键方法以使用 Mock
  void set_config(const DvpConfig& config) override {
    if (mock_camera_) {
      mock_camera_->setConfig(config);
    }
  }

  DvpConfig get_config() const override {
    if (mock_camera_) {
      return mock_camera_->getConfig();
    }
    return DvpConfig();
  }

  bool start(const FrameProcessor& processor) override {
    if (!mock_camera_) return false;

    // 存储回调
    processor_ = std::make_shared<FrameProcessor>(processor);

    // 注册回调到 Mock 相机
    mock_camera_->registerFrameCallback(processor_);

    // 开始捕获
    mock_camera_->startCapture();

    return true;
  }

  void stop() override {
    if (mock_camera_) {
      mock_camera_->stopCapture();
      mock_camera_->disconnect();
    }
    processor_.reset();
  }

  bool is_running() const {
    if (mock_camera_) {
      return mock_camera_->isCapturing();
    }
    return false;
  }

  void register_frame_callback(const FrameProcessor& proc) {
    processor_ = std::make_shared<FrameProcessor>(proc);
    if (mock_camera_) {
      mock_camera_->registerFrameCallback(processor_);
    }
  }

  // 模拟帧数据注入
  void inject_frame(const std::vector<uint8_t>& frame_data, int width,
                    int height) {
    if (processor_) {
      // 创建一个CapturedFrame对象来传递给processor
      CapturedFrame frame;
      frame.data = frame_data;
      frame.meta.iWidth = width;
      frame.meta.iHeight = height;
      processor_->process(frame);
    }
  }

  // 获取 Mock 相机用于设置期望
  std::shared_ptr<IDVPCamera> get_mock_camera() const { return mock_camera_; }

 private:
  std::shared_ptr<IDVPCamera> mock_camera_;
  std::shared_ptr<FrameProcessor> processor_;
};
