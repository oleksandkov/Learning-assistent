#include <iostream>
#include "Game.h"


// using namespace sf;

int main() {

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
