/* localtime example */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>


const char * curTime(){
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    return asctime (timeinfo);
}
const double curMilis(){
    struct timeval  tv;
    gettimeofday(&tv, NULL);

    return ((tv.tv_sec) * 1000 + (tv.tv_usec) / 1000.0) +0.5; // convert tv_sec & tv_usec to millisecond
}

/*
sprintf(result, "%.3s                       %.3s                     %3d                %.2d:            %.2d:            %.2d            %d\n",
                wday_name[timeptr->tm_wday],mon_name[timeptr->tm_mon],timeptr->tm_mday, timeptr->tm_hour,timeptr->tm_min, timeptr->tm_sec,1900 + timeptr->tm_year
*/

int main ()
{
    int i;
    for(i=0;i<30;i++){
        printf("Current local time and date: %f\n", curMilis());
        usleep(1000);
    }


    return 0;
}
