CFLAG = -Wall -Wextra -std=c++11 # Good practice to include warnings and a C++ standard

all: clean query

query: main.o datapage.o
	@echo "Producing query application"
	g++ $(CFLAG) main.o datapage.o -o query # Use g++ for linking
	@chmod +x query

main.o: main.cpp
	@g++ $(CFLAG) -c main.cpp 

datapage.o: includes/datapage.cpp includes/datapage.h
	@g++ $(CFLAG) -c includes/datapage.cpp


clean:
	@echo "Removing .o files"
	rm -f *.o *.mdf *.ldf query