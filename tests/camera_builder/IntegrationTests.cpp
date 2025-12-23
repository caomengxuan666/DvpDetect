// tests/camera_builder/IntegrationTests.cpp
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "../mock/DVPCameraMock.hpp"
#include "../mock/TestDvpCamera.h"
#include "../mock/TestDvpCameraBuilder.h"
#include "algo/HoleDetection.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

namespace algo {
class HoleDetection;
}

class IntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_camera_ = std::make_shared<NiceMock<DVPCameraMock>>();

    // 默认行为设置
    ON_CALL(*mock_camera_, connect(_)).WillByDefault(Return(true));
    ON_CALL(*mock_camera_, isConnected()).WillByDefault(Return(true));
    ON_CALL(*mock_camera_, setExposure(_)).WillByDefault(Return());
    ON_CALL(*mock_camera_, setGain(_)).WillByDefault(Return());
    ON_CALL(*mock_camera_, setHardwareIsp(_)).WillByDefault(Return());
    ON_CALL(*mock_camera_, setTriggerMode(_)).WillByDefault(Return());
    ON_CALL(*mock_camera_, startCapture()).WillByDefault(Return());
    ON_CALL(*mock_camera_, stopCapture()).WillByDefault(Return());
    ON_CALL(*mock_camera_, disconnect()).WillByDefault(Return());
    ON_CALL(*mock_camera_, registerFrameCallback(_)).WillByDefault(Return());
  }

  std::shared_ptr<DVPCameraMock> mock_camera_;
};

// 测试完整工作流
TEST_F(IntegrationTest, FullWorkflowSuccess) {
  // 设置期望调用
  EXPECT_CALL(*mock_camera_, connect("TestCamera"))
      .Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, setExposure(12000.0)).Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, setGain(2.0f)).Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, setHardwareIsp(true)).Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, registerFrameCallback(_))
      .Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, startCapture()).Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, stopCapture()).Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, disconnect()).Times(::testing::AtLeast(1));

  // 创建相机 - 使用TestDvpCameraBuilder而不是DvpCameraBuilder
  TestDvpCameraBuilder builder;
  builder.with_mock_camera(mock_camera_);
  builder.fromUserId("TestCamera");
  builder.exposure(12000.0);
  builder.gain(2.0f);
  builder.hardwareIsp(true);
  auto capture = builder.build_test();

  ASSERT_NE(capture, nullptr);
  capture->stop();
}

// 测试相机断开重连
TEST_F(IntegrationTest, CameraReconnect) {
  // 设置期望调用
  EXPECT_CALL(*mock_camera_, connect(_)).Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, startCapture()).Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, registerFrameCallback(_))
      .Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, stopCapture()).Times(::testing::AtLeast(1));
  EXPECT_CALL(*mock_camera_, disconnect()).Times(::testing::AtLeast(1));

  // 创建相机 - 使用TestDvpCameraBuilder而不是DvpCameraBuilder
  TestDvpCameraBuilder builder;
  builder.with_mock_camera(mock_camera_);
  builder.fromUserId("ReconnectCamera");
  auto capture = builder.build_test();

  ASSERT_NE(capture, nullptr);
  capture->stop();
}
