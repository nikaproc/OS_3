#include "second.h"
#include "ui_second.h"
#include "mainwindow.h"
#include <QProcess>
#include <windows.h>
#include <tlhelp32.h>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <string>
#include <Windows.h>
#include <psapi.h>  // Для використання функції GetPriorityClass
#include <tchar.h>  // Для використання функції _tprintf
#include <QInputDialog>

using namespace std;

second::second(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::second)
{
    ui->setupUi(this);
}

second::~second()
{
    delete ui;
}

void second::on_returnButt_clicked()
{
    hide();
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
}

int second::getProcessPriority(int pid) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    int priority = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return priority;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (pe32.th32ProcessID == pid) {
                priority = GetPriorityClass(OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid));
                break;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return priority;
}

void second::GetProcessInformation(DWORD processId, LPWSTR nameBuffer, DWORD nameBufferSize, FILETIME* createTime, FILETIME* exitTime, FILETIME* kernelTime, FILETIME* userTime, QString* priorityClass)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess != NULL)
    {
        // Отримуємо ім'я процесу
        GetProcessImageFileName(hProcess, nameBuffer, nameBufferSize);

        // Отримуємо часи процесу
        GetProcessTimes(hProcess, createTime, exitTime, kernelTime, userTime);

        // Отримуємо пріоритет процесу
        DWORD getPrior = GetPriorityClass(hProcess);
        switch(getPrior)
        {
        case REALTIME_PRIORITY_CLASS:
            *priorityClass = "Реального часу"; break;
        case HIGH_PRIORITY_CLASS:
            *priorityClass = "Високий"; break;
        case ABOVE_NORMAL_PRIORITY_CLASS:
            *priorityClass = "Вище середнього"; break;
        case NORMAL_PRIORITY_CLASS:
            *priorityClass = "Звичайний"; break;
        case BELOW_NORMAL_PRIORITY_CLASS:
            *priorityClass = "Нижче середнього"; break;
        case IDLE_PRIORITY_CLASS:
            *priorityClass = "Низький"; break;
        }
    }
    CloseHandle(hProcess);
}

void second::getProcessState(PROCESS_INFORMATION processInfo, QString *state)
{
    DWORD exitCode;
    if (GetExitCodeProcess(processInfo.hProcess, &exitCode))
    {
        if (exitCode == STILL_ACTIVE)
        {
            *state = "Виконується";
        }
        else
        {
            *state = "Призупинено";
        }
    }
    else
    {
        *state = "Завершено";
    }
}

void second::on_addProc_clicked()
{

    wstring programPath = L"L:\\University\\2_year\\3_term\\lab3_os\\fibonacci\\x64\\Debug\\fibonacci.exe";
    PROCESS_INFORMATION processInfo;
    STARTUPINFO startUpInfo;
    ZeroMemory(&startUpInfo, sizeof(STARTUPINFO));
    startUpInfo.cb = sizeof(STARTUPINFO);

    DWORD startTime = GetTickCount(); // Отримуємо початковий час

    if (CreateProcess(
        nullptr,
        (LPWSTR)programPath.c_str(),
        nullptr, nullptr, false,
        CREATE_NEW_CONSOLE,
        nullptr, nullptr,
        &startUpInfo, &processInfo))
    {
        QString state;
        getProcessState(processInfo, &state);
        QTableWidgetItem* stateItem = new QTableWidgetItem(state);

        WCHAR nameBuffer[MAX_PATH];
        FILETIME createTime, exitTime, kernelTime, userTime;
        QString priorityClass;
          GetProcessInformation(processInfo.dwProcessId, nameBuffer, MAX_PATH, &createTime, &exitTime, &kernelTime, &userTime, &priorityClass);
        // Додаємо інформацію до QTableWidget
        QTableWidgetItem* pidItem = new QTableWidgetItem(QString::number(processInfo.dwProcessId));
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromWCharArray(nameBuffer));
        QTableWidgetItem* priorityItem = new QTableWidgetItem(priorityClass);

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, pidItem); // Додаємо PID
        ui->tableWidget->setItem(row, 1, nameItem); // Додаємо ім'я
        ui->tableWidget->setItem(row, 2, stateItem); // Додаємо стан
        ui->tableWidget->setItem(row, 3, priorityItem); // Додаємо пріоритет

        // WaitForSingleObject(processInfo.hProcess, INFINITE); // Чекаємо завершення процесу
         DWORD endTime = GetTickCount(); // Отримуємо час після завершення процесу
         DWORD executionTime = endTime - startTime; // Розраховуємо час виконання в мілісекундах
         QTableWidgetItem* executionTimeItem = new QTableWidgetItem(QString::number(executionTime) + " ms"); // Додаємо час виконання
         ui->tableWidget->setItem(row, 4, executionTimeItem); //час виконання
         CloseHandle(processInfo.hProcess);
         CloseHandle(processInfo.hThread);
    }
    else
    {
        QMessageBox::critical(this, "Error", "Process was not created");
    }

}

