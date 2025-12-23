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
 *  - File: DvpConfigModel.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpConfigModel.h"

#include <QApplication>
#include <QIcon>

DvpConfigModel::DvpConfigModel(QObject* parent)
    : QAbstractItemModel(parent), m_rootItem(new ConfigItem) {
  setupModel();
}

QModelIndex DvpConfigModel::index(int row, int column,
                                  const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  ConfigItem* parentItem;

  if (!parent.isValid())
    parentItem = m_rootItem;
  else
    parentItem = static_cast<ConfigItem*>(parent.internalPointer());

  ConfigItem* childItem = parentItem->children.value(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex DvpConfigModel::parent(const QModelIndex& index) const {
  if (!index.isValid()) return QModelIndex();

  ConfigItem* childItem = static_cast<ConfigItem*>(index.internalPointer());
  ConfigItem* parentItem = childItem->parent;

  if (parentItem == m_rootItem) return QModelIndex();

  return createIndex(parentItem->children.indexOf(childItem), 0, parentItem);
}

int DvpConfigModel::rowCount(const QModelIndex& parent) const {
  ConfigItem* parentItem;
  if (parent.column() > 0) return 0;

  if (!parent.isValid())
    parentItem = m_rootItem;
  else
    parentItem = static_cast<ConfigItem*>(parent.internalPointer());

  return parentItem->children.count();
}

int DvpConfigModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 3;  // Name, Value, Description
}

QVariant DvpConfigModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) return QVariant();

  ConfigItem* item = static_cast<ConfigItem*>(index.internalPointer());

  switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
        case 0:  // Name
          return item->name;
        case 1:  // Value
          // 对于布尔类型，显示 ON/OFF 而不是 true/false
          if (item->type == QVariant::Bool) {
            return item->value.toBool() ? tr("ON") : tr("OFF");
          }
          return item->value;
        case 2:  // Description
          return item->description;
      }
      break;
    case Qt::DecorationRole:
      if (index.column() == 0) return item->icon;
      break;
    case Qt::UserRole:  // Type information for editor
      return static_cast<int>(item->type);
    case Qt::UserRole + 1:  // Min value
      return item->minValue;
    case Qt::UserRole + 2:  // Max value
      return item->maxValue;
    case Qt::UserRole + 3:  // Options for enum types
      return item->options;
    case Qt::TextAlignmentRole:
      // 布尔值居中显示
      if (index.column() == 1 && item->type == QVariant::Bool) {
        return Qt::AlignCenter;
      }
      break;
  }

  return QVariant();
}

bool DvpConfigModel::setData(const QModelIndex& index, const QVariant& value,
                             int role) {
  if (!index.isValid() || role != Qt::EditRole) return false;

  ConfigItem* item = static_cast<ConfigItem*>(index.internalPointer());
  item->value = value;
  emit dataChanged(index, index);
  return true;
}

Qt::ItemFlags DvpConfigModel::flags(const QModelIndex& index) const {
  if (!index.isValid()) return Qt::NoItemFlags;

  Qt::ItemFlags flags = QAbstractItemModel::flags(index);

  if (index.column() == 1)  // Value column is editable
    flags |= Qt::ItemIsEditable;

  return flags;
}

