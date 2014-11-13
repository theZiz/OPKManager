#==stuff linked to
DYNAMIC = -lSDL_ttf -lSDL_mixer -lSDL_image -lSDL -lm -lopk
#==global Flags. Even on the gp2x with 16 kb Cache, -O3 is much better then -Os
CFLAGS = -g -fsingle-precision-constant -fPIC
# Testtweaks: -fgcse-lm -fgcse-sm -fsched-spec-load -fmodulo-sched -funsafe-loop-optimizations -Wunsafe-loop-optimizations -fgcse-las -fgcse-after-reload -fvariable-expansion-in-unroller -ftracer -fbranch-target-load-optimize
GENERAL_TWEAKS = -ffast-math
#==PC==
FLAGS = -g -DDESKTOP $(GENERAL_TWEAKS)
SDL = `sdl-config --cflags`

SPARROW_FOLDER = ../sparrow3d

ifdef TARGET
include $(SPARROW_FOLDER)/target-files/$(TARGET).mk
BUILD = ./build/$(TARGET)/OPKManager
SPARROW_LIB = $(SPARROW_FOLDER)/build/$(TARGET)/sparrow3d
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = .
SPARROW_LIB = $(SPARROW_FOLDER)
endif
LIB += -L$(SPARROW_LIB)
INCLUDE += -I$(SPARROW_FOLDER)
DYNAMIC += -lsparrow3d -lsparrowNet

CFLAGS += $(PARAMETER) $(FLAGS)

all: OPKManager
	@echo "=== Built for Target "$(TARGET)" ==="

targets:
	@echo "The targets are the same like for sparrow3d. :P"

OPKManager: OPKManager.c list.c details.c selection.c systemcalls.c makeBuildDir
	cp $(SPARROW_LIB)/libsparrow3d.so $(BUILD)
	cp $(SPARROW_LIB)/libsparrowNet.so $(BUILD)
	$(CC) $(CFLAGS) OPKManager.c $(SDL) $(INCLUDE) $(LIB) $(STATIC) $(DYNAMIC) -o $(BUILD)/OPKManager

makeBuildDir:
	 @if [ ! -d $(BUILD:/OPKManager=/) ]; then mkdir $(BUILD:/OPKManager=/);fi
	 @if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi

clean:
	rm -f *.o
	rm -f OPKManager
