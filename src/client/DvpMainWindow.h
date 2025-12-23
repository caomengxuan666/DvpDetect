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
 *  - File: DvpMainWindow.h
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#ifndef DVPMAINWINDOW_H
#define DVPMAINWINDOW_H

#include <QCloseEvent>
#include <QEvent>
#include <QHoverEvent>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QtWidgets/QWidget>

#include "AntInput.h"
#include "CircularAvatar.h"
#include "CustomToolButton.h"
#include "SlideStackedWidget.h"
#include "ui_QtAntDesign.h"

// 前向声明
class DvpCameraModel;
class DvpCameraManager;
class AntBaseComboBox;
class DvpConfigModel;  // 添加缺失的前向声明

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QSplitter)
QT_FORWARD_DECLARE_CLASS(QStackedWidget)
QT_FORWARD_DECLARE_CLASS(QTabWidget)
QT_FORWARD_DECLARE_CLASS(QToolButton)

// ButtonInfo结构体声明
// 按钮数据结构
struct ButtonInfo {
  CustomToolButton* button;
  QString normalIcon;
  QString activeIcon;
  QWidget* page;
};

// 判断是否在windows平台
#ifdef Q_OS_WIN

#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>

#pragma comment(lib, "Dwmapi.lib")

#endif  // Q_OS_WIN

// 检测鼠标是否在边缘
enum ResizeRegion {
  None,
  Left,
  Right,
  Top,
  Bottom,
  TopLeft,
  TopRight,
  BottomLeft,
  BottomRight
};

// 前向声明
class AntTreeView;
class AntBaseTableView;
class AntBaseListView;
class AntButton;
class AntLabel;
class DvpConfigDelegate;
class DvpCameraView;  // 新增前向声明

class DvpMainWindow : public QWidget {
  Q_OBJECT

 signals:
  void resized(int width, int height);
  void windowMoved(QPoint globalPos);  // 窗口移动时发出信号
  void playMaximizeAnim();

 public:
  DvpMainWindow(QWidget* parent = nullptr);
  ~DvpMainWindow();

 protected:
  bool eventFilter(QObject* obj, QEvent* event);
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;
  void changeEvent(QEvent* event) override;
  void moveEvent(QMoveEvent* event) override;
  void paintEvent(QPaintEvent* event) override;
  void showEvent(QShowEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void closeEvent(QCloseEvent* event) override;
  // 修复函数签名，与cpp文件中定义的一致
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool nativeEvent(const QByteArray& eventType, void* message,
                   qintptr* result) override;
#else
  bool nativeEvent(const QByteArray& eventType, void* message,
                   long* result) override;
#endif

 private slots:
  void onMinimumBtnClicked();
  void onMaximumBtnClicked();
  void onCloseBtnClicked();
  void onNavigationBtnClicked();
  void onPageIndexChanged();
  void onSideBarResized(int width);

  // 相机相关槽函数
  void disconnectFromCamera();
  void startCapture();
  void stopCapture();

 private:
  void initWindow();
  void initTitleBar();
  void initNaviBar();
  void initMainPage();
  void initSettingPage();
  void initProfilePage();
  void initSideBar();
  void initConnect();
  void updateMaximizeButton(bool maximized);
  ResizeRegion getResizeRegion(const QPoint& pos);
  void updateCursorShape(const QPoint& pos);
  void restoreMouseCursor();

#ifdef Q_OS_WIN
  void updateWindowShadow();
  int edgeWidth = 10;
  QSize originalSize;
  QRect normalGeometry;
  Qt::Edges currentEdge;  // 鼠标当前所在边/角
#endif

 private:
  // UI组件
  QAction* m_connectAction = nullptr;
  QAction* m_disconnectAction = nullptr;
  QAction* m_startAction = nullptr;
  QAction* m_stopAction = nullptr;
  QAction* m_saveConfigAction = nullptr;
  QAction* m_loadConfigAction = nullptr;
  QAction* m_exitAction = nullptr;
  QAction* m_aboutAction = nullptr;

  // 中央部件
  QSplitter* m_centralSplitter = nullptr;
  QTabWidget* m_previewTabs = nullptr;  // 支持多个相机预览标签页
  QStackedWidget* m_detailStack = nullptr;

  // 左侧停靠窗口 (使用AntDesign控件)
  QWidget* m_cameraDock = nullptr;
  AntTreeView* m_cameraView = nullptr;

  QWidget* m_parameterDock = nullptr;
  AntTreeView* m_parameterView = nullptr;

  // 底部停靠窗口 (使用AntDesign控件)
  QWidget* m_logDock = nullptr;
  AntBaseListView* m_logView = nullptr;

  // 标题栏按钮
  QToolButton* btnMin = nullptr;
  QToolButton* btnMax = nullptr;
  QToolButton* btnClose = nullptr;

 private:
  void setupConfigPage();

 private:
  Ui::QtAntDesignClass ui;
#ifdef Q_OS_WIN
  HWND m_hwnd;
#endif
  QPoint dragPos;
  int m_titleBarHeight = 60;   // 标题栏高度
  int m_naviWidth = 62;        // 导航栏宽度
  int m_widgetTotalWidth = 0;  // 标题栏上控件总宽度
  bool m_disableShadow = false;
  bool m_beforeMax = false;  // 最小化之前是最大化的窗口状态

  int titleBarSpacing = 6;
  int totalSpacingWidth = 0;
  int rightMargin = 12;

  // 窗口缩放
  ResizeRegion m_resizeRegion = None;
  QPoint m_dragStartPos;

  // 物理像素相关变量
  int m_widgetTotalWidthPhysicalPixels = 0;
  int m_titleLeftTotalWidthPhysicalPixels = 0;
  int m_titleBarHeightPhysicalPixels = 0;

  // 相机相关组件
  DvpCameraModel* m_cameraModel = nullptr;
  DvpConfigModel* m_configModel = nullptr;
  DvpCameraManager* m_cameraManager = nullptr;  // 相机管理器

  // 配置页面相关
  DvpConfigModel* configModel = nullptr;
  DvpConfigDelegate* configDelegate = nullptr;

  // 控件
  AntTreeView* configView = nullptr;
  AntButton* m_connectButton = nullptr;
  AntButton* m_disconnectButton = nullptr;
  AntButton* m_startButton = nullptr;
  AntButton* m_stopButton = nullptr;
  AntBaseComboBox* m_cameraSelector = nullptr;

  AntLabel* m_statusLabel = nullptr;
  AntLabel* m_fpsLabel = nullptr;

  // 页面组件
  SlideStackedWidget* stackedWidget = nullptr;
  QWidget* configPage = nullptr;
  DvpCameraView* cameraPage = nullptr;
  QList<ButtonInfo> buttonInfos;
};
#endif  // DVPMAINWINDOW_H
