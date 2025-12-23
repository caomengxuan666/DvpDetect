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
 *  - File: DvpCameraView.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpCameraView.h"

#include <QApplication>
#include <QDrag>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMimeData>
#include <QMouseEvent>
#include <QSplitter>
#include <QVBoxLayout>

#include "AntBaseInput.h"
#include "AntBaseSpinBox.h"
#include "AntButton.h"
#include "AntComboBox.h"
#include "AntSlider.h"
#include "AntToggleButton.h"
#include "AntTreeView.h"
#include "DvpCameraDelegate.h"
#include "DvpCameraModel.h"

DvpCameraView::DvpCameraView(QWidget* parent)
    : QWidget(parent),
      previewArea(new QWidget),
      controlPanel(new QWidget),
      cameraModel(new DvpCameraModel(this)),
      cameraDelegate(new DvpCameraDelegate(this)),
      filePathEdit(new AntBaseInput) {
  setupUI();
  loadVisionParams();
}

void DvpCameraView::setupUI() {
  // 创建主布局，使用分割器实现左右布局
  QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

  // 设置左右区域
  setupPreviewArea();
  setupControlPanel();

  splitter->addWidget(previewArea);
  splitter->addWidget(controlPanel);
  splitter->setSizes(QList<int>() << 600 << 400);  // 设置初始大小

  // 创建整体布局
  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);
  layout->addWidget(splitter);
  setLayout(layout);
}

void DvpCameraView::setupPreviewArea() {
  previewArea->setObjectName("previewArea");

  QVBoxLayout* layout = new QVBoxLayout(previewArea);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(10);

  // 标题
  QLabel* titleLabel = new QLabel("图像预览区");
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(16);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
  layout->addWidget(titleLabel);

  // 空白占位区域 - 替代原有的视频预览内容
  // TODO 我们需要在这里实现真正的视频预览区域，从队列中获取图像
  /*
   * 我的计划是这样的：用不同的信号源来进行代表，我们可以新增信号源
   * 我们把这个分成：新增相机，以及新增信号源两个步骤，
   * 一个相机可以获取多个信号源，比如说原始图像，以及每个处理过的图像
   * 这也就意味着我在SDK里面需要提供一个接口，每个处理后的图像都可以通过这个接口被返回显示
   * 我的想法就是说，原始图像是在图像队列里面的，我们取出来做图像算法处理之后最终结果在另一个队列中
   * 那么在这个过程总如果我们需要DEBUG，我们必须在每个SDK的算法步骤中内置AOP插入点，获取每一个算法处理后的图像
   * 我们的SDK必须是
   */
  QWidget* placeholder = new QWidget;
  placeholder->setStyleSheet(
      "background-color: #222222; border: 1px solid #444444; border-radius: "
      "8px;");
  placeholder->setMinimumHeight(400);
  placeholder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  layout->addWidget(placeholder);
}

void DvpCameraView::loadVisionParams() { cameraModel->loadVisionParams(); }

void DvpCameraView::onCameraSelectionChanged(const QModelIndex& current,
                                             const QModelIndex& previous) {
  Q_UNUSED(current)
  Q_UNUSED(previous)
  // TODO: 实现相机选择变更处理逻辑
}

void DvpCameraView::setupControlPanel() {
  controlPanel->setObjectName("controlPanel");
  controlPanel->setMinimumWidth(350);

  QVBoxLayout* layout = new QVBoxLayout(controlPanel);
  layout->setContentsMargins(10, 0, 10, 0);
  layout->setSpacing(15);

  // 标题
  QLabel* titleLabel = new QLabel("视觉检测参数设置");
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(16);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
  layout->addWidget(titleLabel);

  // 创建树形视图展示视觉检测参数
  cameraTreeView = new AntTreeView(30, controlPanel);
  cameraTreeView->setModel(cameraModel);
  cameraTreeView->setItemDelegateForColumn(1, cameraDelegate);

  // 启用编辑功能
  cameraTreeView->setEditTriggers(QAbstractItemView::DoubleClicked |
                                  QAbstractItemView::EditKeyPressed |
                                  QAbstractItemView::SelectedClicked);

  // 展开所有项
  for (int i = 0; i < cameraModel->rowCount(); ++i) {
    QModelIndex index = cameraModel->index(i, 0);
    cameraTreeView->expand(index);
  }

  // 设置列宽 - 使用三栏布局规范
  cameraTreeView->setColumnWidth(0, 200);  // 名称列
  cameraTreeView->setColumnWidth(1, 80);   // 值列
  cameraTreeView->setColumnWidth(2, 150);  // 描述列

  // 启用用户交互调整列宽
  cameraTreeView->header()->setSectionResizeMode(QHeaderView::Interactive);
  // 允许第一列根据内容自动调整大小
  cameraTreeView->header()->setSectionResizeMode(0, QHeaderView::Interactive);

  // 设置提示信息，当文本过长时显示完整内容
  cameraTreeView->setMouseTracking(true);

  layout->addWidget(cameraTreeView);

  // 文件路径设置 - 改为按钮打开对话框
  QGroupBox* fileGroup = new QGroupBox("文件设置");
  QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);
  fileLayout->setSpacing(10);

  QHBoxLayout* fileLayout1 = new QHBoxLayout;
  QLabel* filePathLabel = new QLabel("配置文件:");
  filePathEdit->setPlaceholderText("配置文件路径");

  AntButton* browseButton = new AntButton("浏览", 12, nullptr);  // 增加按钮高度
  browseButton->setFixedHeight(35);

  fileLayout1->addWidget(filePathLabel);
  fileLayout1->addWidget(filePathEdit);
  fileLayout1->addWidget(browseButton);

  QHBoxLayout* fileLayout2 = new QHBoxLayout;
  AntButton* loadButton =
      new AntButton("加载配置", 12, nullptr);  // 增加按钮高度
  loadButton->setFixedHeight(35);

  AntButton* saveButton =
      new AntButton("保存配置", 12, nullptr);  // 增加按钮高度
  saveButton->setFixedHeight(35);

  AntButton* resetButton = new AntButton("重置", 12, nullptr);  // 增加按钮高度
  resetButton->setFixedHeight(35);

  fileLayout2->addWidget(loadButton);
  fileLayout2->addWidget(saveButton);
  fileLayout2->addWidget(resetButton);

  fileLayout->addLayout(fileLayout1);
  fileLayout->addLayout(fileLayout2);

  layout->addWidget(fileGroup);

  // 相机连接设置 - 改为按钮打开对话框
  QGroupBox* cameraGroup = new QGroupBox("相机连接");
  QVBoxLayout* cameraLayout = new QVBoxLayout(cameraGroup);
  cameraLayout->setSpacing(10);

  AntButton* connectButton =
      new AntButton("连接相机", 12, nullptr);  // 增加按钮高度
  connectButton->setFixedHeight(35);

  AntButton* disconnectButton =
      new AntButton("断开连接", 12, nullptr);  // 增加按钮高度
  disconnectButton->setFixedHeight(35);

  cameraLayout->addWidget(connectButton);
  cameraLayout->addWidget(disconnectButton);

  layout->addWidget(cameraGroup);

  // 移除了控制面板中关于相机连接的信号槽连接，因为相关按钮已被移除
  connect(cameraTreeView->selectionModel(),
          &QItemSelectionModel::currentChanged, this,
          &DvpCameraView::onCameraSelectionChanged);
}
