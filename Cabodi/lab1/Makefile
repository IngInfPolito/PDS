#
# Compile with one of these:
# 	make client
# 	make clientWithMalloc
#

CC=gcc
CCFLAGS=-Wall -O3

HSRC=heap.h
CSRC=client.c heap.c
CMSRC=clientWithMalloc.c heap.c

OUTC=client
OUTCM=clientWithMalloc

$(OUTC): $(CSRC) $(HSRC)
	$(CC) $(CCFLAGS) $(CSRC) -o $(OUTC)

$(OUTCM): $(CMSRC) $(HSRC)
	$(CC) $(CCFLAGS) $(CMSRC) -o $(OUTCM)
