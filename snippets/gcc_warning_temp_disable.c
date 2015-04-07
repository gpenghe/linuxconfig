#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "sp_util.h"
 
//#define USE_FIFO

#ifdef USE_FIFO
#define TRACE_FILE "trace.fifo"
static int fd_trace = -1;
#else
#define TRACE_FILE "trace.log"
static FILE *fp_trace;
#endif

static uint64_t g_sp_base = 0;

void __attribute__ ((constructor)) trace_begin (void)
{
    g_sp_base = GET_SP();
    printf("Writing instrumentation info to trace.out (sp base: %016lx...\n", g_sp_base);
#ifdef USE_FIFO
    fd_trace = open(TRACE_FILE, O_WRONLY);
    if (fd_trace == -1) {
        fprintf(stderr, "Opening %s failed.\n", TRACE_FILE);
        exit(-1);
    }
#else
    fp_trace = fopen("trace.out", "w");
    if (fp_trace == NULL) {
        fprintf(stderr, "Opening %s failed.\n", TRACE_FILE);
        exit(-1);
    }
#endif
}

void __attribute__ ((destructor)) trace_end (void)
{
#ifdef USE_FIFO
    if(fd_trace != -1) {
        close(fd_trace);
    }
#else
    if(fp_trace != NULL) {
        fclose(fp_trace);
    }
#endif
    printf("Instrumentation ended.\n");
}

#pragma GCC diagnostic ignored "-Wunused-variable"
static inline void print_instru_info(void* func, void* caller)
{
    char buf[200];
    int len = snprintf(buf, 200, "e %16lx %16lx %lu %lu\n", (unsigned long)func, (unsigned long)caller, time(NULL), g_sp_base - GET_SP()); 
#ifdef USE_FIFO
    write(fp_fd, buf, len);
#else
    fprintf(fp_trace, "%s", buf);
#endif
}
#pragma GCC diagnostic pop

void __cyg_profile_func_enter (void *func,  void *caller)
{
    print_instru_info(func, caller);
}

void __cyg_profile_func_exit (void *func, void *caller)
{
    print_instru_info(func, caller);
}

