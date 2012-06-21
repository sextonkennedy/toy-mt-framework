#Change this to point to your boost installation
BOOST_INCLUDES := -I/Users/cdj/src/cms/software/FWLiteVersions/osx106_amd64_gcc461/external/boost/1.47.0-cms3/include
#BOOST_INCLUDES := -I$(BOOST_INC)
UNAME:=(shell uname -s)
DISPATCH_INCLUDES :=
DISPATCH_LIB :=
DISPATCH_LIB_PATH :=
ifeq ($(UNAME), Linux)
#change the following to to point to where you installed libdispatch and kqueue
DISPATCH_INCLUDES :=-I/home/paterno/include
DISPATCH_LIB_PATH :=-L/home/paterno/lib
DISPATCH_LIB := $(DISPATCH_LIB_PATH) -ldispatch -lkqueue
endif
CXX=g++
CPPFLAGS=$(DISPATCH_INCLUDES) $(BOOST_INCLUDES) -ISingleThreadedFrameworkCore/ -I../BusyWaitCalibration/
CXXFLAGS=-O3 -g -std=c++0x -fPIC
LDFLAGS= -std=c++0x
LINKER=g++

all: libSingleThreadedFrameworkCore.so libSingleThreadedTestModules.so SingleThreadedDemo

SingleThreadedDemo: libSingleThreadedFrameworkCore.so libSingleThreadedTestModules.so main.o
	$(LINKER) $+ $(LDFLAGS) $(DISPATCH_LIB) -o $@

fc_objects := $(patsubst %.cpp,%.o,$(wildcard SingleThreadedFrameworkCore/*.cpp))
tm_objects := $(patsubst %.cpp,%.o,$(wildcard SingleThreadedTestModules/*.cpp))

libSingleThreadedFrameworkCore.so: $(fc_objects)
	$(LINKER) $+ $(DISPATCH_LIB) -shared -o $@

libSingleThreadedTestModules.so: $(tm_objects) ../BusyWaitCalibration/busyWait.o ../BusyWaitCalibration/busy_wait_scale_factor.o libSingleThreadedFrameworkCore.so
	$(LINKER) $+ -shared -o $@

clean:
	rm -f *.o */*.o *.so
	rm -f SingleThreadedDemo
	rm -f $(fc_objects)
	rm -f $(tm_objects)
