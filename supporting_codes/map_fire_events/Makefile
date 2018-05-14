LDFLAGS = -L/usr/local/netcdf-4.1.3/lib -lnetcdf_c++ -L/home/jaideep/libgsm/lib -lgsm
CPPFLAGS = -c -I/usr/local/netcdf-4.1.3/include


MAINFILE = ffevents_bypft
OUTFILE = ffevents_bypft

INC = $(MAINFILE).o

all: $(MAINFILE)

$(MAINFILE): $(INC)
	g++ -o $(OUTFILE) $(INC) $(LDFLAGS)

.cpp.o:
	g++ $(CPPFLAGS) $< -o $@

clean:
	rm -Rf *.o

re: clean all

