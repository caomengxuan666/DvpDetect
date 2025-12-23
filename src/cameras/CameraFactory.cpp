#include "cameras/CameraFactory.hpp"

#include "DvpCameraBuilder.hpp"
#include "config/CameraConfig.hpp"

std::unique_ptr<CameraCapture> create_camera(const CameraBrand& brand,
                                             const std::string& identifier,
                                             const CameraConfig& config) {
  switch (brand) {
    case CameraBrand::DVP: {
      // 使用DvpCameraBuilder创建DVP相机
      auto builder = DvpCameraBuilder::fromUserId(identifier);
      // 应用配置
      builder.exposure(config.exposure_us)
          .gain(config.gain)
          .roi(config.roi_x, config.roi_y, config.roi_w, config.roi_h)
          .hardwareIsp(config.hardware_isp)
          .autoExposure(config.auto_exposure)
          .autoGain(config.auto_gain)
          .aeTargetBrightness(config.ae_target_brightness)
          .antiFlickerMode(config.anti_flicker_mode)
          .acquisitionMode(config.acquisition_mode)
          .contrast(config.contrast)
          .gamma(config.gamma)
          .saturation(config.saturation)
          .sharpness(config.sharpness);

      // 构建并返回相机
      return std::unique_ptr<CameraCapture>(builder.build());
    }
    case CameraBrand::IK: {
      // TODO: 实现IK相机的创建逻辑
      // 这里需要IK相机的具体实现
      break;
    }
    case CameraBrand::MIND: {
      // TODO: 实现Mind相机的创建逻辑
      // 这里需要Mind相机的具体实现
      break;
    }
  }
  return nullptr;
}
