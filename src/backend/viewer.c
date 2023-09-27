#include "viewer.h"

void init(data_t *viewer) {
  viewer->count_of_polygons = 0;
  viewer->count_of_vertexes = 0;
  viewer->matrix_of_vertexes.columns = 0;
  viewer->matrix_of_vertexes.rows = 0;
  viewer->matrix_of_vertexes.matrix = NULL;
  viewer->array_of_polygon = NULL;
  viewer->minX = DBL_MAX;
  viewer->minY = DBL_MAX;
  viewer->minZ = DBL_MAX;
  viewer->maxX = DBL_MIN;
  viewer->maxY = DBL_MIN;
  viewer->maxZ = DBL_MIN;
}

void free_data_t_memory(data_t *viewer) {
  free_double_matrix(&viewer->matrix_of_vertexes.matrix,
                     viewer->matrix_of_vertexes.rows);

  viewer->matrix_of_vertexes.columns = 0;
  viewer->matrix_of_vertexes.rows = 0;

  for (unsigned int i = 1; i <= viewer->count_of_polygons; i++) {
    free(viewer->array_of_polygon[i].vertexes);
  }
  free(viewer->array_of_polygon);

  viewer->count_of_vertexes = 0;
  viewer->count_of_polygons = 0;
}

int open_file(const char *file_name, FILE **f) {
  *f = fopen(file_name, "r");
  return *f ? OK : ERROR;
}

void close_file(FILE *f) {
  if (f) fclose(f);
}

int core_parser(const char *file_name, data_t *viewer) {
  init(viewer);
  if (first_read_parser(file_name, viewer) != OK) return ERROR;
  if (create_matrix_of_vertexes(viewer) != OK) return ERROR;
  if (polygon_memory_allocation(viewer) != OK) return ERROR;
  return second_read_parser(file_name, viewer);
}

int first_read_parser(const char *file_name, data_t *viewer) {
  FILE *f;
  int status = open_file(file_name, &f);
  if (status == OK) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
      if (strncmp(line, "v ", 2) == 0) viewer->count_of_vertexes++;
      if (strncmp(line, "f ", 2) == 0) viewer->count_of_polygons++;
    }
    free(line);
    close_file(f);
  }
  return status;
}

int second_read_parser(const char *file_name, data_t *viewer) {
  FILE *f;
  int status = open_file(file_name, &f);
  if (status == OK) {
    char *line = NULL;
    size_t len = 0;
    int i = 1, j = 1;
    while (getline(&line, &len, f) != -1) {
      if (strncmp(line, "v ", 2) == 0) {
        status = process_vertex_line(line, viewer, i++);
      } else if (strncmp(line, "f ", 2) == 0) {
        status = process_polygon_line(line, viewer, j++);
      }
    }
    free(line);
    close_file(f);
  }
  return status;
}

int process_vertex_line(char *line, data_t *viewer, int i) {
  char ch;
  sscanf(line, "%c %lf %lf %lf", &ch, &viewer->matrix_of_vertexes.matrix[i][0],
         &viewer->matrix_of_vertexes.matrix[i][1],
         &viewer->matrix_of_vertexes.matrix[i][2]);
  min_max(viewer, i);
  return OK;
}

int process_polygon_line(char *line, data_t *viewer, int j) {
  int status = count_vertexes_for_polygon(line, viewer, j);
  status = vertexes_for_polygon_memory_allocation(viewer, j);
  status = extract_vertexes_from_polygon(line, viewer, j);
  return status;
}

int create_matrix_of_vertexes(data_t *viewer) {
  viewer->matrix_of_vertexes.columns = 3;
  viewer->matrix_of_vertexes.rows = viewer->count_of_vertexes + 1;
  return allocate_double_matrix(&viewer->matrix_of_vertexes.matrix,
                                viewer->matrix_of_vertexes.rows,
                                viewer->matrix_of_vertexes.columns);
}

int polygon_memory_allocation(data_t *viewer) {
  viewer->array_of_polygon =
      calloc(viewer->count_of_polygons + 1, sizeof(polygon_t));
  return viewer->array_of_polygon ? OK : ERROR;
}

