pp = g++
exe = main1 main2
head = DATA.h TABLE.h COLUMN.h ALLBASES.h PARSE.h DATABASE.h
obj = main.o DATA.o TABLE.o COLUMN.o ALLBASES.o PARSE.o
cpp = main.cpp TABLE.cpp COLUMN.cpp ALLBASES.cpp PARSE.cpp

ALL: $(exe)
	
$(exe) : $(obj) 
	$(pp) $^ -o $@ -lm -O2 -DONLINE_JUDGE -std=c++11

*.o : *.cpp *.h
	$(pp) $< -c *.o -lm -O2 -DONLINE_JUDGE -std=c++11
clean:
	rm $(obj) $(exe)