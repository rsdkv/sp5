#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define S 100

int main(int argc, char *argv[]) {

//    Sleep(30000);
    //READING FILE
    FILE *file;
    char arr[S];

    file = fopen(argv[1], "r");
    if (file == NULL) {
        char *errMsg = strerror(errno);
        printf("Error occurred: %s with %s\n", errMsg, argv[1]);
        return -1;
    }
    while (fgets(arr, S, file) != NULL);
    fclose(file);

    //SOLVING TASK
    int result = 0;
    for (int i = 0; i < strlen(arr); i++) {
        if (isdigit(arr[i])) result++;//подсчёт кол-ва цифр
    }
    //CREATING AND WRITING FILE
    char filename[50];
    sprintf(filename, "%lu", GetCurrentProcessId());// получение pid
    strcat(filename, ".txt");// создание и запись res --> xxx.txt
    FILE *nfile = fopen(filename, "w+");
    fprintf(nfile, "%d", result);
    fclose(nfile);
    Sleep(30000);
    ExitProcess(1);

}
