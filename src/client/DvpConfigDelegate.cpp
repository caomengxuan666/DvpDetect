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
 *  - File: DvpConfigDelegate.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpConfigDelegate.h"

#include <QDebug>

// Include AntDesign widgets
#include "AntBaseDoubleSpinBox.h"
#include "AntBaseInput.h"
#include "AntBaseSpinBox.h"
#include "AntComboBox.h"
#include "AntToggleButton.h"

DvpConfigDelegate::DvpConfigDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

QWidget *DvpConfigDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const {
  if (index.column() != 1)  // Only edit the value column
    return QStyledItemDelegate::createEditor(parent, option, index);

  // Get the type of data from UserRole
  QVariant::Type type =
      static_cast<QVariant::Type>(index.data(Qt::UserRole).toInt());

  switch (type) {
    case QVariant::Int: {
      AntBaseSpinBox *spinBox = new AntBaseSpinBox(parent);
      setupSpinBox(spinBox, index);
      return spinBox;
    }

    case QVariant::Double: {
      AntBaseDoubleSpinBox *spinBox = new AntBaseDoubleSpinBox(parent);
      setupDoubleSpinBox(spinBox, index);
      return spinBox;
    }

    case QVariant::Bool: {
      AntToggleButton *toggleButton =
          new AntToggleButton(QSize(52, 26), parent);
      toggleButton->setShowText(true);

      // 立即设置按钮的初始状态，避免动画问题
      QVariant value = index.model()->data(index, Qt::EditRole);
      toggleButton->setChecked(value.toBool());

      return toggleButton;
    }

    case QVariant::String: {
      AntBaseInput *lineEdit = new AntBaseInput(parent);
      return lineEdit;
    }

    default:
      // For enum types with options
      QStringList options = index.data(Qt::UserRole + 3).toStringList();
      if (!options.isEmpty()) {
        QStringList subItemMapKeys;             // Empty for simple combo box
        QMap<QString, QStringList> subItemMap;  // Empty map

        AntComboBox *comboBox = new AntComboBox(tr("请选择"), options, parent,
                                                240, false, subItemMap);
        setupComboBox(comboBox, index);
        return comboBox;
      }
      break;
  }

  return QStyledItemDelegate::createEditor(parent, option, index);
}

void DvpConfigDelegate::setEditorData(QWidget *editor,
                                      const QModelIndex &index) const {
  QVariant value = index.model()->data(index, Qt::EditRole);
  QVariant::Type type =
      static_cast<QVariant::Type>(index.data(Qt::UserRole).toInt());

  switch (type) {
    case QVariant::Int: {
      AntBaseSpinBox *spinBox = static_cast<AntBaseSpinBox *>(editor);
      spinBox->setValue(value.toInt());
      break;
    }

    case QVariant::Double: {
      AntBaseDoubleSpinBox *spinBox =
          static_cast<AntBaseDoubleSpinBox *>(editor);
      spinBox->setValue(value.toDouble());
      break;
    }

    case QVariant::Bool: {
      AntToggleButton *toggleButton = static_cast<AntToggleButton *>(editor);
      toggleButton->setChecked(value.toBool());
      break;
    }

    case QVariant::String: {
      AntBaseInput *lineEdit = static_cast<AntBaseInput *>(editor);
      lineEdit->setText(value.toString());
      break;
    }

    default:
      // For enum types with options
      QStringList options = index.data(Qt::UserRole + 3).toStringList();
      if (!options.isEmpty()) {
        AntComboBox *comboBox = static_cast<AntComboBox *>(editor);
        comboBox->setCurrentText(value.toString());
      } else {
        QStyledItemDelegate::setEditorData(editor, index);
      }
      break;
  }
}

void DvpConfigDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const {
  QVariant::Type type =
      static_cast<QVariant::Type>(index.data(Qt::UserRole).toInt());

  switch (type) {
    case QVariant::Int: {
      AntBaseSpinBox *spinBox = static_cast<AntBaseSpinBox *>(editor);
      spinBox->interpretText();
      model->setData(index, spinBox->value(), Qt::EditRole);
      break;
    }

    case QVariant::Double: {
      AntBaseDoubleSpinBox *spinBox =
          static_cast<AntBaseDoubleSpinBox *>(editor);
      spinBox->interpretText();
      model->setData(index, spinBox->value(), Qt::EditRole);
      break;
    }

    case QVariant::Bool: {
      AntToggleButton *toggleButton = static_cast<AntToggleButton *>(editor);
      model->setData(index, toggleButton->isChecked(), Qt::EditRole);
      break;
    }

    case QVariant::String: {
      AntBaseInput *lineEdit = static_cast<AntBaseInput *>(editor);
      model->setData(index, lineEdit->text(), Qt::EditRole);
      break;
    }

    default:
      // For enum types with options
      QStringList options = index.data(Qt::UserRole + 3).toStringList();
      if (!options.isEmpty()) {
        AntComboBox *comboBox = static_cast<AntComboBox *>(editor);
        // 使用属性方式获取当前选中的文本
        model->setData(index, comboBox->property("currentText").toString(),
                       Qt::EditRole);
      } else {
        QStyledItemDelegate::setModelData(editor, model, index);
      }
      break;
  }
}

void DvpConfigDelegate::updateEditorGeometry(QWidget *editor,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const {
  Q_UNUSED(index)
  editor->setGeometry(option.rect);
}

void DvpConfigDelegate::setupSpinBox(AntBaseSpinBox *spinBox,
                                     const QModelIndex &index) const {
  QVariant minVal = index.data(Qt::UserRole + 1);
  QVariant maxVal = index.data(Qt::UserRole + 2);

  if (minVal.isValid()) spinBox->setMinimum(minVal.toInt());
  if (maxVal.isValid()) spinBox->setMaximum(maxVal.toInt());
}

void DvpConfigDelegate::setupDoubleSpinBox(AntBaseDoubleSpinBox *spinBox,
                                           const QModelIndex &index) const {
  QVariant minVal = index.data(Qt::UserRole + 1);
  QVariant maxVal = index.data(Qt::UserRole + 2);

  if (minVal.isValid()) spinBox->setMinimum(minVal.toDouble());
  if (maxVal.isValid()) spinBox->setMaximum(maxVal.toDouble());
}

void DvpConfigDelegate::setupComboBox(AntComboBox *comboBox,
                                      const QModelIndex &index) const {
  Q_UNUSED(comboBox)
  Q_UNUSED(index)
  // ComboBox is already initialized with options in constructor
}
