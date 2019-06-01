ALL:main
	
main:DATA COLUMN TABLE ALLBASES PARSE MAIN 
	g++ main.o DATA.o TABLE.o COLUMN.o ALLBASES.o PARSE.o -o main -lm -O2 -DONLINE_JUDGE -std=c++11
DATA:DATA.cpp
	g++ -c DATA.cpp -std=c++11
MAIN:main.cpp
	g++ -c main.cpp -std=c++11
TABLE:TABLE.cpp
	g++ -c TABLE.cpp -std=c++11
COLUMN:COLUMN.cpp
	g++ -c COLUMN.cpp -std=c++11
ALLBASES:ALLBASES.cpp
	g++ -c ALLBASES.cpp -std=c++11
PARSE:PARSE.cpp
	g++ -c PARSE.cpp -std=c++11
clean:
	rm *.o main