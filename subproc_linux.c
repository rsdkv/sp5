#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#define S 100

int main(int argc, char * argv[])
{
    //READING FILE
    FILE *file;
    char arr[S];
//    char *ptr = arr;
    file = fopen(argv[1], "r");
    if (file == NULL)
    {
        char * errMsg = strerror(errno);
        printf("Error occured: %s\n", errMsg);
        return -1;
    }
    //while (fgets(arr, S, file) != NULL);
    fgets(arr, S, file);

    //SOLVING TASK
    int result = 0;
    for(int i = 0; i < strlen(arr); i++)
    {
        if(isdigit(arr[i])) result++;
    }


    //CREATING AND WRITING FILE
    char filename[6];
    printf("%d",getpid());
    sprintf(filename, "%d", getpid());//getpid() возвращает идентификатор процесса вызывающего процесса
    strcat(filename, ".txt");//создание файлов
    FILE *nfile = fopen(filename, "w+");
    fprintf(nfile, "%d", result);//result отправляет в nfile
    fclose(file);
    fclose(nfile);
    exit(EXIT_SUCCESS);
}
