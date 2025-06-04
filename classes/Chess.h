#pragma once
#include "Game.h"
#include "Square.h"
#include "ChessSquare.h"
#include "Log.h"

//
// the classic game of Rock, Paper, Scissors
//
const int pieceSize = 64;

enum Pieces { 
        NONE = 0, //white is 0,
        PAWN = 1, //white is 1, black is 129 
        ROOK = 2, //white is 2, black is 130
        KNIGHT = 3, //white is 3, black is 131
        BISHOP = 4, //white is 4, black is 132
        QUEEN = 5, //white is 5, black is 133
        KING = 6, //white is 6, black is 134
    };




class Chess : public Game
{
public:
    Chess();
    ~Chess();

    struct Move {
        string from;
        string to;
    };

    // set up the board
    void setUpBoard() override;

    Player* checkForWinner() override;
    bool checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;
    bool actionForEmptyHolder(BitHolder &holder) override;
    bool canBitMoveFrom(Bit&bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void stopGame() override;
    void AI_Move();
    bool is_castle_move(Bit& bit, BitHolder& src, BitHolder& dst);
    void perform_castle_move(Bit& bit, BitHolder& src, BitHolder& dst);
    bool is_promotion_move(Bit& bit, BitHolder& src, BitHolder& dst);
    void perform_promotion_move(Bit& bit, BitHolder& src, BitHolder& dst);
    bool is_en_passant_move(Bit& bit, BitHolder& src, BitHolder& dst);
    void perform_en_passant_move(Bit& bit, BitHolder& src, BitHolder& dst);
    BitHolder &getHolderAt(const int x, const int y) override { return _grid[x][y]; }
    std::vector<Chess::Move> _moves;
    int _countSearch;
    bool can_white_castle_kingside;
    bool can_black_castle_kingside;
    bool can_white_castle_queenside;
    bool can_black_castle_queenside;
    void updateAI() override; 
    bool gameHasAI() override;
    int negamax(char *state, int depth, int color, int alpha, int beta);
    int evaluateBoard(const char *state);
    void filterOutIllegalMoves(char color, vector<Move> &moves, string _lastMove);
    bool squareUnderAttack(char *state, int square, char color);
    vector<int> calculateLineOfAttack(int kingSquare, int checkingPieceSquare);
    // bool isMovePuttingKingInCheck(char* state, int srcSquare, int dstSquare, char color);
    // bool pieceCanAttack(char* state, int srcSquare, int dstSquare);
    bool isMoveValid(char* state, int src_square, int dst_square, int opponent_srcSquare, int opponent_dstSquare, char color);
    bool isKingInCheck(char* state, char color);
    bool pieceCanAttack(char* state,  int targetSquare, char color);
    void orderMoves(std::vector<Move> &moves, char *state);
    void UCIMove(const std::string& move);
    void loadPositionFromFen(string fen);


    void updateAI_MultiThreaded();
    struct ThreadSearchResult { Move bestMove; int bestValue;  };

    ThreadSearchResult searchMovesInRange(vector<Move>& moves, int start, int end, int aiPlayerColor, int depth, string currentState);

    
    ChessSquare   _grid[8][8];


private:
    Bit *PieceForPlayer(const int playerNumber, Pieces piece);
    Player* ownerAt(int index ) const;
    void add_valid_move(std::vector<Move> &moves, int fromFile, int fromRank, int toFile, int toRank);
    std::string index_to_notation(int file, int rank);
    std::string piece_notation(int file, int rank) const;
    void generate_pawn_moves(std::vector<Move> &moves, int file, int rank, char color, string _lastMove,  char *state);
    void generate_knight_moves(std::vector<Move> &moves, int file, int rank,  char *state);
    void generate_bishop_moves(std::vector<Move> &moves, int file, int rank,  char *state);
    void generate_rook_moves(std::vector<Move> &moves, int file, int rank,  char *state);
    void generate_queen_moves(std::vector<Move> &moves, int file, int rank,  char *state);
    void generate_king_moves(std::vector<Move> &moves, int file, int rank,  char *state);
    char opponent_color(char color) ;
    std::vector<Chess::Move> generate_moves(char *state, char color, string _lastMove, bool filter);
    void generate_linear_moves(std::vector<Move> &moves, int file, int rank, const std::vector<std::pair<int, int>> &directions,  char *state);
    void PrecomputedMoveData();
    int notation_to_index(string& notation);
    
    void    scanForMouse();
    Log log;

};





