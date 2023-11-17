﻿#include <windows.h>
#include <string>
#include <iostream>
#include "tchar.h"
#include <fstream>
#include <semaphore>

CRITICAL_SECTION FileLockingCriticalSection;

const int DEPOSIT_AMOUNT = 230;
const int WITHDRAW_AMOUNT = 1000;

int ReadFromFile()
{
    std::fstream myfile("balance.txt", std::ios_base::in);
    int result;
    myfile >> result;
    myfile.close();
    return result;
}

void WriteToFile(int data)
{
    std::fstream myfile("balance.txt", std::ios_base::out);
    myfile << data << std::endl;
    myfile.close();
}

int GetBalance() {
    int balance = ReadFromFile();
    return balance;
}

void Deposit(int money) {
    int balance = GetBalance();
    balance += money;

    WriteToFile(balance);
    printf("Balance after deposit: %d\n", balance);
}

void Withdraw(int money) {
    if (GetBalance() < money) {
        printf("Cannot withdraw money, balance lower than %d\n", money);
        return;
    }

    Sleep(20);
    int balance = GetBalance();
    balance -= money;
    WriteToFile(balance);

    printf("Balance after withdraw: %d\n", balance);
}

DWORD WINAPI DoDeposit(CONST LPVOID lpParameter)
{
    EnterCriticalSection(&FileLockingCriticalSection);
    Deposit((int)lpParameter);
    LeaveCriticalSection(&FileLockingCriticalSection);
    ExitThread(0);
}

DWORD WINAPI DoWithdraw(CONST LPVOID lpParameter)
{
    EnterCriticalSection(&FileLockingCriticalSection);
    Withdraw((int)lpParameter);
    LeaveCriticalSection(&FileLockingCriticalSection);
    ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[])
{
    HANDLE* handles = new HANDLE[50];

    InitializeCriticalSectionAndSpinCount(&FileLockingCriticalSection, 1);

    WriteToFile(0);

    SetProcessAffinityMask(GetCurrentProcess(), 1);
    for (int i = 0; i < 50; i++) {
        handles[i] = (i % 2 == 0)
            ? CreateThread(NULL, 0, &DoDeposit, (LPVOID)DEPOSIT_AMOUNT, CREATE_SUSPENDED, NULL)
            : CreateThread(NULL, 0, &DoWithdraw, (LPVOID)WITHDRAW_AMOUNT, CREATE_SUSPENDED, NULL);
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(50, handles, true, INFINITE);
    printf("Final Balance: %d\n", GetBalance());


    delete[] handles;

    DeleteCriticalSection(&FileLockingCriticalSection);

    getchar();

    return 0;
}