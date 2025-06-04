//THE GAME NEEDS TO BE IN FULL SCREEN FOR SOME REASON AND THEN ALL THE PICES MOVE
#include "UCI.h"

#include "Chess.h"
#include "Square.h"
#include "ChessSquare.h"
#include "Evaluate.h"
#include <map>
#include <algorithm>
#define HUMAN -1
#define AI 1
using namespace std;
#include <iostream>


#if defined(UCI_INTERFACE)
static UCIInterface uciInterface;
#endif
Chess::Chess() : log(Log()), can_white_castle_kingside(true), can_black_castle_kingside(true), can_white_castle_queenside(true), can_black_castle_queenside(true)
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
    string piecepath = string("chesspics/") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(piecepath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    int gameTag = piece;
    // if (playerNumber == 1) {
    //     gameTag += 128;
    // } //changed
    // bit->setGameTag(gameTag);
    // cout << "retuning bit" << endl;
    return bit;
}

void Chess::setUpBoard()
{
    srand((unsigned int)time(0));
    setNumberOfPlayers(2);

     // this allows us to draw the board correctly
    _gameOptions.rowX = 8; //file
    _gameOptions.rowY = 8; //rank
    // setup the board
    for (int x=0; x<_gameOptions.rowX; x++) {
        // _grid[x].initHolder(ImVec2(100*(float)x, 100 + 100), "square.png", x, 0);
        for (int y=0; y<_gameOptions.rowY; y++) {
            log.log(Log::INFO, "hi");
            // cout << "x: " << x << " y: " << y << endl;
            // ImVec2 position((float)pieceSize * x  + pieceSize, (float)pieceSize * (_gameOptions.rowX - (float)pieceSize * y));

            _grid[x][y].initHolder(ImVec2(pieceSize+ pieceSize*(float)x, pieceSize*(float)(_gameOptions.rowY-y)+ pieceSize), "chesspics/boardsquare.png", x, y);
            _grid[x][y].setNotation(index_to_notation(x, y));
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
    // cout << "moves size: " << _moves.size() << endl;    
   
    
    startGame();
    #if defined(UCI_INTERFACE)
    uciInterface.Run(this);
    #endif
}

bool Chess::gameHasAI()
{
    if (_gameOptions.AIPlayer == false){
        return false;
    }
    return true;
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

        //  endTurn();
    //     return true;
    // }   
    return true;
}







bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src)
{

    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    for (auto move: _moves){
        // cout<<"move.from: " << move.to << " srcSquare.getNotation():" << srcSquare.getNotation() << endl;  
        if (move.from == srcSquare.getNotation()){
            return true;
        }
    }
    // return true;

    // cout << "this can't move from " << srcSquare.getNotation() << endl;
    return false;




}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);

    if (srcSquare.getNotation() == "e1"){
        // cout << "WHITE KING IS MOVING WK" << endl;
    }
    
    for (auto move: _moves){
        if (move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation()){
            return true;
        }
    }
    
    // cout << "can't move from " << srcSquare.getNotation() << " to " << dstSquare.getNotation() << endl;
    return false;


}


void Chess::bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst){
    Game::bitMovedFromTo(bit, src, dst);
    const char *bpieces = "prnbqk";
    const char *wpieces = "PRNBQK";
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    _lastMove = "x-" + srcSquare.getNotation() + "-" + dstSquare.getNotation();
    // cout << "last move: " << _lastMove << endl;
    _lastMove[0] = (bit.gameTag() < 128) ? wpieces[bit.gameTag() -1 ] : bpieces[bit.gameTag() - 129];
    // cout << "last move: " << _lastMove << endl;

    
    //setting bools for the castleing rules 
    switch (_lastMove[0]){
        case 'K': // if king moves both sides are false 
            this->can_white_castle_kingside = false;
            this->can_white_castle_queenside = false;
            break;
        case 'k': // same as above for black 
            this->can_black_castle_kingside = false;
            this->can_black_castle_queenside = false;
            break;
        case 'R': // if white rook moves set false depending on where they move 
            if (_lastMove[2] == 'a'){
               this->can_white_castle_queenside = false;
            }else if (_lastMove[2] == 'h'){
                this->can_white_castle_kingside = false;

            }
            break;
        case 'r': // same but for white 
            if (_lastMove[1] == 'a'){
                this->can_black_castle_kingside = false;
            }else if (_lastMove[1] == 'h'){
               this->can_black_castle_queenside = false;

            }
            break;
        
    }


    //if is castlemove, aka king moving 2 spaces 
    if (is_castle_move(bit, src, dst)){
        // cout << "castle move" << endl;
        
        perform_castle_move(bit, src, dst);
        dst.dropBitAtPoint(&bit, ImVec2(0, 0));
        src.setBit(nullptr);

    }

    if (is_promotion_move(bit, src, dst)){
        cout << "promotion move" << endl;
        perform_promotion_move(bit, src, dst);
        
    }
    if (is_en_passant_move(bit, src, dst)){
        cout << "en passant move" << endl;
        perform_en_passant_move(bit, src, dst);
    }
    
    else{


        dst.dropBitAtPoint(&bit, ImVec2(0, 0));
        src.setBit(nullptr);


    }

    //was missing these two lines
    
    std::cout << "generating moves" << endl;
    char tempState[64];
    memcpy(tempState, stateString().c_str(), 64);

    _moves = generate_moves( tempState, (_gameOptions.currentTurnNo&1 ) ? 'B' : 'W' , _lastMove, true);



}



