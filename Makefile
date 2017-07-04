#test module

test:test.cc test.o tinylog.o
	g++ -o test test.o tinylog.o -std=c++11 -Wall -pthread -O1 -g 

test.o:test.cc tinylog.h
	g++ -c test.cc tinylog.h autot/auto_tchar.h -std=c++11 -Wall -O1 -g

tinylog.o:tinylog.cc tinylog.h autot/auto_tchar.h
	g++ -c tinylog.cc tinylog.h autot/auto_tchar.h -std=c++11 -Wall -pthread -O1 -g

clean:
	rm -f test *.o *.gch
	rm -f workthread/*.gch
	rm -f logs/*
	ls -l
	ls -l workthread/
