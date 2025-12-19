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
  void addNewSignalSource();  // 添加新信号源

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
