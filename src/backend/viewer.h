#ifndef T3D_VIEWER_H_
#define T3D_VIEWER_H_

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OK 0
#define ERROR 1

// Data Structures
typedef struct facets {
  unsigned int *vertexes;
  unsigned int numbers_of_vertexes_for_polygon;
} polygon_t;

typedef struct matrix_struct {
  double **matrix;
  unsigned int rows;
  unsigned int columns;
} matrix_t;

typedef struct data {
  unsigned int count_of_vertexes;
  unsigned int count_of_polygons;
  matrix_t matrix_of_vertexes;
  polygon_t *array_of_polygon;
  double minX, minY, minZ;
  double maxX, maxY, maxZ;
} data_t;

// Initialization and Memory Management
void init(data_t *);
void free_data_t_memory(data_t *);

// File Handling
int open_file(const char *, FILE **);
void close_file(FILE *);

// Parsing Functions
int core_parser(const char *, data_t *);
int first_read_parser(const char *, data_t *);
int second_read_parser(const char *, data_t *);
int process_vertex_line(char *, data_t *, int);
int process_polygon_line(char *, data_t *, int);

// Memory Allocation
int create_matrix_of_vertexes(data_t *);
int polygon_memory_allocation(data_t *);
int vertexes_for_polygon_memory_allocation(data_t *, int);

// Vertex and Polygon Handling
void min_max(data_t *, int);
void set_in_center(data_t *);
int count_vertexes_for_polygon(char *, data_t *, int);
int extract_vertexes_from_polygon(char *, data_t *,
                                  int);  // renamed for clarity

// Utility
int split_and_count_tokens(char *str, const char *delimiter,
                           unsigned int *count);
void free_double_matrix(double ***matrix, unsigned int rows);
int allocate_double_matrix(double ***matrix, unsigned int rows,
                           unsigned int columns);

// Affine Transformations
void affine_transfrom(double transform_data[3][3], struct data *ptr_obj1);
void move_x(data_t *, double);
void move_y(data_t *, double);
void move_z(data_t *, double);
void rotate_x(data_t *, double);
void rotate_y(data_t *, double);
void rotate_z(data_t *, double);
void scale(data_t *, double);

#endif  // T3D_VIEWER_H_