void second::on_addProcProt_clicked()
{
    wstring programPath = L"U:\\Proteus 8 Professional\\BIN\\PDS.EXE";
    PROCESS_INFORMATION processInfo;

    STARTUPINFO startUpInfo;
    ZeroMemory(&startUpInfo, sizeof(STARTUPINFO));
    startUpInfo.cb = sizeof(STARTUPINFO);

    DWORD startTime = GetTickCount(); // Отримуємо початковий час

    if (CreateProcess(
        nullptr,
        (LPWSTR)programPath.c_str(),
        nullptr, nullptr, false,
        HIGH_PRIORITY_CLASS|CREATE_SUSPENDED,
        nullptr, nullptr,
        &startUpInfo, &processInfo))
    {
        //Sleep(5000);
        ResumeProcess(processInfo.dwProcessId);
        QString state;
        getProcessState(processInfo, &state);
        QTableWidgetItem* stateItem = new QTableWidgetItem(state);

        WCHAR nameBuffer[MAX_PATH];
        FILETIME createTime, exitTime, kernelTime, userTime;
        QString priorityClass;
          GetProcessInformation(processInfo.dwProcessId, nameBuffer, MAX_PATH, &createTime, &exitTime, &kernelTime, &userTime, &priorityClass);
        // Додаємо інформацію до QTableWidget
        QTableWidgetItem* pidItem = new QTableWidgetItem(QString::number(processInfo.dwProcessId));
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromWCharArray(nameBuffer));
        QTableWidgetItem* priorityItem = new QTableWidgetItem(priorityClass);

        int row = ui->tableWidget->rowCount();

        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, pidItem); // Додаємо PID
        ui->tableWidget->setItem(row, 1, nameItem); // Додаємо ім'я
        ui->tableWidget->setItem(row, 2, stateItem); // Додаємо стан
        ui->tableWidget->setItem(row, 3, priorityItem); // Додаємо пріоритет

        DWORD endTime = GetTickCount(); // Отримуємо час після завершення процесу
        DWORD executionTime = endTime - startTime; // Розраховуємо час виконання в мілісекундах
        QTableWidgetItem* executionTimeItem = new QTableWidgetItem(QString::number(executionTime) + " ms"); // Додаємо час виконання
        ui->tableWidget->setItem(row, 4, executionTimeItem); //час виконання
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }
    else
        QMessageBox::critical(this, "Error", "Process was not created");
}

