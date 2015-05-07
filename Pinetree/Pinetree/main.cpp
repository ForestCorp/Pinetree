#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "res_path.h"
#include "cleanup.h"

/*
 * Lesson 6: True Type Fonts with SDL_ttf
 */
//Screen attributes
const int SCREEN_WIDTH  = 2560;
const int SCREEN_HEIGHT = 1440;

/*
 * Log an SDL error with some error message to the output stream of our choice
 * @param os The output stream to write the message too
 * @param msg The error message to write, format will be msg error: SDL_GetError()
 */
void logSDLError(std::ostream &os, const std::string &msg){
   os << msg << " error: " << SDL_GetError() << std::endl;
}
/*
 * Draw an SDL_Texture to an SDL_Renderer at some destination rect
 * taking a clip of the texture if desired
 * @param tex The source texture we want to draw
 * @param rend The renderer we want to draw too
 * @param dst The destination rectangle to render the texture too
 * @param clip The sub-section of the texture to draw (clipping rect)
 *		default of nullptr draws the entire texture
 */
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *clip = nullptr){
   SDL_RenderCopy(ren, tex, clip, &dst);
}

/*
 * Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
 * the texture's width and height and taking a clip of the texture if desired
 * If a clip is passed, the clip's width and height will be used instead of the texture's
 * @param tex The source texture we want to draw
 * @param rend The renderer we want to draw too
 * @param x The x coordinate to draw too
 * @param y The y coordinate to draw too
 * @param clip The sub-section of the texture to draw (clipping rect)
 *		default of nullptr draws the entire texture
 */
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip = nullptr){
   SDL_Rect dst;
   dst.x = x;
   dst.y = y;
   if (clip != nullptr){
      dst.w = clip->w;
      dst.h = clip->h;
   }
   else {
      SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
   }
   renderTexture(tex, ren, dst, clip);
}
/*
 * Render the message we want to display to a texture for drawing
 * @param message The message we want to display
 * @param fontFile The font we want to use to render the text
 * @param color The color we want the text to be
 * @param fontSize The size we want the font to be
 * @param renderer The renderer to load the texture in
 * @return An SDL_Texture containing the rendered message, or nullptr if something went wrong
 */
SDL_Texture* renderText(const std::string &message, const std::string &fontFile, SDL_Color color,
                        int fontSize, SDL_Renderer *renderer)
{
   //Open the font
   TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
   if (font == nullptr){
      logSDLError(std::cout, "TTF_OpenFont");
      return nullptr;
   }
   //We need to first render to a surface as that's what TTF_RenderText returns, then
   //load that surface into a texture
   SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
   if (surf == nullptr){
      TTF_CloseFont(font);
      logSDLError(std::cout, "TTF_RenderText");
      return nullptr;
   }
   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
   if (texture == nullptr){
      logSDLError(std::cout, "CreateTexture");
   }
   //Clean up the surface and font
   SDL_FreeSurface(surf);
   TTF_CloseFont(font);
   return texture;
}
SDL_Texture* drawText(const std::string &message, const std::string &fontFile, SDL_Color color,
                      int fontSize, SDL_Renderer *renderer, int x, int y) {
   SDL_Texture* t = renderText(message,getResourcePath("pinetree")+fontFile, color, 64, renderer);
   renderTexture(t, renderer, x, y);
   return t;
}

auto timePrev = std::chrono::high_resolution_clock::now();

// Returns time since last time this function was called in seconds with nanosecond precision
double GetDelta()
{
   // Gett current time as a std::chrono::time_point
   // which basically contains info about the current point in time
   auto timeCurrent = std::chrono::high_resolution_clock::now();
   
   // Compare the two to create time_point containing delta time in nanosecnds
   auto timeDiff = std::chrono::duration_cast< std::chrono::nanoseconds >( timeCurrent - timePrev );
   
   // Get the tics as a variable
   double delta = timeDiff.count();
   
   // Turn nanoseconds into seconds
   delta /= 1000000000;
   
   timePrev = timeCurrent;
   return delta;
}

