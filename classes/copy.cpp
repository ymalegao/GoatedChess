#include "Chess.h"
#include "Square.h"
#include "ChessSquare.h"
#include <map>
#define HUMAN -1
#define AI 1
using namespace std;
Chess::Chess() : log(Log()) 
{

    


}

Chess::~Chess()
{
}

//
// make a rock, paper, or scissors piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, Pieces piece)
{
    
    cout << playerNumber << " playerNumber" << endl;
    const char *textures[] = {"pawn.png", "rook.png", "knight.png", "bishop.png", "queen.png", "king.png"};
    Bit *bit = new Bit();    
    const char *pieceName = textures[piece -1];
    string piecepath = string("chesspics/") + (playerNumber == 0 ? "b_" : "w_") + pieceName;
    bit->LoadTextureFromFile(piecepath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    int gameTag = piece;
    if (playerNumber == 0) {
        gameTag += 128;
    }
    bit->setGameTag(gameTag);
    cout << "retuning bit" << endl;
    return bit;
}

void Chess::setUpBoard()
{
    srand((unsigned int)time(0));
    setNumberOfPlayers(2);

     // this allows us to draw the board correctly
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    // setup the board
    for (int x=0; x<_gameOptions.rowX; x++) {
        // _grid[x].initHolder(ImVec2(100*(float)x, 100 + 100), "square.png", x, 0);
        for (int y=0; y<_gameOptions.rowY; y++) {
            log.log(Log::INFO, "hi");
            cout << "x: " << x << " y: " << y << endl;
            // ImVec2 position((float)pieceSize * x  + pieceSize, (float)pieceSize * (_gameOptions.rowX - (float)pieceSize * y));

            _grid[x][y].initHolder(ImVec2(100+ 100*(float)x, 100*(float)y+ 100), "chesspics/boardsquare.png", x, y);
            _grid[x][y].setNotation(index_to_notation(y, x));
            _grid[x][y].setGameTag(0);

        }
    }
    // set up the pieces
    Chess::loadPositionFromFen(initialStateString());
    // if we have an AI set it up
    if (gameHasAI())
    {
        setAIPlayer(_gameOptions.AIPlayer);
    }
    // setup up turns etc.
    _moves = generate_moves('W');
    cout << "moves size: " << _moves.size() << endl;    
    for (auto move: _moves){

        cout << "from: " << move.from << " to: " << move.to << endl;
    }
    
    startGame();
}

//
// about the only thing we need to actually fill out for rock paper scissors
//
bool Chess::actionForEmptyHolder(BitHolder& holder)
{
    // if (holder.bit()) {
    //     return false;
    // }
    // Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber(), PAWN);
    // if (bit) {
    //     bit->setPosition(holder.getPosition());
    //     holder.setBit(bit);
    //     std::cout << "Placed piece with tag: " << bit->gameTag() << std::endl;

    //     endTurn();
    //     return true;
    // }   
    return false;
}







bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src)
{

    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    for (auto move: _moves){
        cout<<"move.from: " << move.from << " srcSquare.getNotation():" << srcSquare.getNotation() << endl;  
        if (move.from == srcSquare.getNotation()){
            return true;
        }
    }
    // return true;

    cout << "this can't move from " << srcSquare.getNotation() << endl;
    return false;




}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);

    for (auto move: _moves){
        if (move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation()){
            return true;
        }
    }
    
    cout << "can't move from " << srcSquare.getNotation() << " to " << dstSquare.getNotation() << endl;
    return false;


}


void Chess::bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst){
    Game::bitMovedFromTo(bit, src, dst);
    _moves = generate_moves( (_gameOptions.currentTurnNo&1 ) ? 'B' : 'W');




}

//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
    for (int x=0; x<_gameOptions.rowX; x++) {
        for (int y=0; y<_gameOptions.rowY; y++) {
            _grid[x][y].destroyBit();
        }
    }

}

//
// helper function for the winner check
//
Player* Chess::ownerAt(int index ) const
{
    if (!_grid[index/3][index%3].bit()) {
        return nullptr;
    }
    return _grid[index/3][index%3].bit()->getOwner();
}


