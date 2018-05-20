#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , webInterface(new WebInterface(this))
{
    ui->setupUi(this);
    ui->gridLayout->addWidget(webInterface);
}

MainWindow::~MainWindow()
{
    delete webInterface;
    delete ui;
}
