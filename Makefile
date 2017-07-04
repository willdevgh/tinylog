#test module

#TEST:test
#TESTMT:test-mt

# muti-thread testing
#test-mt:test-mt.cc test-mt.o tinylog.o
#	g++ -o test-mt test-mt.o tinylog.o -std=c++11 -Wall -pthread -O1 -g 

#test-mt.o:test.cc tinylog.h
#	g++ -c test-mt.cc tinylog.h autot/auto_tchar.h -std=c++11 -Wall -O1 -g


# common testing
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

clean:
	rm -f test *.o *.gch
	rm -f workthread/*.gch
	rm -f logs/*
	ls -l
	ls -l workthread/
