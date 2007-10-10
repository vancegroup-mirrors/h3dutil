
include Makefile.sources
include Makefile.objects


H3DUTIL_HOME          = ../..

#
#
###############################################################

CXX        = g++ -Wall
SHARED     = -shared -export-dynamic
INCLUDES   = -I$(H3DUTIL_HOME)/include
CXXFLAGS   = -g $(INCLUDES)
LDFLAGS    = 


###############################################################

all:	libH3DUtil.so

libH3DUtil.so:	$(OBJECTS)
	$(CXX) $(LDFLAGS) $(CXXLIBS) $(SHARED) -o $@ $(OBJECTS) 

clean:	
	$(RM) $(H3DUTIL_HOME)/src/*~ $(H3DUTIL_HOME)/src/*.o 

install: libH3DUtil.so
	cp libH3DUtil.so /usr/lib

update:	
	$(MAKE) sources
	$(MAKE) objects
	$(MAKE) depend

sources:
	ls $(H3DUTIL_HOME)/src/*.cpp > Makefile.sources
	echo -n "SOURCES=" > Makefile.sources2
	cat Makefile.sources \
	  | sed 's/.cpp/.cpp \\/g' \
	  >> Makefile.sources2
	mv Makefile.sources2 Makefile.sources

objects:
	ls $(H3DUTIL_HOME)/src/*.cpp  > Makefile.objects
	echo -n "OBJECTS=" > Makefile.objects2
	cat Makefile.objects \
	  | sed 's/.cpp/.o \\/g' \
	  >> Makefile.objects2
	mv Makefile.objects2 Makefile.objects

depend:
	touch Makefile.depend
	makedepend -fMakefile.depend $(INCLUDES) $(SOURCES)


include Makefile.depend
