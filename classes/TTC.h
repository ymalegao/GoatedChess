#pragma once
#include "Game.h"
#include "Square.h"

#include "Log.h"

//
// the classic game of Rock, Paper, Scissors
//
const int pieceSize = 100;


class TicTacToeAI
{
    
    public:
        int _grid[3][3];
        int evaluateBoard();
        int negamax(TicTacToeAI* state, int depth, int color);
        // void AI_Move(TicTacToe* state, int depth, int color);
        int checkForWinnerAI();
        bool isBoardFull() const;
        int ownerAt(int index ) const;

};






class TicTacToe : public Game
{
public:
    TicTacToe();
    ~TicTacToe();

    // set up the board
    void setUpBoard() override;

    Player* checkForWinner() override;
    bool checkForDraw() override;
    TicTacToeAI* clone();

    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;
    bool actionForEmptyHolder(BitHolder &holder) override;
    bool canBitMoveFrom(Bit&bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void stopGame() override;
    void AI_Move(TicTacToe* state, int depth, int color);

    BitHolder &getHolderAt(const int x, const int y) override { return _grid[x][y]; }
    // int negamax(TicTacToe* state, int depth, int color);
    // int evaluate();
    // int checkForWinnerAI();
    // bool isBoardFull(TicTacToe*state) const;
    Square   _grid[3][3];


private:
    Bit *PieceForPlayer(const int playerNumber);
    Player* ownerAt(int index ) const;
    void    scanForMouse();
    Log log;

};