Player* Chess::checkForWinner()
{
    // Check rows
    for (int row = 0; row < _gameOptions.rowY; ++row) {
        if (_grid[0][row].bit() && _grid[1][row].bit() && _grid[2][row].bit()) {
            if (_grid[0][row].bit()->gameTag() != 0 && _grid[1][row].bit()->gameTag() != 0 && _grid[2][row].bit()->gameTag() != 0) {
                if (_grid[0][row].bit()->gameTag() == _grid[1][row].bit()->gameTag() && _grid[1][row].bit()->gameTag() == _grid[2][row].bit()->gameTag()) {
                    return _grid[0][row].bit()->getOwner();
                }
            }
        }
    }

    // Check columns
    for (int col = 0; col < _gameOptions.rowX; ++col) {
        if (_grid[col][0].bit() && _grid[col][1].bit() && _grid[col][2].bit()) {
            if (_grid[col][0].bit()->gameTag() != 0 && _grid[col][1].bit()->gameTag() != 0 && _grid[col][2].bit()->gameTag() != 0) {
                if (_grid[col][0].bit()->gameTag() == _grid[col][1].bit()->gameTag() && _grid[col][1].bit()->gameTag() == _grid[col][2].bit()->gameTag()) {
                    return _grid[col][0].bit()->getOwner();
                }
            }
        }
    }

    // Check diagonals
    if (_grid[0][0].bit() && _grid[1][1].bit() && _grid[2][2].bit()) {
        if (_grid[0][0].bit()->gameTag() != 0 && _grid[1][1].bit()->gameTag() != 0 && _grid[2][2].bit()->gameTag() != 0) {
            if (_grid[0][0].bit()->gameTag() == _grid[1][1].bit()->gameTag() && _grid[1][1].bit()->gameTag() == _grid[2][2].bit()->gameTag()) {
                return _grid[0][0].bit()->getOwner();
            }
        }
    }

    if (_grid[2][0].bit() && _grid[1][1].bit() && _grid[0][2].bit()) {
        if (_grid[2][0].bit()->gameTag() != 0 && _grid[1][1].bit()->gameTag() != 0 && _grid[0][2].bit()->gameTag() != 0) {
            if (_grid[2][0].bit()->gameTag() == _grid[1][1].bit()->gameTag() && _grid[1][1].bit()->gameTag() == _grid[0][2].bit()->gameTag()) {
                return _grid[2][0].bit()->getOwner();
            }
        }
    }

    return nullptr;
}


bool Chess::checkForDraw()
{
    for (int x=0; x<_gameOptions.rowX; x++) {
        for (int y=0; y<_gameOptions.rowY; y++) {
            if (!_grid[x][y].bit()) {
                return false;
            }
        }
    }

    if (!checkForWinner()){
        return true;
    }

    return true;

}

//
// state strings
//
std::string Chess::initialStateString()
{
    return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}