void second::on_addProcTracert_clicked()
{
    wstring command = L"tracert google.com";
    PROCESS_INFORMATION processInfo;
    STARTUPINFO startUpInfo;
    ZeroMemory(&startUpInfo, sizeof(STARTUPINFO));
    startUpInfo.cb = sizeof(STARTUPINFO);

    DWORD startTime = GetTickCount(); // Отримуємо початковий час

    if (CreateProcess(
        nullptr,
        const_cast<LPWSTR>(command.c_str()),
        nullptr, nullptr, false,
        CREATE_NEW_CONSOLE,
        nullptr, nullptr,
        &startUpInfo, &processInfo))
    {
        WCHAR nameBuffer[MAX_PATH];
        FILETIME createTime, exitTime, kernelTime, userTime;
        QString priorityClass;

        QString state;
        getProcessState(processInfo, &state);
        QTableWidgetItem* stateItem = new QTableWidgetItem(state);

          GetProcessInformation(processInfo.dwProcessId, nameBuffer, MAX_PATH, &createTime, &exitTime, &kernelTime, &userTime, &priorityClass);
        // Додаємо інформацію до QTableWidget
        QTableWidgetItem* pidItem = new QTableWidgetItem(QString::number(processInfo.dwProcessId));
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromWCharArray(nameBuffer));
        QTableWidgetItem* priorityItem = new QTableWidgetItem(priorityClass);


        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, pidItem); //PID
        ui->tableWidget->setItem(row, 1, nameItem); //ім'я
        ui->tableWidget->setItem(row, 2, stateItem); //стан
        ui->tableWidget->setItem(row, 3, priorityItem); //пріоритет

        // WaitForSingleObject(processInfo.hProcess, INFINITE); // Чекаємо завершення процесу
         DWORD endTime = GetTickCount(); // Отримуємо час після завершення процесу
         DWORD executionTime = endTime - startTime; // Розраховуємо час виконання в мілісекундах
         QTableWidgetItem* executionTimeItem = new QTableWidgetItem(QString::number(executionTime) + " ms"); // Додаємо час виконання
         ui->tableWidget->setItem(row, 4, executionTimeItem); //час виконання
         CloseHandle(processInfo.hProcess);
         CloseHandle(processInfo.hThread);
    }
    else {
        QMessageBox::critical(this, "Error", "Process was not created");
    }
}

void second::on_addTabProc_clicked()
{
    wstring command = L"\"L:\\University\\2_year\\3_term\\lab3_os\\tab\\x64\\Debug\\tab.exe\" " +
            ui->aBound->text().toStdWString() + L" " + ui->bBound->text().toStdWString() + L" " + ui->step->text().toStdWString();
    PROCESS_INFORMATION processInfo;
    STARTUPINFO startUpInfo;
    ZeroMemory(&startUpInfo, sizeof(STARTUPINFO));
    startUpInfo.cb = sizeof(STARTUPINFO);

    DWORD startTime = GetTickCount(); // Отримуємо початковий час

    if (CreateProcess(
        nullptr,
        const_cast<LPWSTR>(command.c_str()),
        nullptr, nullptr, false,
        CREATE_NEW_CONSOLE,
        nullptr, nullptr,
        &startUpInfo, &processInfo))
    {
        WCHAR nameBuffer[MAX_PATH];
        QString priorityClass;
        FILETIME createTime, exitTime, kernelTime, userTime;

        QString state;
        getProcessState(processInfo, &state);
        QTableWidgetItem* stateItem = new QTableWidgetItem(state);

          GetProcessInformation(processInfo.dwProcessId, nameBuffer, MAX_PATH, &createTime, &exitTime, &kernelTime, &userTime, &priorityClass);
        // Додаємо інформацію до QTableWidget
        QTableWidgetItem* pidItem = new QTableWidgetItem(QString::number(processInfo.dwProcessId));
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromWCharArray(nameBuffer));
        QTableWidgetItem* priorityItem = new QTableWidgetItem(priorityClass);


        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, pidItem); //PID
        ui->tableWidget->setItem(row, 1, nameItem); //ім'я
        ui->tableWidget->setItem(row, 2, stateItem); //стан
        ui->tableWidget->setItem(row, 3, priorityItem); //пріоритет

        // WaitForSingleObject(processInfo.hProcess, INFINITE); // Чекаємо завершення процесу
         DWORD endTime = GetTickCount(); // Отримуємо час після завершення процесу
         DWORD executionTime = endTime - startTime; // Розраховуємо час виконання в мілісекундах
         QTableWidgetItem* executionTimeItem = new QTableWidgetItem(QString::number(executionTime) + " ms"); // Додаємо час виконання
         ui->tableWidget->setItem(row, 4, executionTimeItem); //час виконання
         CloseHandle(processInfo.hProcess);
         CloseHandle(processInfo.hThread);
    }
    else {
        QMessageBox::critical(this, "Error", "Process was not created");
    }
}

