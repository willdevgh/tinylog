#test module

# targets:
all:test test-mt
TEST:test
TESTMT:test-mt

# muti-thread test
test-mt:test-mt.cc test-mt.o logitf.o tinylog.o
	g++ -o test-mt test-mt.o logitf.o tinylog.o -std=c++11 -Wall -pthread -O1 -g 

test-mt.o:test.cc tinylog.h
	g++ -c test-mt.cc logitf.h logitf.cc tinylog.h tinylog.cc autot/auto_tchar.h -std=c++11 -Wall -O1 -g


# common test
test:test.cc test.o logitf.o tinylog.o
	g++ -o test test.o logitf.o tinylog.o -std=c++11 -Wall -pthread -O1 -g 

test.o:test.cc logitf.h logitf.cc tinylog.h tinylog.cc
	g++ -c test.cc logitf.h logitf.cc tinylog.h tinylog.cc autot/auto_tchar.h -std=c++11 -Wall -O1 -g


# logitf(log interface)
logitf.o:logitf.cc logitf.h tinylog.cc tinylog.h autot/auto_tchar.h
	g++ -c logitf.cc logitf.h tinylog.cc tinylog.h autot/auto_tchar.h -std=c++11 -Wall -pthread -O1 -g
# tinylog class
tinylog.o:tinylog.cc tinylog.h autot/auto_tchar.h
	g++ -c tinylog.cc tinylog.h autot/auto_tchar.h -std=c++11 -Wall -pthread -O1 -g


.PHONY:clean
clean:
	rm -f test test-mt core *.o *.gch
	rm -f autot/*.gch
	rm -f workthread/*.gch
	rm -f logs/*
	ls -l
	ls -l autot/
	ls -l workthread/
	ls -l logs
