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
 *  - File: DvpMainWindow.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpMainWindow.h"

#include <qevent.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qscreen.h>

#include <QHeaderView>
#include <QShowEvent>
#include <QToolButton>
#include <QToolTip>
#include <QWindow>

#include "AntMessageManager.h"
#include "AntTooltipManager.h"
#include "DesignSystem.h"
#include "DialogViewController.h"
#include "MaskWidget.h"
#include "NotificationManager.h"
#include "SlideStackedWidget.h"
#include "StyleSheet.h"
#include "ThemeSwitcher.h"
#include "TransparentMask.h"

// 包含我们新的配置相关类
#include "DvpCameraView.h"  // 添加新的相机视图类
#include "DvpConfigDelegate.h"
#include "DvpConfigModel.h"

// 包含AntDesign控件头文件
#include "AntBaseDoubleSpinBox.h"
#include "AntBaseInput.h"
#include "AntBaseSpinBox.h"
#include "AntButton.h"
#include "AntComboBox.h"
#include "AntSlider.h"
#include "AntToggleButton.h"
#include "AntTreeView.h"

DvpMainWindow::DvpMainWindow(QWidget* parent) : QWidget(parent) {
  ui.setupUi(this);

  setObjectName("DvpMainWindow");
#ifdef Q_OS_LINUX
  setWindowFlags(Qt::FramelessWindowHint);
  // 开启悬浮事件处理鼠标边界样式变化
  setAttribute(Qt::WA_Hover, true);
  installEventFilter(this);
#endif

#ifdef Q_OS_WIN
  setWindowFlags(Qt::FramelessWindowHint);
#endif
  QSize miniSize(1120, 725);
  setMinimumSize(miniSize);

  ui.main_widget->setStyleSheet(
      StyleSheet::mainQss(DesignSystem::instance()->backgroundColor()));

  // 获取主屏幕尺寸
  int w = 0, h = 0;
  QScreen* screen = QGuiApplication::primaryScreen();
  if (screen) {
    QSize screenSize = screen->availableSize();  // 可用屏幕大小，不包括任务栏
    w = int(screenSize.width() * 0.50);          // % 宽度
    h = int(screenSize.height() * 0.60);         // % 高度
    if (w < miniSize.width() && h < miniSize.height()) {
      w = miniSize.width();
      h = miniSize.height();
    }
    resize(w, h);
  }
  setContentsMargins(0, 0, 0, 0);

  // 初始化全局设计系统 必须写在最前面
  // 因为它会设置主题和主窗口指针注册一些全局变量
  DesignSystem::instance()->setThemeMode(DesignSystem::Light);  // 默认亮主题
  DesignSystem::instance()->setMainWindow(this);  // 获取主窗口指针
  ThemeSwitcher* themeSwitcher = new ThemeSwitcher(this);
  themeSwitcher->setThemeColor();
  // 注册全局透明遮罩
  TransparentMask* tpMask = new TransparentMask(this);
  DesignSystem::instance()->setTransparentMask(tpMask);
  // 全局深色动画遮罩
  MaskWidget* darkMask = new MaskWidget(w, h, this);
  DesignSystem::instance()->setDarkMask(darkMask);

  // 任务栏 内容区域 导航栏 布局调整
  ui.navi_widget->setFixedWidth(m_naviWidth);  // 希望的宽度
  ui.navi_widget->setSizePolicy(
      QSizePolicy::Fixed, QSizePolicy::Expanding);  // 水平方向缩放策略固定
  ui.navi_widget->setStyleSheet(
      StyleSheet::naviQss(DesignSystem::instance()->widgetBgColor()));
  ui.titleBar->setStyleSheet(StyleSheet::titleBarQss());
  ui.central->setStyleSheet(StyleSheet::centralQss());
  ui.titleBar->setFixedHeight(m_titleBarHeight);  // 显示标题栏

  // 标题栏
  QHBoxLayout* titleLay = new QHBoxLayout(ui.titleBar);
  titleLay->setContentsMargins(20, 0, rightMargin, 0);
  titleLay->setSpacing(titleBarSpacing);
  QFont font;
  font.setPointSizeF(16);
  font.setBold(true);
  QLabel* title = new QLabel("DVP Client", ui.titleBar);
  title->setFont(font);
  // 创建按钮
  btnMin = new QToolButton(ui.titleBar);
  btnMax = new QToolButton(ui.titleBar);
  btnClose = new QToolButton(ui.titleBar);
  // 设置图标
  btnMin->setIcon(DesignSystem::instance()->btnMinIcon());
  btnMax->setIcon(DesignSystem::instance()->btnMaxIcon());
  btnClose->setIcon(DesignSystem::instance()->btnCloseIcon());
  // 设置QSS
  btnMin->setStyleSheet(StyleSheet::toolBtnQss());
  btnMax->setStyleSheet(StyleSheet::toolBtnQss());
  btnClose->setStyleSheet(StyleSheet::toolBtnQss());
  // 设置固定大小（根据图标适当调整）
  btnMin->setFixedSize(32, 32);
  btnMax->setFixedSize(32, 32);
  btnClose->setFixedSize(34, 34);

  // 标题栏右侧所有控件的长宽转为物理像素后在native事件中限制标题栏的范围
  qreal dpiScale = QApplication::primaryScreen()->devicePixelRatio();
  // 计算控件总宽度（逻辑像素） 右侧4个控件3个间隔
  int widgetTotalWidth =
      rightMargin + btnMin->width() + btnMax->width() + btnClose->width();
  // 转换为物理像素
  m_widgetTotalWidthPhysicalPixels =
      static_cast<int>(widgetTotalWidth * dpiScale);
  // 同理转换标题栏左侧
  m_titleLeftTotalWidthPhysicalPixels =
      static_cast<int>(m_naviWidth * dpiScale);
  // 同理转换标题栏高度
  m_titleBarHeightPhysicalPixels =
      static_cast<int>(m_titleBarHeight * dpiScale);

  // 将标题和按钮添加到布局
  titleLay->addWidget(title);
  titleLay->addStretch();
  titleLay->addWidget(btnMin);
  titleLay->addWidget(btnMax);
  titleLay->addWidget(btnClose);
  totalSpacingWidth = 3 * titleBarSpacing;  // 标题栏右侧4个控件中间3个间隔

  // 导航栏添加控件
  QVBoxLayout* naviLay = new QVBoxLayout(ui.navi_widget);
  ;
  ui.navi_widget->layout()->setContentsMargins(0, 0, 0, 0);
  // 添加页面布局
  QVBoxLayout* contentLay = new QVBoxLayout(ui.central);
  contentLay->setContentsMargins(0, 0, 0, 0);
  contentLay->setSpacing(0);

  stackedWidget = new SlideStackedWidget(ui.central);
  contentLay->addWidget(stackedWidget);

  // 创建我们自己的页面
  configPage = new QWidget(stackedWidget);
  cameraPage = new DvpCameraView(stackedWidget);  // 使用新的相机视图类

  // 为我们的页面设置基本样式和布局
  setupConfigPage();
  // 移除setupCameraPage的调用，因为DvpCameraView会在构造函数中初始化界面

  stackedWidget->addWidget(configPage);
  stackedWidget->addWidget(cameraPage);
  stackedWidget->setCurrentIndex(0);  // 默认显示配置页

  // 导航按钮
  const int naviWidth = ui.navi_widget->width();  // 动态获取导航栏宽度
  const double iconSizeRatio = 0.56;              // 图标占按钮的比例
  // 调整按钮尺寸比例，使高度适中，宽度不过宽
  const int buttonHeight =
      static_cast<int>(naviWidth * 0.8);  // 按钮高度为导航栏宽度的80%
  const int buttonWidth =
      static_cast<int>(naviWidth * 0.9);  // 按钮宽度为导航栏宽度的90%
  const int iconSize = static_cast<int>(buttonHeight * iconSizeRatio);
  CustomToolButton* btnConfig =
      new CustomToolButton(QSize(iconSize, iconSize), ui.navi_widget);
  CustomToolButton* btnCamera =
      new CustomToolButton(QSize(iconSize, iconSize), ui.navi_widget);

  // 设置导航按钮样式
  auto* ins = DesignSystem::instance();
  buttonInfos = {
      {btnConfig, ins->btnSettingsIconPath(), ins->btnSettingsActiveIconPath(),
       configPage},  // 使用设置页面图标（齿轮）
      {btnCamera, ins->btnFuncIconPath(), ins->btnFuncActiveIconPath(),
       cameraPage}  // 使用功能页面图标
  };

  // 设置统一样式和连接信号
  buttonInfos[stackedWidget->currentIndex()].button->setBtnChecked(
      true);  // 设置当前页面按钮为选中状态
  for (const ButtonInfo& info : buttonInfos) {
    CustomToolButton* btn = info.button;
    btn->setSvgIcons(info.normalIcon, info.activeIcon);
    // 调整按钮尺寸
    btn->setFixedSize(QSize(buttonWidth, buttonHeight));
    // 连接信号
    connect(btn, &QToolButton::clicked, [btn, contentLay, this]() {
      // 禁用所有按钮，防止视觉反馈+误点
      for (ButtonInfo& infos : buttonInfos) infos.button->setEnabled(false);

      if (stackedWidget->isAnimationRunning()) return;

      // 更新UI
      QWidget* nextPage = nullptr;
      btn->setBtnChecked(true);
      for (ButtonInfo& infos : buttonInfos) {
        if (infos.button == btn)
          nextPage = infos.page;
        else
          infos.button->setBtnChecked(false);
      }

      // 页面切换 + 结束后恢复按钮
      stackedWidget->slideFromBottomToTop(nextPage, 250, [this]() {
        for (ButtonInfo& infos : buttonInfos) infos.button->setEnabled(true);
      });
    });
  }

  // 主题切换按钮
  QPushButton* themeBtn = new QPushButton(ui.navi_widget);
  themeBtn->setStyleSheet(
      "QPushButton { background-color: transparent; border: none; }");
  themeBtn->setFixedSize(naviWidth * 0.38, naviWidth * 0.38);
  themeBtn->setIcon(DesignSystem::instance()->setThemeIcon());
  themeBtn->setIconSize(themeBtn->size());
  connect(themeBtn, &QPushButton::clicked, this,
          [this, themeSwitcher, themeBtn]() {
            // 获取按钮中心点的全局坐标
            themeSwitcher->startSwitchTheme(
                this->grab(), themeBtn,
                themeBtn->mapToGlobal(themeBtn->rect().center()));
          });
  connect(this, &DvpMainWindow::resized, themeSwitcher,
          &ThemeSwitcher::resizeByMainWindow);

  //  导航栏布局
  naviLay->addSpacing(28);
  naviLay->addSpacing(16);
  naviLay->addWidget(btnConfig, 0, Qt::AlignHCenter);
  naviLay->addSpacing(10);  // 增加按钮之间的间距
  naviLay->addWidget(btnCamera, 0, Qt::AlignHCenter);
  naviLay->addStretch();
  naviLay->addWidget(themeBtn, 0, Qt::AlignHCenter);
  naviLay->addSpacing(70);

  // 初始化全局管理器
  AntMessageManager::instance();  // 全局消息

  // 连接按钮信号
  connect(btnMin, &QToolButton::clicked, this, &QWidget::showMinimized);
  connect(btnMax, &QToolButton::clicked, this, [=]() {
    if (isMaximized()) {
      showNormal();
      btnMax->setIcon(DesignSystem::instance()->btnMaxIcon());
    } else {
      showMaximized();
      btnMax->setIcon(DesignSystem::instance()->btnRestoreIcon());
    }
  });
  connect(btnClose, &QToolButton::clicked, this, &QWidget::close);

  // 添加标题栏鼠标事件处理
  ui.titleBar->installEventFilter(this);
}

