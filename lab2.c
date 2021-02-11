#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

 extern char *tzname[];

 main()
 {
    time_t now;
    struct tm *struct_pointer;

    putenv ("TZ=America/Los_Angeles");
    
    // функция  int putenv(char*) кладет указатель на строку в extern char** environ
    // проверить, какие часовые пояса поддерживаются ОС можно в каталоге /usr/share/lib/zoneinfo/ 

    (void) time( &now );

    // time_t time(time_t* tloc) возвращает значение времени и если tloc не null записывает по нему возвращаемое значение 

    printf("%s", ctime( &now ) );

    struct_pointer = localtime(&now);
    
    printf("%d/%d/%02d %d:%02d %s\n",
        struct_pointer->tm_mon + 1, struct_pointer->tm_mday,
        struct_pointer->tm_year, struct_pointer->tm_hour,
        struct_pointer->tm_min, tzname[struct_pointer->tm_isdst]);
    exit(0);
 }