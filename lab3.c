   #include <sys/types.h>
   #include <unistd.h>
   #include <stdio.h>
  
   int main(int argc, char *argv[])
   {
       FILE *file;
 
      printf("Real user id: %d\nEffective user id: %d\n", getuid(), geteuid());
      
        // uid_t getuid(void) возвращает реальный id пользователя процесса.
        // uid_t geteuid(void) возвращает эффективный id пользователя процесса.

      file = fopen (argv[1], "r");
      if (file == NULL) {
          perror("File not opened at first try");
        }
      else {
          printf("File opened at first try\n\n");
          fclose(file);
      }
 
      if(seteuid(getuid()) == -1)
      {
        perror("Can't set this id.");
      }
 
        // int setuid(uid_t uid) присваивает эффективному id пользователя процесса uid.
        // В данном случае аргументом является реальный id пользователя процесса.

      printf("New real user id: %d\nNew effective user id: %d\n", getuid(), geteuid());

      file = fopen (argv[1], "r");      
       if (file == NULL) {
          perror("File not opened at second try");
      }
      else {
          printf("File opened at second try\n");
          fclose(file);
      }
      exit(0);
 }