#ifndef DVPCONFIGDELEGATE_H
#define DVPCONFIGDELEGATE_H

#include <QStyledItemDelegate>

class AntBaseSpinBox;
class AntBaseDoubleSpinBox;
class AntToggleButton;
class AntComboBox;
class AntBaseInput;

class DvpConfigDelegate : public QStyledItemDelegate {
  Q_OBJECT

 public:
  explicit DvpConfigDelegate(QObject *parent = nullptr);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;

  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const override;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;

 private:
  void setupSpinBox(AntBaseSpinBox *spinBox, const QModelIndex &index) const;
  void setupDoubleSpinBox(AntBaseDoubleSpinBox *spinBox,
                          const QModelIndex &index) const;
  void setupComboBox(AntComboBox *comboBox, const QModelIndex &index) const;
};

#endif  // DVPCONFIGDELEGATE_H
