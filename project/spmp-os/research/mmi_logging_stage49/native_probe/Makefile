# To build, copy this directory into upstream libspmp8k/demos/mmilogprobe/
# or set LIBSPMP8K to an absolute path to the checked-out upstream repository.
LIBSPMP8K ?= ../..
TARGET = mmilogprobe
OBJS = $(RESOBJS) game.o
LIBS = -lgame -lc -lgcc
include $(LIBSPMP8K)/main.cfg
include $(LIBGAME)/libgame.mk
