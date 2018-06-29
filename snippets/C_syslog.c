#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

int main(void) {

 openlog("CTB.Discovery", LOG_PID|LOG_CONS, LOG_USER);
 syslog(LOG_DEBUG, "LOG_DEBUG");
 syslog(LOG_INFO, "LOG_INFO");
 syslog(LOG_WARNING, "LOG_WARNING");
 open("/dev/xxxxx", O_RDONLY);
 syslog(LOG_ERR, "LOG_ERR: %m");
 closelog();

 return 0;
}
