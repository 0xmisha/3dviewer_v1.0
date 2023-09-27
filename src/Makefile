CC=gcc -std=c11 -D_GNU_SOURCE -g
CFLAGS=-c -Wall -Wextra -Werror
TEST_FLAGS=-lcheck -lm -lpthread
GCOV_FLAGS=--coverage -Wall -Wextra -Werror
SOURCES=$(wildcard c_core/*.c)
OBJECTS=$(SOURCES:.c=.o)
TESTS_SOURCES= $(wildcard tests/*.c)
TESTS_OBJECTS=$(TESTS_SOURCES:.c=.o)
EXECUTABLE=s21_3DViewer
OS = $(shell uname)
QMAKE = qmake
INSTALL_DIR = build
ifeq ($(OS), Linux)
	BUILD_DIR = build-3DViewer_v1_0-Desktop_Qt_5_15_2_GCC_64bit-Debug/
	APP_DIR=
	TEST_FLAGS = -lcheck -lm -lpthread -lsubunit -lrt
else
	BUILD_DIR = build-3DViewer_v1_0-Desktop_Qt_5_15_2_GCC_64bit-Debug/
	APP_DIR=3DViewer_v1_0.app/Contents/MacOS
	TEST_FLAGS = -lcheck
endif

all: clean build install open

build:
	$(QMAKE) -o $(BUILD_DIR)/Makefile ./frontend/3DViewer_v1_0.pro
	make -C $(BUILD_DIR)/

rebuild: clean all

open:
	$(INSTALL_DIR)/./3DViewer_v1_0

install: build
	mkdir $(INSTALL_DIR)
	install $(BUILD_DIR)$(APP_DIR)/3DViewer_v1_0 $(INSTALL_DIR)/3DViewer_v1_0

uninstall: clean
	rm -rf $(INSTALL_DIR)
	rm -rf build-3D*


clean: uninstall
	rm -rf $(OBJECTS) $(EXECUTABLE) $(TESTS_OBJECTS) *.o s21_3DViewer.a *.gcov *.gcno *.gcda *.info test *.dSYM report html_dvi 3D_Viewer_copy


tests: backend/tests.c backend/viewer.c backend/affine.c
	$(CC) $^ -o $@  $(TEST_FLAGS)
	./tests

gcov_report: $(TESTS_OBJECTS)
	$(CC) $(GCOV_FLAGS) $(TESTS_OBJECTS) $(SOURCES) $(TEST_FLAGS) -o test
	./test
	gcov *.gcno
	lcov -t "3DViewer_v1_0" -o 3DViewer_v1_0.info -c -d . --rc lcov_coverage
	genhtml -o report 3DViewer_v1_0.info --rc lcov_coverage
	open report/index.html

dist:
	mkdir 3D_Viewer_copy
	mkdir 3D_Viewer_copy/c_core
	mkdir 3D_Viewer_copy/tests
	mkdir 3D_Viewer_copy/3DViewer_v1_0
	cp c_core/*.c 3D_Viewer_copy/c_core
	cp c_core/*.h 3D_Viewer_copy/c_core
	cp 3DViewer_v1_0/*.cpp 3D_Viewer_copy/3DViewer_v1_0
	cp 3DViewer_v1_0/*.ui 3D_Viewer_copy/3DViewer_v1_0
	cp 3DViewer_v1_0/*.h 3D_Viewer_copy/3DViewer_v1_0
	cp tests/*.c 3D_Viewer_copy/tests
	cp tests/*.h 3D_Viewer_copy/tests
	cp Makefile 3D_Viewer_copy
	tar czf 3D_Viewer_Arh.tar.gz 3D_Viewer_copy
	rm -rf 3D_Viewer_copy

dvi:
	rm -rf ./html_dvi
	./doxygen Doxyfile && open html_dvi/index.html

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

rebuild: clean all

s21_3DViewer.a: $(OBJECTS)
	ar -rc s21_3DViewer.a $(OBJECTS)
	ranlib s21_3DViewer.a


style_check:
	cp ../materials/linters/.clang-format ./
	clang-format -style=Google -n c_core/*.c
	clang-format -style=Google -n c_core/*.h
	clang-format -style=Google -n tests/*.c
	clang-format -style=Google -n tests/*.h
	clang-format -style=Google -n 3DViewer_v1_0/*.h
	clang-format -style=Google -n 3DViewer_v1_0/*.cpp


style_fix: 
	cp ../materials/linters/.clang-format ./
	clang-format -style=Google -i c_core/*.c
	clang-format -style=Google -i c_core/*.h
	clang-format -style=Google -i tests/*.c
	clang-format -style=Google -i tests/*.h
	clang-format -style=Google -i 3DViewer_v1_0/*.h
	clang-format -style=Google -i 3DViewer_v1_0/*.cpp


leaks_check: tests
	leaks -atExit -- ./test

valgrind_check: tests
	valgrind --leak-check=full --leak-resolution=med ./test
