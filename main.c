#ifndef _WIN32
#include <sys/wait.h>
#else

#include <windows.h>

#endif

#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>
#include <memory>
#include <numeric>

using std::cout;
using std::endl;

void print_usage() {
    cout << "Usage:\n\t./linux.bin N path\nwhere:\n";
    cout << "\tN - int digit, number of threads to use\n";
    cout << "\tpath - path to file with data\n";
}

int file_operation(char *file, int mode,
                   std::shared_ptr <std::vector<float>> digits) {
    switch (mode) {
        case 1: // read file and push nums to array
        {
            std::ifstream input(file);
            if (!input) {
                cout << "No such file: " << file << endl;
                return 1;
            }
            float ch;
            while (input >> ch)
                digits->push_back(ch);
            input.close();
            break;
        }
        case 2: // move array nums to file
        {
            std::ofstream out(file);
            if (!out) {
                cout << "Can't open file as ofstream" << file << endl;
                return 1;
            }
            for (int i = 0; i < digits->size(); ++i) {
                out << digits->at(i) << " ";
            }
            out.close();
            break;
        }
        default: {
            cout << "No such mode: " << mode << endl;
            return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (strcmp("subproc", argv[0]) == 0) {
//sleep(100);
        std::shared_ptr <std::vector<float>> arr =
                std::make_shared<std::vector<float>>(std::vector<float>());
        if (file_operation((char *) ("./input/" +
                                     std::string(argv[1])).c_str(), 1, arr)) {
            cout << "Error in read file_operation in subproc " << argv[1]
                 << endl;
            return 1;
        }
        float sum = 0;
        for (int i = 0; i < arr->size(); ++i) {
            sum += arr->at(i) * arr->at(i);
        }
        arr->clear();
        arr->push_back(sum);
        if (file_operation((char *) ("./output/" +
                                     std::string(argv[1])).c_str(), 2, arr)) {
            cout << "Error in write file_operation in subproc " << argv[1]
                 << endl;
            return 1;
        }
        return 0;
    }
    if (argc != 3) {
        print_usage();
        return 0;
    }
    int N = 0;
    try
    {
        N = std::stoi(argv[1]);
    }
    catch(...)
    {
        print_usage();
        return 0;
    }
    if (N < 1) {
        print_usage();
        cout << "N must be >= 1" << endl;
        return 0;
    }
    std::shared_ptr <std::vector<float>> digits =
            std::make_shared<std::vector<float>>(std::vector<float>());
    if (file_operation(argv[2], 1, digits)) {
        cout << "Error in file_operation function: " << endl;
    }
    int M = digits->size();
    cout << "M = " << M << endl;
    if (M < 2) {
        cout << "Too small digit given: " << M << endl;
        return 0;
    }
    if (N > M) {
        cout << "Too much threads requested!" << endl;
        N = M / 2;
    }
    int digitsPerProcess = M / N;
    int digitsForLastProcess = M - digitsPerProcess * (N - 1);
    int index = 0;
// i-ый вектор - переметр для i-ого потока
    std::vector <std::vector<float>> arguments;
    for (int i = 0; i < N - 1; i++) {
        arguments.push_back(std::vector<float>());
        for (int d = 0; d < digitsPerProcess; d++) {
            arguments[i].push_back(digits->at(index));
            index += 1;
        }
    }
    arguments.push_back(std::vector<float>());
    for (int d = 0; d < digitsForLastProcess; d++) {
        arguments[N - 1].push_back(digits->at(index));
        index += 1;
    }
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < arguments[i].size(); ++j) {
//std::cout << arguments[i][j] << '\t';
        }
        file_operation((char *) ("./input/" + std::to_string(i)).c_str(),
                       2, std::make_shared<std::vector<float>>(arguments[i]));
//cout << endl;
    }
#ifndef _WIN32
    for (int i = 0; i < N; ++i){
    pid_t res = fork();
    switch (res)
        {
        case 0:{
            char *args[3] = {"subproc", (char *)std::to_string(i).c_str(),
            NULL};
            execve("./main", args, NULL);
            }
        case -1:{
            printf("Error %d\n", errno);
            return 1;
            }
        }
    }
    for (int i = 0; i < N; ++i){
        int code = 0;
        pid_t child = wait(&code);
        if (child == -1){
            printf("Error %d\n", errno);
            return 1;
        }
        printf("Process with pid %d exited with code %d\n", child,
        WEXITSTATUS(code));
    }
#else
    HANDLE v[N];
    for (int i = 0; i < N; ++i) {
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        GetStartupInfo(&si);
        std::string f = "subproc.exe subproc ";
        char *s = (char *) (f + std::to_string(i)).c_str();
//cout << s;
        BOOL res = CreateProcess("main.exe",
                                 (char *) ((std::string) "subproc " +
                                           std::to_string(i)).c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
        if (!res) {
            printf("Error %x\n", GetLastError());
        }
//printf("%d\n", pi.dwProcessId);
        v[i] = pi.hProcess;
    }
// DWORD dwRes = WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD dwRes = WaitForMultipleObjects(N, v, TRUE, INFINITE);
    if (dwRes == WAIT_FAILED) {
        printf("Error %x\n", GetLastError());
        return 1;
    }
//printf("Child process exited \n");
#endif
    digits->clear();
    for (int i = 0; i < N; ++i) {
        file_operation((char *) ("./output/" + std::to_string(i)).c_str(),
                       1, digits);
    }
    float s = 0.0;
    for (int i = 0; i < digits->size(); ++i) {
        s += digits->at(i);
    }
    cout << "The answer is " << s;
    return 0;
}