int main(int, char**){
   //Start up SDL and make sure it went ok
   if (SDL_Init(SDL_INIT_VIDEO) != 0){
      logSDLError(std::cout, "SDL_Init");
      return 1;
   }
   //Also need to init SDL_ttf
   if (TTF_Init() != 0){
      logSDLError(std::cout, "TTF_Init");
      SDL_Quit();
      return 1;
   }
   
   //Setup our window and renderer
   SDL_Window *window = SDL_CreateWindow("Pinetree", SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
   SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
   if (window == nullptr){
      logSDLError(std::cout, "CreateWindow");
      TTF_Quit();
      SDL_Quit();
      return 1;
   }
   SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
   if (renderer == nullptr){
      logSDLError(std::cout, "CreateRenderer");
      cleanup(window);
      TTF_Quit();
      SDL_Quit();
      return 1;
   }
   
   const std::string resPath = getResourcePath("pinetree");
   std::cout << resPath << std::endl;
   //We'll render the string "TTF fonts are cool!" in white
   //Color is in RGB format
   //   if (image == nullptr){
   //      cleanup(image, renderer, window);
   //      TTF_Quit();
   //      SDL_Quit();
   //      return 1;
   //   }
   
   //Get the texture w/h so we can center it in the screen
   
   
   
   SDL_Event e;
   bool quit = false;
   
   SDL_Color color = {255,255,255,255};
   SDL_Texture* t = renderText("Pinetree",getResourcePath("pinetree")+"Tuffy.ttf", color, 64, renderer);
   int iW, iH;
   SDL_QueryTexture(t, NULL, NULL, &iW, &iH);
   int x = SCREEN_WIDTH / 2 - iW / 2;
   int y = SCREEN_HEIGHT / 2 - iH / 2;
   
   double last = 0;
   float deltaTime = 0.0;
   
   
   unsigned int bIndex = 0;
   
   SDL_Surface *image = SDL_LoadBMP("bg.bmp");
   if (image == NULL)
      SDL_ShowSimpleMessageBox(0, "Image init error", SDL_GetError(),
                               window);
   SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer,
                                                        image);
   if (texture == NULL)
      SDL_ShowSimpleMessageBox(0, "Texture init error",
                               SDL_GetError(), window);
   
   while (!quit){
      //Event Polling
      while (SDL_PollEvent(&e)){
         
         if (e.type == SDL_QUIT){
            quit = true;
         }
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
            quit = true;
         }
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT) {
            
         }
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
            if (bIndex!=2) {
               ++bIndex;
            }
            else {
               bIndex=0;
            }
            
         }
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
            if (bIndex!=0) {
               --bIndex;
            }
            else {
               bIndex=0;
            }
         }
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT) {
            
         }
         if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
            switch(bIndex) {
               case 0:
                  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                           "You clicked list",
                                           "NOT IMPLEMENTED YET",
                                           NULL);
                  break;
                  
               case 1:
                  quit=true;
                  break;
                case 2:
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                             "You clicked settings",
                                             "NOT IMPLEMENTED YET",
                                             NULL);
                
                break;
            }
            
         }
      }
      double now = SDL_GetTicks();
      //deltatime is in seconds
      if (now > last) {
         deltaTime = ((float)(now - last)) / 1000;
         last = now;
      }
      
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_RenderClear(renderer);
      
      
      renderTexture(texture, renderer, 0,0);
      
      //We can draw our message as we do any other texture, since it's been
      //rendered to a texture
      
      
      renderTexture(t, renderer, x, y);
      
      SDL_Texture* t2=renderText("(C)ForestCorp"+std::to_string(bIndex), getResourcePath("pinetree")+"Tuffy.ttf", {255,255,255,255}, 40, renderer);
      renderTexture(t2, renderer, x-10, y+65);
      
      //List button
      SDL_Rect listButtonRect = {100,100,400,100}; /* Define button rectangle */
      SDL_Texture* listButtonText = renderText("List", getResourcePath("pinetree")+"Tuffy.ttf", {0,0,0,255}, 100, renderer); /* Define button text */
      int listButtonOn=bIndex-0; /* Calculate if the button should be on */
      SDL_SetRenderDrawColor(renderer, 255*(bIndex-0)*listButtonOn/* Calculate again */, 255, 255, 255); /* Set rectangle color */
      SDL_RenderFillRect(renderer, &listButtonRect); /* Draw the rect */
      renderTexture(listButtonText, renderer, 100,100); /* Render the text's texture */
      
      //Shutdown button
      SDL_Rect shutdownButtonRect = {100,200,400,100}; /* Define button rectangle */
      SDL_Texture* shutdownButtonText = renderText("Shutdown", getResourcePath("pinetree")+"Tuffy.ttf", {0,0,0,255}, 100, renderer); /* Define button text */
      int shutdownButtonOn=bIndex-1; /* Calculate if the button should be on */
      SDL_SetRenderDrawColor(renderer, 255*(bIndex-1)*shutdownButtonOn/* Calculate again */, 255, 255, 255); /* Set rectangle color */
      SDL_RenderFillRect(renderer, &shutdownButtonRect); /* Draw the rect */
      renderTexture(shutdownButtonText, renderer, 100,200); /* Render the text's texture */
      
    
       //Settings Button
       SDL_Rect settingsButtonRect = {100,300,400,100}; /* Define button rectangle */
      SDL_Texture* settingsButtonText = renderText("Settings", getResourcePath("pinetree")+"Tuffy.ttf", {0,0,0,255}, 100, renderer); /* Define button text */
       int settingsButtonOn=bIndex-2; /* Calculate if the button should be on */
       SDL_SetRenderDrawColor(renderer, 255*(bIndex-2)*settingsButtonOn/* Calculate again */, 255, 255, 255); /* Set rectangle color */
       SDL_RenderFillRect(renderer, &settingsButtonRect); /* Draw the rect */
       renderTexture(settingsButtonText, renderer, 100,300); /* Render the text's texture */

      
      SDL_RenderPresent(renderer);
      
      
      
   }
   //Clean up
   //   cleanup(image, renderer, window);
   //   TTF_Quit();
   //   SDL_Quit();
   
   return 0;
}