bool Chess::is_en_passant_move(Bit& bit, BitHolder& src, BitHolder& dst){
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    // cout << "notions : " << srcSquare.getNotation() << " " << dstSquare.getNotation() << endl;
    string black_pawn = string(1, dstSquare.getNotation()[0]) + string(1, srcSquare.getNotation()[1]);
    // cout << "black_pawn: " << black_pawn << endl;
   //check white en passant 
    if (bit.gameTag() == 1 || bit.gameTag() == 129){
        if (srcSquare.getNotation()[1] == '5' && dstSquare.getNotation()[1] == '6'){
            // cout << "EN PASSANT MOVE" << endl;
            return true;
        }
    }
    //check black enpassant
    if (bit.gameTag() == 1 || bit.gameTag() == 129){
        if (srcSquare.getNotation()[1] == '4' && dstSquare.getNotation()[1] == '3'){
            // cout << "EN PASSANT MOVE BUT FOR BLACK" << endl;
            return true;
        }
    }
    return false;
}

void Chess::perform_en_passant_move(Bit& bit, BitHolder& src, BitHolder& dst){
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    
    //if the white pawn was doing the enpassant
    if (srcSquare.getNotation()[1] == '5' && dstSquare.getNotation()[1] == '6'){
        string blackPawn = string(1, dstSquare.getNotation()[0]) + srcSquare.getNotation()[1];

        // cout << "blackPawn: " << blackPawn << endl;
        BitHolder& blackPawnHolder = _grid[blackPawn[0] - 'a'][blackPawn[1] - '1'];
        
        Bit *blackPawnToDelete = blackPawnHolder.bit();
        // cout << "blackPawnToDelete: " << blackPawnToDelete << endl;
        dst.dropBitAtPoint(&bit, ImVec2(0, 0));
        src.setBit(nullptr);
        if (blackPawnToDelete){
            blackPawnHolder.setBit(nullptr);

        }
      
        

    }

    //if black pawn is doing enpassant
    //3 lastMoveFromRank: 4 lastMoveToFile: 2 lastMoveToRank: 3
    if (srcSquare.getNotation()[1] == '4' && dstSquare.getNotation()[1] == '3'){
        // cout << "we are here AIFHAIHFOIAF" << endl;
        string whitepawn = string(1, dstSquare.getNotation()[0]) + srcSquare.getNotation()[1];
        BitHolder& whitePawnHolder = _grid[whitepawn[0] - 'a'][whitepawn[1] - '1'];
        Bit *whitePawnToDelete = whitePawnHolder.bit();
        dst.dropBitAtPoint(&bit, ImVec2(0, 0));
        src.setBit(nullptr);
        if (whitePawnToDelete){
            whitePawnHolder.setBit(nullptr);

        }
    }
    
}


bool Chess::is_promotion_move(Bit& bit, BitHolder& src, BitHolder& dst){
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    
    
    if (bit.gameTag() == 1 || bit.gameTag() == 129){
        // cout << "DST SQUARE NOTATION" << dstSquare.getNotation() << endl;
        
        if (dstSquare.getNotation()[1] == '8'){
            return true;
        }
    }
    if (bit.gameTag() == 1 || bit.gameTag() == 129){
        if (dstSquare.getNotation()[1] == '1'){
            return true;
        }
    }
    return false;
}

void Chess::perform_promotion_move(Bit& bit, BitHolder& src, BitHolder& dst){
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    dst.dropBitAtPoint(&bit, ImVec2(0, 0));
    Bit *queen = PieceForPlayer(bit.gameTag() < 128 ? 0 : 1, QUEEN); // make new queen bit 
    queen->setPosition(dst.getPosition()); 
    queen->setParent(&dst);
    queen->setGameTag(QUEEN + (bit.gameTag() < 128 ? 0 : 128));
    bit = *queen; // set the pawn bit to point to the queen bit 
    src.setBit(nullptr);
    
    
}

bool Chess::is_castle_move(Bit& bit, BitHolder& src, BitHolder& dst){
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    
    if (bit.gameTag() == 6 || bit.gameTag() == 134){
        if (srcSquare.getNotation() == "e1" && dstSquare.getNotation() == "g1"){
            return true;
        }
        if (srcSquare.getNotation() == "e1" && dstSquare.getNotation() == "c1"){
            return true;
        }
        if (srcSquare.getNotation() == "e8" && dstSquare.getNotation() == "g8"){
            return true;
        }
        if (srcSquare.getNotation() == "e8" && dstSquare.getNotation() == "c8"){
            return true;
        }
    }
    return false;


}

