CXX = gcc
EXE = nom
SOURCES = $(wildcard src/*.c)
OBJS = $(SOURCES:.c=.o)
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	CXXFLAGS = -lm
endif

.c.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete.

$(EXE): $(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS) -lc

clean:
		rm $(EXE) $(OBJS)
