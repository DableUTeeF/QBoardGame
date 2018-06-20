#include "chessengine.h"
#include "main/endgame.h"
ChessEngine::ChessEngine(QObject *parent, UIGameScene *uiGameScene, QTextEdit *eventList, QTextEdit *infoList) : QObject(parent)
{
    m_uiGameScene = uiGameScene;
    m_eventList = eventList;
    m_infoList = infoList;

    m_legalMoves = new QVector<Square* >;
}

ChessEngine::~ChessEngine()
{

}

void ChessEngine::startGame(int numberOfHumans, double timeLimit)
{
    m_numberOfHumans = numberOfHumans;
    createPlayers(m_numberOfHumans);

    m_timeLimit = timeLimit;

    // TODO use these variables...
    m_numberOfActualMoves = 0;
    m_numberOfTotalMoves = 0;
    m_elapsedTime = 0;

    m_gameOver = false;

    m_board = new ChessBoard(m_currentPlayer, 1);  // Tehe 0 is for Othello, use 1 for Checker

    connect(m_board, SIGNAL(signalBoardChanged(int,int,Player::Color)), this, SLOT(updateUI(int,int,Player::Color)));

    // show infos like whos turn it is and the time needed so far (TODO is this really needed?!).
    updateInfoText("Current Player");

    // display legal moves for player that starts the game (usually black)
    showLegalMoves();

    // TODO thinking timer start and stop (toggle, ...)
    //connect(&m_thinkingTime, SIGNAL(timeout()), this, SLOT(loop()));
    m_thinkingTime.start();
    //timer.elapsed() << "milliseconds";

    // create the AI
    m_ai = new AI(m_board);


//    if (m_numberOfHumans == 1 && m_currentPlayer->m_type == Player::COMPUTER)
//    {
//        Square* square = m_ai->makeMove(m_board);
//        revertAllowedUISquares(square->m_x, square->m_y);
//        togglePlayer();
//    }

}

void ChessEngine::mouseReleased(QPointF point)
{
    qDebug() << "PlayerType" << m_currentPlayer->m_type;
    if (m_numberOfHumans == 1 && m_currentPlayer->m_type == Player::COMPUTER)
    {
        qDebug() << "Computer move! Don't click!";
        makePass();
        return;
    }
    int x = point.x() / m_uiGameScene->m_sizeSceneRect * m_uiGameScene->m_numberColumns;
    int y = point.y() / m_uiGameScene->m_sizeSceneRect * m_uiGameScene->m_numberRows;
    // TODO comment all qDebug()s
    //qDebug() << "Mouse pointer is at" << point << "x" << x << "y" << y;
    eventHandling(x, y);
}

void ChessEngine::createPlayers(int numberOfHumans)
{
    if (numberOfHumans == 2)
    {
        m_humanPlayerB = new HumanPlayer(Player::BLACK);
        m_humanPlayerW = new HumanPlayer(Player::WHITE);
        m_currentPlayer = m_humanPlayerB;
        m_opponentPlayer = m_humanPlayerW;
    }
    else if (numberOfHumans == 1)
    {
        // TODO let palyer choose color
        m_computerPlayerB = new ComputerPlayer(Player::BLACK);
        m_humanPlayerW = new HumanPlayer(Player::WHITE);
        m_currentPlayer = m_computerPlayerB;
        m_opponentPlayer = m_humanPlayerW;
    }

    // TODO set players correctly according to parameter numberOfHumans
}

bool ChessEngine::gameOver()
{
    // check if current player has options to make a move
    if (m_board->getLegalMoves(m_legalMoves) == true)
    {
        return false;
    }

    // make a pass if there are no legal moves left ...
    makePass();
    // ... and check if the opponent has legal moves left
    if(m_board->getLegalMoves(m_legalMoves) == true)
    {
        return false;
    }
    endgame *ended = new endgame();
    qDebug() << "Game Over";

    m_gameOver = true;
    QString gameStats = getGameStats();
    updateInfoText(gameStats);
    int numberOfBlackDisks = m_board->m_numberOfBlackDisks;
    int numberOfWhiteDisks = m_board->m_numberOfWhiteDisks;
    ended->whowon = numberOfBlackDisks > numberOfWhiteDisks;
    ended->show();
    ended->setState(numberOfBlackDisks, numberOfWhiteDisks);
    return true;
}

