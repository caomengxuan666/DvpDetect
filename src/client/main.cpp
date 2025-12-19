#include <QDir>
#include <QFontDatabase>
#include <QtWidgets/QApplication>

#include "DvpMainWindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setOrganizationName("Organization");
  a.setApplicationName("DvpDetect");

  // 设置高DPI支持
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  // 加载字体
  QDir dir("fonts");
  if (dir.exists()) {
    const QStringList fonts = dir.entryList(QStringList("*.ttf"));
    for (const QString &font : fonts) {
      QFontDatabase::addApplicationFont(dir.filePath(font));
    }
  }

  DvpMainWindow w;
  w.show();
  return a.exec();
}
