#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

 extern char *tzname[];

 main()
 {
    time_t now;
    struct tm *struct_pointer;

    if (putenv("TZ=America/Los_Angeles"))
    {
        perror("Not enough memory.");
    };
    
    // функция  int putenv(char*) кладет указатель на строку в extern char** environ.
    // проверить, какие часовые пояса поддерживаются ОС можно в каталоге /usr/share/lib/zoneinfo/ 

    (void) time( &now );

    // time_t time(time_t* tloc) возвращает значение времени и если tloc не null записывает по нему возвращаемое значение 

    printf("%s", ctime( &now ) );

    // 1) char* ctime(time_t* clock) конвертирует clock в строку определенного формата и возвращает указатель на нее. 
    // 2) Корректировка временного пояса происходит перед генерацией строки.
    struct_pointer = localtime(&now);
    
    if (struct_pointer == NULL)
    {
        perror("Can't print info.");
    }

    // struct tm* localtime(time_t clock) 
    // 1) вызывает void tzset(void), которая в свою очередь по информации в TZ переписывает внешние переменные
    // 2) также tzset инициализирует tzname[]
    // 3) конвертирует clock в поля структуры tm и возвращает на нее указатель

    printf("%d/%d/%02d %d:%02d %s\n",
        struct_pointer->tm_mon + 1, struct_pointer->tm_mday,
        struct_pointer->tm_year, struct_pointer->tm_hour,
        struct_pointer->tm_min, tzname[struct_pointer->tm_isdst]);
    exit(0);
 }