#include "Application.h"
#include <iostream>
#include <string>
#include <cstring>
#include "imgui/imgui.h"
#include "classes/Chess.h"
#include "classes/Log.h"
#include "classes/ChessSquare.h"

using namespace std;

namespace ClassGame {
        //
        // our global variables
        //
        Chess *game = nullptr;
        int gameWinner = -1;
        Log log = Log();
        bool selectedColor = false;
        bool gameOver = false;
        int AIPlayer = 1;





        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = new Chess();
            game->setUpBoard();
            gameWinner = -1;
            game->getCurrentPlayer();
            cout << "game current player" << game->getCurrentPlayer()->playerNumber() << endl;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
#if defined(UCI_INTERFACE)
            if (!selectedColor)
            {
                AIPlayer = 1;
                selectedColor = true;
                game = new Chess();
                game->_gameOptions.AIPlayer = AIPlayer;
                game->setUpBoard();
            }
            if (game->gameHasAI() && game->getCurrentPlayer()->isAIPlayer())
            {
                game->updateAI();
            }
            game->drawFrame();
#else
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
                log.displayLog();

                ImGui::Begin("Settings");
                ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                ImGui::Text("Current Board State: %s", game->stateString().c_str());
                if (game->checkForDraw()) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Draw!");
                } else {
                    if (gameWinner != -1) {
                        ImGui::Text("Game Over!");
                        ImGui::Text("Winner: %d", gameWinner);
                    }
                }
                if (ImGui::Button("Reset Game")) {
                    game->stopGame();
                    game->setUpBoard();
                    gameWinner = -1;
                }

                if (ImGui::Button("AI Game")) {
                    game->stopGame();
                    game->setUpBoard();
                    gameWinner = -1;
                    game->setAIPlayer(1);
                    // game->setAIPlayer(2);
                    log.log(Log::INFO, "AI Game");
                    cout << "game current player" << game->getCurrentPlayer()->playerNumber() << endl;
                    cout << game->gameHasAI() << endl;






                    
                }
                if (ImGui::Button("Human Game")) {
                    game->stopGame();
                    game->setUpBoard();
                    gameWinner = -1;
                    log.log(Log::INFO, "Human Game");




                    
                }
                
                
                
                ImGui::End();


                


                if (game->gameHasAI() && game->getCurrentPlayer()->isAIPlayer())
                {

                    // Player *winner = game->checkForWinner();
                    // if (!winner)
                    // {
                        cout<<"AI is thinking..."<<endl;
                        // log.log(Log::INFO, "AI is thinking...");
                        // game->AI_Move(game, 2, 1);
                        cout << "Before updateAI()" << endl;

                        // game->updateAI();
                        game->updateAI_MultiThreaded();
                        
                        cout << "made a move" << endl;
                        game->endTurn();
                    // }
                    
                }

                ImGui::Begin("GameWindow");
                game->drawFrame();
                ImGui::End();

#endif        
        }


        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameWinner = winner->playerNumber();
                
            }
        }
}