DvpMainWindow::~DvpMainWindow() {}

void DvpMainWindow::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  emit resized(width(), height());
}

bool DvpMainWindow::nativeEvent(const QByteArray& eventType, void* message,
                                qintptr* result) {
#ifdef Q_OS_WIN
  if (eventType == "windows_generic_MSG" ||
      eventType == "windows_dispatcher_MSG") {
    MSG* msg = static_cast<MSG*>(message);

    switch (msg->message) {
      case WM_NCCALCSIZE: {
        if (msg->wParam) {
          NCCALCSIZE_PARAMS* ncParams =
              reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
          *result = WVR_REDRAW;
          return true;
        }
        break;
      }
      case WM_NCHITTEST: {
        QPoint pos = mapFromGlobal(
            QPoint(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)));

        // 左侧边框
        if (pos.x() >= 0 && pos.x() <= 5) m_resizeRegion = Left;
        // 右侧边框
        else if (pos.x() >= width() - 5 && pos.x() <= width())
          m_resizeRegion = Right;
        // 上边框
        else if (pos.y() >= 0 && pos.y() <= 5)
          m_resizeRegion = Top;
        // 下边框
        else if (pos.y() >= height() - 5 && pos.y() <= height())
          m_resizeRegion = Bottom;
        // 左上角
        else if (pos.x() >= 0 && pos.x() <= 5 && pos.y() >= 0 && pos.y() <= 5)
          m_resizeRegion = TopLeft;
        // 右上角
        else if (pos.x() >= width() - 5 && pos.x() <= width() && pos.y() >= 0 &&
                 pos.y() <= 5)
          m_resizeRegion = TopRight;
        // 左下角
        else if (pos.x() >= 0 && pos.x() <= 5 && pos.y() >= height() - 5 &&
                 pos.y() <= height())
          m_resizeRegion = BottomLeft;
        // 右下角
        else if (pos.x() >= width() - 5 && pos.x() <= width() &&
                 pos.y() >= height() - 5 && pos.y() <= height())
          m_resizeRegion = BottomRight;
        else
          m_resizeRegion = None;

        // 如果鼠标在可调整大小区域内，则返回相应的结果
        if (m_resizeRegion != None) {
          *result = HTNOWHERE;
          switch (m_resizeRegion) {
            case Left:
              *result = HTLEFT;
              break;
            case Right:
              *result = HTRIGHT;
              break;
            case Top:
              *result = HTTOP;
              break;
            case Bottom:
              *result = HTBOTTOM;
              break;
            case TopLeft:
              *result = HTTOPLEFT;
              break;
            case TopRight:
              *result = HTTOPRIGHT;
              break;
            case BottomLeft:
              *result = HTBOTTOMLEFT;
              break;
            case BottomRight:
              *result = HTBOTTOMRIGHT;
              break;
          }
          return true;
        }
        break;
      }
      case WM_NCLBUTTONDOWN: {
        if (m_resizeRegion != None) {
          SendMessage(m_hwnd, WM_SYSCOMMAND, SC_SIZE + m_resizeRegion, 0);
          *result = 0;
          return true;
        }
        break;
      }
      case WM_CREATE: {
        m_hwnd = reinterpret_cast<HWND>(winId());
        break;
      }
      case WM_ACTIVATE: {
        if (!m_disableShadow) {
          BOOL value = TRUE;
          DwmSetWindowAttribute(m_hwnd, DWMWA_NCRENDERING_ENABLED, &value,
                                sizeof(value));
        }
        break;
      }
    }
  }
