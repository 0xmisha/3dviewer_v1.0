
#include "mainwindow.h"

#include <QtWidgets>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  set = new QSettings("settings.init", QSettings::IniFormat);

  connect(ui->widget, &MyOpenGLWidget::send_info, this,
          &MainWindow::receiveInfo);
  connect(this, &MainWindow::signal_data, ui->widget,
          &MyOpenGLWidget::on_transformButton_clicked2);
  connect(this, &MainWindow::signal_settings, ui->widget,
          &MyOpenGLWidget::on_applySettingsButton_clicked);

  ui->lineColorBox->setCurrentText(set->value("lineColor").toString());
  ui->lineWidthBox->setValue(set->value("lineWidth").toInt());
  ui->vertexColorBox->setCurrentText(set->value("vertexColor").toString());
  ui->vertexSizeBox->setValue(set->value("vertexSize").toInt());
  ui->backgroundColorBox->setCurrentText(
      set->value("backgroundColor").toString());
}

MainWindow::~MainWindow() { delete ui; }


void MainWindow::setupUI() {
    ui->setupUi(this);
}

void MainWindow::setupConnections() {
    connect(ui->widget, &MyOpenGLWidget::send_info, this, &MainWindow::receiveInfo);
    connect(this, &MainWindow::signal_data, ui->widget, &MyOpenGLWidget::on_transformButton_clicked2);
    connect(this, &MainWindow::signal_settings, ui->widget, &MyOpenGLWidget::on_applySettingsButton_clicked);
}

void MainWindow::setupSettings() {
    set = new QSettings("launch_settings.init", QSettings::IniFormat);

    ui->lineColorBox->setCurrentText(set->value("lineColor").toString());
    ui->lineWidthBox->setValue(set->value("lineWidth").toInt());
    ui->vertexColorBox->setCurrentText(set->value("vertexColor").toString());
    ui->vertexSizeBox->setValue(set->value("vertexSize").toInt());
    ui->backgroundColorBox->setCurrentText(set->value("backgroundColor").toString());
}

void MainWindow::receiveInfo(int vertices_count, int polygons_count,
                             QString f_name) {
  ui->vertices->setText(QString::number(vertices_count));
  ui->polygons->setText(QString::number(polygons_count));
  ui->fileName->setText(f_name);

  update();
}

void MainWindow::on_transformButton_clicked() {
  double transform_data[3][3] = {
      {ui->doubleSpinBox->value(), ui->doubleSpinBox_2->value(),
       ui->doubleSpinBox_3->value()},
      {ui->doubleSpinBox_4->value(), ui->doubleSpinBox_5->value(),
       ui->doubleSpinBox_6->value()},
      {ui->doubleSpinBox_7->value(), 0, 0}};

  emit signal_data(transform_data);
}

void MainWindow::on_applySettings_clicked() {
    QMap<QString, QString> map = collectSettings();

    emit signal_settings(map);
}

QMap<QString, QString> MainWindow::collectSettings() {
    QMap<QString, QString> map;

    map["projection"] = ui->radioButton_projection_central->isChecked() ? "Central" : "Parallel";
    map["line_type"] = ui->radioButton_line_dashed->isChecked() ? "Dashed" : "Solid";
    map["vertex_display"] = determineVertexDisplay();
    map["line_color"] = ui->lineColorBox->currentText();
    map["line_width"] = QString::number(ui->lineWidthBox->value());
    map["vertex_color"] = ui->vertexColorBox->currentText();
    map["vertex_size"] = QString::number(ui->vertexSizeBox->value());
    map["background_color"] = ui->backgroundColorBox->currentText();

    return map;
}

QString MainWindow::determineVertexDisplay() {
  QString result;

    if (ui->radioButton_vertex_none->isChecked()) {
        result = "None";
    } else if (ui->radioButton_vertex_circle->isChecked()) {
        result = "Circle";
    } else {
        result = "Square";
    }

    return result;
}

void MainWindow::on_saveImage_clicked() {
  QString save = NULL;

  save = QFileDialog::getSaveFileName(this, NULL, NULL,
                                      "JPEG (*.jpeg) ;; BMP (*.bmp)");
  if (!save.isNull()) ui->widget->grabFramebuffer().save(save, NULL, 100);
}

void MainWindow::on_recordScreencast_clicked() {
  gif = new QGifImage;
  frame = new QImage;
  timer = new QTimer(this);
  count = 0;
  connect(timer, SIGNAL(timeout()), this, SLOT(save()));
  timer->start(FRAMES_FOR_GIF);
}

void MainWindow::save() {
  count++;
  *frame = ui->widget->grabFramebuffer();
  gif->addFrame(*frame);
  if (count == FRAMES_FOR_GIF) {
    timer->stop();
    QString save = NULL;
    save = QFileDialog::getSaveFileName(this, NULL, NULL, "GIF (*.gif)");
    if (!save.isNull()) gif->save(save);
  }
}