int vertexes_for_polygon_memory_allocation(data_t *viewer, int j) {
  viewer->array_of_polygon[j].vertexes =
      calloc(viewer->array_of_polygon[j].numbers_of_vertexes_for_polygon,
             sizeof(unsigned int));
  return viewer->array_of_polygon[j].vertexes ? OK : ERROR;
}

int split_and_count_tokens(char *str, const char *delimiter,
                           unsigned int *count) {
  char *tmp = strdup(str);
  if (!tmp) return ERROR;

  char *sep_str = strtok(tmp, delimiter);
  *count = 0;
  while (sep_str) {
    (*count)++;
    sep_str = strtok(NULL, delimiter);
  }

  free(tmp);
  return OK;
}

void min_max(data_t *viewer, int i) {
  double *vertex = viewer->matrix_of_vertexes.matrix[i];
  viewer->maxX = fmax(vertex[0], viewer->maxX);
  viewer->maxY = fmax(vertex[1], viewer->maxY);
  viewer->maxZ = fmax(vertex[2], viewer->maxZ);
  viewer->minX = fmin(vertex[0], viewer->minX);
  viewer->minY = fmin(vertex[1], viewer->minY);
  viewer->minZ = fmin(vertex[2], viewer->minZ);
}

void set_in_center(data_t *viewer) {
  double delta_x = viewer->maxX - viewer->minX;
  double delta_y = viewer->maxY - viewer->minY;
  double delta_z = viewer->maxZ - viewer->minZ;

  double max_delta = fmax(fmax(delta_x, delta_y), delta_z);
  double zoom = 1.5 / max_delta;

  double center_x = viewer->minX + delta_x / 2.0;
  double center_y = viewer->minY + delta_y / 2.0;
  double center_z = viewer->minZ + delta_z / 2.0;

  for (unsigned int i = 1; i <= viewer->count_of_vertexes; i++) {
    viewer->matrix_of_vertexes.matrix[i][0] =
        (viewer->matrix_of_vertexes.matrix[i][0] - center_x) * zoom;
    viewer->matrix_of_vertexes.matrix[i][1] =
        (viewer->matrix_of_vertexes.matrix[i][1] - center_y) * zoom;
    viewer->matrix_of_vertexes.matrix[i][2] =
        (viewer->matrix_of_vertexes.matrix[i][2] - center_z) * zoom;
  }
}

int count_vertexes_for_polygon(char *str, data_t *viewer, int j) {
  unsigned int count = 0;
  int status = split_and_count_tokens(str, " ", &count);
  if (status != OK) return ERROR;

  viewer->array_of_polygon[j].numbers_of_vertexes_for_polygon = count - 1;
  return OK;
}

int extract_vertexes_from_polygon(char *str, data_t *viewer, int j) {
  unsigned int count = 0;
  int status = split_and_count_tokens(str, " ", &count);
  if (status != OK) return ERROR;

  char *token = strtok(str, " ");
  int e = 0;
  while (token) {
    unsigned int vertex = atof(token);
    if (vertex != 0) {
      viewer->array_of_polygon[j].vertexes[e++] = vertex;
    }
    token = strtok(NULL, " ");
  }
  return OK;
}

int allocate_double_matrix(double ***matrix, unsigned int rows,
                           unsigned int columns) {
  *matrix = calloc(rows, sizeof(double *));
  if (!(*matrix)) return ERROR;

  for (unsigned int i = 0; i < rows; i++) {
    (*matrix)[i] = calloc(columns, sizeof(double));
    if (!(*matrix)[i]) {
      while (i-- > 0) {
        free((*matrix)[i]);
      }
      free(*matrix);
      return ERROR;
    }
  }
  return OK;
}

void free_double_matrix(double ***matrix, unsigned int rows) {
  for (unsigned int i = 0; i < rows; i++) {
    free((*matrix)[i]);
  }
  free(*matrix);
  *matrix = NULL;
}
