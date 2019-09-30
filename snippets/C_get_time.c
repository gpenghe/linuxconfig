#include <time.h>
#include <sys/time.h>

    time_t tm = time(NULL);
    SYS_HIGHLIGHT("Syncing time to %s", asctime(localtime(&tm)));
