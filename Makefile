###################### user-configuration section #######################
INC := -I/usr/local/include
LIB := -L/usr/local/lib
MARCH := -m64

# set HAVE_CGAL to 0 to build without CGAL support; psa will then omit
# the computation of the bond-orientational order (BOO)
HAVE_CGAL := 1
#########################################################################

CXX := g++
CXXFLAGS := -Wall -fopenmp
OPTFLAGS := -O2
LINKFLAGS := -lcairo
DEFS :=
ifeq ($(HAVE_CGAL),1)
	CXXFLAGS += -frounding-math
	LINKFLAGS += -lCGAL -lCGAL_Core -lgmp -lboost_thread-mt -lmpfr
	DEFS += -DPSA_HAS_CGAL -DCGAL_CFG_NO_CPP0X_VARIADIC_TEMPLATES
endif

OBJDIR := obj
SRCDIR := src
CXXFILES := main.cpp analysis.cpp config.cpp curve.cpp delaunay.cpp image.cpp param.cpp periodogram.cpp point.cpp result.cpp spectrum.cpp statistics.cpp

OBJS   := $(patsubst %.cpp,$(OBJDIR)/%.cpp.o,$(notdir $(CXXFILES)))
TARGET := psa

VERBOSE := @

.PHONY: all clean

all: $(TARGET)

$(OBJDIR)/%.cpp.o : $(SRCDIR)/%.cpp
	$(VERBOSE)$(CXX) $(CXXFLAGS) $(MARCH) $(DEFS) $(INC) -o $@ -c $<

$(TARGET): makedir $(OBJS) Makefile
	$(VERBOSE)$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(OBJS) $(LINKFLAGS) $(LIB) -o $(TARGET)

makedir:
	$(VERBOSE)mkdir -p $(OBJDIR)

clean:
	$(VERBOSE)rm -f $(OBJS)
	$(VERBOSE)rm -f $(TARGET)
	$(VERBOSE)rmdir -p $(OBJDIR)
