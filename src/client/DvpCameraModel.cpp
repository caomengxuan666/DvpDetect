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
 *  - File: DvpCameraModel.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpCameraModel.h"

DvpCameraModel::DvpCameraModel(QObject* parent)
    : QAbstractItemModel(parent), m_rootItem(new CameraItem) {
  setupModel();
}

QModelIndex DvpCameraModel::index(int row, int column,
                                  const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  CameraItem* parentItem;

  if (!parent.isValid())
    parentItem = m_rootItem;
  else
    parentItem = static_cast<CameraItem*>(parent.internalPointer());

  CameraItem* childItem = parentItem->children.value(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex DvpCameraModel::parent(const QModelIndex& index) const {
  if (!index.isValid()) return QModelIndex();

  CameraItem* childItem = static_cast<CameraItem*>(index.internalPointer());
  CameraItem* parentItem = childItem->parent;

  if (parentItem == m_rootItem) return QModelIndex();

  int row = parentItem->parent->children.indexOf(parentItem);
  return createIndex(row, 0, parentItem);
}

int DvpCameraModel::rowCount(const QModelIndex& parent) const {
  CameraItem* parentItem;
  if (parent.column() > 0) return 0;

  if (!parent.isValid())
    parentItem = m_rootItem;
  else
    parentItem = static_cast<CameraItem*>(parent.internalPointer());

  return parentItem->children.count();
}

int DvpCameraModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 3;  // 名称、值、描述三列
}

QVariant DvpCameraModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) return QVariant();

  CameraItem* item = static_cast<CameraItem*>(index.internalPointer());

  switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
      switch (index.column()) {
        case 0:  // 名称
          return item->name;
        case 1:  // 值
          return item->value;
        case 2:  // 描述
          return item->description;
      }
      break;
    case Qt::UserRole:
      return QVariant::fromValue((void*)item);
  }

  return QVariant();
}

bool DvpCameraModel::setData(const QModelIndex& index, const QVariant& value,
                             int role) {
  if (!index.isValid() || role != Qt::EditRole) return false;

  CameraItem* item = static_cast<CameraItem*>(index.internalPointer());

  if (index.column() == 1) {  // 只有值列可以编辑
    item->value = value;
    emit dataChanged(index, index);
    return true;
  }

  return false;
}

Qt::ItemFlags DvpCameraModel::flags(const QModelIndex& index) const {
  if (!index.isValid()) return Qt::NoItemFlags;

  if (index.column() == 1)  // 值列可编辑
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

  return QAbstractItemModel::flags(index);
}

QVariant DvpCameraModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0:
        return tr("参数名称");
      case 1:
        return tr("参数值");
      case 2:
        return tr("描述");
    }
  }

  return QVariant();
}

void DvpCameraModel::setupModel() {
  // 清除现有数据
  qDeleteAll(m_rootItem->children);
  m_rootItem->children.clear();

  // 添加视觉检测参数分类
  m_rootItem->children << createThresholdItem();
  m_rootItem->children << createMorphologyOperationItem();
  m_rootItem->children << createContourDetectionItem();
  m_rootItem->children << createEdgeDetectionItem();

  emit layoutChanged();
}

DvpCameraModel::CameraItem* DvpCameraModel::createThresholdItem() {
  CameraItem* thresholdItem = new CameraItem;
  thresholdItem->name = tr("阈值处理");
  thresholdItem->description = tr("图像二值化处理参数");
  thresholdItem->parent = m_rootItem;

  CameraItem* methodItem = new CameraItem;
  methodItem->name = tr("阈值方法");
  methodItem->description = tr("选择阈值处理的方法");
  methodItem->value = tr("OTSU");
  methodItem->type = QVariant::String;
  methodItem->options = QStringList{tr("OTSU"), tr("自适应"), tr("手动")};
  methodItem->parent = thresholdItem;

  CameraItem* valueItem = new CameraItem;
  valueItem->name = tr("阈值");
  valueItem->description = tr("二值化阈值");
  valueItem->value = 128;
  valueItem->type = QVariant::Int;
  valueItem->minValue = 0;
  valueItem->maxValue = 255;
  valueItem->parent = thresholdItem;

  thresholdItem->children << methodItem << valueItem;
  return thresholdItem;
}

