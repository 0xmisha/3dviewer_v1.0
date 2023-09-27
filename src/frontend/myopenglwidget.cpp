#include "myopenglwidget.h"

#include <QOpenGLWidget>
#include <QSettings>
#include <QtWidgets>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) : QOpenGLWidget{parent} {
  axis_check = 0;
  ptr_obj1 = nullptr;
  set = new QSettings("launch_settings.init", QSettings::IniFormat);
}

void MyOpenGLWidget::setColorFromString(const QString& color) {
    if (color == "Red") glColor3d(1, 0, 0);
    else if (color == "Blue") glColor3d(0, 0, 1);
}

void MyOpenGLWidget::setBackgroundColor() {
    background_color = set->value("backgroundColor").toString();
    if (background_color == "White") {
      glClearColor(1, 1, 1, 1.0);
    } else if (background_color == "Black"){
        glClearColor(0, 0.0, 0.0, 1.0);
    }
}


void MyOpenGLWidget::setProjection() {
    projection_type = set->value("projection").toString();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (projection_type == "Central") glFrustum(-2, +2, -2, +2, 5.0, 15.0);
    else if (projection_type == "Parallel") glOrtho(-2, +2, -2, +2, 1.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void MyOpenGLWidget::on_SelectFileButton_clicked() {
  QString f_name;

  QString filename = QFileDialog::getOpenFileName(this, "Choose File");

  QByteArray ba = filename.toLocal8Bit();
  const char *filename_c = ba.data();
  ptr_obj1 = &obj1;
  core_parser(filename_c, ptr_obj1);
  set_in_center(ptr_obj1);
  f_name = filename.split('/').last();

  emit send_info(ptr_obj1->count_of_vertexes, ptr_obj1->count_of_polygons,
                 f_name);
  update();
}

void MyOpenGLWidget::on_checkAxes_clicked() {
  axis_check = (axis_check == 0) ? 1 : 0;
  update();
}

void MyOpenGLWidget::initializeGL() {}

void MyOpenGLWidget::resizeGL(int w, int h) {}

void MyOpenGLWidget::paintGL() {
  setBackgroundColor();
  setProjection();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -7.0);
  glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
  glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
  if (ptr_obj1 != nullptr) {
    drawLines();
    vertex_display = set->value("vertexDisplay").toString();
    if (vertex_display != "None") drawPoints();
  }
}

void MyOpenGLWidget::setColorFromSetting(const QString& settingValue) {
    QColor color = QColor(settingValue);
    if (color.isValid()) {
        glColor3d(color.redF(), color.greenF(), color.blueF());
    } else {
        glColor3d(1, 1, 1);
    }
}

void MyOpenGLWidget::drawPolygons(GLenum mode) {
    for (int i = 1; i < ptr_obj1->count_of_polygons + 1; i++) {
        glBegin(mode);
        for (int j = 0; j < ptr_obj1->array_of_polygon[i].numbers_of_vertexes_for_polygon; j++) {
            glVertex3d(
                ptr_obj1->matrix_of_vertexes.matrix[ptr_obj1->array_of_polygon[i].vertexes[j]][0],
                ptr_obj1->matrix_of_vertexes.matrix[ptr_obj1->array_of_polygon[i].vertexes[j]][1],
                ptr_obj1->matrix_of_vertexes.matrix[ptr_obj1->array_of_polygon[i].vertexes[j]][2]
            );
        }
        glEnd();
    }
}

void MyOpenGLWidget::drawLines() {
    setColorFromSetting(set->value("lineColor").toString());
  
    QString line_type = set->value("lineType").toString();
    if (line_type == "Dashed") {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0X00FF);
    } else {
        glDisable(GL_LINE_STIPPLE);
    }
  
    glLineWidth(set->value("lineWidth").toInt());

    drawPolygons(GL_LINE_LOOP);
}

void MyOpenGLWidget::drawPoints() {
    setColorFromSetting(set->value("vertexColor").toString());

    glEnable(GL_BLEND);
    if (set->value("vertexDisplay").toString() == "Square") {
        glDisable(GL_POINT_SMOOTH);
    } else {
        glEnable(GL_POINT_SMOOTH);
    }

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(set->value("vertexSize").toInt());

    drawPolygons(GL_POINTS);
}


static void qNormalizeAngle(int &angle) {
  while (angle < 0) angle += 360 * 16;
  while (angle > 360) angle -= 360 * 16;
}

void MyOpenGLWidget::setRotation(int &currentAngle, int angleDelta, int &targetAngle) {
    int newAngle = currentAngle + angleDelta;
    qNormalizeAngle(newAngle);
    if (newAngle != targetAngle) {
        targetAngle = newAngle;
        update();
    }
}

void MyOpenGLWidget::setXRotation(int angle) {
    setRotation(xRot, angle, xRot);
}

void MyOpenGLWidget::setYRotation(int angle) {
    setRotation(yRot, angle, yRot);
}

void MyOpenGLWidget::setZRotation(int angle) {
    setRotation(zRot, angle, zRot);
}


void MyOpenGLWidget::on_transformButton_clicked2(double transform_data[3][3]) {
  affine_transfrom(transform_data, ptr_obj1);
  update();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event) {
  lastPos = event->pos();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    int delta = 8;

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(delta * dy);
        setYRotation(delta * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(delta * dy);
        setZRotation(delta * dx);
    }

    lastPos = event->pos();
}

void MyOpenGLWidget::on_applySettingsButton_clicked(
    QMap<QString, QString> map) {
  projection_type = map["projection"];
  line_type = map["line_type"];
  line_color = map["line_color"];
  line_width = map["line_width"].toInt();
  vertex_color = map["vertex_color"];
  vertex_size = map["vertex_size"].toInt();
  vertex_display = map["vertex_display"];
  background_color = map["background_color"];
  set->setValue("projection", map["projection"]);
  set->setValue("lineType", map["line_type"]);
  set->setValue("lineColor", map["line_color"]);
  set->setValue("lineWidth", map["line_width"]);
  set->setValue("vertexColor", map["vertex_color"]);
  set->setValue("vertexSize", map["vertex_size"]);
  set->setValue("vertexDisplay", map["vertex_display"]);
  set->setValue("backgroundColor", map["background_color"]);
  update();
}
