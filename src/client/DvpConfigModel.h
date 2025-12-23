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
 *  - File: DvpConfigModel.h
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#ifndef DVPCONFIGMODEL_H
#define DVPCONFIGMODEL_H

#include <QAbstractItemModel>
#include <QIcon>
#include <QStringList>

#include "DvpConfig.hpp"

class DvpConfigModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  explicit DvpConfigModel(QObject* parent = nullptr);

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
  void loadConfig(const DvpConfig& config);
  void saveConfig(DvpConfig& config) const;

 private:
  struct ConfigItem;

  ConfigItem* m_rootItem;
  void setupModel();
  ConfigItem* createBasicImageParamsItem(const DvpConfig& config);
  ConfigItem* createTriggerModeItem(const DvpConfig& config);
  ConfigItem* createAutoExposureItem(const DvpConfig& config);
  ConfigItem* createAntiFlickerItem(const DvpConfig& config);
  ConfigItem* createAcquisitionModeItem(const DvpConfig& config);
  ConfigItem* createTriggerDetailItem(const DvpConfig& config);
  ConfigItem* createExposureRangeItem(const DvpConfig& config);
  ConfigItem* createGainRangeItem(const DvpConfig& config);
  ConfigItem* createImageEnhanceItem(const DvpConfig& config);
  ConfigItem* createImageProcessItem(const DvpConfig& config);
  ConfigItem* createOtherParamsItem(const DvpConfig& config);
  ConfigItem* createAwbParamsItem(const DvpConfig& config);
  ConfigItem* createTriggerParamsItem(const DvpConfig& config);
  ConfigItem* createLineScanParamsItem(const DvpConfig& config);
  ConfigItem* createAdvancedParamsItem(const DvpConfig& config);
};

// 在类外部定义嵌套结构体
struct DvpConfigModel::ConfigItem {
  QString name;
  QString description;
  QVariant value;
  QVariant::Type type;
  QVariant minValue;
  QVariant maxValue;
  QStringList options;  // 用于枚举类型
  QIcon icon;
  QList<ConfigItem*> children;
  ConfigItem* parent;

  ConfigItem() : parent(nullptr) {}
  ~ConfigItem() { qDeleteAll(children); }
};

#endif  // DVPCONFIGMODEL_H
