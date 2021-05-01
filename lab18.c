#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_TIME_LENGTH 128
#define LSTAT_ERROR -1
#define PRINT_INFO_FAILURE -1
#define PRINT_INFO_SUCCESS 0
#define CHECK_INIT -1
#define GETPWUID_ERROR NULL
#define GETGRGID_ERROR NULL
#define LOCALTIME_ERROR NULL
#define BUFFER_OVERFLOW 0
#define END_OF_ARGS NULL

const char* default_args[] = { ".", END_OF_ARGS };

int print_directory_info(char* pathname)
{
    char mask[11] = { '\0' };
    char date_time_info[MAX_TIME_LENGTH] = { '\0' };
    struct stat file_stat;
    struct passwd* owner = NULL;
    struct group* owner_group = NULL;
    struct tm* timezone = NULL;
    int strftime_check = CHECK_INIT;
    int lstat_check = CHECK_INIT;   
    
    lstat_check = lstat(pathname, &file_stat);
    if (lstat_check == LSTAT_ERROR) 
    {
        perror("Can't get file info");
        return PRINT_INFO_FAILURE;
    }

    owner = getpwuid(file_stat.st_uid);
    if (owner == GETPWUID_ERROR)
    {
        perror("Can't get user's id");
        return PRINT_INFO_FAILURE;
    }
    
    owner_group = getgrgid(file_stat.st_gid);
    if (owner_group == GETGRGID_ERROR)
    {
        perror("Can't get group's id");
        return PRINT_INFO_FAILURE;
    }

    timezone = localtime(&(file_stat.st_ctime));
    if (timezone == LOCALTIME_ERROR)
    {
        perror("Can't get time info");
        return PRINT_INFO_FAILURE;
    }

    strftime_check = strftime(date_time_info, MAX_TIME_LENGTH, "%e %b %H:%M", timezone);
    if (strftime_check == BUFFER_OVERFLOW)
    {
        printf("strftime buffer overflow\n");
        return PRINT_INFO_FAILURE;
    }

    if      (S_ISDIR(file_stat.st_mode)) { mask[0] = 'd'; }
    else if (S_ISREG(file_stat.st_mode)) { mask[0] = '-'; }
    else                                  { mask[0] = '?'; }

    mask[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    mask[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    mask[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';

    mask[3] = (file_stat.st_mode & S_ISUID) ? 's' : mask[3];

    mask[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    mask[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    mask[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';

    mask[6] = (file_stat.st_mode & S_ISGID) ? 's' : mask[6];

    mask[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    mask[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    mask[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';

    printf("%s\t%d", mask, file_stat.st_nlink);
    printf("\t%s\t%s", owner->pw_name, owner_group->gr_name);
    printf("\t%lu\t%s\t%s\n", file_stat.st_size, date_time_info, pathname);
    
    return PRINT_INFO_SUCCESS;
}

int main(int argc, char** argv)
{
    char** files_in_directory;

    if (argc < 2) 
    {
        files_in_directory = (char**)default_args;
    } 
    else 
    {
        files_in_directory = &(argv[1]);
    }

    int print_info_check = CHECK_INIT;
    for (int i = 0; files_in_directory[i] != END_OF_ARGS; i++) 
    {
        print_info_check = print_directory_info(files_in_directory[i]);
        if (print_info_check == PRINT_INFO_FAILURE)
        {
            printf("Can't print info about %s\n", files_in_directory[i]);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}