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
 *  - File: DvpCameraModel.h
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

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
