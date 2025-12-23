// tests/mock/IDVPCamera.h
#pragma once
#include <memory>
#include <string>
#include <vector>

class FrameProcessor;
class DvpConfig;

class IDVPCamera {
 public:
  virtual ~IDVPCamera() = default;

  virtual bool connect(const std::string& id) = 0;
  virtual void disconnect() = 0;
  virtual bool isConnected() const = 0;

  virtual void setExposure(double us) = 0;
  virtual double getExposure() const = 0;

  virtual void setGain(float gain) = 0;
  virtual float getGain() const = 0;

  virtual void setHardwareIsp(bool enable) = 0;
  virtual bool getHardwareIsp() const = 0;

  virtual void setTriggerMode(bool enable) = 0;
  virtual bool getTriggerMode() const = 0;

  virtual void setConfig(const DvpConfig& config) = 0;
  virtual DvpConfig getConfig() const = 0;

  virtual void startCapture() = 0;
  virtual void stopCapture() = 0;
  virtual bool isCapturing() const = 0;

  virtual std::vector<uint8_t> getFrameData() = 0;
  virtual void registerFrameCallback(
      const std::shared_ptr<FrameProcessor>& callback) = 0;
};