#endif  // Q_OS_WIN
  return QWidget::nativeEvent(eventType, message, result);
}

void DvpMainWindow::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);
#ifdef Q_OS_WIN
  m_hwnd = reinterpret_cast<HWND>(winId());
  if (!m_disableShadow) {
    BOOL value = TRUE;
    DwmSetWindowAttribute(m_hwnd, DWMWA_NCRENDERING_ENABLED, &value,
                          sizeof(value));
  }
#endif
}

void DvpMainWindow::moveEvent(QMoveEvent* event) {
  QWidget::moveEvent(event);
  emit windowMoved(event->pos());
}

void DvpMainWindow::changeEvent(QEvent* event) {
  QWidget::changeEvent(event);
  if (event->type() == QEvent::WindowStateChange) {
    if (isMaximized()) {
      btnMax->setIcon(DesignSystem::instance()->btnRestoreIcon());
    } else {
      btnMax->setIcon(DesignSystem::instance()->btnMaxIcon());
    }
  }
}

bool DvpMainWindow::eventFilter(QObject* obj, QEvent* event) {
  // 处理标题栏的鼠标事件以支持窗口拖动
  if (obj == ui.titleBar) {
    static QPoint dragPosition;

    if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      if (mouseEvent->button() == Qt::LeftButton) {
        dragPosition = mouseEvent->globalPos() - frameGeometry().topLeft();
        return true;
      }
    } else if (event->type() == QEvent::MouseMove) {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      if (mouseEvent->buttons() & Qt::LeftButton) {
        move(mouseEvent->globalPos() - dragPosition);
        return true;
      }
    }
  }

