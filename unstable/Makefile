ROOTLIBS   = $(shell root-config --libs)
ROOTCFLAGS = $(shell root-config --cflags)

CFLAG      = -I $(ROOTCFLAGS)
LFLAG      = $(ROOTLIBS)

OBJ     = Config.o BinaryToRoot.o main.o



run: $(OBJ)		
	g++ $(OBJ) $(LFLAG) -o run
	@echo -e 'Done.\n\n   Type "./run config-file-name"\n\n'
Config.o: Config.h Config.cc 
	g++ $(CFLAG) -c Config.cc
BinaryToRoot.o: BinaryToRoot.h BinaryToRoot.cc Config.h Config.cc 
	g++ $(CFLAG) -c BinaryToRoot.cc
main.o: main.cc BinaryToRoot.h BinaryToRoot.cc
	g++ $(CFLAG) -c  main.cc


clean:
	@rm -f *.o 
	@rm -f run
	@rm -f *~
	@rm -f *#
	@rm -f .#*


