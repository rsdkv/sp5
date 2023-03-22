#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>

#define S 100
int main(int argc, char * argv [])
{
    if (argc != 3)
    {
        char * errMsg = strerror(errno);
        printf("Error enter the required number of arguments %s\n", errMsg);
        return -1;
    }
    // READING FILE
    FILE *file;
    char arr[S];
    char *ptr = arr;
    file = fopen(argv[1], "r");
    if (file == NULL)
    {
        char * errMsg = strerror(errno);
        printf("Error occured: %s\n", errMsg);
        return -1;
    }
    fgets(arr, S, file);
    fclose(file);

    if (strlen(arr) < 2)
    {
        char * errMsg = strerror(errno);
        printf("There must be at least 2 arguments in the file (%s)\n", errMsg);
        return -1;
    }
    else printf("file content: %s\n", arr);


    int N = atoi(argv[2]);
    int M = strlen(arr);
    if (N > M/2)
    {
        N = M/2;
        printf("the number of child processes is reduced to %d\n", N);
    }
    // CREATING N FILES
    char c = '1';
    char filename[5];
    char slice[M];
    int len;
    for (size_t i = 0; i < N; i++)
    {
        slice[0] = 0;
        if (i == N - 1 & M % N != 0)
        {
            len = M - (N-1) * (M/N);
            strncat(slice, ptr, len);
        }
        else
        {
            len = M/N;
            strncat(slice, ptr, len);
            ptr = ptr + len;
        }

        filename[0] = c;
        strcat(filename, ".txt");
        FILE *nfile = fopen(filename, "w+");
        filename[1] = '\0';
        c++;

        // WRITING N FILES
        fwrite(slice, sizeof(char), len, nfile);
        slice[1] = '\0';
        fclose(nfile);
    }

    char chc = '1';
    char chfname[5];
    for (int i = 0; i < N; i++)
    {
        chfname[0] = chc;
        pid_t res = fork();
        if (res == 0)
        {
            strcat(chfname, ".txt");
            char * args[3] = {"subproc", chfname, NULL};
            execve("./subproc", args, NULL);
        }
        if (res == -1)
        {
            char * errMsg = strerror(errno);
            printf("Error occured: %s\n", errMsg);
            return 1;
        }
        chfname[1] = '\0';
        chc++;
    }
    int result = 0;
    for (int i = 0; i < N; i++)
    {
        int code = 0;
//        getchar();//зомби процесс
        pid_t child = wait(&code);
        if (child == -1)
        {
            printf("Error %d\n", errno);
            return 1;
        }

        // CALCULATING RESULT
        char filename[50];
        char fres[S];
        sprintf(filename, "%d", child);
        strcat(filename, ".txt");
        FILE *resfile = fopen(filename, "r");
        if (resfile == NULL)
        {
            char * errMsg = strerror(errno);
            printf("Error occured: %s\n", errMsg);
            return -1;
        }
        while (fgets(fres, S, resfile) != NULL);
        fclose(resfile);
        result += atoi(fres);
        printf("Process with pid %d exited with code %d\n", child, WEXITSTATUS(code));
    }
    printf("Number of digits %d\n", result);
    return 0;
}
