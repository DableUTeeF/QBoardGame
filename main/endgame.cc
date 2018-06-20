#include "endgame.h"
#include "ui_endgame.h"

endgame::endgame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::endgame)
{
    ui->setupUi(this);
}

void endgame::setState(int black, int white){
    if(black > white){
        char txt[100];
        snprintf(txt, sizeof(txt), "Black won with score %d/%d", black, white);
        ui->label_2->setText(txt);
    }
    else if(white > black){
        char txt[100];
        snprintf(txt, sizeof(txt), "White won with score %d/%d", white, black);
        ui->label_2->setText(txt);
    }
    else {
        ui->label_2->setText("Draw");
    }
    update();
}

endgame::~endgame()
{
    delete ui;
}