#ifdef Q_OS_LINUX
  if (event->type() == QEvent::MouseButtonPress) {
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    if (mouseEvent->button() == Qt::LeftButton) {
      dragPos = mouseEvent->globalPos() - frameGeometry().topLeft();
    }
  } else if (event->type() == QEvent::MouseMove) {
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    if (mouseEvent->buttons() & Qt::LeftButton) {
      move(mouseEvent->globalPos() - dragPos);
    } else {
      updateCursor(mouseEvent->pos());
    }
    return true;
  } else if (event->type() == QEvent::MouseButtonDblClick) {
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    if (mouseEvent->button() == Qt::LeftButton) {
      if (isMaximized()) {
        showNormal();
      } else {
        showMaximized();
      }
    }
  }
#endif

  return QWidget::eventFilter(obj, event);
}

#ifdef Q_OS_LINUX
void DvpMainWindow::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    dragPos = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
}

void DvpMainWindow::mouseDoubleClickEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    if (isMaximized()) {
      showNormal();
    } else {
      showMaximized();
    }
    event->accept();
  }
}

void DvpMainWindow::updateCursor(const QPoint& pos) {
  int x = pos.x();
  int y = pos.y();
  int w = width();
  int h = height();

  if (x < edgeWidth && y < edgeWidth) {
    setCursor(Qt::SizeFDiagCursor);
    currentEdge = Qt::TopEdge | Qt::LeftEdge;
  } else if (x > w - edgeWidth && y < edgeWidth) {
    setCursor(Qt::SizeBDiagCursor);
    currentEdge = Qt::TopEdge | Qt::RightEdge;
  } else if (x < edgeWidth && y > h - edgeWidth) {
    setCursor(Qt::SizeBDiagCursor);
    currentEdge = Qt::BottomEdge | Qt::LeftEdge;
  } else if (x > w - edgeWidth && y > h - edgeWidth) {
    setCursor(Qt::SizeFDiagCursor);
    currentEdge = Qt::BottomEdge | Qt::RightEdge;
  } else if (x < edgeWidth) {
    setCursor(Qt::SizeHorCursor);
    currentEdge = Qt::LeftEdge;
  } else if (x > w - edgeWidth) {
    setCursor(Qt::SizeHorCursor);
    currentEdge = Qt::RightEdge;
  } else if (y < edgeWidth) {
    setCursor(Qt::SizeVerCursor);
    currentEdge = Qt::TopEdge;
  } else if (y > h - edgeWidth) {
    setCursor(Qt::SizeVerCursor);
    currentEdge = Qt::BottomEdge;
  } else {
    unsetCursor();
    currentEdge = Qt::Edges();
  }
}
#endif  // Q_OS_LINUX

