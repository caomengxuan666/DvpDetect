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
 *  - File: DvpCameraDelegate.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpCameraDelegate.h"

#include "AntBaseDoubleSpinBox.h"
#include "AntBaseInput.h"
#include "AntBaseSpinBox.h"
#include "AntComboBox.h"
#include "AntToggleButton.h"

DvpCameraDelegate::DvpCameraDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

QWidget* DvpCameraDelegate::createEditor(QWidget* parent,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const {
  Q_UNUSED(option)

  if (index.column() != 1)  // 只在值列创建编辑器
    return nullptr;

  // 获取模型中的数据项
  void* ptr = index.data(Qt::UserRole).value<void*>();
  if (!ptr) return nullptr;

  // 注意：由于我们在头文件中前向声明了嵌套结构体，这里无法直接访问其字段
  // 我们只能通过模型的data接口获取数据

  QVariant value = index.model()->data(index, Qt::EditRole);
  QString typeName = value.typeName();

  if (typeName == "int") {
    AntBaseSpinBox* spinBox = new AntBaseSpinBox(parent);
    setupSpinBox(spinBox, index);
    return spinBox;
  } else if (typeName == "double" || typeName == "float") {
    AntBaseDoubleSpinBox* spinBox = new AntBaseDoubleSpinBox(parent);
    setupDoubleSpinBox(spinBox, index);
    return spinBox;
  } else if (typeName == "bool") {
    AntToggleButton* toggleButton = new AntToggleButton(QSize(52, 26), parent);
    setupToggle(toggleButton, index);
    return toggleButton;
  } else {
    // 检查是否有选项列表
    QVariant optionsData = index.model()->data(index, Qt::UserRole + 1);
    if (optionsData.isValid() && optionsData.type() == QVariant::StringList) {
      QStringList options = optionsData.toStringList();
      QString currentText = value.toString();

      AntComboBox* comboBox =
          new AntComboBox(currentText, options, parent, 240, false);
      setupComboBox(comboBox, index);
      return comboBox;
    }

    AntBaseInput* input = new AntBaseInput(parent);
    return input;
  }

  return nullptr;
}

void DvpCameraDelegate::setEditorData(QWidget* editor,
                                      const QModelIndex& index) const {
  QVariant value = index.model()->data(index, Qt::EditRole);

  if (AntBaseSpinBox* spinBox = qobject_cast<AntBaseSpinBox*>(editor)) {
    spinBox->setValue(value.toInt());
  } else if (AntBaseDoubleSpinBox* doubleSpinBox =
                 qobject_cast<AntBaseDoubleSpinBox*>(editor)) {
    doubleSpinBox->setValue(value.toDouble());
  } else if (AntToggleButton* toggleButton =
                 qobject_cast<AntToggleButton*>(editor)) {
    toggleButton->setChecked(value.toBool());
  } else if (AntComboBox* comboBox = qobject_cast<AntComboBox*>(editor)) {
    QString text = value.toString();
    comboBox->setCurrentText(text);
  } else if (AntBaseInput* input = qobject_cast<AntBaseInput*>(editor)) {
    input->setText(value.toString());
  }
}

void DvpCameraDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                     const QModelIndex& index) const {
  if (AntBaseSpinBox* spinBox = qobject_cast<AntBaseSpinBox*>(editor)) {
    spinBox->interpretText();
    model->setData(index, spinBox->value());
  } else if (AntBaseDoubleSpinBox* doubleSpinBox =
                 qobject_cast<AntBaseDoubleSpinBox*>(editor)) {
    doubleSpinBox->interpretText();
    model->setData(index, doubleSpinBox->value());
  } else if (AntToggleButton* toggleButton =
                 qobject_cast<AntToggleButton*>(editor)) {
    model->setData(index, toggleButton->isChecked());
  } else if (AntComboBox* comboBox = qobject_cast<AntComboBox*>(editor)) {
    model->setData(index, comboBox->getCurrentText());
  } else if (AntBaseInput* input = qobject_cast<AntBaseInput*>(editor)) {
    model->setData(index, input->text());
  }
}

void DvpCameraDelegate::updateEditorGeometry(QWidget* editor,
                                             const QStyleOptionViewItem& option,
                                             const QModelIndex& index) const {
  Q_UNUSED(index)
  editor->setGeometry(option.rect);
}

void DvpCameraDelegate::setupSpinBox(AntBaseSpinBox* spinBox,
                                     const QModelIndex& index) const {
  QVariant minData = index.model()->data(index, Qt::UserRole + 2);
  QVariant maxData = index.model()->data(index, Qt::UserRole + 3);

  if (minData.isValid()) {
    spinBox->setMinimum(minData.toInt());
  } else {
    spinBox->setMinimum(0);
  }

  if (maxData.isValid()) {
    spinBox->setMaximum(maxData.toInt());
  } else {
    spinBox->setMaximum(1000);
  }
}

void DvpCameraDelegate::setupDoubleSpinBox(AntBaseDoubleSpinBox* spinBox,
                                           const QModelIndex& index) const {
  QVariant minData = index.model()->data(index, Qt::UserRole + 2);
  QVariant maxData = index.model()->data(index, Qt::UserRole + 3);

  if (minData.isValid()) {
    spinBox->setMinimum(minData.toDouble());
  } else {
    spinBox->setMinimum(0.0);
  }

  if (maxData.isValid()) {
    spinBox->setMaximum(maxData.toDouble());
  } else {
    spinBox->setMaximum(1000.0);
  }

  spinBox->setDecimals(2);
}

void DvpCameraDelegate::setupComboBox(AntComboBox* comboBox,
                                      const QModelIndex& index) const {
  Q_UNUSED(comboBox)
  Q_UNUSED(index)
  // ComboBox已在创建时设置好选项，不需要额外设置
}

void DvpCameraDelegate::setupToggle(AntToggleButton* toggleButton,
                                    const QModelIndex& index) const {
  Q_UNUSED(toggleButton)
  Q_UNUSED(index)
  // Toggle button已在创建时设置了必要属性，不需要额外设置
}
