
#include "main.h"
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

SDL_Renderer *renderer;
short int bIndex = 0;
int numberOfButtons = 0;

struct Button {
   int id;
   std::string text;
   std::function<void(void)> f;
};


void drawButton(std::string text,int x, int id, std::string) {
   
   
   SDL_Rect buttonRect = {x,100*numberOfButtons,400,100}; /* Define button rectangle */
   SDL_Texture* buttonText = renderText(text, getResourcePath("pinetree")+"Tuffy.ttf", {0,0,0,255}, 50, renderer); /* Define button text */
   unsigned int buttonOn=bIndex-numberOfButtons; /* Calculate if the button should be on */
   SDL_SetRenderDrawColor(renderer, 255*(bIndex)*buttonOn/* Calculate again */, 255, 255, 255); /* Set rectangle color */
   SDL_RenderFillRect(renderer, &buttonRect); /* Draw the rect */
   renderTexture(buttonText, renderer, x,100*numberOfButtons); /* Render the text's texture */
   numberOfButtons++;
}
Button buttons[20];
void addButton(std::string text,std::function<void(void)> f) {
   numberOfButtons++;
//   buttons.insert(ButtonPair(numberOfButtons,text));
   buttons[numberOfButtons] = {numberOfButtons,text,f};
   
}
bool showMenu = true;
void games() {
   showMenu=false;
}
bool quit = false;
void shutdown() {
   quit=true;
}
int main(int, char**){
   addButton("Games",games);
   addButton("Quit",shutdown);
   
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
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

   
   //MARK: Top Variables
   SDL_Color color = {255,255,255,255};
   SDL_Texture* t = renderText("Pinetree",getResourcePath("pinetree")+"Tuffy.ttf", color, 64, renderer);
   int iW, iH;
   SDL_QueryTexture(t, NULL, NULL, &iW, &iH);
   int x = SCREEN_WIDTH / 2 - iW / 2;
   int y = SCREEN_HEIGHT / 2 - iH / 2;
   double last = 0;
   float deltaTime = 0.0;
   int oldTime = SDL_GetTicks();
   
   int aniSLideInButtons = 0;
   
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
      int newTime = SDL_GetTicks();
      float delta = newTime - oldTime;
      
      if(aniSLideInButtons>50&&aniSLideInButtons<100){aniSLideInButtons+=0.07*delta;}
      if(aniSLideInButtons<50){aniSLideInButtons+=0.09*delta;}
      
      //Event Polling
      while (SDL_PollEvent(&e)){
         
         if (e.type == SDL_QUIT){
            quit = true;
         }
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
            quit = true;
         }
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN && showMenu) {
            if (bIndex!=1) {
               ++bIndex;
            }
            else {
               bIndex=0;
            }
            
         }
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP && showMenu) {
            if (bIndex!=0) {
               --bIndex;
            }
            else {
               bIndex=0;
            }
         }
         if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
            buttons[bIndex+1].f();
         }
      }
      double now = SDL_GetTicks();
      //deltatime is in seconds
      
      
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_RenderClear(renderer);
      
      
      renderTexture(texture, renderer, 0,0);
      
      //We can draw our message as we do any other texture, since it's been
      //rendered to a texture
      
      
      renderTexture(t, renderer, x, y);
      
      SDL_Texture* t2=renderText("(C)ForestCorp"+std::to_string(delta), getResourcePath("pinetree")+"Tuffy.ttf", {255,255,255,255}, 40, renderer);
      renderTexture(t2, renderer, x-10, y+65);
      
      if(showMenu) {
         for (int ai=1; buttons[ai].text!=""; ++ai) {
            SDL_Rect buttonRect = {x,100*buttons[ai].id,400,100}; /* Define button rectangle */
                        SDL_Texture* buttonText = renderText(buttons[ai].text, getResourcePath("pinetree")+"Tuffy.ttf", {0,0,0,255}, 50, renderer); /* Define button text */
                        bool buttonOn=bIndex+1-buttons[ai].id; /* Calculate if the button should be on */
                        SDL_SetRenderDrawColor(renderer, 255*(bIndex+1)*buttonOn/* Calculate again */, 255, 255, 255); /* Set rectangle color */
                        SDL_RenderFillRect(renderer, &buttonRect); /* Draw the rect */
                        renderTexture(buttonText, renderer, x,100*buttons[ai].id); /* Render the text's texture */
         }
      }
      
      
      SDL_RenderPresent(renderer);
      
      SDL_Delay(16.666666667);
      oldTime = newTime;
      if (now > last) {
         deltaTime = ((float)(now - last)) / 1000;
         last = now;
      }
      
   }
   //Clean up
      cleanup(image, renderer, window);
      TTF_Quit();
      SDL_Quit();
   
   return 0;
}
