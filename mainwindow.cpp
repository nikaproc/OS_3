#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    hide();
    fstwind = new first(this);
    fstwind->show();
}


void MainWindow::on_pushButton_2_clicked()
{
    //hide();
    secondwind = new second(this);
    secondwind->show();
}