QString ChessEngine::getGameStats()
{
    QString gameResult;

    m_board->countDisks();
    int numberOfBlackDisks = m_board->m_numberOfBlackDisks;
    int numberOfWhiteDisks = m_board->m_numberOfWhiteDisks;

    // TODO is it needed?
    int numberOfDisks = m_board->m_numberOfDisks;

    // TODO check rules ... count empty squares too???
    if (numberOfBlackDisks > numberOfWhiteDisks)
    {
        gameResult = QString(QString("Black player (") + QString::number(numberOfBlackDisks) + QString(") wins! \n") +
                             QString("White player (") + QString::number(numberOfWhiteDisks) + QString(") loses."));
    }
    else if (numberOfWhiteDisks > numberOfBlackDisks)
    {
        gameResult = QString(QString("White player (") + QString::number(numberOfWhiteDisks) + QString(") wins! \n") +
                             QString("Black player (") + QString::number(numberOfBlackDisks) + QString(") loses."));
    }
    else if (numberOfBlackDisks == numberOfWhiteDisks)
    {
        gameResult = QString(QString("Draw \n") + QString("Both players have ") + QString::number(numberOfBlackDisks));
    }

    return gameResult;
}

void ChessEngine::makePass()
{
    if (m_currentPlayer->m_color == Player::BLACK)
    {
        updateInfoText("Black passed, it's White's turn");
    }
    else if (m_currentPlayer->m_color == Player::WHITE)
    {
        updateInfoText("White passed, it's Black's turn");
    }
    m_numberOfTotalMoves++;
    togglePlayer();

}

void ChessEngine::eventHandling(int x, int y)
{
    // first check if the game is over
    if (m_gameOver)
        return;

    // this string will hopefully overwritten by a legal event.
    QString eventString = "Something went wrong in eventHandling";

    Player::Color currentPlayer = m_currentPlayer->m_color;
    switch(currentPlayer)
    {
    case Player::BLACK:
        if (m_board->legalMove(x, y))
        {
            m_board->makeMove(x, y);
            revertAllowedUISquares(x, y);
            //updateUI(x, y, Player::BLACK);
            m_numberOfActualMoves++;
            m_numberOfTotalMoves++;
            eventString = QString(QString::number(m_numberOfActualMoves) + ". Black played at (" +
                                  QString::number(x) + "," + QString::number(y) +
                                  ") in " + QString::number(getThinkingTime()) + " sec");

            togglePlayer();
        }
        // check if game is over if no legal move possible
//        else
//        {
//            gameOver();
//        }
        break;

    case Player::WHITE:
        if (m_board->legalMove(x, y))
        {
            m_board->makeMove(x, y);
            revertAllowedUISquares(x, y);
            //updateUI(x, y, Player::WHITE);
            m_numberOfActualMoves++;
            m_numberOfTotalMoves++;
            eventString = QString(QString::number(m_numberOfActualMoves) + ". White played at (" +
                                  QString::number(x) + "," + QString::number(y) +
                                  ") in " + QString::number(getThinkingTime()) + " sec");

            togglePlayer();
        }
        // check if game is over if no legal move possible
//        else
//        {
//            gameOver();
//        }
        break;

    case Player::NONE:
        qDebug() << "ChessEngine::eventHandling" << "Player::NONE?! Debug this";
        break;

    default:
        qDebug() << "ChessEngine::eventHandling" << "default case. Debug this";
        break;
    }


    bool movesAvailable = m_board->getLegalMoves(m_legalMoves);
//    if (!movesAvailable)
//    {
//        gameOver();
//    }

    updateEventText(eventString);

    // restart the stopwatch
    m_elapsedTime = 0;
    m_thinkingTime.start();

    // make a new computer move if it is his turn.
    if (!gameOver() && m_numberOfHumans == 1 && m_currentPlayer->m_type == Player::COMPUTER)
    {
        Square* square = m_ai->makeMove(m_board);
        if (square != NULL)
        {
            revertAllowedUISquares(square->m_x, square->m_y);
            //updateUI(square->m_x, square->m_y, Player::BLACK); // UPDATE UI?????
            m_numberOfActualMoves++;
            m_numberOfTotalMoves++;
            eventString = QString(QString::number(m_numberOfActualMoves) + ". Computer played at (" +
                                QString::number(square->m_x) + "," + QString::number(square->m_y) +
                                ") in " + QString::number(getThinkingTime()) + " sec");
            togglePlayer();
        }
        else // there was no legal move possible because legalMoves list is empty -> see m_ai->makeMove
        {
            makePass();
        }

        bool movesAvailable = m_board->getLegalMoves(m_legalMoves);
        if (!movesAvailable)
        {
            gameOver();
        }

        // restart the stopwatch
        m_elapsedTime = 0;
        m_thinkingTime.start();
    }
    updateEventText(eventString);
}

void ChessEngine::updateUI(int x, int y, Player::Color currentPlayer)
{
    // update chosen square with player color
    switch (currentPlayer) {
    case Player::BLACK:
        m_uiGameScene->setSquareState(x, y, UISquare::BLACK);
        break;
    case Player::WHITE:
        m_uiGameScene->setSquareState(x, y, UISquare::WHITE);
        break;
    case Player::NONE:
        m_uiGameScene->setSquareState(x, y, UISquare::BOARD);
        break;
    default:
        break;
    }
}

