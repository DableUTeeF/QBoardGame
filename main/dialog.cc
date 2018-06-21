#include "dialog.h"
#include "ui_dialog.h"
#include "main/mainwindow.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->startOthelloButton, SIGNAL(released()), this, SLOT(startOthello()));
    connect(ui->startChessButton, SIGNAL(released()), this, SLOT(startChess()));

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::startOthello()
{
    MainWindow *othello = new MainWindow(0, this);
    othello->show();
    this->hide();
}

void Dialog::startChess()
{
    MainWindow *chess = new MainWindow(1, this);
    chess->show();
    this->hide();
}
