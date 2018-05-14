# makefile for fire model

TARGET = fire
LIBPATH = -L/usr/local/netcdf-cxx-legacy/lib -L/home/jaideep/codes/libgsm_v3/lib
INCPATH = -I/usr/local/netcdf-c/include -I/usr/local/netcdf-cxx-legacy/include
INCPATH += -I/home/jaideep/codes/libgsm_v3/include
LDFLAGS =  
CPPFLAGS = -g

LIBS = -l:libgsm.so.3 -lnetcdf_c++

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



