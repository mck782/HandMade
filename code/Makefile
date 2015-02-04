PKG_CONFIG=`pkg-config --cflags --libs opencv`
GTEST=-lgtest -lgtest_main -lpthread
PROFILE=-fprofile-arcs -ftest-coverage
FLAGS=-pedantic -std=c++11 -Wall

UNAME := $(shell uname)
cc = g++-4.7
cov = gcov-4.7

ifeq ($(UNAME), Darwin)
	FLAGS += -framework GLUT -framework OpenGL
	cc = g++
	cov = gcov
endif

$(info $$cc is [${cc}])
$(info $$cov is [${cov}])

all:

HandMade: Preprocess.h Preprocess.cpp Detect.h Detect.cpp HandMade.cpp
	$(cc) ${FLAGS} Preprocess.cpp Detect.cpp HandMade.cpp -o HandMade ${PKG_CONFIG}

TestDetect: Detect.h Detect.cpp TestDetect.cpp
	$(cc) ${PROFILE} ${FLAGS} Detect.cpp TestDetect.cpp -o TestDetect ${PKG_CONFIG} ${GTEST}

TestDetect.out: TestDetect
	$(cov) -b Detect.cpp     >> TestDetect.out
	$(cov) -b Preprocess.cpp >> TestDetect.out
	$(cov) -b HandMade.cpp   >> TestDetect.out
	$(cov) -b TestDetect.cpp >> TestDetect.out

clean:
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -f *.gch
	rm -f *.o
	rm -f *.out
	rm -f HandMade
	rm -f TestDetect
