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
