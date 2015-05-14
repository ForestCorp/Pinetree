//
//  main.h
//  Pinetree
//
//  Created by Felix Kranich on 08.05.15.
//
//

#ifndef Pinetree_main_h
#define Pinetree_main_h
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "res_path.h"
#include "cleanup.h"
class Main {

   
   Main(){}
public:
   SDL_Renderer* renderer = nullptr;
   void renderText(const std::string &message, const std::string &fontFile, SDL_Color color, int fontSize, SDL_Renderer *renderer);

};

#endif
