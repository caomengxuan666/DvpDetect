#ifndef DVP_MULTICAMERA_MANAGER_H
#define DVP_MULTICAMERA_MANAGER_H

#include <QObject>
#include <QString>
#include <memory>
#include <unordered_map>

#include "DvpConfig.hpp"
// Forward declarations
class DvpCapture;
struct DvpConfig;  // 使用struct关键字并确保与实际定义一致
class FrameProcessor;

/**
 * @brief 多相机管理器类
 *
 * 负责管理多个相机实例，包括它们的配置、连接状态和数据流处理
 */
class DvpMultiCameraManager : public QObject {
  Q_OBJECT

 public:
  explicit DvpMultiCameraManager(QObject* parent = nullptr);
  ~DvpMultiCameraManager();

  /**
   * @brief 添加相机到管理器
   * @param cameraId 相机唯一标识符
   * @param capture 相机捕获对象
   * @return 是否添加成功
   */
  bool addCamera(const QString& cameraId, std::unique_ptr<DvpCapture> capture);

  /**
   * @brief 从管理器中移除相机
   * @param cameraId 相机唯一标识符
   */
  void removeCamera(const QString& cameraId);

  /**
   * @brief 检查是否存在指定ID的相机
   * @param cameraId 相机唯一标识符
   * @return 是否存在
   */
  bool hasCamera(const QString& cameraId) const;

  /**
   * @brief 获取指定ID的相机对象
   * @param cameraId 相机唯一标识符
   * @return 相机对象指针，如果不存在则返回nullptr
   */
  DvpCapture* getCamera(const QString& cameraId) const;

  /**
   * @brief 获取所有相机ID列表
   * @return 相机ID列表
   */
  QStringList getAllCameraIds() const;

  /**
   * @brief 设置相机配置
   * @param cameraId 相机唯一标识符
   * @param config 配置对象
   */
  void setCameraConfig(const QString& cameraId, const DvpConfig& config);

  /**
   * @brief 获取相机配置
   * @param cameraId 相机唯一标识符
   * @return 配置对象
   */
  DvpConfig getCameraConfig(const QString& cameraId) const;

  /**
   * @brief 保存相机配置到文件
   * @param cameraId 相机唯一标识符
   * @param filePath 文件路径
   * @return 是否保存成功
   */
  bool saveCameraConfig(const QString& cameraId, const QString& filePath) const;

  /**
   * @brief 从文件加载相机配置
   * @param cameraId 相机唯一标识符
   * @param filePath 文件路径
   * @return 是否加载成功
   */
  bool loadCameraConfig(const QString& cameraId, const QString& filePath);

  /**
   * @brief 启动相机捕获
   * @param cameraId 相机唯一标识符
   * @param processor 帧处理对象
   * @return 是否启动成功
   */
  bool startCamera(const QString& cameraId, const FrameProcessor& processor);

  /**
   * @brief 停止相机捕获
   * @param cameraId 相机唯一标识符
   */
  void stopCamera(const QString& cameraId);

 private:
  // 相机数据结构
  struct CameraData {
    std::unique_ptr<DvpCapture> capture;  // 相机捕获对象
    DvpConfig config;                     // 相机配置
    QString configPath;                   // 配置文件路径

    // 添加默认构造函数
    CameraData() = default;
    explicit CameraData(std::unique_ptr<DvpCapture> cap)
        : capture(std::move(cap)) {}
  };

  // 相机映射表
  std::unordered_map<QString, std::unique_ptr<CameraData>> m_cameras;
};

#endif  // DVP_MULTICAMERA_MANAGER_H
