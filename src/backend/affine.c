#include "viewer.h"

void move_x(data_t *data, double a) {
  for (unsigned int i = 1; i < data->matrix_of_vertexes.rows; ++i)
    data->matrix_of_vertexes.matrix[i][0] += a;
}

void move_y(data_t *data, double a) {
  for (unsigned int i = 1; i < data->matrix_of_vertexes.rows; ++i)
    data->matrix_of_vertexes.matrix[i][1] += a;
}

void move_z(data_t *data, double a) {
  for (unsigned int i = 1; i < data->matrix_of_vertexes.rows; ++i)
    data->matrix_of_vertexes.matrix[i][2] += a;
}

void rotate_x(data_t *data, double a) {
  for (unsigned int i = 1; i < data->matrix_of_vertexes.rows; ++i) {
    double temp_y = data->matrix_of_vertexes.matrix[i][1];
    double temp_z = data->matrix_of_vertexes.matrix[i][2];
    data->matrix_of_vertexes.matrix[i][1] = cos(a) * temp_y - sin(a) * temp_z;
    data->matrix_of_vertexes.matrix[i][2] = sin(a) * temp_y + cos(a) * temp_z;
  }
}

void rotate_y(data_t *data, double a) {
  for (unsigned int i = 1; i < data->matrix_of_vertexes.rows; ++i) {
    double temp_x = data->matrix_of_vertexes.matrix[i][0];
    double temp_z = data->matrix_of_vertexes.matrix[i][2];
    data->matrix_of_vertexes.matrix[i][0] = cos(a) * temp_x + sin(a) * temp_z;
    data->matrix_of_vertexes.matrix[i][2] = -sin(a) * temp_x + cos(a) * temp_z;
  }
}

void rotate_z(data_t *data, double a) {
  for (unsigned int i = 1; i < data->matrix_of_vertexes.rows; ++i) {
    double temp_x = data->matrix_of_vertexes.matrix[i][0];
    double temp_y = data->matrix_of_vertexes.matrix[i][1];
    data->matrix_of_vertexes.matrix[i][0] = cos(a) * temp_x - sin(a) * temp_y;
    data->matrix_of_vertexes.matrix[i][1] = sin(a) * temp_x + cos(a) * temp_y;
  }
}

void scale(data_t *data, double a) {
  if (a > 0) {
    for (unsigned int i = 1; i < data->matrix_of_vertexes.rows; ++i) {
      for (unsigned int j = 0; j < data->matrix_of_vertexes.columns; ++j) {
        data->matrix_of_vertexes.matrix[i][j] *= a;
      }
    }
  }
}

void affine_transfrom(double transform_data[3][3], data_t *ptr_obj1) {
  move_x(ptr_obj1, transform_data[0][0]);
  move_y(ptr_obj1, transform_data[0][1]);
  move_z(ptr_obj1, transform_data[0][2]);
  rotate_x(ptr_obj1, transform_data[1][0]);
  rotate_y(ptr_obj1, transform_data[1][1]);
  rotate_z(ptr_obj1, transform_data[1][2]);
  scale(ptr_obj1, transform_data[2][0]);
}
