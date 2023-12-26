#ifndef SECOND_H
#define SECOND_H

#include <QWidget>
#include <windows.h>
#include <psapi.h>  // Для використання функції GetPriorityClass
#include <tchar.h>  // Для використання функції _tprintf

namespace Ui {
class second;
}

class second : public QWidget
{
    Q_OBJECT

public:
    explicit second(QWidget *parent = nullptr);
    ~second();

private slots:
    void on_returnButt_clicked();

    void on_addProc_clicked();

    void on_addProcProt_clicked();

    void on_addProcTracert_clicked();

    void on_addTabProc_clicked();

    void on_terminateButt_clicked();

    void on_suspendButt_clicked();

    void on_resumeButt_clicked();

    void on_priorButt_clicked();

private:
    Ui::second *ui;
    int getProcessPriority(int pid);
    void GetProcessInformation(DWORD processId, LPWSTR nameBuffer, DWORD nameBufferSize, FILETIME* createTime, FILETIME* exitTime, FILETIME* kernelTime, FILETIME* userTime, QString* priorityClass);

    void getProcessState(PROCESS_INFORMATION processInfo, QString* state);

    void changeProcessPriority(int pid, int priority);
    void suspendProcess(int pid);
    void resumeProcess(int pid);
    void terminateProcess(int pid);

    void SuspendProcess(DWORD pid);
    void ResumeProcess(DWORD pid);
};




#endif // SECOND_H
