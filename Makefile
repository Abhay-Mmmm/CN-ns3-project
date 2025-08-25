# Compact Makefile
CXX = g++ -std=c++20 -O2
NS3 = -I../build/include -L../build/lib \
      -lns3.45-core-default -lns3.45-network-default \
      -lns3.45-internet-default -lns3.45-point-to-point-default \
      -lns3.45-applications-default -lns3.45-netanim-default
CV  = $(shell pkg-config --cflags --libs opencv4)

# Default target
all: compact-classifier

# Build compact-classifier
compact-classifier: compact-classifier.cc
	$(CXX) $< $(NS3) $(CV) -o $@

# Run compact-classifier
run: compact-classifier
	LD_LIBRARY_PATH=../build/lib:$$LD_LIBRARY_PATH ./compact-classifier

# Clean build files
clean:
	rm -f compact-classifier *.xml

.PHONY: all run clean
