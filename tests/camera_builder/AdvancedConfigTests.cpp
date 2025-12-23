#include <gtest/gtest.h>

#include "../mock/DVPCameraMock.hpp"
#include "DvpCameraBuilder.hpp"
// 包含DVPCamera.h以使用TRIGGER_SOURCE_SOFTWARE枚举
#include "../../third_party/DVP_SDK/include/DVPCamera.h"

// 测试高级配置功能
class AdvancedConfigTests : public ::testing::Test {
 protected:
  DvpCameraBuilder builder;
  std::unique_ptr<MockDvpCapture> mockCapture;

  /**
   * @brief
   */
  void SetUp() override {
    builder = DvpCameraBuilder::fromFriendlyName("Test Camera");
    mockCapture = std::make_unique<MockDvpCapture>();
  }
};

// 测试自动曝光配置
TEST_F(AdvancedConfigTests, AutoExposureTest) {
  builder.autoExposure(true);
  builder.aeTargetBrightness(150);
  auto config = builder.toDvpConfig();

  EXPECT_TRUE(config.auto_exposure);
  EXPECT_EQ(config.ae_target_brightness, 150);
}

// 测试自动增益配置
TEST_F(AdvancedConfigTests, AutoGainTest) {
  builder.autoGain(true);
  auto config = builder.toDvpConfig();
  EXPECT_TRUE(config.auto_gain);

  builder.autoGain(false);
  config = builder.toDvpConfig();
  EXPECT_FALSE(config.auto_gain);
}

// 测试抗频闪配置
TEST_F(AdvancedConfigTests, AntiFlickerTest) {
  builder.antiFlickerMode(1);  // 50Hz
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.anti_flicker_mode, 1);

  builder.antiFlickerMode(2);  // 60Hz
  config = builder.toDvpConfig();
  EXPECT_EQ(config.anti_flicker_mode, 2);
}

// 测试采集模式配置
TEST_F(AdvancedConfigTests, AcquisitionModeTest) {
  builder.acquisitionMode(0);  // 连续
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.acquisition_mode, 0);

  builder.acquisitionMode(1);  // 单帧
  config = builder.toDvpConfig();
  EXPECT_EQ(config.acquisition_mode, 1);

  builder.acquisitionMode(2);  // 多帧
  config = builder.toDvpConfig();
  EXPECT_EQ(config.acquisition_mode, 2);
}

// 测试自动曝光ROI配置
TEST_F(AdvancedConfigTests, AeRoiTest) {
  builder.aeRoi(50, 50, 320, 240);
  auto config = builder.toDvpConfig();

  EXPECT_EQ(config.ae_roi_x, 50);
  EXPECT_EQ(config.ae_roi_y, 50);
  EXPECT_EQ(config.ae_roi_w, 320);
  EXPECT_EQ(config.ae_roi_h, 240);
}

// 测试自动白平衡ROI配置
TEST_F(AdvancedConfigTests, AwbRoiTest) {
  builder.awbRoi(100, 100, 400, 300);
  auto config = builder.toDvpConfig();

  EXPECT_EQ(config.awb_roi_x, 100);
  EXPECT_EQ(config.awb_roi_y, 100);
  EXPECT_EQ(config.awb_roi_w, 400);
  EXPECT_EQ(config.awb_roi_h, 300);
}

// 测试制冷器配置
TEST_F(AdvancedConfigTests, CoolerStateTest) {
  builder.coolerState(true);
  auto config = builder.toDvpConfig();
  EXPECT_TRUE(config.cooler_state);

  builder.coolerState(false);
  config = builder.toDvpConfig();
  EXPECT_FALSE(config.cooler_state);
}

// 测试缓存队列配置
TEST_F(AdvancedConfigTests, BufferQueueSizeTest) {
  builder.bufferQueueSize(10);
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.buffer_queue_size, 10);

  builder.bufferQueueSize(5);
  config = builder.toDvpConfig();
  EXPECT_EQ(config.buffer_queue_size, 5);
}

// 测试连接超时配置
TEST_F(AdvancedConfigTests, LinkTimeoutTest) {
  builder.linkTimeout(5000);
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.link_timeout, 5000);
}

// 测试白平衡操作配置
TEST_F(AdvancedConfigTests, AwbOperationTest) {
  builder.awbOperation(2);  // 预设模式
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.awb_operation, 2);
}

// 测试触发激活配置
TEST_F(AdvancedConfigTests, TriggerActivationTest) {
  builder.triggerActivation(1);  // 上升沿
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.trigger_activation, 1);
}

// 测试触发计数配置
TEST_F(AdvancedConfigTests, TriggerCountTest) {
  builder.triggerCount(5);
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.trigger_count, 5);
}

// 测试Strobe配置
TEST_F(AdvancedConfigTests, StrobeConfigTest) {
  builder.strobeSource(1);
  builder.strobeDelay(100.0);
  builder.strobeDuration(500.0);
  auto config = builder.toDvpConfig();

  EXPECT_EQ(config.strobe_source, 1);
  EXPECT_EQ(config.strobe_delay, 100.0);
  EXPECT_EQ(config.strobe_duration, 500.0);
}

// 测试线扫触发配置
TEST_F(AdvancedConfigTests, LineTrigConfigTest) {
  builder.lineTrigEnable(true);
  builder.lineTrigSource(2);
  builder.lineTrigEdgeSel(1);
  auto config = builder.toDvpConfig();

  EXPECT_TRUE(config.line_trig_enable);
  EXPECT_EQ(config.line_trig_source, 2);
  EXPECT_EQ(config.line_trig_edge_sel, 1);
}

// 测试帧率配置
TEST_F(AdvancedConfigTests, FrameRateTest) {
  builder.acquisitionFrameRate(30.0);
  builder.acquisitionFrameRateEnable(true);
  auto config = builder.toDvpConfig();

  EXPECT_EQ(config.acquisition_frame_rate, 30.0);
  EXPECT_TRUE(config.acquisition_frame_rate_enable);
}

// 测试平场校正配置
TEST_F(AdvancedConfigTests, FlatFieldTest) {
  builder.flatFieldEnable(true);
  auto config = builder.toDvpConfig();
  EXPECT_TRUE(config.flat_field_enable);
}

// 测试触发源配置
TEST_F(AdvancedConfigTests, TriggerSourceTest) {
  // 使用正确的枚举值TRIGGER_SOURCE_SOFTWARE代替DVP_TRIGGER_SOFTWARE
  builder.triggerSource(TRIGGER_SOURCE_SOFTWARE);
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.trigger_source, TRIGGER_SOURCE_SOFTWARE);
}

// 测试触发延迟配置
TEST_F(AdvancedConfigTests, TriggerDelayTest) {
  builder.triggerDelay(2000.0);
  auto config = builder.toDvpConfig();
  EXPECT_EQ(config.trigger_delay_us, 2000.0);
}