void DvpMainWindow::setupConfigPage() {
  // 创建配置页面的布局和控件，使用AntDesign控件
  QVBoxLayout* layout = new QVBoxLayout(configPage);
  layout->setSpacing(10);
  layout->setContentsMargins(20, 20, 20, 20);

  // 标题
  QLabel* titleLabel = new QLabel("配置文件修改");
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(18);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
  layout->addWidget(titleLabel);

  // 创建配置模型和视图
  configModel = new DvpConfigModel(this);
  configDelegate = new DvpConfigDelegate(this);

  // 加载默认配置
  DvpConfig defaultConfig;
  configModel->loadConfig(defaultConfig);

  // 创建树形视图展示配置项
  configView = new AntTreeView(30, configPage);
  configView->setModel(configModel);
  configView->setItemDelegateForColumn(
      1, configDelegate);  // 仅对值列使用自定义委托

  // 启用编辑功能
  configView->setEditTriggers(QAbstractItemView::DoubleClicked |
                              QAbstractItemView::EditKeyPressed |
                              QAbstractItemView::SelectedClicked);

  // 展开所有项
  for (int i = 0; i < configModel->rowCount(); ++i) {
    QModelIndex index = configModel->index(i, 0);
    configView->expand(index);
  }

  // 设置列宽 - 允许用户调整列宽
  configView->setColumnWidth(0, 300);  // 名称列，设置较小的初始宽度
  configView->setColumnWidth(1, 100);  // 值列，固定宽度
  configView->setColumnWidth(2, 200);  // 描述列，适当宽度
  configView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // 启用用户交互调整列宽
  configView->header()->setSectionResizeMode(QHeaderView::Interactive);
  // 允许第一列根据内容自动调整大小
  configView->header()->setSectionResizeMode(0, QHeaderView::Interactive);
  // 设置提示信息，当文本过长时显示完整内容
  configView->setMouseTracking(true);

  layout->addWidget(configView);

  // 文件路径选择和按钮区域 - 使用水平布局并减小占用空间
  QHBoxLayout* bottomLayout = new QHBoxLayout();

  // 文件路径选择
  QLabel* filePathLabel = new QLabel("配置文件路径:");
  AntBaseInput* filePathEdit = new AntBaseInput(configPage);
  AntButton* browseButton = new AntButton("浏览...", 9, configPage);

  // 设置按钮合适的尺寸
  browseButton->setFixedHeight(30);

  bottomLayout->addWidget(filePathLabel);
  bottomLayout->addWidget(filePathEdit);
  bottomLayout->addWidget(browseButton);

  // 按钮区域 - 使用AntDesign按钮
  AntButton* loadButton = new AntButton("加载配置", 9, configPage);
  AntButton* saveButton = new AntButton("保存配置", 9, configPage);
  AntButton* resetButton = new AntButton("重置", 9, configPage);

  // 设置按钮合适的尺寸
  loadButton->setFixedHeight(30);
  saveButton->setFixedHeight(30);
  resetButton->setFixedHeight(30);

  bottomLayout->addWidget(loadButton);
  bottomLayout->addWidget(saveButton);
  bottomLayout->addWidget(resetButton);

  // 限制底部布局的高度
  QWidget* bottomWidget = new QWidget(configPage);
  bottomWidget->setLayout(bottomLayout);
  bottomWidget->setMaximumHeight(60);  // 限制最大高度

  layout->addWidget(bottomWidget);

  configPage->setLayout(layout);

  // 连接信号槽，处理鼠标悬停显示完整文本
  connect(configView, &AntTreeView::entered, this,
          [this](const QModelIndex& index) {
            if (index.column() == 0) {  // 只处理名称列
              QString fullText =
                  configModel->data(index, Qt::DisplayRole).toString();
              QToolTip::showText(QCursor::pos(), fullText, configView);
            }
          });
}