DvpCameraModel::CameraItem* DvpCameraModel::createMorphologyOperationItem() {
  CameraItem* morphItem = new CameraItem;
  morphItem->name = tr("形态学操作");
  morphItem->description = tr("图像形态学处理参数");
  morphItem->parent = m_rootItem;

  CameraItem* enableItem = new CameraItem;
  enableItem->name = tr("启用");
  enableItem->description = tr("是否启用形态学操作");
  enableItem->value = false;
  enableItem->type = QVariant::Bool;
  enableItem->parent = morphItem;

  CameraItem* operationItem = new CameraItem;
  operationItem->name = tr("操作类型");
  operationItem->description = tr("形态学操作类型");
  operationItem->value = tr("开运算");
  operationItem->type = QVariant::String;
  operationItem->options =
      QStringList{tr("腐蚀"), tr("膨胀"), tr("开运算"), tr("闭运算")};
  operationItem->parent = morphItem;

  CameraItem* kernelSizeItem = new CameraItem;
  kernelSizeItem->name = tr("核大小");
  kernelSizeItem->description = tr("形态学操作核大小");
  kernelSizeItem->value = 3;
  kernelSizeItem->type = QVariant::Int;
  kernelSizeItem->minValue = 1;
  kernelSizeItem->maxValue = 15;
  kernelSizeItem->parent = morphItem;

  morphItem->children << enableItem << operationItem << kernelSizeItem;
  return morphItem;
}

DvpCameraModel::CameraItem* DvpCameraModel::createContourDetectionItem() {
  CameraItem* contourItem = new CameraItem;
  contourItem->name = tr("轮廓检测");
  contourItem->description = tr("图像轮廓检测参数");
  contourItem->parent = m_rootItem;

  CameraItem* enableItem = new CameraItem;
  enableItem->name = tr("启用");
  enableItem->description = tr("是否启用轮廓检测");
  enableItem->value = true;
  enableItem->type = QVariant::Bool;
  enableItem->parent = contourItem;

  CameraItem* modeItem = new CameraItem;
  modeItem->name = tr("检测模式");
  modeItem->description = tr("轮廓检测模式");
  modeItem->value = tr("外部轮廓");
  modeItem->type = QVariant::String;
  modeItem->options =
      QStringList{tr("外部轮廓"), tr("内部轮廓"), tr("全部轮廓")};
  modeItem->parent = contourItem;

  CameraItem* minAreaItem = new CameraItem;
  minAreaItem->name = tr("最小面积");
  minAreaItem->description = tr("轮廓最小面积阈值");
  minAreaItem->value = 100;
  minAreaItem->type = QVariant::Int;
  minAreaItem->minValue = 0;
  minAreaItem->maxValue = 10000;
  minAreaItem->parent = contourItem;

  contourItem->children << enableItem << modeItem << minAreaItem;
  return contourItem;
}

DvpCameraModel::CameraItem* DvpCameraModel::createEdgeDetectionItem() {
  CameraItem* edgeItem = new CameraItem;
  edgeItem->name = tr("边缘检测");
  edgeItem->description = tr("图像边缘检测参数");
  edgeItem->parent = m_rootItem;

  CameraItem* enableItem = new CameraItem;
  enableItem->name = tr("启用");
  enableItem->description = tr("是否启用边缘检测");
  enableItem->value = false;
  enableItem->type = QVariant::Bool;
  enableItem->parent = edgeItem;

  CameraItem* methodItem = new CameraItem;
  methodItem->name = tr("检测方法");
  methodItem->description = tr("边缘检测算法");
  methodItem->value = tr("Canny");
  methodItem->type = QVariant::String;
  methodItem->options = QStringList{tr("Canny"), tr("Sobel"), tr("Laplacian")};
  methodItem->parent = edgeItem;

  CameraItem* threshold1Item = new CameraItem;
  threshold1Item->name = tr("阈值1");
  threshold1Item->description = tr("边缘检测第一阈值");
  threshold1Item->value = 50;
  threshold1Item->type = QVariant::Int;
  threshold1Item->minValue = 0;
  threshold1Item->maxValue = 255;
  threshold1Item->parent = edgeItem;

  CameraItem* threshold2Item = new CameraItem;
  threshold2Item->name = tr("阈值2");
  threshold2Item->description = tr("边缘检测第二阈值");
  threshold2Item->value = 150;
  threshold2Item->type = QVariant::Int;
  threshold2Item->minValue = 0;
  threshold2Item->maxValue = 255;
  threshold2Item->parent = edgeItem;

  edgeItem->children << enableItem << methodItem << threshold1Item
                     << threshold2Item;
  return edgeItem;
}

void DvpCameraModel::loadVisionParams() {
  beginResetModel();
  setupModel();
  endResetModel();
}

void DvpCameraModel::addCamera(const QString& cameraId,
                               const QString& cameraName) {
  m_cameras.append(new CameraInfo(cameraId, cameraName));
}

void DvpCameraModel::removeCamera(const QString& cameraId) {
  for (auto it = m_cameras.begin(); it != m_cameras.end(); ++it) {
    if ((*it)->id == cameraId) {
      delete *it;
      m_cameras.erase(it);
      break;
    }
  }
}

int DvpCameraModel::cameraCount() const { return m_cameras.size(); }

QString DvpCameraModel::cameraIdAt(int index) const {
  if (index >= 0 && index < m_cameras.size()) {
    return m_cameras[index]->id;
  }
  return QString();
}
