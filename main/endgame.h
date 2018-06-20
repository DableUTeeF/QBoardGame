#ifndef ENDGAME_H
#define ENDGAME_H

#include <QDialog>

namespace Ui {
class endgame;
}

class endgame : public QDialog
{
    Q_OBJECT

public:
    explicit endgame(QWidget *parent = 0);
    void setState(int black, int white);
    int whowon = -1;
    ~endgame();

private:
    Ui::endgame *ui;
};

#endif // ENDGAME_H
