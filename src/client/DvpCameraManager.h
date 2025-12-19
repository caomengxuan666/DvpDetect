#ifndef DVPCAMERAMANAGER_H
#define DVPCAMERAMANAGER_H

#include <QObject>
#include <QString>
#include <memory>
#include <unordered_map>

#include "DvpCapture.hpp"
#include "DvpConfig.hpp"

class DvpCameraManager : public QObject {
  Q_OBJECT

 public:
  explicit DvpCameraManager(QObject* parent = nullptr);
  ~DvpCameraManager();

  // 相机管理
  bool addCamera(const QString& cameraId, std::unique_ptr<DvpCapture> capture);
  void removeCamera(const QString& cameraId);
  bool hasCamera(const QString& cameraId) const;
  DvpCapture* getCamera(const QString& cameraId) const;
  QStringList getAllCameraIds() const;

  // 相机配置管理
  void setCameraConfig(const QString& cameraId, const DvpConfig& config);
  DvpConfig getCameraConfig(const QString& cameraId) const;
  bool saveCameraConfig(const QString& cameraId, const QString& filePath) const;
  bool loadCameraConfig(const QString& cameraId, const QString& filePath);

  // 相机操作
  bool startCamera(const QString& cameraId, const FrameProcessor& processor);
  void stopCamera(const QString& cameraId);

 private:
  struct CameraData {
    std::unique_ptr<DvpCapture> capture;
    DvpConfig config;
    QString configPath;

    // 添加默认构造函数以解决unique_ptr拷贝问题
    CameraData() = default;
    CameraData(std::unique_ptr<DvpCapture> cap) : capture(std::move(cap)) {}
  };

  std::unordered_map<QString, std::unique_ptr<CameraData>> m_cameras;
};

#endif  // DVPCAMERAMANAGER_H
