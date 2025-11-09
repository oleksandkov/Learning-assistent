#include <iostream>
#include "Game.h"


// using namespace sf;

int main() {

    //Init srand
    std::srand(static_cast<unsigned>(time(NULL)));
    //Init game engine
    Game game;


    // Game loop

    while (game.getWindowIsOpen())
    {
       
        //Update
        game.update();

        //Render
        game.render();

        



    }


    // End 
    return 0;
}
