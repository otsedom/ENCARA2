cpp-compiler := g++

# OpenCV
openCVIncludeFlag := -I$(OPENCV_PATH)
openCVFlags := `pkg-config --cflags --libs opencv`

# IPL
#iplIncludeFlag := -I$(IPL_PATH)
#iplFlags := 


# Project objects
projectDir := $(CURDIR)
projectName := $(notdir $(CURDIR))
projectObjDir := $(projectDir)/obj
projectLibDir := $(projectDir)/lib
projectFlags := -DENCARA2INLINUX

flags = $(extraFlags) -shared -fexceptions -DREENTRANT -D_THREAD_SAFE $(projectFlags) $(openCVFlags) $(iplFlags) -lpthread

target := libencara-raw.so
target := $(projectLibDir)/$(target)

objects := $(patsubst %.o.lib.pre,%.o,$(shell ls $(projectObjDir)/*.o.lib.pre))
objects := $(filter-out %-test.o,$(objects))
#objects := $(filter-out %priorityibox.o,$(objects))

all: $(target)
	echo Done it.

$(target): $(objects)
	$(cpp-compiler) -o $@ $(flags) $(objects)

include $(projectObjDir)/makeObjects.mak

.PHONY: clean

clean:
	rm $(objects) $(target)


