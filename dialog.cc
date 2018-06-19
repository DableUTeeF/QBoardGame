#include "dialog.h"
#include "ui_dialog.h"
#include "main/mainwindow.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(released()), this, SLOT(startOthello()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::startOthello()
{
    MainWindow *othello = new MainWindow();
    othello->show();
    this->hide();
}
