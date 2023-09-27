#include <check.h>

#include "viewer.h"

START_TEST(test_open_file) {
  FILE *f;
  int status = open_file("test_model.obj", &f);
  ck_assert_int_eq(status, OK);
  close_file(f);
}
END_TEST

START_TEST(test_core_parser) {
  data_t viewer;
  int status = core_parser("test_model.obj", &viewer);
  ck_assert_int_eq(status, OK);

  // Проверка количества вершин и полигонов
  ck_assert_int_eq(viewer.count_of_vertexes, 5);
  ck_assert_int_eq(viewer.count_of_polygons, 6);

  // Проверка координат вершин
  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][0], 0.0, 1e-6);
  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][1], 0.0, 1e-6);
  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][2], 0.0, 1e-6);

  // Проверка полигонов
  ck_assert_uint_eq(viewer.array_of_polygon[1].vertexes[0], 1);
  ck_assert_uint_eq(viewer.array_of_polygon[1].vertexes[1], 2);
  ck_assert_uint_eq(viewer.array_of_polygon[1].vertexes[2], 3);

  free_data_t_memory(&viewer);
}
END_TEST

START_TEST(test_process_vertex_line) {
  data_t viewer;
  init(&viewer);
  allocate_double_matrix(&viewer.matrix_of_vertexes.matrix, 2, 3);
  viewer.matrix_of_vertexes.rows = 2;
  viewer.matrix_of_vertexes.columns = 3;

  char line[] = "v 1.0 2.0 3.0";
  int status = process_vertex_line(line, &viewer, 1);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][0], 1.0, 1e-6);
  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][1], 2.0, 1e-6);
  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][2], 3.0, 1e-6);

  free_data_t_memory(&viewer);
}
END_TEST

START_TEST(test_split_and_count_tokens) {
  char str[] = "f 1 2 3";
  unsigned int count = 0;

  int status = split_and_count_tokens(str, " ", &count);

  ck_assert_int_eq(status, OK);
  ck_assert_uint_eq(count, 4);  // Включая первый токен "f"
}
END_TEST

START_TEST(test_move_x) {
  data_t viewer;
  init(&viewer);
  allocate_double_matrix(&viewer.matrix_of_vertexes.matrix, 2, 3);
  viewer.matrix_of_vertexes.rows = 2;
  viewer.matrix_of_vertexes.columns = 3;

  viewer.matrix_of_vertexes.matrix[1][0] = 1.0;

  move_x(&viewer, 1.0);

  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][0], 2.0, 1e-6);

  free_data_t_memory(&viewer);
}
END_TEST

START_TEST(test_rotate_x) {
  data_t viewer;
  init(&viewer);
  allocate_double_matrix(&viewer.matrix_of_vertexes.matrix, 2, 3);
  viewer.matrix_of_vertexes.rows = 2;
  viewer.matrix_of_vertexes.columns = 3;

  viewer.matrix_of_vertexes.matrix[1][1] = 1.0;
  viewer.matrix_of_vertexes.matrix[1][2] = 0.0;

  rotate_x(&viewer, M_PI / 2);  // Поворот на 90 градусов

  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][1], 0.0, 1e-6);

  free_data_t_memory(&viewer);
}
END_TEST

START_TEST(test_scale) {
  data_t viewer;
  init(&viewer);
  allocate_double_matrix(&viewer.matrix_of_vertexes.matrix, 2, 3);
  viewer.matrix_of_vertexes.rows = 2;
  viewer.matrix_of_vertexes.columns = 3;

  viewer.matrix_of_vertexes.matrix[1][0] = 1.0;
  viewer.matrix_of_vertexes.matrix[1][1] = 2.0;
  viewer.matrix_of_vertexes.matrix[1][2] = 3.0;

  scale(&viewer, 2.0);

  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][0], 2.0, 1e-6);
  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][1], 4.0, 1e-6);
  ck_assert_double_eq_tol(viewer.matrix_of_vertexes.matrix[1][2], 6.0, 1e-6);

  free_data_t_memory(&viewer);
}
END_TEST

Suite *parser_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Parser");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_open_file);
  tcase_add_test(tc_core, test_core_parser);
  tcase_add_test(tc_core, test_process_vertex_line);
  tcase_add_test(tc_core, test_split_and_count_tokens);

  suite_add_tcase(s, tc_core);

  return s;
}

Suite *transform_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Transform");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_move_x);

  tcase_add_test(tc_core, test_rotate_x);

  tcase_add_test(tc_core, test_scale);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = parser_suite();
  sr = srunner_create(s);

  srunner_add_suite(sr, transform_suite());

  srunner_run_all(sr, CK_NORMAL);

  number_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
