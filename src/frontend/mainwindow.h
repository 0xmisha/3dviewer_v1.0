
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "qgifimage.h"

#define FRAMES_FOR_GIF 50

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  QGifImage *gif;
  QImage *frame;
  QTimer *timer;
  int count;

 signals:
  void signal_settings(QMap<QString, QString> map);
  void signal_data(double transform_data[3][3]);

 public slots:
  void receiveInfo(int vertices_count, int polygons_count, QString f_name);

 private slots:
  void on_transformButton_clicked();
  void on_applySettings_clicked();

  void on_saveImage_clicked();

  void on_recordScreencast_clicked();
  void save();

 private:
  void setupUI();
  void setupConnections();
  void setupSettings();
  QMap<QString, QString> collectSettings();
  QString determineVertexDisplay();
  Ui::MainWindow *ui;
  QSettings *set;
};

#endif  // MAINWINDOW_H
