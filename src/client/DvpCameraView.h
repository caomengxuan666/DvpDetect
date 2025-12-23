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
 *  - File: DvpCameraView.h
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#ifndef DVPCAMERAVIEW_H
#define DVPCAMERAVIEW_H

#include <QModelIndex>
#include <QWidget>

class DvpCameraModel;
class DvpCameraDelegate;
class AntTreeView;
class AntButton;
class AntBaseInput;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class AntTabBarWidget;     // 添加AntTabBarWidget的前向声明
class FlowLayout;          // 添加FlowLayout的前向声明
class SignalSourceWidget;  // 添加SignalSourceWidget的前向声明

class DvpCameraView : public QWidget {
  Q_OBJECT

 public:
  explicit DvpCameraView(QWidget* parent = nullptr);

  void loadVisionParams();

 private slots:
  void onCameraSelectionChanged(const QModelIndex& current,
                                const QModelIndex& previous);

 private:
  void setupUI();
  void setupPreviewArea();
  void setupControlPanel();

  // UI 组件
  QWidget* previewArea;
  QWidget* controlPanel;
  AntTreeView* cameraTreeView;
  DvpCameraModel* cameraModel;
  DvpCameraDelegate* cameraDelegate;

  // 控件
  AntButton* settingsButton;  // 更改为设置按钮
  AntBaseInput* filePathEdit;

  // 布局
  QHBoxLayout* mainLayout;
  QVBoxLayout* previewLayout;
  QVBoxLayout* controlLayout;
};

#endif  // DVPCAMERAVIEW_H