void Chess::perform_castle_move(Bit& bit, BitHolder& src, BitHolder& dst){
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    
    
    if (srcSquare.getNotation() == "e1" && dstSquare.getNotation() == "g1"){
        BitHolder& rookBit = _grid[7][0];
        BitHolder& emptyBit = _grid[5][0];
        ChessSquare &rookSquare = static_cast<ChessSquare&>(rookBit);
        ChessSquare &emptySquare = static_cast<ChessSquare&>(emptyBit);
        Bit *rook = rookSquare.bit();
        
        // cout<<"here"<<endl;
        //move rook to the square 
        emptySquare.dropBitAtPoint(rook, ImVec2(0, 0));
        rookSquare.setBit(nullptr);
    }
    //queen side movement 
    if (srcSquare.getNotation() == "e1" && dstSquare.getNotation() == "c1"){
        BitHolder& rookBit = _grid[0][0];
        BitHolder& emptyBit = _grid[3][0];
        ChessSquare &rookSquare = static_cast<ChessSquare&>(rookBit);
        ChessSquare &emptySquare = static_cast<ChessSquare&>(emptyBit);
        Bit *rook = rookSquare.bit();
        emptySquare.dropBitAtPoint(rook, ImVec2(0, 0));
        rookSquare.setBit(nullptr); 

    }
    //black castle 
    if (srcSquare.getNotation() == "e8" && dstSquare.getNotation() == "g8"){
        BitHolder& rookBit = _grid[7][7];
        BitHolder& emptyBit = _grid[5][7];
        ChessSquare &rookSquare = static_cast<ChessSquare&>(rookBit);
        ChessSquare &emptySquare = static_cast<ChessSquare&>(emptyBit);
        Bit *rook = rookSquare.bit();
        emptySquare.dropBitAtPoint(rook, ImVec2(0, 0));
        rookSquare.setBit(nullptr);


    }
    //black castle queen side 
    if (srcSquare.getNotation() == "e8" && dstSquare.getNotation() == "c8"){
        BitHolder& rookBit = _grid[0][7];
        BitHolder& emptyBit = _grid[3][7];
        ChessSquare &rookSquare = static_cast<ChessSquare&>(rookBit);
        ChessSquare &emptySquare = static_cast<ChessSquare&>(emptyBit);
        Bit *rook = rookSquare.bit();
        emptySquare.dropBitAtPoint(rook, ImVec2(0, 0));
        rookSquare.setBit(nullptr);
    }



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
    // return "8/8/8/8/4p3/8/8/P7 b - - 0 1";

    // return "4k3/1P6/8/8/8/8/K7/8 w - - 0 1";
    // return "5k2/8/8/8/8/8/8/4K2R w K - 0 1"; //white can castle kingside
    // return "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1"; //white can castle queenside
    return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            
    // return "r3k2r/8/8/8/8/8/8/R3K2R w KQ - 0 1"; // black castle

}




//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString()
{
    std::string s;
    for (int y = 0; y < 8; y++) { // Start from the 1st rank and go up to the 8th
        for (int x = 0; x < 8; x++) { // Iterate over the files from 'a' to 'h'
            std::string piece = piece_notation(x, y);
            if (piece[1] == '0') { // If the square is empty
                s += '0';
            } else {
                // If the piece is white, convert to uppercase, else leave it as lowercase
                s += piece[1];
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
            int index = y * _gameOptions.rowX + x;
            int player = s[index]-'0';
            if (player != 0) {
                Bit *bit = PieceForPlayer(player, PAWN);
                if (bit) {
                    _grid[x][y].setBit(bit);
                    bit->setPosition(_grid[x][y].getPosition());
                    bit->setParent(&_grid[x][y]);
                }
                else{
                    _grid[x][y].setBit(nullptr);
                }
            }
         }
    }
}

//yay did this easrly
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

    //changed everything to file and rank due to avoid confusion
    string fenBoard = fen.substr(0, fen.find(" "));
    string playerTurn = fen.substr(fen.find(" ") + 1, 1);
    string castlingRights = fen.substr(fen.find(" ") + 3, 4);
    // cout << "castleing rifhgts "<< castlingRights << endl;
    
    int file = 0;
    int rank = 7;
    for (int i = 0; i < fenBoard.length(); i++){
        if (fenBoard[i] == '/'){
            rank--;
            file = 0;
            continue;
        }

        cout<<"fenBoard[i]: " << fenBoard[i] << endl;
        
        if (isdigit(fenBoard[i])){
            int num = fenBoard[i] - '0';
            file += num;
        }else{
            int player = isupper(fenBoard[i]) ? 0 : 1;
            
            char piece = tolower(fenBoard[i]); // Convert the piece to lowercase to ensure it's found in the map
            Bit *bit = PieceForPlayer(player, (Pieces)pieceMap[piece]);
            if (file < 8 && rank < 8){
                _grid[file][rank].setBit(bit);
                bit->setPosition(_grid[file][rank].getPosition());
                bit->setParent(&_grid[file][rank]);
                if (player == 0){
                    bit->setGameTag(pieceMap[piece]);
                }else{
                    bit->setGameTag(pieceMap[piece] + 128); //set player 1 to be above 128
                }
            }
            file++;
            
        }

        if (file > 8){ //if we have reached the end of the file, move to the next rank
            rank--;
            file = 0;
        }

        
    }

       
        
    char whos_turn = playerTurn[0];
    if (castlingRights[0] == "-"[0]){
        can_white_castle_kingside = false;
    }
    if (castlingRights[1] == "-"[0]){
        can_white_castle_queenside = false;
    }
    if (castlingRights[2] == "-"[0]){
        can_black_castle_kingside = false;
    }
    if (castlingRights[3] == "-"[0]){
        can_black_castle_queenside = false;
    }
    
    
    whos_turn =  putchar(toupper(whos_turn));
    cout << "whos_turn: " << whos_turn << endl;
    char firstState[64];
    memcpy(firstState, stateString().c_str(), 64);
    _moves = generate_moves(firstState, whos_turn, "", true);
    
    // Debug: Print the corrected board state
    string debugState = stateString();
    cout << "Board state: " << debugState << endl;
    
    for (auto move: _moves){

        cout << "from: " << move.from << " to: " << move.to << endl;
    }
    cout << "moves size: " << _moves.size() << endl;
    
}

std::string Chess::piece_notation(int file, int rank) const{
    const char *pieces = {"?PRNBQK"}; //? is for empty square
    const char *lower_pieces = {"?prnbqk"};
    string notation = "";
    Bit *bit = _grid[file][rank].bit();
    if (bit) {
        // cout << "bit->gameTag(): " << bit->gameTag() << endl;
        notation += bit->gameTag() < 128 ? "W" : "B"; //if the game tag is less than 128, it's a white piece
        notation += bit->gameTag() < 128 ? pieces[bit->gameTag() & 127] : lower_pieces[bit->gameTag() & 127];
        // notation += pieces[bit->gameTag() & 127] //and it to get the piece ttype

    }else{
        notation = "00"; //empty square
    }
    return notation;
    


}


