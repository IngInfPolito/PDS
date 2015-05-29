CC=g++
CCFLAGS=-std=c++0x -Wall -O3 # compile
CCDFLAGS=-std=c++0x -Wall -g # debug

CPPSRC=*.cpp
HSRC=*.h

OUT=Main

Main: $(HSRC) $(CPPSRC)
	$(CC) $(CCFLAGS) $(CPPSRC) -o $(OUT)

debug: $(HSRC) $(CPPSRC)
	$(CC) $(CCDFLAGS) $(CPPSRC) -o $(OUT)

clean:
	rm $(OUT)
