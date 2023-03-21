#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define S 100

int main(int argc, char *argv[]) {
    if (argc != 3) { // если количество аргументов командной строки не равно 3
        char *errMsg = strerror(errno); // строка, содержащая сообщение об ошибке
        printf("Error occurred: the number of variables should be strongly 2 - file_name and N (%s)\n", errMsg); // вывод сообщения об ошибке на экран
        return -1; // возврат -1, чтобы программа закончилась с ошибкой
    }
    // ЧТЕНИЕ ФАЙЛА
    FILE *file;
    char arr[S]; // буфер для хранения содержимого файла
    char *ptr = arr; // указатель на начало буфера
    file = fopen(argv[1], "r"); // открытие файла для чтения
    if (file == NULL) { // если произошла ошибка при открытии файла
        char *errMsg = strerror(errno); // строка, содержащая сообщение об ошибке
        printf("Error occurred: %s\n", errMsg); // вывод сообщения об ошибке на экран
        return -1; // возврат -1, чтобы программа закончилась с ошибкой
    }
    fgets(arr, S, file); // чтение содержимого файла в буфер
    fclose(file); // закрытие файла

    if (strlen(arr) < 2) { // если длина содержимого файла меньше 2 символов
        char *errMsg = strerror(errno); // строка, содержащая сообщение об ошибке
        printf("Error occurred: the number of symbols is less than 2 in the file (%s)\n", errMsg); // вывод сообщения об ошибке на экран
        return -1; // возврат -1, чтобы программа закончилась с ошибкой
    } else printf("File consists: %s\n", arr); // иначе вывод содержимого файла на экран


    int N = atoi(argv[2]); // конвертирование второго аргумента командной строки в целое число
    int M = strlen(arr); // вычисление длины содержимого файла
    if (N > M / 2) { // если количество дочерних процессов больше, чем половина длины файла
        N = M / 2; // установка количества дочерних процессов в половину длины файла
        printf("Warning: children processes have been reduced to %d\n", N); // вывод предупреждения на экран
    }
    // СОЗДАНИЕ N ФАЙЛОВ
    char c = 'a'; // символ для формирования имени файла
    char filename[5]; // массив для хранения имени файла
    char slice[M]; // буфер для хранения части содержимого файла
    int len; // длина части содержимого файла
    for (size_t i = 0; i < N; i++) { // цикл по количеству файлов, которые нужно создать
        slice[0] = 0; // обнуление слайса (часть файла)
        if (i == N - 1 & M % N != 0) { // если это последний файл и M не делится нацело на N
            len = M - (N - 1) * (M / N); // вычисление длины последней части файла
            strncat(slice, ptr, len); // добавление последней части в слайс
        } else { // если это не последний файл
            len = M / N; // вычисление длины части файла
            strncat(slice, ptr, len); // добавление части файла в слайс
            ptr = ptr + len; // сдвиг указателя на следующую часть файла
        }

        filename[0] = c; // задание имени файла, первый символ - латинская буква в алфавитном порядке
        strcat(filename, ".txt"); // добавление расширения к имени файла
        FILE *nfile = fopen(filename, "w+"); // открытие файла на запись
        filename[1] = '\0'; // удаление первого символа из имени файла
        c++; // увеличение латинской буквы в имени файла на следующую

        // ЗАПИСЬ N ФАЙЛОВ
        fwrite(slice, sizeof(char), len, nfile); // запись слайса в файл
        slice[1] = '\0'; // удаление первого символа из слайса
        fclose(nfile); // закрытие файла
    }

    char chc = 'a'; // задание начальной латинской буквы имени файла
    char chfname[5]; // объявление имени файла
    u_long pid_arr[N]; // объявление массива идентификаторов процессов
    HANDLE proc_arr[N]; // объявление массива процессов
    HANDLE thread_arr[N]; // объявление массива потоков
    for (int i = 0; i < N; i++) { // цикл по количеству процессов
        PROCESS_INFORMATION pi; // структура для информации о процессе
        STARTUPINFO si; // структура для информации о запуске процесса
        GetStartupInfo(&si); // заполнение структуры si
        chfname[0] = chc; // задание имени файла
        strcat(chfname, ".txt "); // добавление расширения к имени файла
        char source[100] = "subproc.exe "; // объявление командной строки для запуска подпроцесса
        strcat(source, chfname); // добавление имени файла к командной строке

        // Создание процесса с помощью функции CreateProcess
        BOOL res = CreateProcess(
                "subproc.exe", // Имя исполняемого файла подпроцесса
                source, // Аргументы командной строки для подпроцесса
                NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
        // Проверка успешности создания процесса
        if (!res) {
            char *errMsg = strerror(errno); // Получение сообщения об ошибке
            printf("Error occurred: %s\n", errMsg);
            return 1; // Возврат ошибки
        }
        chfname[1] = '\0'; // Изменение имени файла
        chc++; // Увеличение счетчика процессов
        pid_arr[i] = pi.dwProcessId; // Сохранение идентификатора процесса
        proc_arr[i] = pi.hProcess; // Сохранение дескриптора процесса
        thread_arr[i] = pi.hThread; // Сохранение дескриптора потока

// Ожидание завершения всех созданных процессов
        int result = 0; // Инициализация результата
        DWORD excode; // Инициализация кода завершения процесса

        for (int i = 0; i < N; i++) {
            DWORD dwRes = WaitForSingleObject(proc_arr[i], INFINITE); // Ожидание завершения процесса

            if (dwRes == WAIT_FAILED) { // Проверка ошибок
                printf("Error %lx\n", GetLastError());
                return 1;
            }
            // Вычисление результата выполнения процесса
            char filen[50];
            char fres[S];

            sprintf(filen, "%lu", pid_arr[i]); // Генерация имени файла результата
            strcat(filen, ".txt"); // Добавление расширения
            FILE *resfile;
            resfile = fopen(filen, "r"); // Открытие файла с результатом
            if (resfile == NULL) { // Проверка на ошибку
                char *errMsg = strerror(errno);
                printf("Error occurred: %s\n", errMsg);
                return -1;
            }
            while (fgets(fres, S, resfile) != NULL); // Чтение результата из файла

            result += atoi(fres); // Преобразование и добавление к результату
            printf("Child process exited with pid %lu with code %d\n", pid_arr[i],
                   GetExitCodeProcess(proc_arr[i], &excode)); // Вывод информации о завершении процесса
            fclose(resfile); // Закрытие файла
            CloseHandle(proc_arr[i]); // Освобождение ресурсов процесса
            CloseHandle(thread_arr[i]); // Освобождение ресурсов потока
        }

        printf("Result is %d\n", result); // Вывод результата

        return 0; // Возврат успешного завершения программы
