CC=clang++


CFLAGS=-std=c++11 -O3 -Wall -g
LINKFLAGS=-O3

#debug = true
ifdef debug
	CFLAGS +=-g
	LINKFLAGS += -flto
endif

INCDIR= -I./middleware -Imiddleware/glad/include

LIBDIR=-L/usr/X11R6 -L/usr/local/lib

LIBS=

OS_NAME:=$(shell uname -s)

ifeq ($(OS_NAME),Darwin)
	LIBS += `pkg-config --static --libs glfw3 gl`
endif
ifeq ($(OS_NAME),Linux)
	LIBS += `pkg-config --static --libs glfw3 gl`
endif

SRCDIR=./boilerplate

SRCLIST=$(wildcard $(SRCDIR)/*cpp) 

HEADERDIR=./boilerplate

OBJDIR=./obj

OBJLIST=$(addprefix $(OBJDIR)/,$(notdir $(SRCLIST:.cpp=.o))) $(OBJDIR)/glad.o

EXECUTABLE=boilerplate.out

all: buildDirectories $(EXECUTABLE) 

$(EXECUTABLE): $(OBJLIST)
	$(CC) $(LINKFLAGS) $(OBJLIST) -o $@ $(LIBS) $(LIBDIR)

$(OBJDIR)/glad.o: middleware/glad/src/glad.c
	$(CC) -c $(CFLAGS) -I$(HEADERDIR) $(INCDIR) $(LIBDIR) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) -I$(HEADERDIR) $(INCDIR) $(LIBDIR) $< -o $@


.PHONY: buildDirectories
buildDirectories:
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -f *.out $(OBJDIR)/*.o; rmdir obj;
