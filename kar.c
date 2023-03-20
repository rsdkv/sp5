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
    fgets(arr, S, file);
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
    for (size_t i = 0; i < N; i++) {
        slice[0] = 0;
        if (i == N - 1 & M % N != 0) {
            len = M - (N - 1) * (M / N);
            strncat(slice, ptr, len);
        } else {
            len = M / N;
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

    char chc = 'a';
    char chfname[5];
    u_long pid_arr[N];
    HANDLE proc_arr[N];
    HANDLE thread_arr[N];
    for (int i = 0; i < N; i++) {
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        GetStartupInfo(&si);
        chfname[0] = chc;
        strcat(chfname, ".txt ");
        char source[100] = "subproc.exe ";
        strcat(source, chfname);

        BOOL res = CreateProcess(
                "subproc.exe",
                source,
                NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

        if (!res) {
            char *errMsg = strerror(errno);
            printf("Error occurred: %s\n", errMsg);
            return 1;
        }
        chfname[1] = '\0';
        chc++;
        pid_arr[i] = pi.dwProcessId;
        proc_arr[i] = pi.hProcess;
        thread_arr[i] = pi.hThread;
    }

    int result = 0;
    DWORD excode;

    for (int i = 0; i < N; i++) {
        DWORD dwRes = WaitForSingleObject(proc_arr[i], INFINITE);

        if (dwRes == WAIT_FAILED) {
            printf("Error %lx\n", GetLastError());
            return 1;
        }
        // CALCULATING RESULT
        char filen[50];
        char fres[S];

        sprintf(filen, "%lu", pid_arr[i]);
        strcat(filen, ".txt");
        FILE *resfile;
        resfile = fopen(filen, "r");
        if (resfile == NULL) {
            char *errMsg = strerror(errno);
            printf("Error occurred: %s\n", errMsg);
            return -1;
        }
        while (fgets(fres, S, resfile) != NULL);

        result += atoi(fres);
        printf("Child process exited with pid %lu with code %d\n", pid_arr[i],
               GetExitCodeProcess(proc_arr[i], &excode));
        fclose(resfile);
        CloseHandle(proc_arr[i]);
        CloseHandle(thread_arr[i]);
    }

    printf("Result is %d\n", result);

    return 0;
}

//--------------------------------------------------------------subproc.c

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define S 100

int main(int argc, char *argv[]) {

    Sleep(30000);
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
        if (isdigit(arr[i])) result++;
    }


    //CREATING AND WRITING FILE
    char filename[50];
    sprintf(filename, "%lu", GetCurrentProcessId());
    strcat(filename, ".txt");
    FILE *nfile = fopen(filename, "w+");
    fprintf(nfile, "%d", result);
    fclose(nfile);
    ExitProcess(1);

}