void Chess::add_valid_move(std::vector <Move> &moves, int fromFile, int fromRank, int toFile, int toRank){
    if (toFile >= 0 && toFile < 8 && toRank >= 0 && toRank < 8){ //if the move is on the board
        
        moves.push_back({index_to_notation(fromFile, fromRank), index_to_notation(toFile, toRank)});
    }
   
}
std::string Chess::index_to_notation(int file, int rank){
    return std::string(1, 'a' + file) + std::string(1, '1' + rank );

}

int Chess::notation_to_index(std::string& notation){
    int square  = notation[0] - 'a';
    square += (notation[1] - '1') * 8;
    return square;

}



void Chess::generate_knight_moves(std::vector<Move> &moves, int file, int rank, char *state){
    int directions[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}}; // All possible knight moves

    for (int i = 0; i < 8; i++) {
        int newFile = file + directions[i][0];
        int newRank = rank + directions[i][1];
        if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8) {
            char piece = state[newRank * 8 + newFile]; // get piece on new square
            if (piece == '0' || islower(piece) != islower(state[rank * 8 + file])) { // if empty or opponent piece 
                add_valid_move(moves, file, rank, newFile, newRank);
            }
        }
    }
}


void Chess::generate_pawn_moves(std::vector<Move> &moves, int file, int rank, char color, string lastMove, char *state){
    
    
    int direction = color == 'W' ? 1 : -1; //if the color is white, the direction is 1, otherwise it's -1
    int startRank = color == 'W' ? 1 : 6;
    int newRank = rank + direction;
    if (newRank >= 0 && newRank < 8){
        string piece = piece_notation(file, newRank); //get the piece notation of the square in front of the pawn
        if (piece == "00"){ //if the square is empty, add the move
            add_valid_move(moves, file, rank, file, newRank);
            if (rank == startRank){
                newRank = rank + 2 * direction;
                piece = piece_notation(file, newRank);
                if (piece == "00"){
                    add_valid_move(moves, file, rank, file, newRank);
                }
            }
        }
    }
    int directions[2][2] = {{1, 1}, {-1, 1}};
    for (int i = 0; i < 2; i++){
        int newFile = file + directions[i][0];
        int newRank = rank + direction;
        if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8){
            string piece = piece_notation(newFile, newRank);
            if (piece != "00" && piece[0] == opponent_color(color)){
                add_valid_move(moves, file, rank, newFile, newRank);
            }
        }
    }

    // rank is up and down row
    //file is left and right column

    //en passant
    //if pawn is on row 4, and white, then check its position, and the position of the black pawn, which would be left or right of it
    //then let it move diagonally that way

    // bug case is that if a pawn is already on the square, that is 2 moves, and then another pawn makes a 2 move, this allows to go either left or right
    if (lastMove.length()){
        char lastmovePiece = lastMove[0];
        int lastMoveFromRank = lastMove[3] - '0';
        int lastMoveToRank = lastMove[6] - '0';
        int lastMoveFromFile = lastMove[2] - 'a';
        int lastMoveToFile = lastMove[5] - 'a';
        // cout << "lastMoveFromFile: " << lastMoveFromFile << " lastMoveFromRank: " << lastMoveFromRank << " lastMoveToFile: " << lastMoveToFile << " lastMoveToRank: " << lastMoveToRank << endl;

            
        //en passant for white
        if (rank == 4 && color == 'W' && lastMove.length() && lastMoveToRank == 5 && lastMoveFromRank == 7){
            // cout << "checking for en passant" << endl;
            // cout << "pawns file: " << file << " pawns rank: " << rank << endl;
            if (file > 0){
                string piece = piece_notation(file-1, rank);
                //&& lastMoveFromFile == file-1
                if (piece[0] == 'B' && piece[1] == 'P' ){
                    add_valid_move(moves, file, rank, file-1, rank+1);
                }
            }

            if (file < 8){
                string piece = piece_notation(file+1, rank);
                if (piece[0] == 'B' && piece[1] == 'P'){
                    add_valid_move(moves, file, rank, file+1, rank+1);
                }
            }
        }
    
        //en passant for black 
        if (rank == 3 && color == 'B' && lastMove.length() && lastMoveToRank == 4 && lastMoveFromRank == 2) {
            // cout << "checking for black enpasant" << endl;
            // cout << "pawns file: " << file << " pawns rank: " << rank << endl;
            if (file > 0){
                string piece = piece_notation(file-1, rank);
                if (piece[0] == 'W' && piece[1] == 'P'){
                    add_valid_move(moves, file, rank, file-1, rank-1);
                }
            }

            if (file < 8){
                string piece = piece_notation(file+1, rank);
                if (piece[0] == 'W' && piece[1] == 'P'){
                    add_valid_move(moves, file, rank, file+1, rank-1);
                }
            }





        }
    
    
    
    
    
    
    
    }

        
        

    
}

void Chess::generate_linear_moves(std::vector<Move> &moves, int file, int rank,  const std::vector<std::pair<int, int>> &directions, char *state){
    for (const auto &dir : directions) {
        int newFile = file + dir.first;
        int newRank = rank + dir.second;
        while (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8) {
            char piece = state[newRank * 8 + newFile]; // get piece at new square
            if (piece == '0') { // if the square is empty, add the move
                add_valid_move(moves, file, rank, newFile, newRank);
            } else if (islower(piece) != islower(state[rank * 8 + file])) { // if piece on the square is of different color
                add_valid_move(moves, file, rank, newFile, newRank);
                break; // stop if an opponent's piece is encountered
            } else { // if piece on the square is of the same color
                break; // stop as further movement in this direction is blocked
            }
            newFile += dir.first;
            newRank += dir.second;
        }
    }
}

