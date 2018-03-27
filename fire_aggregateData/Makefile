# makefile for fire model

TARGET = aggregate
LIBPATH = -L/usr/local/netcdf-cxx-legacy/lib -L/home/jaideep/codes/FIRE_CODES/libgsm_v2/lib
INCPATH = -I/usr/local/netcdf-c-4.3.2/include -I/usr/local/netcdf-cxx-legacy/include
INCPATH += -I/home/jaideep/codes/FIRE_CODES/libgsm_v2/include
LDFLAGS =  
CPPFLAGS = -g

LIBS = -l:libgsm.so.2 -lnetcdf_c++

SOURCEDIR = src
BUILDDIR = build

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))


all: dir $(TARGET)

dir:
	mkdir -p $(BUILDDIR)

$(TARGET): $(OBJECTS)
	g++ -o $(LDFLAGS) $(TARGET) $(OBJECTS) $(LIBPATH) $(LIBS)

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	g++ -c $(CPPFLAGS) $(INCPATH) $< -o $@ 

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)



