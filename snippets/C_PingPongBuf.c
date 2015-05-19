#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uartStdio.h"

#include "config.h"
#include "PingpongBuf.h"

//#define PP_DEBUG

typedef struct{
  // properties
  int size;   // size of one PingPong block
  double* blocks[2];

  // states
  int blkFull[2];         // 0 or 1
  int readingIndex;       // 0 or 1
  int writingIndex;       // 0 or 1
  int writingPos;
} PingpongBuf ;

static PingpongBuf _pingpongbuf;

// _initPingPongBuf(ID_ADC, &g_adcPingPongBuf, g_adcBuffers, ADC_BUFLEN);
PingpongBufHandle PingpongBuf_init(int blksize, const double* buf)
{
  double *blk0 = (double*)buf;
  double *blk1 = (double*)buf + blksize;
  PingpongBuf *pb = &_pingpongbuf;

#if 0
  memset(pb, FILL_VALUE, sizeof(PingpongBuf));
  memset(blk0, FILL_VALUE, size *sizeof(double));
  memset(blk1, FILL_VALUE, size *sizeof(double));
#endif

  pb->size = blksize;
  pb->blocks[0] = blk0;
  pb->blocks[1] = blk1;

  pb->blkFull[0] = 0;
  pb->blkFull[1] = 0;
  pb->readingIndex = 0;
  pb->writingIndex = 0;
  pb->writingPos = 0;

  return (PingpongBufHandle)pb;
}

BOOL PingpongBuf_addOneItem(PingpongBufHandle handle, const double* item, int size)
{
  PingpongBuf* pb = (PingpongBuf*)handle;

  {//TODO: use semaphore here.

    if(pb->blkFull[pb->writingIndex] == 1){ // old data not consumed yet?
      ASSERT(pb->blkFull[!pb->readingIndex] == 1); // the other block must be full
      ASSERT(pb->writingPos == 0);
#ifdef PP_DEBUG
      printf("Old data not consumed yet! not adding and wait...\n");
#endif
      return FALSE;
    }

    if(pb->writingPos + size > pb->size){
#ifdef PP_DEBUG
      printf("Not enough space in the current buffer block.\n");
#endif
      return FALSE;
    }
    memcpy(&pb->blocks[pb->writingIndex][pb->writingPos], item, size * sizeof(double));
    pb->writingPos += size;

    if(pb->writingPos == pb->size){ // current block full
#ifdef PP_DEBUG
      printf("current block finished:%d, next to write: %d\n", pb->writingIndex, pb->writingIndex^1);
#endif

      pb->blkFull[pb->writingIndex] = 1;
      pb->writingIndex = !pb->writingIndex;   // toggle
      pb->writingPos = 0;
    }
  }
  return TRUE;
}

/*
 * Copy PingPongBuf::size double to dst
 */
BOOL PingpongBuf_readOneBlock(PingpongBufHandle handle, double* dst)
{
  PingpongBuf* pb = (PingpongBuf*)handle;
  {//TODO: use semaphore here.

    if(pb->blkFull[pb->readingIndex] == 0){ // not a full block of data yet
      ASSERT(pb->blkFull[!pb->readingIndex] == 0); // the other block must be empty
      return FALSE;
    }

#ifdef PP_DEBUG
    printf("use data:%d\n", pb->readingIndex);
#endif
    memcpy(dst, pb->blocks[pb->readingIndex], sizeof(double) * pb->size);

    pb->blkFull[pb->readingIndex] = 0;
    pb->readingIndex = !pb->readingIndex;
  }
  return TRUE;
}

void PingpongBuf_dump(PingpongBufHandle handle)
{
  int i;
  PingpongBuf* pb = (PingpongBuf*)handle;
  printf("\nPingPong Buffer dump:\n");
  printf("size:%d, blkFull: %d, %d, r: %d, w:%d, wpos:%d\n",
      pb->size, pb->blkFull[0], pb->blkFull[1],
      pb->readingIndex, pb->writingIndex, pb->writingPos);
  printf("blk[0]:\n");
  for(i=0; i<pb->size; i++){
    printf("%.1f ", pb->blocks[0][i]);
  }
  printf("\nblk[1]:\n");
  for(i=0; i<pb->size; i++){
    printf("%.1f ", pb->blocks[1][i]);
  }
  printf("\n\n");
}