void Chess::generate_bishop_moves(std::vector<Move> &moves, int file, int rank, char *state){

    static const std::vector<std::pair<int, int>> directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    generate_linear_moves(moves, file, rank, directions, state);
}

void Chess::generate_rook_moves(std::vector<Move> &moves, int file, int rank,char *state){
    static const std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    generate_linear_moves(moves, file, rank, directions, state);
}

void Chess::generate_queen_moves(std::vector<Move> &moves, int file, int rank,char *state){
    static const std::vector<std::pair<int, int>> directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    generate_linear_moves(moves, file, rank, directions, state);

}

void Chess::generate_king_moves(std::vector<Move> &moves, int file, int rank,char *state){
    static const std::vector<std::pair<int, int>> directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    for (auto& dir: directions){
        int newFile = file + dir.first;
        int newRank = rank + dir.second;
        if (newFile >= 0 && newFile < 8 && newRank >=0 && newRank < 8){
            string piece = piece_notation(newFile, newRank);
            if (piece == "00" || piece[0] == opponent_color(piece_notation(file, rank)[0])){
                add_valid_move(moves, file, rank, newFile, newRank);
            }
        }

        //castling for white
        // cout << "file: " << file << " rank: " << rank << " can_white_castle_kingside: " << can_white_castle_kingside << " can_white_castle_queenside: " << can_white_castle_queenside << endl;
        if (file == 4 && rank == 0){
            if (dir.first == 1 && dir.second == 0 && this->can_white_castle_kingside){
                if (piece_notation(5, 0) == "00" && piece_notation(6, 0) == "00"){
                    add_valid_move(moves, file, rank, 6, 0);
                }
            }
            if (dir.first == -1 && dir.second == 0 && this->can_white_castle_queenside){
                if (piece_notation(3, 0) == "00" && piece_notation(2, 0) == "00" && piece_notation(1, 0) == "00"){
                    add_valid_move(moves, file, rank, 2, 0);
                }
            }
        }
        
        if (file == 4 && rank == 7 ){
            if (dir.first == 1 && dir.second == 0 && this->can_black_castle_kingside){
                if (piece_notation(5, 7) == "00" && piece_notation(6, 7) == "00"){
                    add_valid_move(moves, file, rank, 6, 7);
                }
            }
            if (dir.first == -1 && dir.second == 0 && this->can_black_castle_queenside){
                if (piece_notation(3, 7) == "00" && piece_notation(2, 7) == "00" && piece_notation(1, 7) == "00"){
                    add_valid_move(moves, file, rank, 2, 7);
                }
            }





        }
    }

}

char Chess::opponent_color(char color) {
    return (color == 'W' ) ? 'B' : 'W';
}




std::vector<Chess::Move> Chess::generate_moves(char *state, char color, string _lastMove, bool filter){
    std::vector<Move> moves;
    int colorAsInt = color == 'W' ? 1 : -1;
    // cout << "are we aborting here" << endl;
    for (int i = 0; i < 64; i++){
        int file = i % 8;
        int rank = i / 8;
        char piece = state[i];
        int pieceColor = (piece == '0') ? 0 : (piece < 'a' ? 1 : -1);
        if (pieceColor == colorAsInt){
            // string piece = piece_notation(file, rank);
            // cout << "piece: " << piece << endl;
            if (piece > 'a') piece = piece - ('a' - 'A');
            switch (piece){
                case 'P':
                    generate_pawn_moves(moves, file, rank, color, _lastMove, state);
                    break;
                case 'p':
                    generate_pawn_moves(moves, file, rank, color, _lastMove, state);
                    break;
                
                case 'R':
                    generate_rook_moves(moves, file, rank, state);
                    break;
                case 'r':
                    generate_rook_moves(moves, file, rank, state);
                    break;

                case 'N':
                    generate_knight_moves(moves, file, rank, state);
                    break;
                case 'n':
                    generate_knight_moves(moves, file, rank, state);
                    break;
                case 'B':
                    generate_bishop_moves(moves, file, rank, state);
                    break;
                case 'b':
                    generate_bishop_moves(moves, file, rank, state);
                    break;
                case 'Q':
                    generate_queen_moves(moves, file, rank, state);
                    break;
                case 'q':
                    generate_queen_moves(moves, file, rank, state);
                    break;
                case 'K':
                    generate_king_moves(moves, file, rank, state);
                    break;
                case 'k':
                    generate_king_moves(moves, file, rank, state);
                    break;
            }
        }
    }
    if (filter){
        filterOutIllegalMoves(color, moves, _lastMove);
    }
    // cout << "moves size: " << moves.size() << endl;

    if (moves.size() == 0 && !isKingInCheck(state, color)){
        cout << "stalemate" << endl;
        cout << "winner is none of you " << endl;
    }

    if (moves.size() == 0){ 
        cout << "checkmate" << endl;
        cout << "winner is " << (color == 'W' ? "black" : "white") << endl;

    }
    return moves;
    
}

static map <char, int> scores = {
        {'P', 100}, {'p', -100},
        {'N', 200}, {'n', -200},
        {'B', 230}, {'b', -230},
        {'R', 400}, {'r', -400},
        {'Q', 900}, {'q', -900},
        {'K', 2000}, {'k', -2000},
        {'0', 0}
    };


