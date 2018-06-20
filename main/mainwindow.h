#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSize>
#include <QDebug>

#include "gamelogic/gameengine.h"
#include "gamelogic/chessengine.h"
#include "board/board.h"
#include "ui/uigamescene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int type, QWidget *parent = 0);
    int gametype;
    ~MainWindow();

public slots:
    void startNewGame();


private:
    Ui::MainWindow *ui;

    QSize m_boardSize;
    int m_iBoardSize;

    GameEngine *m_gameEngine;
    ChessEngine *m_chessEngine;
    UIGameScene *m_uiGameScene;
    Board *m_board;
};

#endif // MAINWINDOW_H
