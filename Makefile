CFLAG = -Wall -Wextra -std=c++11 # Good practice to include warnings and a C++ standard

all: query

reset: clean query

query: main.o datapage.o component.o readInput.o
	@echo "Producing query application"
	g++ $(CFLAG) main.o datapage.o component.o readInput.o -o query # Use g++ for linking
	@chmod +x query

main.o: main.cpp
	@g++ $(CFLAG) -c main.cpp 

datapage.o: includes/datapage.cpp includes/datapage.h
	@g++ $(CFLAG) -c includes/datapage.cpp

component.o: includes/component.cpp includes/component.h
	@g++ $(CFLAG) -c includes/component.cpp

readInput.o: includes/readInput.cpp includes/readInput.h
	@g++ $(CFLAG) -c includes/readInput.cpp

clean:
	@echo "Removing .o .mdf .ldf files"
	rm -f *.o *.mdf *.ldf query

cleano:
	@echo "Removing .o files"
	rm -f *.o 

cleani:
	@echo "Removing .mdf .ldf files"
	rm -f *.mdf *.ldf query