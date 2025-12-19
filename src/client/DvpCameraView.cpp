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
#include "FlowLayout.h"

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
  QWidget* placeholder = new QWidget;
  placeholder->setStyleSheet(
      "background-color: #222222; border: 1px solid #444444; border-radius: "
      "8px;");
  placeholder->setMinimumHeight(400);
  placeholder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  layout->addWidget(placeholder);
}

void DvpCameraView::addNewSignalSource() {
  // 直接创建新的信号源，不再弹出对话框
  QString signalName = "新信号源";

  // 查找当前的FlowLayout
  QWidget* containerWidget = previewArea->findChild<QWidget*>();
  if (!containerWidget) return;

  FlowLayout* flowLayout = static_cast<FlowLayout*>(containerWidget->layout());
  if (!flowLayout) return;
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
