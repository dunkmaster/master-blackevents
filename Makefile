#SYSTEMC_ARCH=linux64
export LD_LIBRARY_PATH= /opt/systemc-2.3.1/lib-linux

SYSTEMC_ARCH=linux
SYSTEMC = /opt/systemc-2.3.1
LIB_DIRS=$(SYSTEMC)/lib-$(SYSTEMC_ARCH)

# Include directories.
INCLUDE_DIRS =  -I. -I$(SYSTEMC)/include

# header files used, for dependency checking
HEADERS = Graph.h Channel.h Mapper.h Monitor.h GBT.h CRU.h GlobalConstants.h SAMPA.h DataGenerator.h Packet.h Sample.h RandomGenerator.h

# source files used, for dependency checking
SOURCES = main.cpp Monitor.cpp Graph.cpp Mapper.cpp Channel.cpp GBT.cpp CRU.cpp SAMPA.cpp DataGenerator.cpp Packet.cpp Sample.cpp RandomGenerator.cpp

DEPENDENCIES = \
	Makefile \
	$(HEADERS) \
	$(SOURCES)

LIBS= -lsystemc -lstdc++ -std=c++11 -lm
# -std=c++11 for ISO C++ 2011 standard
TESTS= main_run

# Default make rule compiles source and runs simulation
all: $(TESTS)
	./$(TESTS)
#	@make cmp_result

$(TESTS): $(DEPENDENCIES)
	g++ -g -o $@ $(SOURCES) $(INCLUDE_DIRS) -L$(LIB_DIRS) $(LIBS)
	#g++ -std=gnu++11 -g -o $@ $(SOURCES) $(INCLUDE_DIRS) -L$(LIB_DIRS) $(LIBS)


# Clean rule to remove output result file and sim executable
#clean:
#	rm -f $(TESTS) *.dat

# The cmp_result target compares the simulation results file with the
# golden results file in ./golden/ref_output.dat. The testbench
# writes the results file (output.dat) in the current directory.
# The cmp_result target is automatically run at the end of each simulation.

#GOLD_DIR = ./golden
#GOLD_FILE = $(GOLD_DIR)/ref_output.dat

#cmp_result:
#	@echo "*****************************************"
#	@if diff -w $(GOLD_FILE) ./output.dat ; then \
#		echo "SIMULATION PASSED"; \
#	else \
#		echo "SIMULATION FAILED "; \
#	fi
#	@echo "*****************************************"
#
