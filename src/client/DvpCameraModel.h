#ifndef DVPCAMERAMODEL_H
#define DVPCAMERAMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QVariant>
#include <QVector>

class DvpCameraModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  explicit DvpCameraModel(QObject* parent = nullptr);

  // 基本的模型接口实现
  QModelIndex index(int row, int column,
                    const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value,
               int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  // 自定义方法
  void loadVisionParams();

  // 多相机支持相关方法
  void addCamera(const QString& cameraId, const QString& cameraName);
  void removeCamera(const QString& cameraId);
  int cameraCount() const;
  QString cameraIdAt(int index) const;

 private:
  struct CameraItem;
  struct CameraInfo;

  CameraItem* m_rootItem;
  QVector<CameraInfo*> m_cameras;  // 存储相机列表

  struct CameraInfo {
    QString id;          // 相机唯一标识符
    QString name;        // 相机名称
    QString configPath;  // 相机配置文件路径

    CameraInfo(const QString& cameraId, const QString& cameraName)
        : id(cameraId), name(cameraName), configPath("") {}
  };

  struct CameraItem {
    QString name;
    QString description;
    QVariant value;
    QVariant::Type type;
    QVariant minValue;
    QVariant maxValue;
    QStringList options;  // 用于枚举类型
    QList<CameraItem*> children;
    CameraItem* parent;

    CameraItem() : parent(nullptr) {}
    ~CameraItem() { qDeleteAll(children); }
  };

  void setupModel();
  CameraItem* createThresholdItem();
  CameraItem* createMorphologyOperationItem();
  CameraItem* createContourDetectionItem();
  CameraItem* createEdgeDetectionItem();
};

#endif  // DVPCAMERAMODEL_H