void DvpMainWindow::onMinimumBtnClicked() { showMinimized(); }

void DvpMainWindow::onMaximumBtnClicked() {
  if (isMaximized()) {
    showNormal();
    btnMax->setIcon(DesignSystem::instance()->btnMaxIcon());
  } else {
    showMaximized();
    btnMax->setIcon(DesignSystem::instance()->btnRestoreIcon());
  }
}

void DvpMainWindow::onCloseBtnClicked() { close(); }

void DvpMainWindow::onNavigationBtnClicked() {
  // TODO: 实现导航按钮点击逻辑
}

void DvpMainWindow::onPageIndexChanged() {
  // TODO: 实现页面索引改变逻辑
}

void DvpMainWindow::onSideBarResized(int width) {
  // TODO: 实现侧边栏调整大小逻辑
}

void DvpMainWindow::disconnectFromCamera() {
  // TODO: 实现断开相机连接逻辑
}

void DvpMainWindow::startCapture() {
  // TODO: 实现开始捕获逻辑
}

void DvpMainWindow::stopCapture() {
  // TODO: 实现停止捕获逻辑
}

void DvpMainWindow::mousePressEvent(QMouseEvent* event) {
#ifdef Q_OS_WIN
  if (event->button() == Qt::LeftButton) {
    dragPos = event->globalPosition().toPoint();
    event->accept();
  }
#endif
  QWidget::mousePressEvent(event);
}

void DvpMainWindow::mouseMoveEvent(QMouseEvent* event) {
#ifdef Q_OS_WIN
  if (event->buttons() & Qt::LeftButton) {
    if (isMaximized()) {
      // 最大化时不允许拖动
      return;
    }

    QPoint delta = event->globalPosition().toPoint() - dragPos;
    move(pos() + delta);
    dragPos = event->globalPosition().toPoint();
    event->accept();
  }
#endif
  QWidget::mouseMoveEvent(event);
}

void DvpMainWindow::mouseReleaseEvent(QMouseEvent* event) {
  QWidget::mouseReleaseEvent(event);
}

void DvpMainWindow::mouseDoubleClickEvent(QMouseEvent* event) {
#ifdef Q_OS_WIN
  // 双击标题栏最大化/还原
  if (event->button() == Qt::LeftButton) {
    if (isMaximized()) {
      showNormal();
      btnMax->setIcon(DesignSystem::instance()->btnMaxIcon());
    } else {
      showMaximized();
      btnMax->setIcon(DesignSystem::instance()->btnRestoreIcon());
    }
  }
#endif
  QWidget::mouseDoubleClickEvent(event);
}

void DvpMainWindow::paintEvent(QPaintEvent* event) {
  QWidget::paintEvent(event);

  // 绘制窗口阴影效果
#ifdef Q_OS_WIN
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // 绘制边框阴影
  if (!isMaximized()) {
    QColor shadowColor = QColor(0, 0, 0, 30);
    painter.setPen(QPen(shadowColor, 1));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
  }
#endif
}

void DvpMainWindow::closeEvent(QCloseEvent* event) {
  // 停止所有相机捕获
  if (m_cameraManager) {
    // TODO: 停止所有相机
  }

  QWidget::closeEvent(event);
}
