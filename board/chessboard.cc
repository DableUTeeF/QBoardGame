#include "chessboard.h"

ChessBoard::ChessBoard(Player *currentPlayer, int game, QObject *parent) : QObject(parent),
  m_boardMatrix(BOARD_SIZE, QVector<Square* >(BOARD_SIZE))
{
    gametype = game;
    m_currentPlayer = currentPlayer;
    newBoard();
}

ChessBoard::ChessBoard(const ChessBoard &board)
{
    m_legalMoves = new QVector<Square* >;

    // copy the board
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {

            m_boardMatrix[x][y] = board.getSquare(x,y);
        }
    }

    m_gameOver = board.m_gameOver;
}

Square *ChessBoard::getSquare(int x, int y) const
{
    return m_boardMatrix[x][y];
}

Square::State ChessBoard::getSquareState(int x, int y)
{
    return m_boardMatrix[x][y]->getSquareState();
}

void ChessBoard::newBoard()
{
    m_legalMoves = new QVector<Square* >;

    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {

            m_boardMatrix[x][y] = new Square(x,y);
        }
    }
    if(gametype == 0)
    {
        m_boardMatrix[3][3]->setSquareState(Square::WHITE);
        m_boardMatrix[3][4]->setSquareState(Square::BLACK);
        m_boardMatrix[4][3]->setSquareState(Square::BLACK);
        m_boardMatrix[4][4]->setSquareState(Square::WHITE);

        m_numberOfBlackDisks = 2;
        m_numberOfWhiteDisks = 2;
        m_numberOfDisks = 4;
    }
    else if(gametype == 1)
    {
        m_boardMatrix[1][0]->setSquareState(Square::WHITE);
        m_boardMatrix[3][0]->setSquareState(Square::WHITE);
        m_boardMatrix[5][0]->setSquareState(Square::WHITE);
        m_boardMatrix[7][0]->setSquareState(Square::WHITE);
        m_boardMatrix[2][1]->setSquareState(Square::WHITE);
        m_boardMatrix[4][1]->setSquareState(Square::WHITE);
        m_boardMatrix[6][1]->setSquareState(Square::WHITE);
        m_boardMatrix[0][1]->setSquareState(Square::WHITE);

        m_boardMatrix[1][6]->setSquareState(Square::BLACK);
        m_boardMatrix[3][6]->setSquareState(Square::BLACK);
        m_boardMatrix[5][6]->setSquareState(Square::BLACK);
        m_boardMatrix[7][6]->setSquareState(Square::BLACK);
        m_boardMatrix[0][7]->setSquareState(Square::BLACK);
        m_boardMatrix[2][7]->setSquareState(Square::BLACK);
        m_boardMatrix[4][7]->setSquareState(Square::BLACK);
        m_boardMatrix[6][7]->setSquareState(Square::BLACK);

        m_numberOfBlackDisks = 8;
        m_numberOfWhiteDisks = 5;
        m_numberOfDisks = 16;
    }

    m_gameOver = false;
}

void ChessBoard::countDisks(void)
{
    m_numberOfBlackDisks = 0;
    m_numberOfWhiteDisks = 0;
    m_numberOfDisks = 0;


    Player::Color currentOwner;
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            currentOwner = m_boardMatrix[x][y]->getOwner();
            if (currentOwner == Player::BLACK)
            {
                m_numberOfBlackDisks++;
                m_numberOfDisks++;
            }
            else if (currentOwner == Player::WHITE)
            {
                m_numberOfWhiteDisks++;
                m_numberOfDisks++;
            }
        }
    }
    qDebug() << "There are" << m_numberOfBlackDisks << "Black disks";
    qDebug() << "There are" << m_numberOfWhiteDisks << "White disks";
    qDebug() << "There are" << m_numberOfDisks << "total disks";
}

bool ChessBoard::legalMove(int x, int y)
{
    //qDebug() << "--------------------------- Check new legal Move at -------------------------------";
    // check if inside board
    if(!onBoard(x, y))
    {
        return false;
    }
    // check if pressed square is occupied by player
    if (!m_boardMatrix[x][y]->isEmpty())
    {
        return false;
    }
    //qDebug() << "Board(" << x << "," << y << ") is not occupied" << m_boardMatrix[x][y]->getOwner() << "Square State" << m_boardMatrix[x][y]->getSquareState();
    // test left for possible flips
    bool moveLegal = false;
    // For Othello
    for(int dir = 0; dir < BOARD_SIZE; dir++)
    {
        int dx = m_direction[dir][0];
        int dy = m_direction[dir][1];
        int tx = x + 2*dx;
        int ty = y + 2*dy;
        // need to be at least 2 grids away from the edge
        if (!onBoard(tx, ty))
        {
            continue;
        }
        //qDebug() << "(tx,ty) = (" << tx << "," << ty << ") is on the board";

        // oppenent disk must be adjacent in the current direction
        if (m_boardMatrix[x+dx][y+dy]->getOwner() != getOtherPlayer(m_currentPlayer))
        {
            QString string = QString(getOtherPlayer(m_currentPlayer));
            //qDebug() << "but there is no adjacent opponent" << string;
            continue;
        }
        //qDebug() << "Adjacent opponent" << getOtherPlayer(m_currentPlayer) << "found at ([x+dx],[y+dy]) = (" << x+dx << "," << y+dy << ") CurrentPlayer is" << m_currentPlayer->m_color;

        // as long as we stay on the board going in the current direction, we search for the surrounding disk
        while(onBoard(tx, ty) && m_boardMatrix[tx][ty]->getOwner() == getOtherPlayer(m_currentPlayer))
        {
            tx += dx;
            ty += dy;
        }

        // if we are still on the board and we found the surrounding disk in the current direction
        // the move is legal.
        if(onBoard(tx, ty) && m_boardMatrix[tx][ty]->getOwner() == m_currentPlayer->m_color)
        {
            //qDebug() << "Found surrounding disk of Player" << m_currentPlayer->m_color << "at (tx,ty) = (" << tx << "," << ty << ")";
            moveLegal = true;
            break;
        }
    }

    //qDebug() << "Board::legalMove" << moveLegal;
    return moveLegal;

}

bool ChessBoard::getLegalMoves(QVector<Square* > *legalMoves)
{
    // this function appends possible moves (legal squares) to the provided QVector legalMoves.
    // this also updates the member variable m_legalMoves. A NULL parameter is use by board itself.
    if (legalMoves == NULL)
    {
        legalMoves = new QVector<Square* >;
    }
    else
    {
        legalMoves->clear();
    }

    Square *allowedSquare = NULL;
    bool legalMovesAvailable = false;
    for(int x = 0; x < BOARD_SIZE; x++)
    {
        for(int y = 0; y < BOARD_SIZE; y++)
        {
            if (legalMove(x, y))
            {
                allowedSquare = m_boardMatrix[x][y];

                // if move is legal, updated the board: set current square in allowed state
                allowedSquare->setSquareState(Square::ALLOWED);

                // append newly found legal move to legalMoves vector.
                legalMoves->append(allowedSquare);
                legalMovesAvailable = true;
            }
        }
    }
    m_legalMoves = legalMoves;
    return legalMovesAvailable;
}

QVector<ChessBoard *> Board::makeLegalMoves()
{
    QVector<ChessBoard *> possibleBoards(1);

    // get possible moves (squares)
    getLegalMoves(NULL);

    // foreach possible move in m_legalMoves list, append the future board to QVector.
    foreach (Square *square, *m_legalMoves)
    {
        Board *tempBoard = new Board(*this);
        tempBoard->makeMove(square->m_x, square->m_y);
        possibleBoards.append(tempBoard);
    }
}

void ChessBoard::makeMove(int x, int y)
{
    // TODO update number of moves if valid;
    // TODO append to a tree?!

    for(int dir = 0; dir < BOARD_SIZE; dir++)
    {
        int dx = m_direction[dir][0];
        int dy = m_direction[dir][1];
        int tx = x + 2*dx;
        int ty = y + 2*dy;
        // need to be at least 2 grids away from the edge
        if (!onBoard(tx, ty))
        {
            continue;
        }
        // oppenent piece must be adjacent in the current direction
        if (m_boardMatrix[x+dx][y+dy]->getOwner() != getOtherPlayer(m_currentPlayer))
        {
            continue;
        }
        // as long as we stay on the board going in the current direction, we search for the surrounding disk
        while(onBoard(tx, ty) && m_boardMatrix[tx][ty]->getOwner() == getOtherPlayer(m_currentPlayer))
        {
            tx += dx;
            ty += dy;
        }
        // if we are still on the board and we found the surrounding disk in the current direction
        // the move is legal.

        // go back and flip the pieces if move is legal
        if(onBoard(tx, ty) && m_boardMatrix[tx][ty]->getOwner() == m_currentPlayer->m_color)
        {
            tx -= dx;
            ty -= dy;

            while(m_boardMatrix[tx][ty]->getOwner() == getOtherPlayer(m_currentPlayer))
            {
                //qDebug() << "Flipping" << tx << "," << ty;
                m_boardMatrix[tx][ty]->setOwner(m_currentPlayer->m_color);
                if (m_currentPlayer->m_color == Player::BLACK)
                {
                    //updateUI(tx, ty, UISquare::BLACK, Player::BLACK);
                    // TODO: avoid emit if AI move
                    emit signalBoardChanged(tx, ty, Player::BLACK);
                }
                else
                {
                    //updateUI(tx, ty, UISquare::WHITE, Player::WHITE);
                    // TODO: avoid emit if AI move
                    emit signalBoardChanged(tx, ty, Player::WHITE);
                }

                tx -= dx;
                ty -= dy;
            }
            // set color of placed disk to current player
            m_boardMatrix[x][y]->setOwner(m_currentPlayer->m_color);

            // TODO this needs some improvement - see also inside while loop
            if (m_currentPlayer->m_color == Player::BLACK)
            {
                //updateUI(tx, ty, UISquare::BLACK, Player::BLACK);
                // TODO: avoid emit if AI move
                emit signalBoardChanged(x, y, Player::BLACK);
            }
            else
            {
                //updateUI(tx, ty, UISquare::WHITE, Player::WHITE);
                // TODO: avoid emit if AI move
                emit signalBoardChanged(x, y, Player::WHITE);
            }

        }
    }

    //m_boardStack<Board*
}

bool ChessBoard::undoMove()
{

}

bool ChessBoard::onBoard(int x, int y)
{
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE)
    {
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

Player::Color ChessBoard::getOtherPlayer(Player *currentPlayer)
{
    if (currentPlayer->m_color == Player::BLACK)
    {
        return Player::WHITE;
    }
    else if (currentPlayer->m_color == Player::WHITE)
    {
        return Player::BLACK;
    }
    else if (currentPlayer->m_color == Player::NONE)
    {
        qDebug() << "Board::getOtherPlayer" << "Player::NONE?! Debug this";
    }
}