QVariant DvpConfigModel::headerData(int section, Qt::Orientation orientation,
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

void DvpConfigModel::setupModel() {
  // Clear existing items
  qDeleteAll(m_rootItem->children);
  m_rootItem->children.clear();

  // Create a default DvpConfig instance to get default values
  DvpConfig config;

  // Create and add all configuration groups
  m_rootItem->children.append(createBasicImageParamsItem(config));
  m_rootItem->children.append(createTriggerModeItem(config));
  m_rootItem->children.append(createAutoExposureItem(config));
  m_rootItem->children.append(createAntiFlickerItem(config));
  m_rootItem->children.append(createAcquisitionModeItem(config));
  m_rootItem->children.append(createTriggerDetailItem(config));
  m_rootItem->children.append(createExposureRangeItem(config));
  m_rootItem->children.append(createGainRangeItem(config));
  m_rootItem->children.append(createImageEnhanceItem(config));
  m_rootItem->children.append(createImageProcessItem(config));
  m_rootItem->children.append(createOtherParamsItem(config));
  m_rootItem->children.append(createAwbParamsItem(config));
  m_rootItem->children.append(createTriggerParamsItem(config));
  m_rootItem->children.append(createLineScanParamsItem(config));
  m_rootItem->children.append(createAdvancedParamsItem(config));

  emit layoutChanged();
}

void DvpConfigModel::loadConfig(const DvpConfig& config) {
  // Clear existing items
  qDeleteAll(m_rootItem->children);
  m_rootItem->children.clear();

  // Create and add all configuration groups
  m_rootItem->children.append(createBasicImageParamsItem(config));
  m_rootItem->children.append(createTriggerModeItem(config));
  m_rootItem->children.append(createAutoExposureItem(config));
  m_rootItem->children.append(createAntiFlickerItem(config));
  m_rootItem->children.append(createAcquisitionModeItem(config));
  m_rootItem->children.append(createTriggerDetailItem(config));
  m_rootItem->children.append(createExposureRangeItem(config));
  m_rootItem->children.append(createGainRangeItem(config));
  m_rootItem->children.append(createImageEnhanceItem(config));
  m_rootItem->children.append(createImageProcessItem(config));
  m_rootItem->children.append(createOtherParamsItem(config));
  m_rootItem->children.append(createAwbParamsItem(config));
  m_rootItem->children.append(createTriggerParamsItem(config));
  m_rootItem->children.append(createLineScanParamsItem(config));
  m_rootItem->children.append(createAdvancedParamsItem(config));

  emit layoutChanged();
}

// 修复函数定义，使用完整的嵌套类名称
DvpConfigModel::ConfigItem* DvpConfigModel::createBasicImageParamsItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("基本图像参数");
  group->description = tr("相机的基本图像参数设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* exposureItem = new ConfigItem;
  exposureItem->name = tr("曝光时间 (μs)");
  exposureItem->description = tr("相机曝光时间，单位为微秒");
  exposureItem->value = config.exposure_us;
  exposureItem->type = QVariant::Double;
  exposureItem->minValue = 0.0;
  exposureItem->maxValue = 1000000.0;
  exposureItem->parent = group;
  group->children.append(exposureItem);

  ConfigItem* gainItem = new ConfigItem;
  gainItem->name = tr("增益");
  gainItem->description = tr("相机增益设置");
  gainItem->value = config.gain;
  gainItem->type = QVariant::Double;
  gainItem->minValue = 0.0;
  gainItem->maxValue = 100.0;
  gainItem->parent = group;
  group->children.append(gainItem);

  ConfigItem* roiXItem = new ConfigItem;
  roiXItem->name = tr("ROI X");
  roiXItem->description = tr("感兴趣区域X坐标");
  roiXItem->value = config.roi_x;
  roiXItem->type = QVariant::Int;
  roiXItem->minValue = 0;
  roiXItem->maxValue = 10000;
  roiXItem->parent = group;
  group->children.append(roiXItem);

  ConfigItem* roiYItem = new ConfigItem;
  roiYItem->name = tr("ROI Y");
  roiYItem->description = tr("感兴趣区域Y坐标");
  roiYItem->value = config.roi_y;
  roiYItem->type = QVariant::Int;
  roiYItem->minValue = 0;
  roiYItem->maxValue = 10000;
  roiYItem->parent = group;
  group->children.append(roiYItem);

  ConfigItem* roiWItem = new ConfigItem;
  roiWItem->name = tr("ROI Width");
  roiWItem->description = tr("感兴趣区域宽度");
  roiWItem->value = config.roi_w;
  roiWItem->type = QVariant::Int;
  roiWItem->minValue = 0;
  roiWItem->maxValue = 10000;
  roiWItem->parent = group;
  group->children.append(roiWItem);

  ConfigItem* roiHItem = new ConfigItem;
  roiHItem->name = tr("ROI Height");
  roiHItem->description = tr("感兴趣区域高度");
  roiHItem->value = config.roi_h;
  roiHItem->type = QVariant::Int;
  roiHItem->minValue = 0;
  roiHItem->maxValue = 10000;
  roiHItem->parent = group;
  group->children.append(roiHItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createTriggerModeItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("触发模式配置");
  group->description = tr("相机触发模式相关设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* triggerModeItem = new ConfigItem;
  triggerModeItem->name = tr("触发模式");
  triggerModeItem->description = tr("启用/禁用触发模式");
  triggerModeItem->value = config.trigger_mode;
  triggerModeItem->type = QVariant::Bool;
  triggerModeItem->parent = group;
  group->children.append(triggerModeItem);

  ConfigItem* hardwareIspItem = new ConfigItem;
  hardwareIspItem->name = tr("硬件ISP");
  hardwareIspItem->description = tr("启用/禁用硬件图像信号处理");
  hardwareIspItem->value = config.hardware_isp;
  hardwareIspItem->type = QVariant::Bool;
  hardwareIspItem->parent = group;
  group->children.append(hardwareIspItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createAutoExposureItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("自动曝光相关");
  group->description = tr("自动曝光和自动增益设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* autoExposureItem = new ConfigItem;
  autoExposureItem->name = tr("自动曝光");
  autoExposureItem->description = tr("启用/禁用自动曝光");
  autoExposureItem->value = config.auto_exposure;
  autoExposureItem->type = QVariant::Bool;
  autoExposureItem->parent = group;
  group->children.append(autoExposureItem);

  ConfigItem* autoGainItem = new ConfigItem;
  autoGainItem->name = tr("自动增益");
  autoGainItem->description = tr("启用/禁用自动增益");
  autoGainItem->value = config.auto_gain;
  autoGainItem->type = QVariant::Bool;
  autoGainItem->parent = group;
  group->children.append(autoGainItem);

  ConfigItem* aeTargetBrightnessItem = new ConfigItem;
  aeTargetBrightnessItem->name = tr("AE目标亮度");
  aeTargetBrightnessItem->description = tr("自动曝光目标亮度 (0-255)");
  aeTargetBrightnessItem->value = config.ae_target_brightness;
  aeTargetBrightnessItem->type = QVariant::Int;
  aeTargetBrightnessItem->minValue = 0;
  aeTargetBrightnessItem->maxValue = 255;
  aeTargetBrightnessItem->parent = group;
  group->children.append(aeTargetBrightnessItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createAntiFlickerItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("抗频闪设置");
  group->description = tr("抗频闪模式设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* antiFlickerItem = new ConfigItem;
  antiFlickerItem->name = tr("抗频闪模式");
  antiFlickerItem->description =
      tr("抗频闪模式设置 (0: 关闭, 1: 50Hz, 2: 60Hz)");
  antiFlickerItem->value = config.anti_flicker_mode;
  antiFlickerItem->type = QVariant::Int;
  antiFlickerItem->options = QStringList()
                             << tr("关闭") << tr("50Hz") << tr("60Hz");
  antiFlickerItem->parent = group;
  group->children.append(antiFlickerItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createAcquisitionModeItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("采集模式");
  group->description = tr("图像采集模式设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* acquisitionModeItem = new ConfigItem;
  acquisitionModeItem->name = tr("采集模式");
  acquisitionModeItem->description =
      tr("图像采集模式 (0: 连续, 1: 单帧, 2: 多帧)");
  acquisitionModeItem->value = config.acquisition_mode;
  acquisitionModeItem->type = QVariant::Int;
  acquisitionModeItem->options = QStringList()
                                 << tr("连续") << tr("单帧") << tr("多帧");
  acquisitionModeItem->parent = group;
  group->children.append(acquisitionModeItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createTriggerDetailItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("触发详细配置");
  group->description = tr("详细的触发配置参数");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* triggerSourceItem = new ConfigItem;
  triggerSourceItem->name = tr("触发源");
  triggerSourceItem->description = tr("触发信号来源 (0: 软触发, 1-8: 硬触发)");
  triggerSourceItem->value = config.trigger_source;
  triggerSourceItem->type = QVariant::Int;
  triggerSourceItem->options =
      QStringList() << tr("软触发") << tr("硬触发 Line1") << tr("硬触发 Line2")
                    << tr("硬触发 Line3") << tr("硬触发 Line4")
                    << tr("硬触发 Line5") << tr("硬触发 Line6")
                    << tr("硬触发 Line7") << tr("硬触发 Line8");
  triggerSourceItem->parent = group;
  group->children.append(triggerSourceItem);

  ConfigItem* triggerDelayItem = new ConfigItem;
  triggerDelayItem->name = tr("触发延迟 (μs)");
  triggerDelayItem->description = tr("触发信号延迟时间，单位为微秒");
  triggerDelayItem->value = config.trigger_delay_us;
  triggerDelayItem->type = QVariant::Double;
  triggerDelayItem->minValue = 0.0;
  triggerDelayItem->maxValue = 1000000.0;
  triggerDelayItem->parent = group;
  group->children.append(triggerDelayItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createExposureRangeItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("曝光参数范围");
  group->description = tr("曝光时间的最小和最大范围设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* exposureMinItem = new ConfigItem;
  exposureMinItem->name = tr("最小曝光时间 (μs)");
  exposureMinItem->description = tr("最小曝光时间设置，单位为微秒");
  exposureMinItem->value = config.exposure_min_us;
  exposureMinItem->type = QVariant::Double;
  exposureMinItem->minValue = 0.0;
  exposureMinItem->maxValue = 1000000.0;
  exposureMinItem->parent = group;
  group->children.append(exposureMinItem);

  ConfigItem* exposureMaxItem = new ConfigItem;
  exposureMaxItem->name = tr("最大曝光时间 (μs)");
  exposureMaxItem->description = tr("最大曝光时间设置，单位为微秒");
  exposureMaxItem->value = config.exposure_max_us;
  exposureMaxItem->type = QVariant::Double;
  exposureMaxItem->minValue = 0.0;
  exposureMaxItem->maxValue = 1000000.0;
  exposureMaxItem->parent = group;
  group->children.append(exposureMaxItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createGainRangeItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("增益范围");
  group->description = tr("增益的最小和最大范围设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* gainMinItem = new ConfigItem;
  gainMinItem->name = tr("最小增益");
  gainMinItem->description = tr("最小增益设置");
  gainMinItem->value = config.gain_min;
  gainMinItem->type = QVariant::Double;
  gainMinItem->minValue = 0.0;
  gainMinItem->maxValue = 100.0;
  gainMinItem->parent = group;
  group->children.append(gainMinItem);

  ConfigItem* gainMaxItem = new ConfigItem;
  gainMaxItem->name = tr("最大增益");
  gainMaxItem->description = tr("最大增益设置");
  gainMaxItem->value = config.gain_max;
  gainMaxItem->type = QVariant::Double;
  gainMaxItem->minValue = 0.0;
  gainMaxItem->maxValue = 100.0;
  gainMaxItem->parent = group;
  group->children.append(gainMaxItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createImageEnhanceItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("图像增强参数");
  group->description = tr("图像增强相关参数设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* contrastItem = new ConfigItem;
  contrastItem->name = tr("对比度");
  contrastItem->description = tr("图像对比度 (-100 到 100)");
  contrastItem->value = config.contrast;
  contrastItem->type = QVariant::Int;
  contrastItem->minValue = -100;
  contrastItem->maxValue = 100;
  contrastItem->parent = group;
  group->children.append(contrastItem);

  ConfigItem* gammaItem = new ConfigItem;
  gammaItem->name = tr("伽马值");
  gammaItem->description = tr("伽马校正值");
  gammaItem->value = config.gamma;
  gammaItem->type = QVariant::Int;
  gammaItem->minValue = 0;
  gammaItem->maxValue = 300;
  gammaItem->parent = group;
  group->children.append(gammaItem);

  ConfigItem* saturationItem = new ConfigItem;
  saturationItem->name = tr("饱和度");
  saturationItem->description = tr("图像饱和度 (0-200)");
  saturationItem->value = config.saturation;
  saturationItem->type = QVariant::Int;
  saturationItem->minValue = 0;
  saturationItem->maxValue = 200;
  saturationItem->parent = group;
  group->children.append(saturationItem);

  ConfigItem* sharpnessEnableItem = new ConfigItem;
  sharpnessEnableItem->name = tr("锐度使能");
  sharpnessEnableItem->description = tr("启用/禁用锐度增强");
  sharpnessEnableItem->value = config.sharpness_enable;
  sharpnessEnableItem->type = QVariant::Bool;
  sharpnessEnableItem->parent = group;
  group->children.append(sharpnessEnableItem);

  ConfigItem* sharpnessItem = new ConfigItem;
  sharpnessItem->name = tr("锐度值");
  sharpnessItem->description = tr("锐度增强值");
  sharpnessItem->value = config.sharpness;
  sharpnessItem->type = QVariant::Int;
  sharpnessItem->minValue = -100;
  sharpnessItem->maxValue = 100;
  sharpnessItem->parent = group;
  group->children.append(sharpnessItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createImageProcessItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("图像处理参数");
  group->description = tr("图像处理相关参数设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* inverseStateItem = new ConfigItem;
  inverseStateItem->name = tr("负片使能");
  inverseStateItem->description = tr("启用/禁用负片效果");
  inverseStateItem->value = config.inverse_state;
  inverseStateItem->type = QVariant::Bool;
  inverseStateItem->parent = group;
  group->children.append(inverseStateItem);

  ConfigItem* flipHorizontalItem = new ConfigItem;
  flipHorizontalItem->name = tr("横向翻转");
  flipHorizontalItem->description = tr("启用/禁用横向翻转");
  flipHorizontalItem->value = config.flip_horizontal_state;
  flipHorizontalItem->type = QVariant::Bool;
  flipHorizontalItem->parent = group;
  group->children.append(flipHorizontalItem);

  ConfigItem* flipVerticalItem = new ConfigItem;
  flipVerticalItem->name = tr("纵向翻转");
  flipVerticalItem->description = tr("启用/禁用纵向翻转");
  flipVerticalItem->value = config.flip_vertical_state;
  flipVerticalItem->type = QVariant::Bool;
  flipVerticalItem->parent = group;
  group->children.append(flipVerticalItem);

  ConfigItem* rotateStateItem = new ConfigItem;
  rotateStateItem->name = tr("旋转使能");
  rotateStateItem->description = tr("启用/禁用图像旋转");
  rotateStateItem->value = config.rotate_state;
  rotateStateItem->type = QVariant::Bool;
  rotateStateItem->parent = group;
  group->children.append(rotateStateItem);

  ConfigItem* rotateOppositeItem = new ConfigItem;
  rotateOppositeItem->name = tr("旋转方向");
  rotateOppositeItem->description = tr("旋转方向设置 (false=顺时针)");
  rotateOppositeItem->value = config.rotate_opposite;
  rotateOppositeItem->type = QVariant::Bool;
  rotateOppositeItem->parent = group;
  group->children.append(rotateOppositeItem);

  ConfigItem* blackLevelItem = new ConfigItem;
  blackLevelItem->name = tr("黑电平");
  blackLevelItem->description = tr("黑电平校正值");
  blackLevelItem->value = config.black_level;
  blackLevelItem->type = QVariant::Double;
  blackLevelItem->minValue = -100.0;
  blackLevelItem->maxValue = 100.0;
  blackLevelItem->parent = group;
  group->children.append(blackLevelItem);

  ConfigItem* colorTemperatureItem = new ConfigItem;
  colorTemperatureItem->name = tr("色温 (K)");
  colorTemperatureItem->description = tr("色温设置，单位为开尔文");
  colorTemperatureItem->value = config.color_temperature;
  colorTemperatureItem->type = QVariant::Int;
  colorTemperatureItem->minValue = 2000;
  colorTemperatureItem->maxValue = 10000;
  colorTemperatureItem->parent = group;
  group->children.append(colorTemperatureItem);

  ConfigItem* flatFieldStateItem = new ConfigItem;
  flatFieldStateItem->name = tr("平场校正使能");
  flatFieldStateItem->description = tr("启用/禁用平场校正");
  flatFieldStateItem->value = config.flat_field_state;
  flatFieldStateItem->type = QVariant::Bool;
  flatFieldStateItem->parent = group;
  group->children.append(flatFieldStateItem);

  ConfigItem* defectFixStateItem = new ConfigItem;
  defectFixStateItem->name = tr("缺陷像素校正使能");
  defectFixStateItem->description = tr("启用/禁用缺陷像素校正");
  defectFixStateItem->value = config.defect_fix_state;
  defectFixStateItem->type = QVariant::Bool;
  defectFixStateItem->parent = group;
  group->children.append(defectFixStateItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createOtherParamsItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("其他参数");
  group->description = tr("其他相机参数设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* monoStateItem = new ConfigItem;
  monoStateItem->name = tr("去色使能");
  monoStateItem->description = tr("启用/禁用去色处理");
  monoStateItem->value = config.mono_state;
  monoStateItem->type = QVariant::Bool;
  monoStateItem->parent = group;
  group->children.append(monoStateItem);

  ConfigItem* aeRoiXItem = new ConfigItem;
  aeRoiXItem->name = tr("AE ROI X");
  aeRoiXItem->description = tr("自动曝光感兴趣区域X坐标");
  aeRoiXItem->value = config.ae_roi_x;
  aeRoiXItem->type = QVariant::Int;
  aeRoiXItem->minValue = 0;
  aeRoiXItem->maxValue = 10000;
  aeRoiXItem->parent = group;
  group->children.append(aeRoiXItem);

  ConfigItem* aeRoiYItem = new ConfigItem;
  aeRoiYItem->name = tr("AE ROI Y");
  aeRoiYItem->description = tr("自动曝光感兴趣区域Y坐标");
  aeRoiYItem->value = config.ae_roi_y;
  aeRoiYItem->type = QVariant::Int;
  aeRoiYItem->minValue = 0;
  aeRoiYItem->maxValue = 10000;
  aeRoiYItem->parent = group;
  group->children.append(aeRoiYItem);

  ConfigItem* aeRoiWItem = new ConfigItem;
  aeRoiWItem->name = tr("AE ROI Width");
  aeRoiWItem->description = tr("自动曝光感兴趣区域宽度");
  aeRoiWItem->value = config.ae_roi_w;
  aeRoiWItem->type = QVariant::Int;
  aeRoiWItem->minValue = 0;
  aeRoiWItem->maxValue = 10000;
  aeRoiWItem->parent = group;
  group->children.append(aeRoiWItem);

  ConfigItem* aeRoiHItem = new ConfigItem;
  aeRoiHItem->name = tr("AE ROI Height");
  aeRoiHItem->description = tr("自动曝光感兴趣区域高度");
  aeRoiHItem->value = config.ae_roi_h;
  aeRoiHItem->type = QVariant::Int;
  aeRoiHItem->minValue = 0;
  aeRoiHItem->maxValue = 10000;
  aeRoiHItem->parent = group;
  group->children.append(aeRoiHItem);

  ConfigItem* awbRoiXItem = new ConfigItem;
  awbRoiXItem->name = tr("AWB ROI X");
  awbRoiXItem->description = tr("自动白平衡感兴趣区域X坐标");
  awbRoiXItem->value = config.awb_roi_x;
  awbRoiXItem->type = QVariant::Int;
  awbRoiXItem->minValue = 0;
  awbRoiXItem->maxValue = 10000;
  awbRoiXItem->parent = group;
  group->children.append(awbRoiXItem);

  ConfigItem* awbRoiYItem = new ConfigItem;
  awbRoiYItem->name = tr("AWB ROI Y");
  awbRoiYItem->description = tr("自动白平衡感兴趣区域Y坐标");
  awbRoiYItem->value = config.awb_roi_y;
  awbRoiYItem->type = QVariant::Int;
  awbRoiYItem->minValue = 0;
  awbRoiYItem->maxValue = 10000;
  awbRoiYItem->parent = group;
  group->children.append(awbRoiYItem);

  ConfigItem* awbRoiWItem = new ConfigItem;
  awbRoiWItem->name = tr("AWB ROI Width");
  awbRoiWItem->description = tr("自动白平衡感兴趣区域宽度");
  awbRoiWItem->value = config.awb_roi_w;
  awbRoiWItem->type = QVariant::Int;
  awbRoiWItem->minValue = 0;
  awbRoiWItem->maxValue = 10000;
  awbRoiWItem->parent = group;
  group->children.append(awbRoiWItem);

  ConfigItem* awbRoiHItem = new ConfigItem;
  awbRoiHItem->name = tr("AWB ROI Height");
  awbRoiHItem->description = tr("自动白平衡感兴趣区域高度");
  awbRoiHItem->value = config.awb_roi_h;
  awbRoiHItem->type = QVariant::Int;
  awbRoiHItem->minValue = 0;
  awbRoiHItem->maxValue = 10000;
  awbRoiHItem->parent = group;
  group->children.append(awbRoiHItem);

  ConfigItem* coolerStateItem = new ConfigItem;
  coolerStateItem->name = tr("制冷器使能");
  coolerStateItem->description = tr("启用/禁用相机制冷器");
  coolerStateItem->value = config.cooler_state;
  coolerStateItem->type = QVariant::Bool;
  coolerStateItem->parent = group;
  group->children.append(coolerStateItem);

  ConfigItem* bufferQueueSizeItem = new ConfigItem;
  bufferQueueSizeItem->name = tr("缓存队列大小");
  bufferQueueSizeItem->description = tr("图像缓存队列大小 (0表示使用默认值)");
  bufferQueueSizeItem->value = config.buffer_queue_size;
  bufferQueueSizeItem->type = QVariant::Int;
  bufferQueueSizeItem->minValue = 0;
  bufferQueueSizeItem->maxValue = 100;
  bufferQueueSizeItem->parent = group;
  group->children.append(bufferQueueSizeItem);

  ConfigItem* streamFlowCtrlSelItem = new ConfigItem;
  streamFlowCtrlSelItem->name = tr("流控制方案选择");
  streamFlowCtrlSelItem->description = tr("启用/禁用流控制方案");
  streamFlowCtrlSelItem->value = config.stream_flow_ctrl_sel;
  streamFlowCtrlSelItem->type = QVariant::Bool;
  streamFlowCtrlSelItem->parent = group;
  group->children.append(streamFlowCtrlSelItem);

  ConfigItem* linkTimeoutItem = new ConfigItem;
  linkTimeoutItem->name = tr("连接超时时间 (ms)");
  linkTimeoutItem->description = tr("连接超时时间，单位为毫秒");
  linkTimeoutItem->value = config.link_timeout;
  linkTimeoutItem->type = QVariant::UInt;
  linkTimeoutItem->minValue = 0;
  linkTimeoutItem->maxValue = 60000;
  linkTimeoutItem->parent = group;
  group->children.append(linkTimeoutItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createAwbParamsItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("白平衡相关参数");
  group->description = tr("白平衡相关参数设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* awbOperationItem = new ConfigItem;
  awbOperationItem->name = tr("自动白平衡操作方式");
  awbOperationItem->description = tr("自动白平衡操作方式 (0=手动, 1=自动)");
  awbOperationItem->value = config.awb_operation;
  awbOperationItem->type = QVariant::Int;
  awbOperationItem->options = QStringList() << tr("手动") << tr("自动");
  awbOperationItem->parent = group;
  group->children.append(awbOperationItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createTriggerParamsItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("触发相关参数");
  group->description = tr("触发相关的详细参数设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* triggerActivationItem = new ConfigItem;
  triggerActivationItem->name = tr("触发激活方式");
  triggerActivationItem->description = tr("触发信号激活方式");
  triggerActivationItem->value = config.trigger_activation;
  triggerActivationItem->type = QVariant::Int;
  triggerActivationItem->options = QStringList()
                                   << tr("上升沿") << tr("下降沿");
  triggerActivationItem->parent = group;
  group->children.append(triggerActivationItem);

  ConfigItem* triggerCountItem = new ConfigItem;
  triggerCountItem->name = tr("触发计数");
  triggerCountItem->description = tr("触发计数设置");
  triggerCountItem->value = config.trigger_count;
  triggerCountItem->type = QVariant::Int;
  triggerCountItem->minValue = 0;
  triggerCountItem->maxValue = 1000;
  triggerCountItem->parent = group;
  group->children.append(triggerCountItem);

  ConfigItem* triggerDebouncerItem = new ConfigItem;
  triggerDebouncerItem->name = tr("触发消抖时间 (μs)");
  triggerDebouncerItem->description = tr("触发信号消抖时间，单位为微秒");
  triggerDebouncerItem->value = config.trigger_debouncer;
  triggerDebouncerItem->type = QVariant::Double;
  triggerDebouncerItem->minValue = 0.0;
  triggerDebouncerItem->maxValue = 1000000.0;
  triggerDebouncerItem->parent = group;
  group->children.append(triggerDebouncerItem);

  ConfigItem* strobeSourceItem = new ConfigItem;
  strobeSourceItem->name = tr("Strobe信号源");
  strobeSourceItem->description = tr("Strobe信号源设置");
  strobeSourceItem->value = config.strobe_source;
  strobeSourceItem->type = QVariant::Int;
  strobeSourceItem->minValue = 0;
  strobeSourceItem->maxValue = 10;
  strobeSourceItem->parent = group;
  group->children.append(strobeSourceItem);

  ConfigItem* strobeDelayItem = new ConfigItem;
  strobeDelayItem->name = tr("Strobe信号延迟 (μs)");
  strobeDelayItem->description = tr("Strobe信号延迟时间，单位为微秒");
  strobeDelayItem->value = config.strobe_delay;
  strobeDelayItem->type = QVariant::Double;
  strobeDelayItem->minValue = 0.0;
  strobeDelayItem->maxValue = 1000000.0;
  strobeDelayItem->parent = group;
  group->children.append(strobeDelayItem);

  ConfigItem* strobeDurationItem = new ConfigItem;
  strobeDurationItem->name = tr("Strobe信号持续时间 (μs)");
  strobeDurationItem->description = tr("Strobe信号持续时间，单位为微秒");
  strobeDurationItem->value = config.strobe_duration;
  strobeDurationItem->type = QVariant::Double;
  strobeDurationItem->minValue = 0.0;
  strobeDurationItem->maxValue = 1000000.0;
  strobeDurationItem->parent = group;
  group->children.append(strobeDurationItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createLineScanParamsItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("线扫相机专用参数");
  group->description = tr("线扫相机专用参数设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* lineTrigEnableItem = new ConfigItem;
  lineTrigEnableItem->name = tr("线扫触发使能");
  lineTrigEnableItem->description = tr("启用/禁用线扫触发");
  lineTrigEnableItem->value = config.line_trig_enable;
  lineTrigEnableItem->type = QVariant::Bool;
  lineTrigEnableItem->parent = group;
  group->children.append(lineTrigEnableItem);

  ConfigItem* lineTrigSourceItem = new ConfigItem;
  lineTrigSourceItem->name = tr("线扫触发源");
  lineTrigSourceItem->description = tr("线扫触发信号源");
  lineTrigSourceItem->value = config.line_trig_source;
  lineTrigSourceItem->type = QVariant::Int;
  lineTrigSourceItem->minValue = 0;
  lineTrigSourceItem->maxValue = 10;
  lineTrigSourceItem->parent = group;
  group->children.append(lineTrigSourceItem);

  ConfigItem* lineTrigFilterItem = new ConfigItem;
  lineTrigFilterItem->name = tr("线扫触发过滤");
  lineTrigFilterItem->description = tr("线扫触发信号过滤");
  lineTrigFilterItem->value = config.line_trig_filter;
  lineTrigFilterItem->type = QVariant::Double;
  lineTrigFilterItem->minValue = 0.0;
  lineTrigFilterItem->maxValue = 1000000.0;
  lineTrigFilterItem->parent = group;
  group->children.append(lineTrigFilterItem);

  ConfigItem* lineTrigEdgeSelItem = new ConfigItem;
  lineTrigEdgeSelItem->name = tr("线扫触发边沿选择");
  lineTrigEdgeSelItem->description = tr("线扫触发信号边沿选择");
  lineTrigEdgeSelItem->value = config.line_trig_edge_sel;
  lineTrigEdgeSelItem->type = QVariant::Int;
  lineTrigEdgeSelItem->minValue = 0;
  lineTrigEdgeSelItem->maxValue = 5;
  lineTrigEdgeSelItem->parent = group;
  group->children.append(lineTrigEdgeSelItem);

  ConfigItem* lineTrigDelayItem = new ConfigItem;
  lineTrigDelayItem->name = tr("线扫触发延时 (μs)");
  lineTrigDelayItem->description = tr("线扫触发信号延时，单位为微秒");
  lineTrigDelayItem->value = config.line_trig_delay;
  lineTrigDelayItem->type = QVariant::Double;
  lineTrigDelayItem->minValue = 0.0;
  lineTrigDelayItem->maxValue = 1000000.0;
  lineTrigDelayItem->parent = group;
  group->children.append(lineTrigDelayItem);

  ConfigItem* lineTrigDebouncerItem = new ConfigItem;
  lineTrigDebouncerItem->name = tr("线扫触发消抖 (μs)");
  lineTrigDebouncerItem->description = tr("线扫触发信号消抖时间，单位为微秒");
  lineTrigDebouncerItem->value = config.line_trig_debouncer;
  lineTrigDebouncerItem->type = QVariant::Double;
  lineTrigDebouncerItem->minValue = 0.0;
  lineTrigDebouncerItem->maxValue = 1000000.0;
  lineTrigDebouncerItem->parent = group;
  group->children.append(lineTrigDebouncerItem);

  return group;
}

DvpConfigModel::ConfigItem* DvpConfigModel::createAdvancedParamsItem(
    const DvpConfig& config) {
  ConfigItem* group = new ConfigItem;
  group->name = tr("其他高级参数");
  group->description = tr("其他高级参数设置");
  group->icon = QIcon(":/Imgs/settings.svg");
  group->parent = m_rootItem;

  ConfigItem* acquisitionFrameRateItem = new ConfigItem;
  acquisitionFrameRateItem->name = tr("帧率控制");
  acquisitionFrameRateItem->description = tr("帧率控制设置 (0表示不限制)");
  acquisitionFrameRateItem->value = config.acquisition_frame_rate;
  acquisitionFrameRateItem->type = QVariant::Double;
  acquisitionFrameRateItem->minValue = 0.0;
  acquisitionFrameRateItem->maxValue = 1000.0;
  acquisitionFrameRateItem->parent = group;
  group->children.append(acquisitionFrameRateItem);

  ConfigItem* acquisitionFrameRateEnableItem = new ConfigItem;
  acquisitionFrameRateEnableItem->name = tr("帧率控制使能");
  acquisitionFrameRateEnableItem->description = tr("启用/禁用帧率控制");
  acquisitionFrameRateEnableItem->value = config.acquisition_frame_rate_enable;
  acquisitionFrameRateEnableItem->type = QVariant::Bool;
  acquisitionFrameRateEnableItem->parent = group;
  group->children.append(acquisitionFrameRateEnableItem);

  ConfigItem* flatFieldEnableItem = new ConfigItem;
  flatFieldEnableItem->name = tr("平场使能");
  flatFieldEnableItem->description = tr("启用/禁用平场校正");
  flatFieldEnableItem->value = config.flat_field_enable;
  flatFieldEnableItem->type = QVariant::Bool;
  flatFieldEnableItem->parent = group;
  group->children.append(flatFieldEnableItem);

  return group;
}

void DvpConfigModel::saveConfig(DvpConfig& config) const {
  // This would require traversing the tree and mapping values back to the
  // config struct Implementation omitted for brevity
}
