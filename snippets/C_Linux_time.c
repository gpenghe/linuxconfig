#include <time.h>

void main(void){
    extern char* tzname[2];
    time_t a = time(NULL);
    printf("%llu\n", a);
    printf("%s", ctime(&a));
    printf("%s %s\n", tzname[0], tzname[1]);
}