void ChessEngine::updateInfoText(QString string)
{
    m_infoList->setText(string);
    switch (m_currentPlayer->m_color)
    {
    case Player::BLACK:
        m_infoList->append(QString("Black to move"));
        break;
    case Player::WHITE:
        m_infoList->append(QString("White to move"));
        break;
    case Player::NONE:
        m_infoList->append(QString("NONE to move?! Debug this"));
        break;
    default:
        m_infoList->append(QString("default case ... Debug this"));
        break;
    }
}

void ChessEngine::updateEventText(QString string)
{
    m_eventList->append(string);
}

double ChessEngine::getThinkingTime()
{
    m_elapsedTime = m_thinkingTime.elapsed();
    m_elapsedTime = m_elapsedTime/1000.0;
    return m_elapsedTime;
}

void ChessEngine::togglePlayer()
{

    // TODO comment or delet; just for debugging
    m_board->countDisks();

    Player **dummyPlayer;

    // TODO swap pointers?????????? this is #?!
    switch(m_currentPlayer->m_color)
    {
    case Player::BLACK:
        qDebug() << "BLACK ChessEngine::togglePlayer before" << "m_currentPlayer" << m_currentPlayer;
        qDebug() << "BLACK ChessEngine::togglePlayer before" << "m_opponentPlayer" << m_currentPlayer;
        m_currentPlayer->m_color = Player::WHITE;
        m_opponentPlayer->m_color = Player::BLACK;

        // TODO this is only correct if there is a computer player.
        if (m_currentPlayer->m_type == Player::COMPUTER)
        {
            m_currentPlayer->m_type = Player::HUMAN;
            m_opponentPlayer->m_type = Player::COMPUTER;
        }
        else if (m_currentPlayer->m_type == Player::HUMAN)
        {
            m_currentPlayer->m_type = Player::COMPUTER;
            m_opponentPlayer->m_type = Player::HUMAN;
        }

        //dummyPlayer = &m_currentPlayer;
        //m_currentPlayer = &m_opponentPlayer;
        //m_opponentPlayer = &dummyPlayer;
        //qDebug() << "BLACK ChessEngine::togglePlayer after" << "m_currentPlayer" << m_currentPlayer;
        //qDebug() << "BLACK ChessEngine::togglePlayer after" << "m_opponentPlayer" << m_currentPlayer;
        break;
    case Player::WHITE:
        qDebug() << "WHITE ChessEngine::togglePlayer before" << "m_currentPlayer" << m_currentPlayer;
        qDebug() << "WHITE ChessEngine::togglePlayer before" << "m_opponentPlayer" << m_currentPlayer;
        m_currentPlayer->m_color = Player::BLACK;
        m_opponentPlayer->m_color = Player::WHITE;

        if (m_currentPlayer->m_type == Player::COMPUTER)
        {
            m_currentPlayer->m_type = Player::HUMAN;
            m_opponentPlayer->m_type = Player::COMPUTER;
        }
        else if (m_currentPlayer->m_type == Player::HUMAN)
        {
            m_currentPlayer->m_type = Player::COMPUTER;
            m_opponentPlayer->m_type = Player::HUMAN;
        }


        qDebug() << "WHITE ChessEngine::togglePlayer after" << "m_currentPlayer" << m_currentPlayer;
        qDebug() << "WHITE ChessEngine::togglePlayer after" << "m_opponentPlayer" << m_currentPlayer;
        break;
    default:
        m_currentPlayer->m_color = Player::NONE;
        m_opponentPlayer->m_color = Player::NONE;
        qDebug() << "togglePlayers: default case?!";
        break;
    }
    updateInfoText("Current Player");
    //qDebug() << "ChessEngine::nextPlayer" << m_currentPlayer->m_color;

    showLegalMoves();
}

void ChessEngine::showLegalMoves()
{
    // clear list first (forget previous legal moves)
    //m_legalMoves->clear();

    // check if there are legal moves available before actually trying to redraw some.
    bool legalMovesAvailable = m_board->getLegalMoves(m_legalMoves);
    if (legalMovesAvailable == true)
    {
        foreach (Square *square, *m_legalMoves) {
            m_uiGameScene->setSquareState(square->m_x, square->m_y, UISquare::ALLOWED);
        }
    }
}

void ChessEngine::revertAllowedUISquares(int x, int y)
{
    // revert allowed squares to Board state that were NOT picked by current player
    Square *movedSquare = m_board->getSquare(x, y);
    m_legalMoves->removeOne(movedSquare);
    foreach (Square *square, *m_legalMoves)
    {
        updateUI(square->m_x, square->m_y, Player::NONE);
    }
}

void ChessEngine::counter()
{

}
