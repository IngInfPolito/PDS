#
# Compile with one of these:
# 	make client
# 	make clientWithMalloc
# 	make clientWithDefrag
# 	make debug
# 	make all
#

CC=gcc
CCFLAGS=-Wall -O3 # compile
CCDFLAGS=-Wall -g # debug

HSRC=heap.h
CSRC=client.c heap.c
CMSRC=clientWithMalloc.c heap.c
CDSRC=clientWithDefrag.c heap.c

OUTC=client
OUTCM=clientWithMalloc
OUTCD=clientWithDefrag

$(OUTC): $(CSRC) $(HSRC)
	$(CC) $(CCFLAGS) $(CSRC) -o $(OUTC)

$(OUTCM): $(CMSRC) $(HSRC)
	$(CC) $(CCFLAGS) $(CMSRC) -o $(OUTCM)

$(OUTCD): $(CDSRC) $(HSRC)
	$(CC) $(CCFLAGS) $(CDSRC) -o $(OUTCD)

debug: $(CSRC) $(CMSRC) $(CDSRC) $(HSRC)
	$(CC) $(CCDFLAGS) $(CSRC) -o $(OUTC)
	$(CC) $(CCDFLAGS) $(CMSRC) -o $(OUTCM)
	$(CC) $(CCDFLAGS) $(CDSRC) -o $(OUTCD)

all: $(OUTC) $(OUTCM) $(OUTCD)

clean:
	rm $(OUTC) $(OUTCM) $(OUTCD)
