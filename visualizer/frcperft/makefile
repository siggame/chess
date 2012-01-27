#
# makefile for frcperft
#


# optional parameters
# these can be set als parameter to make, e.g. 'make win64 sse=4.2'
# See help: for details.
#
sse=
build=build
extract=
mode=
count=
name=frcperft

ifdef sse
CXXFLAGS+=-m$(sse)
endif
ifdef extract
defines+=-DEXTRACT_$(extract)=1
endif
ifdef count
defines+=-DCOUNT_$(count)=1
endif
ifdef mode
defines+=-DPERFT_$(mode)=1
endif


# common options
# custom options may be specified from the command line by defining CXXUSER
# e.g. make CXXUSER='-myoption -myoption2'
#
defines+= -DNDEBUG
CXXFLAGS+=$(defines)
CXXFLAGS+= -O3
CXXFLAGS+= -fomit-frame-pointer
CXXFLAGS+= -fstrict-aliasing
CXXFLAGS+= -Wall -fno-exceptions -fno-rtti
CXXFLAGS+= -save-temps
LDFLAGS := -Wl

# environment
#
STRIP=strip
LIBS=
srcdir=../src
src=$(srcdir)/*.cpp

help:
	@echo
	@echo "Usage:"
	@echo "    make platform [build=...] [sse=...] [extract=...] [count=...] [mode=...]"
	@echo
	@echo "Required:"
	@echo "    platform  win32 | win64 | osx | osx32 | osx64 | lux32 | lux64"
	@echo "              Specifies the platform to create an executable for"
	@echo
	@echo "Options:"
	@echo "    build     build | pgo | profile"
	@echo "              Tells make how to build the target"
	@echo "              build performs an instant build"
	@echo "              pgo uses profile guided optimisation for a sometimes speedier executable"
	@echo
	@echo "    sse       sse2 | sse3 | sse4.2"
	@echo "              Tells the compiler which SSE instruction set to use"
	@echo "              sse4.2 enables the hardware popcntq instruction, vastly improving perft speed"
	@echo "              However, recent hardware (e.g. Intel corei) is required"
	@echo
	@echo "    extract   BUILTIN | BSF64 | BSF32 | DEBRUIJN | MAGIC"
	@echo "              Specifies how bits are extracted from the bitboard"
	@echo "              See Bitboard.h for details"
	@echo
	@echo "    count     BUILTIN | POPCNT | LOOP | WIKI3 | WIKI5"
	@echo "              Specifies how bits are counted (for the 'fast' algorithm)"
	@echo "              See Bitboard.h for details"
	@echo
	@echo "    mode      FAST | BULK | SLOW"
	@echo "              Specifies which perft algorithm to use"
	@echo "              See main.cpp for details"
	@echo

clean:
	-rm -f $(exe) *.a *.o *.s *.ii *.gcda

win32:
	$(MAKE) $(build) exe=$(name)-$@.exe arch=-m32 LIBS='-static -lsupc++ -static-libgcc'

win64:
	$(MAKE) $(build) exe=$(name)-$@$(sse).exe arch=-m64 LIBS='-static -lsupc++ -static-libgcc'

osx32:
	$(MAKE) $(build) exe=$(name)-$@ arch='-arch i386 -mmacosx-version-min=10.5' CXXEXTRA=-fast

osx64:
	$(MAKE) $(build) exe=$(name)-$@ arch='-arch x86_64 -mmacosx-version-min=10.5' CXXEXTRA=-fast

osx: osx32 osx64
	lipo -create -output $(name)-$@ $(name)-osx32 $(name)-osx64

lux32:
	$(MAKE) $(build) exe=$(name)-$@ arch=-m32 

lux64:
	$(MAKE) $(build) exe=$(name)-$@ arch=-m64

compile:
	$(CXX) -c $(arch) $(pgo) $(CXXUSER) $(CXXEXTRA) $(CXXFLAGS) $(src)
	$(CXX) -o $(exe) $(arch) $(pgo) $(LDFLAGS) *.o $(LIBS)

build: compile
	$(STRIP) $(exe)

profile: clean
	$(MAKE) compile pgo=-pg

pgo: clean
	$(MAKE) compile pgo=-fprofile-generate
	./$(exe) -verify 0.5
	-rm -f $(exe)
	$(MAKE) build pgo=-fprofile-use



#experimental, quick & dirty windows SDK compiler support
#target (x86 or x64) must have been set by SDK's "setenv"
#
# -Ox maximum optimisation
# -Gy function level linking
# -GR- no rtti
# -EH- no exception handling
# -MT use static runtime library
# -MD use dll runtime library (mscvrtxx.dll, which may not be installed)
# -FAs create assembly listing
vc:
	cl $(defines) -fp:fast -EHs-c- -GR- -FAs -Ox -Gy -MT $(src) -link -OPT:REF,ICF -OUT:$(name)-$(TARGET_CPU)-vc.exe


.PHONY: help clean compile build profile pgo\
        win32 win64 osx osx32 osx64 lux32 lux64 vc
