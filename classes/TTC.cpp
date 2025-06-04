#include "TTC.h"
#include "Square.h"
#define HUMAN -1
#define AI 1

TicTacToe::TicTacToe() : log(Log()) 
{



}

TicTacToe::~TicTacToe()
{
}

//
// make a rock, paper, or scissors piece for the player
//
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    
    cout << playerNumber << " playerNumber" << endl;
    const char *textures[] = { "o.png", "x.png"};
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    cout << "here" << endl;
    bit->LoadTextureFromFile(textures[playerNumber]);
    cout << "WE here" << endl;

    Player *currentPlayer = getCurrentPlayer();
    if (currentPlayer) {
        bit->setOwner(currentPlayer);
        bit->setGameTag(currentPlayer->playerNumber() + 1);
    } else {
    
        cout << "we are at the else"<< endl;
    
    }
    // bit->setOwner(getCurrentPlayer());
    cout << "we out here" << endl;

    bit->setGameTag(playerNumber+1);
    cout << "HRE AHGA" << endl;

    bit->setSize(pieceSize, pieceSize);
    cout << "retuning bit" << endl;
    return bit;
}

void TicTacToe::setUpBoard()
{
    srand((unsigned int)time(0));
    setNumberOfPlayers(2);

     // this allows us to draw the board correctly
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;
    // setup the board
    for (int x=0; x<_gameOptions.rowX; x++) {
        // _grid[x].initHolder(ImVec2(100*(float)x, 100 + 100), "square.png", x, 0);
        for (int y=0; y<_gameOptions.rowY; y++) {
            log.log(Log::INFO, "hi");
            cout << "x: " << x << " y: " << y << endl;
            _grid[x][y].initHolder(ImVec2(100+ 100*(float)x, 100*(float)y+ 100), "square.png", x, y);

        }
    }   

    
    // if we have an AI set it up
    if (gameHasAI())
    {
        setAIPlayer(_gameOptions.AIPlayer);
    }
    // setup up turns etc.
    startGame();
}

//
// about the only thing we need to actually fill out for rock paper scissors
//
bool TicTacToe::actionForEmptyHolder(BitHolder& holder)
{
    if (holder.bit()) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (bit) {
        bit->setPosition(holder.getPosition());
        holder.setBit(bit);
        endTurn();
        return true;
    }   
    return false;
}




TicTacToeAI* TicTacToe::clone()
{
    TicTacToeAI *newGame = new TicTacToeAI();
    std::string gamestate = stateString();
    cout << gamestate << " gamestate" << endl;

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            int index = x * 3 + y;
            cout << index << " index" << endl;
            int playerNumber = gamestate[index] - '0';
            newGame->_grid[x][y] = playerNumber == 1 ? HUMAN : playerNumber == 2 ? AI : 0;
        }
    }

    return newGame;
}

int TicTacToeAI::ownerAt(int index ) const
{
    
    return _grid[index/3][index%3];
  
}


void TicTacToe::AI_Move(TicTacToe* state, int depth, int color)
{
    
    //negamax
    cout << "AI Move" << endl;
    int bestVal = -1000;
    Square* bestMove = nullptr;
    cout<<"AI_Move2"<<endl;
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            if (!_grid[x][y].bit()){
                cout<<"AI_Move3"<<endl;
                _grid[x][y].setBit(PieceForPlayer(AI));
                TicTacToeAI* newState = this->clone();
                cout<<"AI_Move4"<<endl;

                int moveVal= -newState->negamax(newState, depth+1, -1);

                delete newState;
                cout << "undoing move" << endl;
                state->_grid[x][y].setBit(nullptr);

                if (moveVal > bestVal)
                {
                    bestMove = &state->_grid[x][y];
                    bestVal = moveVal;
                }

               
            }
        }

    }
    if (bestMove){
        if (actionForEmptyHolder(*bestMove)){
            endTurn();
            _gameOptions.AIPlaying = false;
        }
    }


}


int TicTacToeAI::checkForWinnerAI(){
    cout<<"checkForWinner"<<endl;
    static const int Winningcombinations[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };
    for (int i = 0; i < 8; i++){
        const int *triple = Winningcombinations[i];
        int playerInt = ownerAt(triple[0]);
        if ( playerInt != 0 && playerInt == ownerAt(triple[1]) && playerInt == ownerAt(triple[2])){
            cout << playerInt << endl;
            return playerInt;
        }
    
    
    }

    cout << "returning 0" << endl;
    return 0;




}


int TicTacToeAI::evaluateBoard()
{
    
    cout<<"AI PLAYER IS "<<AI <<endl;

    int winner = checkForWinnerAI();
    cout << "winner is " << winner << endl;
    if (winner == 0){
        return 0;
    }
    if (winner == AI){

        return 10;
    }
    
    if (winner == HUMAN){
        return -10;
    }




}

bool TicTacToeAI::isBoardFull() const
{
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            if (!_grid[x][y]){
                return false;
            }
        }
    }
    return true;
}

int TicTacToeAI::negamax(TicTacToeAI* state, int depth, int color){
    cout<<"negamax"<<endl;
    int score = evaluateBoard();
    cout << "score" << score << endl;
    if (score != 0) return score * color;
    if (isBoardFull()) return 0;
    // if (state->checkForDraw()) return 0;

    cout<<"negamax2"<<endl;

    int bestVal = -1000;
    for (int x = 0; x < 3; x++){
        for (int y  = 0; y < 3; y++){
            if (state->_grid[x][y] == 0){
                state->_grid[x][y] = color;
                bestVal = max(bestVal, -negamax(state, depth+1, -color));
                state->_grid[x][y] = 0;
            }

    
        }
    }


    return bestVal;
}




bool TicTacToe::canBitMoveFrom(Bit& bit, BitHolder& src)
{
    // you can't move anything in rock paper scissors
    return true;
}

bool TicTacToe::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    // you can't move anything in rock paper scissors
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame()
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
Player* TicTacToe::ownerAt(int index ) const
{
    if (!_grid[index/3][index%3].bit()) {
        return nullptr;
    }
    return _grid[index/3][index%3].bit()->getOwner();
}


Player* TicTacToe::checkForWinner()
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


bool TicTacToe::checkForDraw()
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
std::string TicTacToe::initialStateString()
{
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString()
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
void TicTacToe::setStateString(const std::string &s)
{
    for (int x=0; x<_gameOptions.rowX; x++) {
         for (int y=0; y<_gameOptions.rowY; y++) {
            _grid[x][y].setBit(PieceForPlayer(0) );
         }
    }
}


