cpp-compiler := g++


# OpenCV
openCVFlags := `pkg-config --cflags --libs opencv`


# Project objects
projectDir := $(CURDIR)
projectObjDir := $(projectDir)/obj
projectBinDir := $(projectDir)/bin
projectLibDir := $(projectDir)/lib
projectLibFlags := -L$(projectLibDir) -lencara-raw
projectFlags := $(projectLibFlags) $(iplFlags) -DENCARA2INLINUX $(gtkFlags) $(openCVFlags)

target := encara2-webcam-test
binTarget := $(projectBinDir)/$(target)

objects := $(patsubst %.o.pre,%.o,$(shell ls $(projectObjDir)/*-test.o.pre))
objects := $(filter-out %-test.o,$(objects))
objects := $(filter-out %-main.o,$(objects))
objects := $(filter-out %/directMotionDemo.o,$(objects))
objects := $(objects) $(projectObjDir)/$(target).o

flags = $(extraFlags) -fexceptions -DREENTRANT -D_THREAD_SAFE $(projectFlags) -lpthread

all: $(binTarget)
	echo Done it.

$(binTarget): $(objects)
	$(cpp-compiler) -o $@ $(objects) $(flags)

include $(projectObjDir)/makeObjects.mak

.PHONY: clean

clean:
	rm $(objects) $(binTarget)



	
