#test module

# targets:
all:test test-mt
TEST:test
TESTMT:test-mt

# muti-thread test
test-mt:test-mt.cc test-mt.o logitf_si.o tinylog.o
	g++ -o test-mt test-mt.o logitf_si.o tinylog.o -std=c++11 -Wall -pthread -O1 -g 

test-mt.o:test.cc tinylog.h
	g++ -c test-mt.cc logitf_si.h logitf_si.cc tinylog.h tinylog.cc autot/auto_tchar.h -std=c++11 -Wall -O1 -g


# common test
test:test.cc test.o logitf_si.o tinylog.o
	g++ -o test test.o logitf_si.o tinylog.o -std=c++11 -Wall -pthread -O1 -g 

test.o:test.cc logitf_si.h logitf_si.cc tinylog.h tinylog.cc
	g++ -c test.cc logitf_si.h logitf_si.cc tinylog.h tinylog.cc autot/auto_tchar.h -std=c++11 -Wall -O1 -g


# logitf_si(log interface)
logitf_si.o:logitf_si.cc logitf_si.h tinylog.cc tinylog.h autot/auto_tchar.h
	g++ -c logitf_si.cc logitf_si.h tinylog.cc tinylog.h autot/auto_tchar.h -std=c++11 -Wall -pthread -O1 -g
#logitf_ci
logitf_ci.o:logitf_ci.cc logitf_ci.h tinylog.cc tinylog.h autot/auto_tchar.h
	g++ -c logitf_ci.cc logitf_ci.h tinylog.cc tinylog.h autot/auto_tchar.h -std=c++11 -Wall -pthread -O1 -g


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