void second::on_terminateButt_clicked() //Завершення процесу
{
    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0)
    {
        // доступ до рядка за допомогою вибраного рядка
        QTableWidgetItem* pidItem = ui->tableWidget->item(selectedRow, 0);
        QTableWidgetItem* stateItem = ui->tableWidget->item(selectedRow, 2);

        DWORD pid = pidItem->text().toUInt();
        PROCESS_INFORMATION processInfo;
        processInfo.hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

        if (processInfo.hProcess != NULL)
        {
            if (TerminateProcess(processInfo.hProcess, 0))
            {
                QString newState;
                getProcessState(processInfo, &newState);
                stateItem->setText(newState);
            }
            else
            {
                qDebug() << "Помилка при завершенні процесу: " << GetLastError();
            }

            CloseHandle(processInfo.hProcess);
        }
        else
        {
            qDebug() << "Помилка при отриманні доступу до процесу: " << GetLastError();
        }
    }

}

void second::SuspendProcess(DWORD pid)//Призупинення процесу
{
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);

        if (Thread32First(hThreadSnap, &te32))
        {
            do
            {
                if (te32.th32OwnerProcessID == pid)
                {
                    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                    if (hThread != NULL)
                    {
                        SuspendThread(hThread);
                        CloseHandle(hThread);
                    }
                }
            } while (Thread32Next(hThreadSnap, &te32));
        }
        CloseHandle(hThreadSnap);
    }
}

void second::on_suspendButt_clicked()
{
    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0)
    {
        QTableWidgetItem* pidItem = ui->tableWidget->item(selectedRow, 0);
        DWORD pid = pidItem->text().toUInt();
        SuspendProcess(pid);
        QTableWidgetItem* stateItem = ui->tableWidget->item(selectedRow, 2);
        stateItem->setText("Призупинено");
    }
}

void second::ResumeProcess(DWORD pid)//Відновлення процесу
{
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);

        if (Thread32First(hThreadSnap, &te32))
        {
            do
            {
                if (te32.th32OwnerProcessID == pid)
                {
                    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                    if (hThread != NULL)
                    {
                        ResumeThread(hThread);
                        CloseHandle(hThread);
                    }
                }
            } while (Thread32Next(hThreadSnap, &te32));
        }
        CloseHandle(hThreadSnap);
    }
}

void second::on_resumeButt_clicked()
{
    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0)
    {
        QTableWidgetItem* pidItem = ui->tableWidget->item(selectedRow, 0);
        DWORD pid = pidItem->text().toUInt();
        ResumeProcess(pid);
        QTableWidgetItem* stateItem = ui->tableWidget->item(selectedRow, 2);
        stateItem->setText("Виконується");
    }
}


void second::changeProcessPriority(int pid, int priority) //Зміна пріоритету
{
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (processHandle != NULL)
    {
        SetPriorityClass(processHandle, priority);
        CloseHandle(processHandle);
    }
}


void second::on_priorButt_clicked()
{
    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0)
    {
        QTableWidgetItem* pidItem = ui->tableWidget->item(selectedRow, 0);
        DWORD pid = pidItem->text().toUInt();

        int newPriority = ui->comboBoxPrior->currentIndex();

        QTableWidgetItem* priorityItem = ui->tableWidget->item(selectedRow, 3);
        QString priorityClass;
        switch (newPriority)
        {
        case 0:
        {
            newPriority = REALTIME_PRIORITY_CLASS;
            priorityClass = "Реального часу"; break;}
        case 1:
        {
            newPriority = HIGH_PRIORITY_CLASS;
            priorityClass = "Високий"; break;}
        case 2:
        {
            newPriority = ABOVE_NORMAL_PRIORITY_CLASS;
            priorityClass = "Вище середнього"; break;}
        case 3:
        {
            newPriority = NORMAL_PRIORITY_CLASS;
            priorityClass = "Звичайний"; break;}
        case 4:
        {
            newPriority = BELOW_NORMAL_PRIORITY_CLASS;
            priorityClass = "Нижче середнього"; break;
        }
        case 5:
        {
            newPriority = IDLE_PRIORITY_CLASS;
            priorityClass = "Низький"; break;}
        }

        changeProcessPriority(pid, newPriority);
        priorityItem->setText(priorityClass);
    }
}


