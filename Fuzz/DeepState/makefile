C = g++
CXXFLAGS = -Wall -g -std=c++11 -Iinclude -no-pie

test.bin: bsd_random.o Random.o Pick.o Pick_default.o fuzztest_deepstateharness.o
	$(C) $(CXXFLAGS) -ldeepstate -o fuzztest_deepstateharness.bin bsd_random.o Random.o Pick.o Pick_default.o fuzztest_deepstateharness.o

bsd_random.o: ../../STest/Random/bsd_random.c
	gcc -Wall -g -c -Iinclude ../../STest/Random/bsd_random.c

Random.o: ../../STest/Random/Random.cpp
	$(C) $(CXXFLAGS) -c ../../STest/Random/Random.cpp

Pick.o: ../../STest/Pick/Pick.cpp
	$(C) $(CXXFLAGS) -c ../../STest/Pick/Pick.cpp

Pick_default.o: ../../STest/Pick/Pick_default.cpp
	$(C) $(CXXFLAGS) -c ../../STest/Pick/Pick_default.cpp  

test.o: ./fuzztest_deepstateharness.cpp
	$(C) $(CXXFLAGS) -c ./fuzztest_deepstateharness.cpp

clean:	
	rm fuzztest_deepstateharness.bin bsd_random.o Random.o Pick.o Pick_default.o fuzztest_deepstateharness.o