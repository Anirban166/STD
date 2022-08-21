# Todo: replace Pick_default with Pick_fuzzer/deepstate
C = g++
STEST_DIR = ../../STest/
CFLAGS = -Wall -g -std=c++11 -Iinclude

fuzztest.bin: bsd_random.o Random.o Pick.o Pick_default.o fuzztest.o
	$(C) $(CFLAGS) -o fuzztest.bin bsd_random.o Random.o Pick.o Pick_default.o fuzztest.o

bsd_random.o: $(STEST_DIR)Random/bsd_random.c
	gcc -Wall -g -c -Iinclude $(STEST_DIR)Random/bsd_random.c

Random.o: $(STEST_DIR)Random/Random.cpp
	$(C) $(CFLAGS) -c $(STEST_DIR)Random/Random.cpp

Pick.o: $(STEST_DIR)Pick/Pick.cpp
	$(C) $(CFLAGS) -c $(STEST_DIR)Pick/Pick.cpp

Pick_default.o: $(STEST_DIR)Pick/Pick_default.cpp
	$(C) $(CFLAGS) -c $(STEST_DIR)Pick/Pick_default.cpp  

fuzztest.o: ./fuzztest.cpp
	$(C) $(CFLAGS) -c ./fuzztest.cpp

clean:	
	rm fuzztest.bin bsd_random.o Random.o Pick.o Pick_default.o fuzztest.o