#include "first.h"
#include "ui_first.h"
#include "mainwindow.h"
#include <windows.h>
#include <chrono>
#include <string>
#include <cfloat>
using namespace std;

first::first(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::first)
{
    ui->setupUi(this);
}

first::~first()
{
    delete ui;
}

void first::on_returnButt_clicked()
{
    hide();
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
}


void first::on_pushButton_clicked()
{
    info.clear();
    int start = ui->aBound->text().toInt();
    int end = ui->bBound->text().toInt();
    int step = ui->step->text().toInt();
    int numProcess = ui->prN->text().toInt();

    double minTime = DBL_MAX; // Мінімальний час виконання
    int optimalNumProcess; // Оптимальна кількість процесів

    for (int i = 1; i <= numProcess; ++i)
    {
        auto start_time = chrono::high_resolution_clock::now();

        // Створення дочірніх процесів
        for (int j = 0; j < i; ++j)
        {
            int startRange = start + (end - start) * j / i;
            int endRange = start + (end - start) * (j + 1) / i - 1;
            string command = (i == 0) ? "L:\\University\\2_year\\3_term\\lab3_os\\tab\\x64\\Debug\\tab.exe" :
                (string("start /b ") +
                    (string() + "L:\\University\\2_year\\3_term\\lab3_os\\tab\\x64\\Debug\\tab.exe " + to_string(startRange) +
                        " " + to_string(endRange) + " " + to_string(step)));
            system(command.c_str());
        }

        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        info += "Execution time for " + QString::number(i) + " processes: " + QString::number(duration) + " milliseconds\n";

        // Оновлення оптимальної кількості процесів, якщо час виконання є меншим
        if (duration < minTime)
        {
            minTime = duration;
            optimalNumProcess = i;
        }
        double speedup = static_cast<double>(duration) / minTime;
        info += "Speedup for " + QString::number(i) + " processes: " + QString::number(speedup, 'f', 2) + "\n";
    }
    info += "Optimal number of processes: " + QString::number(optimalNumProcess);
    ui->timeInfo->setText(info);
}

