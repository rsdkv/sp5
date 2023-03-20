#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32

#include <windows.h>

#else
#include <unistd.h>
#include <sys/wait.h>
#endif


int count_digits(char *str, int start, int end) {
    int count = 0;
    for (int i = start; i < end; i++) {
        if (isdigit(str[i])) {
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    // Проверка количества аргументов командной строки
    if (argc < 3) {
        printf("Usage: %s filename n\n", argv[0]);
        return 1;
    }
    // Открытие файла для чтения
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error: cannot open file '%s'\n", argv[1]);
        return 1;
    }
    // Чтение содержимого файла в буфер
    char buffer[1024];
    size_t size = fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
    buffer[size] = '\0';
    // Закрытие файла
    fclose(fp);
    // Проверка, что содержимое файла достаточно длинное
    if (size < 2) {
        printf("Error: file '%s' contains less than 2 characters\n", argv[1]);
        return 1;
    }
    // Получение количества дочерних процессов
    int n = atoi(argv[2]);
    // Расчет длины каждой части строки
    int part_size = size / n;
    // Запуск дочерних процессов
    int sum = 0;
    for (int i = 0; i < n; i++) {
        int start = i * part_size;
        int end = (i == n - 1) ? size : (i + 1) * part_size;

#ifdef _WIN32
        // Для Windows используем функцию CreateProcess
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        char cmd_line[256];
        sprintf(cmd_line, "%s %d %d %d", argv[0], start, end, i);
        if (!CreateProcess(NULL, cmd_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            printf("Error: cannot create process\n");
            return 1;
        }
        // Ожидание завершения дочернего процесса
        WaitForSingleObject(pi.hProcess, INFINITE
