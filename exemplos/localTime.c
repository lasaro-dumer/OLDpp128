/* localtime example */
#include <stdio.h>
#include <time.h>

const char * curTime(){
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    return asctime (timeinfo);
}

int main ()
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    printf("Current local time and date: %s", asctime (timeinfo) );
    sleep(5);
    printf("Current local time and date: %s", curTime());

    return 0;
}
