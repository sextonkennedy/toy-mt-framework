#Change this to point to your boost installation
BOOST_INCLUDES := -I/Users/cdj/src/cms/software/FWLiteVersions/osx106_amd64_gcc461/external/boost/1.47.0-cms3/include
#BOOST_INCLUDES := -I$(BOOST_INC)
UNAME:=$(shell uname -s)
#UNAME:=Linux
DISPATCH_INCLUDES :=
DISPATCH_LIB :=
DISPATCH_LIB_PATH :=
ifeq ($(UNAME), Linux)
#change the following to to point to where you installed libdispatch and kqueue
DISPATCH_INCLUDES := -I/home/paterno/include
DISPATCH_LIB_PATH := -L/home/paterno/lib
DISPATCH_LIB := $(DISPATCH_LIB_PATH) -ldispatch -lkqueue
endif
CXX=g++
CPPFLAGS=$(DISPATCH_INCLUDES) $(BOOST_INCLUDES) -IDispatchFrameworkCore/ -I../BusyWaitCalibration/
CXXFLAGS=-O3 -g -std=c++14 -fPIC
LDFLAGS= -std=c++14
LINKER=g++

all: libDispatchFrameworkCore.so libDispatchTestModules.so DispatchDemo

DispatchDemo: libDispatchFrameworkCore.so libDispatchTestModules.so main.o
	$(LINKER) $+ $(LDFLAGS) $(DISPATCH_LIB) -o $@

fc_objects := $(patsubst %.cpp,%.o,$(wildcard DispatchFrameworkCore/*.cpp))
tm_objects := $(patsubst %.cpp,%.o,$(wildcard DispatchTestModules/*.cpp))

libDispatchFrameworkCore.so: $(fc_objects)
	$(LINKER) $+ $(DISPATCH_LIB) -shared -o $@

libDispatchTestModules.so: $(tm_objects) ../BusyWaitCalibration/busyWait.o ../BusyWaitCalibration/busy_wait_scale_factor.o libDispatchFrameworkCore.so
	$(LINKER) $+ -shared -o $@

clean:
	rm -f *.o */*.o *.so
	rm -f DispatchDemo
	rm -f $(fc_objects)
	rm -f $(tm_objects)