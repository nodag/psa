###################### user-configuration section #######################
INC := -I/usr/local/include
INC += -I/usr/local/Cellar/cairo/1.12.2/include

LIB := -L/usr/local/lib
LIB += -L/usr/local/Cellar/cairo/1.12.2/lib

# set HAVE_CGAL to 0 to build without CGAL support; psa will then omit
# the computation of the orientational order
HAVE_CGAL := 1
#########################################################################

CXX := g++
CXXFLAGS := -Wall -fopenmp
OPTFLAGS := -O2
MARCH := -m64
LINKFLAGS := -lcairo
DEFS :=
ifeq ($(HAVE_CGAL),1)
	CXXFLAGS += -frounding-math
	LINKFLAGS += -lCGAL -lCGAL_Core -lgmp -lboost_thread-mt -lmpfr
	DEFS += -DPSA_HAS_CGAL -DCGAL_CFG_NO_CPP0X_VARIADIC_TEMPLATES
endif

OBJDIR := obj
SRCDIR := src
CXXFILES := main.cpp analysis.cpp config.cpp curve.cpp image.cpp param.cpp periodogram.cpp point.cpp result.cpp spectrum.cpp statistics.cpp
ifeq ($(HAVE_CGAL),1)
	CXXFILES += delaunay.cpp
endif

OBJS   := $(patsubst %.cpp,$(OBJDIR)/%.cpp.o,$(notdir $(CXXFILES)))
TARGET := psa

VERBOSE := @

.PHONY: all clean

all: $(TARGET)

$(OBJDIR)/%.cpp.o : $(SRCDIR)/%.cpp
	$(VERBOSE)$(CXX) $(CXXFLAGS) $(MARCH) $(DEFS) $(INC) -o $@ -c $<

$(TARGET): makedir $(OBJS) Makefile
	$(VERBOSE)$(CXX) $(CXXFLAGS) $(INC) $(OPTFLAGS) $(LINKFLAGS) \
	-o $(TARGET) $(LIB) $(OBJS)

makedir:
	$(VERBOSE)mkdir -p $(OBJDIR)

clean:
	$(VERBOSE)rm -f $(OBJS)
	$(VERBOSE)rm -f $(TARGET)
	$(VERBOSE)rmdir -p $(OBJDIR)
