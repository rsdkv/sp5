#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define S 100

int main(int argc, char *argv[]) {
    if (argc != 3) {
        char *errMsg = strerror(errno);
        printf("Error occurred: the number of variables should be strongly 2 - file_name and N (%s)\n", errMsg);
        return -1;
    }
    // READING FILE
    FILE *file;
    char arr[S];
    char *ptr = arr;
    file = fopen(argv[1], "r");
    if (file == NULL) {
        char *errMsg = strerror(errno);
        printf("Error occurred: %s\n", errMsg);
        return -1;
    }
    fgets(arr, S, file); // Место хранения данных, Максимальное число считываемых символов, Указатель на структуру FILE
    fclose(file);

    if (strlen(arr) < 2) {
        char *errMsg = strerror(errno);
        printf("Error occurred: the number of symbols is less than 2 in the file (%s)\n", errMsg);
        return -1;
    } else printf("File consists: %s\n", arr);


    int N = atoi(argv[2]);
    int M = strlen(arr);
    if (N > M / 2) {
        N = M / 2;
        printf("Warning: children processes have been reduced to %d\n", N);
    }
    // CREATING N FILES
    char c = 'a';
    char filename[5];
    char slice[M];
    int len;
    for (size_t i = 0; i < N; i++) {// разделение стринги по N файлам
        slice[0] = 0;
        if (i == N - 1 & M % N != 0) {//для последнего процесса
            len = M - (N - 1) * (M / N);
            strncat(slice, ptr, len);
        } else {//для всех остальных
            len = M / N;
            strncat(slice, ptr, len);
            ptr = ptr + len;
        }

        filename[0] = c;
        strcat(filename, ".txt");
        FILE *nfile = fopen(filename, "w+");
        filename[1] = '\0'; //символ для окончания записи null terminator
        c++;

        // WRITING N FILES
        fwrite(slice, sizeof(char), len, nfile); //slice -
        slice[1] = '\0';
        fclose(nfile);
    }

    char chc = 'a';
    char chfname[5];
    u_long pid_arr[N];
    HANDLE proc_arr[N];//создание процесса (массив процессов)
    for (int i = 0; i < N; i++) {
        PROCESS_INFORMATION pi;// презы  - инф о процессе
        STARTUPINFO si; // init process
        GetStartupInfo(&si);
        chfname[0] = chc;// для передаче доч процессу аргументов от родительского
        strcat(chfname, ".txt "); //strcat -
        char source[100] = "subproc.exe ";
        strcat(source, chfname);// название исп файла и название файла,
        // указатель на массив в который будет добавлена строка и указатель на массив из которого будет скопирована строка

        BOOL res = CreateProcess(
                "subproc.exe",
                source,
                NULL,
                NULL,
                TRUE, 0,
                NULL, NULL,
                &si, &pi);
// можно ли певый параметр указать null - можно, т к передан в source
        if (!res) {
            char *errMsg = strerror(errno);
            printf("Error occurred: %s\n", errMsg);
            return 1;
        }// обработка ошибки по созданию процесса
        chfname[1] = '\0';
        chc++;
        pid_arr[i] = pi.dwProcessId;// id текущего доч процесса
        proc_arr[i] = pi.hProcess; // передаётся текущий процесс(handle)
    }

    int result = 0;
    DWORD excode;

    for (int i = 0; i < N; i++) {
        DWORD dwRes = WaitForSingleObject(proc_arr[i], INFINITE);//ожидание завершения доч процессов

        if (dwRes == WAIT_FAILED) {// если ожидание неуспешно завершилось
            printf("Error %lx\n", GetLastError());
            return 1;
        }
        // CALCULATING RESULT
        char filen[50];//массив в который помещается айди процесса
        char fres[S];//содержит рез от доч процесса

        sprintf(filen, "%lu", pid_arr[i]);
        strcat(filen, ".txt");
        FILE *resfile;
        resfile = fopen(filen, "r");
        if (resfile == NULL) {
            char *errMsg = strerror(errno);
            printf("Error occurred: %s\n", errMsg);
            return -1;
        }
        while (fgets(fres, S, resfile) != NULL);// в fres из resfile

        result += atoi(fres);//string to int
        printf("Child process exited with pid %lu with code %d\n", pid_arr[i],
               GetExitCodeProcess(proc_arr[i], &excode));
        fclose(resfile);
        CloseHandle(proc_arr[i]);
//        CloseHandle(thread_arr[i]);
    }

    printf("Result is %d\n", result);

    return 0;
}

