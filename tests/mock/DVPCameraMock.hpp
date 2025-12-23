#pragma once
#include <gmock/gmock.h>

#include <memory>

#include "DVPCamera.h"
#include "DvpCapture.hpp"
#include "DvpConfig.hpp"
#include "IDVPCamera.h"

// 模拟一下dvpRegion结构
struct MockDvpRegion {
  int X, Y, W, H;
  MockDvpRegion(int x, int y, int w, int h) : X(x), Y(y), W(w), H(h) {}
};

// 模拟相机设备信息
struct MockCameraInfo {
  std::string user_id;
  std::string friendly_name;
  bool is_connected = false;
};

// Mock相机接口
class DVPCameraMock : public IDVPCamera {
 public:
  virtual ~DVPCameraMock() = default;

  MOCK_METHOD(bool, connect, (const std::string& id), (override));
  MOCK_METHOD(void, disconnect, (), (override));
  MOCK_METHOD(bool, isConnected, (), (const, override));

  MOCK_METHOD(void, setExposure, (double us), (override));
  MOCK_METHOD(double, getExposure, (), (const, override));

  MOCK_METHOD(void, setGain, (float gain), (override));
  MOCK_METHOD(float, getGain, (), (const, override));

  MOCK_METHOD(void, setHardwareIsp, (bool enable), (override));
  MOCK_METHOD(bool, getHardwareIsp, (), (const, override));

  MOCK_METHOD(void, setTriggerMode, (bool enable), (override));
  MOCK_METHOD(bool, getTriggerMode, (), (const, override));

  MOCK_METHOD(void, setConfig, (const DvpConfig& config), (override));
  MOCK_METHOD(DvpConfig, getConfig, (), (const, override));

  MOCK_METHOD(void, startCapture, (), (override));
  MOCK_METHOD(void, stopCapture, (), (override));
  MOCK_METHOD(bool, isCapturing, (), (const, override));

  MOCK_METHOD(std::vector<uint8_t>, getFrameData, (), (override));
  MOCK_METHOD(void, registerFrameCallback,
              (const std::shared_ptr<FrameProcessor>& callback), (override));
};

// 模拟DvpCapture
class MockDvpCapture : public DvpCapture {
 public:
  // 添加构造函数以解决默认构造函数被删除的问题
  MockDvpCapture() : DvpCapture(0) {}  // 传递一个默认的dvpHandle值

  // 使用MOCK_METHOD宏定义模拟方法，不再使用override关键字
  MOCK_METHOD(void, set_config, (const DvpConfig& config), ());
  MOCK_METHOD(DvpConfig, get_config, (), (const));
  MOCK_METHOD(bool, start, (const FrameProcessor& processor), ());
  MOCK_METHOD(void, stop, (), ());
  MOCK_METHOD(bool, is_running, (), (const));
  MOCK_METHOD(void, register_frame_callback, (const FrameProcessor& proc), ());
  MOCK_METHOD(void, register_event_callback,
              (DvpEventType type, const DvpEventHandler& handler), ());
};
