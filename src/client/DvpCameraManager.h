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
 *  - File: DvpCameraManager.h
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

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
