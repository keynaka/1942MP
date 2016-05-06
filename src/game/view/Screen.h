#ifndef SCREEN_H
#define SCREEN_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <string>
#include "../../libs/xm_sdl/XM_SDL.h"
#include "Texture.h"
using namespace std;

class Screen {
  private:
    XM_SDL* sdlHandler;
    SDL_Renderer* renderer;
    map<string, Texture*> textures;
    map<string, SDL_Rect> rectangles;
    TTF_Font* fontFamily;
    SDL_Color fontColor;
    void addTexture( string id, Texture* texture );

  public:
    Screen( XM_SDL* sdlHandler );
    ~Screen();
    void loadTexture( string id, string path );
    void loadRectangle( string id, int x, int y, int width, int height );
    void loadText( string id, string value );
    void loadFont( const char* FONT_PATH );
    void renderTexture( string id, int x, int y );
    void renderRectangle( string id, bool outline = false );
    int getTextureWidth( string id );
    void setRenderDrawColor( int r, int g, int b, int a );
};
#endif
