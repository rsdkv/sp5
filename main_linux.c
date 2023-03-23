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
        printf("Error enter the required number of arguments\n");
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
    char num_file = '1';
    char pid_filename[6]; // т.к. pid = 2^22 = 6 цифр
    char slice[M];
    int len;
    for (size_t i = 0; i < N; i++)//беззнак инт
    {
        slice[0] = 0;
        if (i == N - 1 & M % N != 0)
        {
            len = M - (N-1) * (M/N);//для последнего процесса
            strncat(slice, ptr, len);//помещение рез слайса в новые строки
        }
        else
        {
            len = M/N;
            strncat(slice, ptr, len);//помещение рез слайса в новые строки
            ptr = ptr + len;
        }

        pid_filename[0] = num_file;
        strcat(pid_filename, ".txt");//создание 1.txt, 2.txt etc
        FILE *nfile = fopen(pid_filename, "w+");//w+ - создание файла если его не сущ
        pid_filename[1] = '\0';//нуль-терминатор, используемый для обозначения конца строк, т к не знаем длину строки
        num_file++;

        // WRITING N FILES
        fwrite(slice, sizeof(char), len, nfile);
        slice[1] = '\0';//нуль-терминатор, используемый для обозначения конца строк, т к не знаем длину строки
        fclose(nfile);
    }

    char char_filename = '1';
    char pid_name[6];
    for (int i = 0; i < N; i++)
    {
        pid_name[0] = char_filename;
        pid_t res = fork();//pid_t - тип данных для процесса, fork - – создаёт полную копию текущего процесса
        if (res == 0)//обработка дочерних процессов (fork'a)
        {
            strcat(char_filename, ".txt");//создание файла под доч процессы
            char * args[3] = {"subproc", char_filename, NULL};//объявление арг для доч проц
            execve("./subproc", args, NULL);//заменяет текущий процесс на новый, исполняемый файл которого передан в качестве аргумента
        }
        if (res == -1)
        {
            char * errMsg = strerror(errno);
            printf("Error occured: %s\n", errMsg);
            return 1;
        }
//        pid_name[1] = '\0';
        char_filename++;
    }
    int result = 0;
    for (int i = 0; i < N; i++)
    {
        int code = 0;//с презентации
//        getchar();//зомби процесс
        pid_t child = wait(&code);//выполняет ожидание завершения процесса и предоставляет код его завершения в качестве выходных данных
        if (child == -1)
        {
            printf("Error waiting %d\n", errno);
            return 1;
        }

        // CALCULATING RESULT
        char filename[6];//массив в который помещается айди процесса
        char result_of_proc[S];//содержит рез от доч процесса
        sprintf(filename, "%lld", child);//помещение в 1 3
        strcat(filename, ".txt");//создание файла под доч процессы
        FILE *result_file = fopen(filename, "r");
        if (result_file == NULL)
        {
            char * errMsg = strerror(errno);
            printf("Error occured: %s\n", errMsg);
            return -1;
        }
//        while (fgets(result_of_proc, S, result_file) != NULL);
        fgets(result_of_proc, S, result_file);//dest-result_of_proc, source - result_file
        fclose(result_file);
        result += atoi(result_of_proc);
        printf("Process with pid %d exited with code %d\n", child, WEXITSTATUS(code));//WEXITSTATUS(code)-макрос, получение корректного кода возврата из дочернего процесса
        //child - pid
    }
    printf("Number of digits %d\n", result);
    return 0;
}