int Chess::evaluateBoard(const char *state){
    
    int score = 0;
    // cout << "len of state: " << strlen(state) << endl;
    for (int i = 0; i < 64; i++ ){
        score += scores[state[i]];
    }
    
    for (int i = 0; i < 64; i++){
       char piece = state[i];
       int j = FLIP(i);
       switch (piece) {
            case 'N':
                score+=knightTable[j];
                break;
            case 'n':
                score -= knightTable[FLIP(j)];
                break;
            case 'P':
                score += pawnTable[j];
                break;
            case 'p':
                score -= pawnTable[FLIP(j)];
                break;
            case 'B':
                score += bishopTable[j];
                break;
            case 'b':
                score -= bishopTable[FLIP(j)];
                break;
            case 'R':
                score += rookTable[j];
                break;
            case 'r':
                score -= rookTable[FLIP(j)];
                break;
            case 'Q':
                score += queenTable[j];
                break;
            case 'q':
                score -= queenTable[FLIP(j)];
                break;
            case 'K':
                score += kingTable[j];
                break;
            case 'k':
                score -= kingTable[FLIP(j)];
                break;
        

       }
       


    }
    
    return score;
}

int Chess::negamax(char *state, int depth, int playerColor, int alpha, int beta){

    // cout << "negamax started" << endl;
    // cout << "negamax started" << endl;
    _countSearch++;
    
    // Generate moves for current position
    auto negamoves = generate_moves(state, playerColor == 1 ? 'W' : 'B', _lastMove, true);
    
    // Check for terminal positions (checkmate/stalemate)
    if (negamoves.size() == 0) {
        if (isKingInCheck(state, playerColor == 1 ? 'W' : 'B')) {
            // Checkmate - return very negative score (adjusted for depth to prefer shorter mates)
            return -999999 + depth;
        } else {
            // Stalemate - return draw score
            return 0;
        }
    }
    
    if (depth == 0){
        int score = evaluateBoard(state);
        // cout << score << endl;
        int newScore =  playerColor * score;
        return newScore;
    }

    int bestVal = -99999999;
    // cout << "last move" <<_lastMove << endl;
    // cout  << "negamoves size: " << negamoves.size() << endl;

    // Order moves for better alpha-beta pruning
    if (depth > 1) {
        orderMoves(negamoves, state);
    }

    for (auto move : negamoves){
        
        int srcSquare = notation_to_index(move.from);
        int dstSquare = notation_to_index(move.to);
        char saveMove = state[dstSquare];
        state[dstSquare] = state[srcSquare];
        state[srcSquare] = '0';
        // int score = -negamax(state,depth-1, -playerColor, -beta, -alpha);
        bestVal = max(bestVal, -negamax(state, depth-1, -playerColor, -beta, -alpha));
        state[srcSquare] = state[dstSquare];
        state[dstSquare] = saveMove;
        // bestVal = max(bestVal, score);
        alpha = max(alpha, bestVal);
        if (alpha >= beta){
            // cout << "alpha: " << alpha << " beta: " << beta << endl;
            break;
        }


        
    }
    // cout << "negamax ended" << endl;
    return bestVal;
}

// Simple move ordering: prioritize captures and checks
void Chess::orderMoves(std::vector<Move> &moves, char *state) {
    // Sort moves to try captures first, then other moves
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        int aSquare = notation_to_index(const_cast<std::string&>(a.to));
        int bSquare = notation_to_index(const_cast<std::string&>(b.to));
        
        // Prioritize captures (target square has a piece)
        bool aIsCapture = state[aSquare] != '0';
        bool bIsCapture = state[bSquare] != '0';
        
        if (aIsCapture && !bIsCapture) return true;
        if (!aIsCapture && bIsCapture) return false;
        
        // If both or neither are captures, prioritize central squares
        int aCentralScore = 0, bCentralScore = 0;
        int aFile = aSquare % 8, aRank = aSquare / 8;
        int bFile = bSquare % 8, bRank = bSquare / 8;
        
        // Simple central square bonus
        if (aFile >= 2 && aFile <= 5 && aRank >= 2 && aRank <= 5) aCentralScore = 1;
        if (bFile >= 2 && bFile <= 5 && bRank >= 2 && bRank <= 5) bCentralScore = 1;
        
        return aCentralScore > bCentralScore;
    });
}




//dont need this anymore
vector<int> Chess::calculateLineOfAttack(int kingSquare, int enemySquare){
    vector<int> lineOfAttack;
    int fileDiff = (enemySquare & 7) - (kingSquare & 7);
    int rankDiff = (enemySquare / 8) - (kingSquare / 8);
    int fileDirection = fileDiff > 0 ? 1 : -1;
    int rankDirection = rankDiff > 0 ? 1 : -1;
    int file = kingSquare & 7;
    int rank = kingSquare / 8;
    while (file != (enemySquare & 7) || rank != (enemySquare / 8)){
        if (file != (enemySquare & 7)) {
            file += fileDirection;
        }
        if (rank != (enemySquare / 8)) {
            rank += rankDirection;
        }
        lineOfAttack.push_back(file + rank * 8);
    }
    return lineOfAttack;
}


bool Chess::pieceCanAttack(char* state,  int targetSquare, char color) {
    char enemyColor = (color == 'W') ? 'B' : 'W';
    auto enemyMoves = generate_moves(state, enemyColor, "", false);

    // check if any of the enemy moves target the kings square
    for ( auto move : enemyMoves) {
        if (notation_to_index(move.to) == targetSquare) {
            return true; //can attack king 
        }
    }

    return false; //cannot attack king
}




bool Chess::isKingInCheck(char* state, char color) {
    char king = (color == 'W') ? 'K' : 'k';
    int kingSquare = -1;
    for (int i = 0; i < 64; ++i) {
        if (state[i] == king) {
            kingSquare = i;
            break;
        }
    }

    for (int i = 0; i < 64; ++i) {
        char piece = state[i];
        if (piece != '0' && (color == 'W' ? islower(piece) : isupper(piece))) {
            // check if the opponent's piece can attack the king square
            if (pieceCanAttack(state, kingSquare, color)) {
                return true;
            }
        }
    }
    
    //king is not in check
    return false;
}