//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString()
{
    std::string s;
    for (int x=0; x<_gameOptions.rowX; x++) {
        for (int y=0; y<_gameOptions.rowY; y++) {
        
            Bit *bit = _grid[x][y].bit();
            if (bit) {
                s += std::to_string(bit->gameTag());
            } else {
                s += "0";
            }
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string &s)
{
    for (int x=0; x<_gameOptions.rowX; x++) {
         for (int y=0; y<_gameOptions.rowY; y++) {
            _grid[x][y].setBit(PieceForPlayer(0, PAWN) );
         }
    }
}


void Chess::loadPositionFromFen(string fen){
    map <char, int> pieceMap = {
        {'p', PAWN}, //lowercase is black pieces, uppercase is white pieces
        {'r', ROOK},
        {'n', KNIGHT},
        {'b', BISHOP},
        {'q', QUEEN},
        {'k', KING},
        {'P', PAWN},
        {'R', ROOK},
        {'N', KNIGHT},
        {'B', BISHOP},
        {'Q', QUEEN},
        {'K', KING}
    };

    
    string fenBoard = fen.substr(0, fen.find(" "));
    int row = 7;
    int col = 0;
    
    for (int i = 0; i < fenBoard.length(); i++){
        if (fenBoard[i] == '/'){
            row--;
            col = 0;
            continue;
        }
        if (isdigit(fenBoard[i])){
            int num = fenBoard[i] - '0';
            col += num;
        }else{
            int player = isupper(fenBoard[i]) ? 1 : 0;
            
            char piece = tolower(fenBoard[i]); // Convert the piece to lowercase to ensure it's found in the map
            
            cout << "Player: " << player << ", Piece: " << piece << endl; // Added logging statement

            Bit *bit = PieceForPlayer(player, (Pieces)pieceMap[piece]);
            if (col < 8 && row < 8){
                _grid[col][row].setBit(bit);
                bit->setPosition(_grid[col][row].getPosition());
                cout << "Bit set for Player: " << player << ", Piece: " << piece << endl; // Added logging statement
                string position = string(1, 'a' + col) + to_string(8 - row);
                string pieceName = player == 0 ? "White " : "Black ";
                pieceName += pieceMap[piece];
                cout << pieceName << " -> " << position << endl;
                if (pieceName[0] == 'W'){
                    bit->setGameTag(pieceMap[piece]);
                }else{
                    bit->setGameTag(pieceMap[piece] + 128);
                }
                bit->setParent(&_grid[col][row]);
                
            }
            col++;
            
        }

        if (col > 8){
            row--;
            col = 0;
        }
    }


}

std::string Chess::piece_notation(int row, int col) const{
    const char *pieces = {"?PRNBQK"};
    string notation = "";
    Bit *bit = _grid[col][row].bit();
    if (bit) {
        notation += bit->gameTag() < 128 ? "W" : "B";
        notation += pieces[bit->gameTag() & 127];

    }else{
        notation = "00";
    }
    return notation;
    


}


void Chess::add_valid_move(std::vector <Move> &moves, int fromCol, int fromRow, int toCol, int toRow){
    if (toRow >= 0 && toRow < 8 && toCol >= 0 && toCol < 8){
        if (piece_notation(fromCol, fromRow)[0] != piece_notation(toCol, toRow)[0]){
            moves.push_back({index_to_notation(fromCol, fromRow), index_to_notation(toCol, toRow)});
        }
    }
}
std::string Chess::index_to_notation(int row, int col){
    string notation = "";
    notation += (char)('a' + col);
    notation += (char)('1' + row);
    return notation;
}




void Chess::generate_knight_moves(std::vector<Move> &moves, int row, int col){
    int rowOffsets[] = {2, 1, -1, -2, -2, -1, 1, 2};
    int colOffsets[] = {1, 2, 2, 1, -1, -2, -2, -1};
    for (int i = 0; i < 8; i++){
        int toCol = col + colOffsets[i];
        int toRow = row + rowOffsets[i];

        add_valid_move(moves, col, row, toCol, toRow);
    }
}


void Chess::generate_pawn_moves(std::vector<Move> &moves, int row, int col, char color){
    int direction = color == 'W' ? -1 : 1; // Adjusted direction for white pawns
    int startRow = color == 'W' ? 6 : 1; // Adjusted start row for white pawns

    if (piece_notation(col, row + direction) == "00"){
        add_valid_move(moves, col, row, col, row + direction);
        
        if (row == startRow && piece_notation(col, row + 2 * direction) == "00"){
            add_valid_move(moves, col, row, col, row + 2 * direction);
        }
    }

    for (int i = -1; i <= 1; i+=2){
        if (col + i >= 0 && col + i < 8){
            if (piece_notation(col + i, row + direction)[0] == opponent_color(color)){
                add_valid_move(moves, col, row, col + i, row + direction);
            }
        }
    }
}

void Chess::generate_linear_moves(std::vector<Move> &moves, int col, int row, const std::vector<std::pair<int, int>> &directions){
    for (auto &dir : directions){
        int currCol = col + dir.first;
        int currRow = row + dir.second;
        while (currCol >= 0 && currCol < 8 && currRow >=0 && currRow < 8){
            string piece = piece_notation(currCol, currRow);
            if (piece != "00"){
                if (piece[0] == opponent_color(piece_notation(col, row)[0])){
                    add_valid_move(moves, col, row, currCol, currRow);
                }
                break;
            }
            cout << "Adding valid move" << endl;
            add_valid_move(moves, col, row, currCol, currRow);
            currCol += dir.first;
            currRow += dir.second;
        }
    }
}


void Chess::generate_bishop_moves(std::vector<Move> &moves, int row, int col){

    static const std::vector<std::pair<int, int>> directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    generate_linear_moves(moves, col, row, directions);
}

void Chess::generate_rook_moves(std::vector<Move> &moves, int row, int col){
    static const std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    generate_linear_moves(moves, col, row, directions);
}

void Chess::generate_queen_moves(std::vector<Move> &moves, int row, int col){
    generate_bishop_moves(moves, row, col);
    generate_rook_moves(moves, row, col);

}

void Chess::generate_king_moves(std::vector<Move> &moves, int row, int col){
    static const std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (auto& dir : directions){
        int newRow = row + dir.second;
        int newCol = col + dir.first;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol <8){
            add_valid_move(moves, col, row, newCol, newRow);
        }

    }

}

char Chess::opponent_color(char color) {
    return (color == 'W' ) ? 'B' : 'W';
}


std::vector<Chess::Move> Chess::generate_moves(char color){
    std::vector<Move> moves;
    for (int col = 0; col < 8; col++){
        for (int row = 0; row < 8; row++){
            string piece = piece_notation(col, row);
            cout << "Piece: " << piece << ", Row: " << row << ", Col: " << col << endl;
            if (piece[0] == color){
                switch (piece[1]){
                    case 'P':
                        cout << "Generating pawn moves" << endl;
                        generate_pawn_moves(moves, row, col, color);
                        break;
                    case 'N':
                        cout << "Generating knight moves" << endl;
                        generate_knight_moves(moves, row, col);
                        break;
                    case 'B':
                        cout<< "Generating bishop moves" << endl;
                        generate_bishop_moves(moves, row, col);
                        break;
                    case 'R':
                        cout << "Generating rook moves" << endl;
                        generate_rook_moves(moves, row, col);
                        break;
                    case 'Q':
                        cout << "Generating queen moves" << endl;
                        generate_queen_moves(moves, row, col);
                        break;
                    case 'K':
                        cout << "Generating king moves" << endl;
                        generate_king_moves(moves, row, col);
                        break;
                }
            }
        }
    }
    return moves;        
    }


