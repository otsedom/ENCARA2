cpp-compiler := g++


# OpenCV
openCVFlags := `pkg-config opencv --libs`

# IPL
iplFlags := -lipla6 -liplm6 -liplpx

# Project objects
projectDir := $(CURDIR)
projectObjDir := $(projectDir)/obj
projectBinDir := $(projectDir)/bin
projectLibDir := $(projectDir)/lib
projectLibFlags := -L$(projectLibDir) -lencara-raw
projectFlags := $(projectLibFlags) $(openCVFlags) $(iplFlags) -DENCARA2INLINUX

target := haarconv-test
binTarget := $(projectBinDir)/$(target)

objects := $(patsubst %.o.pre,%.o,$(shell ls $(projectObjDir)/*.o.pre))
objects := $(filter-out %-test.o,$(objects))
objects := $(filter-out %-main.o,$(objects))
objects := $(filter-out %/directMotionDemo.o,$(objects))
objects := $(objects) $(projectObjDir)/$(target).o

flags = $(extraFlags) -fexceptions -DREENTRANT -D_THREAD_SAFE $(projectFlags) -lpthread

all: $(binTarget)
	echo Done it.

$(binTarget): $(objects)
	$(cpp-compiler) -o $@ $(flags) $(objects)

include $(projectObjDir)/makeObjects.mak

.PHONY: clean

clean:
	rm $(objects) $(binTarget)



	