bool Chess::isMoveValid(char* state, int srcSquare, int dstSquare, int enemysrc, int enemydst, char color) {
    char tempState[65];
    string copyState = std::string(stateString().c_str());

    memcpy(&tempState[0], copyState.c_str(), 64);

    tempState[dstSquare] = tempState[srcSquare];
    tempState[srcSquare] = '0';

    // generate all possible moves for the enemy
    auto enemyMoves = generate_moves(tempState, color == 'W' ? 'B' : 'W', "", false);

    // Check if the enemy can still move to the same square
    for (auto move : enemyMoves) {
        if (notation_to_index(move.to) == enemydst) {
            return true;
        }
    }

    // if the enemy cannot move to the same square, the move is valid
    return false;
}

void Chess::filterOutIllegalMoves(char color, vector<Move> &moves, string _lastMove){
    //find where king is first
    char basestate[65];
    string copyState = stateString();
    // cout << "copyState: " << copyState << endl;
    int kingSquare = -1;
    for (int i = 0; i < 64; i ++){
        if (copyState[i] == 'k' && color == 'B') { kingSquare = i; break; }
        if (copyState[i] == 'K' && color == 'W') { kingSquare = i; break;}
    }

    // cout << "kingSquare: " << kingSquare << endl;
    for (auto it = moves.begin() ; it != moves.end();) {
        bool moveisBad = false;

        memcpy(&basestate[0], copyState.c_str(), 64);
        
        int srcSquare = notation_to_index(it->from);
        int dstSquare = notation_to_index(it->to);
        // cout << "base state: " << basestate << endl;
        basestate[dstSquare] = basestate[srcSquare];
        basestate[srcSquare] = '0';
        // cout << "updated base state: " << basestate << endl;
        int updatedKingSquare = kingSquare;
        if (basestate[dstSquare] == 'k' && color == 'B')
        {
            updatedKingSquare = dstSquare;
        }
        else if (basestate[dstSquare] == 'K' && color == 'W')
        {
            updatedKingSquare = dstSquare;
        }

        
        // vector<int> lineOfAttack;
        auto generateMovesforOpposite = generate_moves(basestate, color == 'W' ? 'B' : 'W', _lastMove, false);
        for (auto enemyMove: generateMovesforOpposite){
            int enemyDstSquare = notation_to_index(enemyMove.to);
            
            if (enemyDstSquare == updatedKingSquare && isMoveValid(basestate , srcSquare, dstSquare,  notation_to_index(enemyMove.from), updatedKingSquare, color)){
                // cout<< "king is in check" << endl;
                //find enemy path to king, and put every square in the line of attack
                // lineOfAttack = calculateLineOfAttack(updatedKingSquare, notation_to_index(enemyMove.from)); // dont really need this anymore 
                moveisBad = true;
                break;
            }
            

        }
        
        if (moveisBad){
            it = moves.erase(it);
        }else{
            ++it;
        }
    }

    
}
            
void Chess::updateAI_MultiThreaded(){
    // Get the correct player color for the AI
    int currentPlayerNumber = getCurrentPlayer()->playerNumber();
    int aiPlayerColor = (currentPlayerNumber == 0) ? 1 : -1;  // White = 1, Black = -1
    char baseState[65];
    string _state = stateString();

    // Generate all moves
    // auto moves = generate_moves(_state, aiPlayerColor == 1 ? 'W' : 'B', _lastMove, true);

    //_moves contains all the moves for the current player
    auto moves = _moves;

    //split moves into N chunks, and search each chunk in a separate thread
    const int numThreads = 4;
    const int movesPerThread = (moves.size() + numThreads - 1) / numThreads;

    vector<thread> threads;
    vector<ThreadSearchResult> results(numThreads);

    //start threads
    for (int i = 0; i < numThreads; ++i){
        threads.emplace_back([this, aiPlayerColor, &moves, i, movesPerThread, &results, _state]() {
            results[i] = searchMovesInRange(moves, i * movesPerThread, (i + 1) * movesPerThread, aiPlayerColor, 5, _state  );
        });
    }

    //wait for all threads to finish
    for (auto& thread : threads){
        thread.join();

    }
    cout << "results size: " << results.size() << endl;
    // assert(results.size() >= 1);

    //find the best move from all the results
    Move bestMove;
    int bestValue = -99999999;
    for (const auto& result : results){
        cout << "result.bestValue: " << result.bestValue << endl;
        if (result.bestValue > bestValue){
            cout << "result.bestValue: " << result.bestValue << endl;
            bestValue = result.bestValue;

            bestMove = result.bestMove;
            cout << "bestMove: " << bestMove.from << " to " << bestMove.to << endl;
        }
    }

    //make the best move
    int srcSquare = notation_to_index(bestMove.from);
    int dstSquare = notation_to_index(bestMove.to);
    
    BitHolder &src =  getHolderAt(srcSquare&7, srcSquare/8);
    BitHolder &dst = getHolderAt(dstSquare&7, dstSquare/8);
    
    Bit *bit = src.bit();
    dst.dropBitAtPoint(bit, ImVec2(0, 0));
    src.setBit(nullptr);
    bitMovedFromTo(*bit, src, dst);
    _gameOptions.AIPlaying = false;
    endTurn();

}

