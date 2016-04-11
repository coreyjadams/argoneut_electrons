#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = argoneut_electrons_analysisCint
SHLIB = libargoneut_electrons_analysis.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += $(shell recotool-config --includes)
INCFLAGS += -I$(LARLITE_USERDEVDIR)/argoneut_electrons

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += -L$(LARLITE_LIBDIR)/ -largoneut_electrons_utils
LDFLAGS += -L$(LARLITE_LIBDIR) -lRecoTool_ClusterRecoUtil
LDFLAGS += -L$(LARLITE_LIBDIR) -lRecoTool_ClusterRecoUtilAlg
LDFLAGS += -L$(LARLITE_LIBDIR) -lRecoTool_Cluster3DRecoUtil
LDFLAGS += -L$(LARLITE_LIBDIR) -lRecoTool_Cluster3DRecoUtilAlg

include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE


