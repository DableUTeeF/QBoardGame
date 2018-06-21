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
            if(y<=1)  // White
            {

                if((x+y) % 2 == 1)
                {
                    m_pieceState[x][y] = 1;
                }
                else
                {
                    m_pieceState[x][y] = 0;
                }
            }
            else if(y>=6)  // Black
            {

                if((x+y) % 2 == 1)
                {
                    m_pieceState[x][y] = -1;
                }
                else
                {
                    m_pieceState[x][y] = 0;
                }
            }
            else{
                m_pieceState[x][y] = 0;
            }

        }
    }

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
//    qDebug() << "There are" << m_numberOfBlackDisks << "Black disks";
//    qDebug() << "There are" << m_numberOfWhiteDisks << "White disks";
//    qDebug() << "There are" << m_numberOfDisks << "total disks";
}

bool ChessBoard::legalMove(int x0, int y0, int x, int y)
{
    if(!m_boardMatrix[x][y]->isEmpty())
    {
        // Square is not empty
        return false;
    }
    else if((x+y) % 2 != 1)
    {
        // Square is on invalid cross
        return false;
    }
    else if(std::abs(m_pieceState[x0][y0])==1)
    {
        if(std::abs(x0 - x) > 1 || std::abs(y0 - y) > 1)
        {
            // Too far square
            return false;
        }
        else if(m_pieceState[x0][y0] * (y0-y) > 0)
        {
            // Backward move is not allowed
            return false;
        }
    }
    return true;

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
            if (legalMove(NULL, NULL, x, y))
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

QVector<ChessBoard *> ChessBoard::makeLegalMoves()
{
    QVector<ChessBoard *> possibleBoards(1);

    // get possible moves (squares)
    getLegalMoves(NULL);

    // foreach possible move in m_legalMoves list, append the future board to QVector.
    foreach (Square *square, *m_legalMoves)
    {
        ChessBoard *tempBoard = new ChessBoard(*this);
        tempBoard->makeMove(square->m_x, square->m_y, 0, 0);
        possibleBoards.append(tempBoard);
    }
}

void ChessBoard::makeMove(int x0, int y0, int x, int y)
{
    // TODO update number of moves if valid;
    // TODO append to a tree?!
    m_boardMatrix[x][y]->setOwner(m_currentPlayer->m_color);
    m_boardMatrix[x0][y0]->setOwner(Player::NONE);
    m_boardMatrix[x0][y0]->setSquareState(Square::NONE);
    m_pieceState[x0][y0] = 0;

    // TODO this needs some improvement - see also inside while loop
    emit signalBoardChanged(x0, y0, Player::NONE);
    emit signalBoardChanged(x, y, m_currentPlayer->m_color);
    if (m_currentPlayer->m_color == Player::BLACK)
    {
        m_pieceState[x][y] = -1;
    }
    else
    {
        m_pieceState[x][y] = 1;
    }

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