Chess::ThreadSearchResult Chess::searchMovesInRange(vector<Move>& moves, int start, int end, int aiPlayerColor, int depth, string currentState){
    //this function is to write into the results vector the best move and value for the moves in the range? 
    int localBestValue = -99999999;
    Move localBestMove;
    int localCountSearch = 0;
    char localBoard[65];
    string copyLocalBoard = currentState; // FIX: Use the passed parameter instead of stateString()
    int myinfinity = 99999999;

    //endIndex is int, moves.size() is unsigned int, so we need to cast it to int
    int endIndex = min(end, static_cast<int>(moves.size()));
    for (int i = start; i < endIndex; ++i){
        memcpy(&localBoard[0], copyLocalBoard.c_str(), 64);
        int srcSquare = notation_to_index(moves[i].from);
        int dstSquare = notation_to_index(moves[i].to);
        if (srcSquare > 64 || dstSquare > 64){
            continue;
        }
        
        localBoard[dstSquare] = localBoard[srcSquare];
        localBoard[srcSquare] = '0';

        localCountSearch = 0;
        int negaMaxValue = negamax(localBoard, depth, aiPlayerColor, -myinfinity, myinfinity);
        
        if (negaMaxValue > localBestValue){
            localBestValue = negaMaxValue;
            localBestMove = moves[i];
        }
    }
    return ThreadSearchResult{localBestMove, localBestValue};
}

    
void Chess::updateAI(){

    char baseState[65];
    const int myinfinity = 99999999;
    Move bestMove;
    string copyState = stateString();
    
    // Get the correct player color for the AI
    int currentPlayerNumber = getCurrentPlayer()->playerNumber();
    int aiPlayerColor = (currentPlayerNumber == 0) ? 1 : -1;  // White = 1, Black = -1
    
    int bestMoveScore = (aiPlayerColor == 1) ? -myinfinity : myinfinity;  // White starts low, Black starts high
    
    cout << "AI Turn: Player " << currentPlayerNumber << " -> aiPlayerColor " << aiPlayerColor << " (" << (aiPlayerColor == 1 ? "White" : "Black") << ")" << endl;
    
    for (auto move: _moves){
        memcpy(&baseState[0], copyState.c_str(), 64);
        // cout << "Processing move: " << move.from << " to " << move.to << endl;
        
        int srcSquare = notation_to_index(move.from);
        int dstSquare = notation_to_index(move.to);
        if (srcSquare > 64){
            // cout << "I am here" << endl;
            return;
        }
        baseState[dstSquare] = baseState[srcSquare];
        baseState[srcSquare] = '0';
        
        _countSearch = 0;
        // cout << "Calling negamax for move: " << move.from << " to " << move.to << endl;
        
        int bestValue = negamax(baseState, 3, aiPlayerColor, -myinfinity, myinfinity);
        
        cout << "Move " << move.from << " to " << move.to << " -> Score: " << bestValue << endl;

        // For White (aiPlayerColor = 1): want HIGHER scores
        // For Black (aiPlayerColor = -1): want LOWER scores  
        bool isNewBest = (aiPlayerColor == 1) ? (bestValue > bestMoveScore) : (bestValue < bestMoveScore);
        
        if (isNewBest){
            bestMoveScore = bestValue;
            bestMove = move;
            cout << "  NEW BEST MOVE! Score: " << bestMoveScore << endl;
        }
    }
    #if defined(UCI_INTERFACE)
    int file = bestMove.from[0] - 'a';
    int rank = bestMove.from[1] - '1';
    int toFile = bestMove.to[0] - 'a';
    int toRank = bestMove.to[1] - '1';
    std::string bestUCIMove = index_to_notation(file, rank);
    bestUCIMove += std::string(index_to_notation(toFile, toRank));
//    if (bestMove.flags & MoveFlags::IsPromotion) {
//        bestUCIMove += "q";
//    }
    uciInterface.SendMove(bestUCIMove);
#else
    std::cout << "searched " << _countSearch << " nodes" << std::endl;
#endif

    // cout << "Searched " << _countSearch << " nodes" << endl;
    
    if (bestMoveScore != -myinfinity){
        
        int srcSquare = notation_to_index(bestMove.from);
        int dstSquare  = notation_to_index(bestMove.to);
        
        // cout << "Best move found: " << bestMove.from << " to " << bestMove.to << endl;
        // cout << "srcSquare: " << srcSquare << ", dstSquare: " << dstSquare << endl;
        
        BitHolder &src =  getHolderAt(srcSquare&7, srcSquare/8);
        BitHolder &dst = getHolderAt(dstSquare&7, dstSquare/8);
        
        Bit *bit = src.bit();
        dst.dropBitAtPoint(bit, ImVec2(0, 0));
        src.setBit(nullptr);
        bitMovedFromTo(*bit, src, dst);
        _gameOptions.AIPlaying = false;
        endTurn();
        // cout << stateString() << endl;

        }

  



}
    
    
    

void Chess::UCIMove(const std::string& move) {
#if defined(UCI_INTERFACE)
    int fromCol = move[0] - 'a';
    int fromRow = move[1] - '1';
    int toCol = move[2] - 'a';
    int toRow = move[3] - '1';
    BitHolder& src = getHolderAt(fromCol, fromRow);
    BitHolder& dst = getHolderAt(toCol, toRow);
    Bit* bit = src.bit();
    if (bit) {
        if (dst.bit()) {
            pieceTaken(dst.bit());
        }
        dst.dropBitAtPoint(bit, ImVec2(0, 0));
        src.setBit(nullptr);
        // this also calls endTurn
        bitMovedFromTo(*bit, src, dst);
        uciInterface.UCILog("Chess::UCIMove: " + move);
    }
#endif
}
    
    
    
    
    
    
    
    
    
    
    
    
    
    