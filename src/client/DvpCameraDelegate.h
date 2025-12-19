#ifndef DVPCAMERADELEGATE_H
#define DVPCAMERADELEGATE_H

#include <QStyledItemDelegate>

class AntBaseSpinBox;
class AntBaseDoubleSpinBox;
class AntToggleButton;
class AntComboBox;
class AntBaseInput;

class DvpCameraDelegate : public QStyledItemDelegate {
  Q_OBJECT

 public:
  explicit DvpCameraDelegate(QObject* parent = nullptr);

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;

  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const override;

  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const override;

 private:
  void setupSpinBox(AntBaseSpinBox* spinBox, const QModelIndex& index) const;
  void setupDoubleSpinBox(AntBaseDoubleSpinBox* spinBox,
                          const QModelIndex& index) const;
  void setupComboBox(AntComboBox* comboBox, const QModelIndex& index) const;
  void setupToggle(AntToggleButton* toggleButton,
                   const QModelIndex& index) const;
};

#endif  // DVPCAMERADELEGATE_H
