C = g++
STEST_DIR = ../../STest/
CFLAGS = -Wall -g -std=c++11 -Iinclude -no-pie

fuzztest_deepstateharness.bin: bsd_random.o Random.o Pick.o Pick_default.o fuzztest_deepstateharness.o
	$(C) $(CFLAGS) -ldeepstate -o fuzztest_deepstateharness.bin bsd_random.o Random.o Pick.o Pick_default.o fuzztest_deepstateharness.o

bsd_random.o: $(STEST_DIR)Random/bsd_random.c
	gcc -Wall -g -c -Iinclude $(STEST_DIR)Random/bsd_random.c

Random.o: $(STEST_DIR)Random/Random.cpp
	$(C) $(CFLAGS) -c $(STEST_DIR)Random/Random.cpp

Pick.o: $(STEST_DIR)Pick/Pick.cpp
	$(C) $(CFLAGS) -c $(STEST_DIR)Pick/Pick.cpp

Pick_default.o: $(STEST_DIR)Pick/Pick_default.cpp
	$(C) $(CFLAGS) -c $(STEST_DIR)Pick/Pick_default.cpp  

fuzztest_deepstateharness.o: ./fuzztest_deepstateharness.cpp
	$(C) $(CFLAGS) -c ./fuzztest_deepstateharness.cpp

clean:	
	rm fuzztest_deepstateharness.bin bsd_random.o Random.o Pick.o Pick_default.o fuzztest_deepstateharness.o