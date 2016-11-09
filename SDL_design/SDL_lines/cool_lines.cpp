// Fractal Drawer
//
#include<iostream>
#include<stdlib.h>
#include<SDL2/SDL.h>
#include<SDL2_image/SDL_image.h>
#include<cmath>
using namespace std;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

const int NUM_LINES = 500;
const int MAX_LENGTH = 1000;
const int ZOOM = 10;
const int START_APPEAR = 60;
const int APPEARANCE = 100000;
const int STRETCH = 10;

bool init();
bool loadMedia();
void close();
SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// SDL FUNCTIONS ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

bool init()
{
        bool success = true;
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
                cout << "couldn't initialize sdl";
                success = false;
        }
        else {
                if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") ){
                        cout << "linear texture failed";
                }
                gWindow = SDL_CreateWindow( "Ari's Program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
                if ( gWindow == NULL ) {
                        cout << "no window";
                        success = false;
                }
                else {
                        // create renderer for window
                        gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED );
                        if (gRenderer == NULL) {
                                cout << "renderer couldn't be created";
                                success = false;
                        }
                        else {
                                // init renderer color
                                SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255);
                                SDL_RenderClear( gRenderer );
                                SDL_RenderCopy( gRenderer, gTexture, NULL, NULL);
                                if (!(IMG_Init(IMG_INIT_PNG))){
                                        cout << "couldn't initialize image";
                                        success = false;
                                }                         
                        }
                }
        }
        return success;
}
bool loadMedia()
{
        bool success = true;

        gTexture = loadTexture( "black.png");
        if (gTexture == NULL){
                success = false;
        }
        return success;
}
void close()
{
        SDL_DestroyTexture(gTexture);
        gTexture = NULL;

        SDL_DestroyRenderer( gRenderer );
        SDL_DestroyWindow( gWindow );
        gWindow = NULL;
        gRenderer = NULL;
        

        IMG_Quit();
        SDL_Quit();

}
SDL_Texture* loadTexture(std::string path)
{
        SDL_Texture* newTexture = NULL;
        SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
        if (loadedSurface ==NULL) {
                cout << "can't load image";
        }
        else
        {
                newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface);
                if (newTexture == NULL){
                        cout << "couldn't create texture";
                }
                SDL_FreeSurface(loadedSurface);
        }
        return newTexture;
}

static int rand_seed = -1;
void seed_random(int s)
{
        if (s>0){
                srand(s);
                rand_seed = s;
        }
}
int random_int(int lo, int hi)
{
        int range = hi - lo + 1;
        char *seed_str;
        if (rand_seed == -1 ) {
                seed_str = getenv("SNAKE_SEED");
                if (seed_str != NULL)
                        seed_random( (unsigned) atoi(seed_str) );
                else
                        seed_random( (unsigned) time(NULL) );
        }
        if ( range <= 0)
                return 17;
        return lo + ( rand() % range );
}

void expand_lines();
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// CLASSES //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Color {
        unsigned short r;
        unsigned short g;
        unsigned short b;
        unsigned short a;
};


class Line {
        public:
                Line();
                Line(float a, float l, int start_x, int start_y, Color color);
                void draw_line();
                
                void set_color(Color color);
                void set_angle(float a);
                void set_length(float l);
                void set_position(int xx, int yy);
                float get_length();
                float get_angle();
                int get_x();
                int get_y();
                void set_alpha(unsigned short a);
                unsigned short get_alpha();

        private:
                float pythag(int x1, int y1, int x2, int y2);
                Color c;
                float angle;
                float length;
                
                // top left
                int x;
                int y;
};
Line::Line(){}
Line::Line(float a, float l, int start_x, int start_y, Color color)
{
        c.r = color.r;
        c.g = color.g;
        c.b = color.b;
        c.a = color.a;
        angle = a;
        length = l;
        x = start_x;
        y = start_y;
}
void Line::set_color(Color color)
{
        c.r = color.r;
        c.g = color.g;
        c.b = color.b;
        c.a = color.a;
}
void Line::set_angle(float a)
{
        angle = a;
}
void Line::set_length(float l)
{
        length = l;
}
void Line::set_position(int xx, int yy)
{
        x = xx;
        y = yy;
}
float Line::get_angle()
{
        return angle;
}
float Line::get_length()
{
        return length;
}
int Line::get_x()
{
        return x;
}
int Line::get_y()
{
        return y;
}
unsigned short Line::get_alpha()
{
        return c.a;
}
void Line::set_alpha(unsigned short a)
{
        c.a = a;
}
void Line::draw_line()
{
        SDL_SetRenderDrawColor(gRenderer, c.r, c.g, c.b, c.a);
        
        float dx = cos (angle * M_PI/180);
        float dy = -1*sin (angle * M_PI/180);

        float mag = sqrt( pow(dx, 2) + pow(dy, 2) );
        float unit_x = dx/mag;
        float unit_y = dy/mag;
        
        float current_x = (float)x;
        float current_y = (float)y;
        while ((int)pythag(x, y, (int)current_x, (int)current_y) <= length)
        {
                SDL_RenderDrawPoint (gRenderer, current_x, current_y);
                current_x += unit_x;
                current_y += unit_y;
        }
}
float Line::pythag(int x1, int y1, int x2, int y2)
{
      return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));  
}

///////////////////////////// Line Contents in Main
int capacity_lines = NUM_LINES+1;
int num_lines = NUM_LINES;
Line *lines = new Line[capacity_lines];

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// MAIN IMPLEMENTATION //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char* args[])
{
        bool zoom = false;
        cout << "zoom?? (y/n) " << endl;
        string s;
        cin >> s;
        if (s == "y")
        {
                zoom = true;
        }

        if (!init()){
                cout << "couldn't init";
        }
        else{
                if( !loadMedia() ) {
                        cout << "failed to load media";
                }               
                else {

                        bool quit = false;
                        SDL_Event e;

                        ///////////// CONTENTS ///////////////////////


                        for (int i = 0; i < num_lines; i++) {
                                Color color_line = {(unsigned short)random_int(0, 255),
                                        (unsigned short)random_int(0, 255),
                                        (unsigned short)random_int(0, 255),
                                        255};
                                lines[i].set_color(color_line);
                                lines[i].set_angle((float)random_int(0, 359));
                                lines[i].set_length((float)random_int(0, MAX_LENGTH));
                                lines[i].set_position(random_int(0, SCREEN_WIDTH), random_int(0, SCREEN_HEIGHT));
                        }


                        ///////////// TIMER //////////////////////////
                        int timer = 0; 

                        ///////////// GAME LOOP //////////////////////
                        while (!quit)
                        {
                                SDL_RenderCopy( gRenderer, gTexture, NULL, NULL);
                                while (SDL_PollEvent(&e) != 0)
                                {
                                        if (e.type == SDL_QUIT){
                                                quit = true;
                                        }
                                        else if ( e.type == SDL_KEYDOWN ) 
                                        {
                                                switch(e.key.keysym.sym)
                                                {
                                                        case SDLK_RIGHT:
                                                                break;

                                                        case SDLK_LEFT:
                                                                break;

                                                        case SDLK_UP:
                                                                break;

                                                        case SDLK_DOWN:
                                                                break;

                                                }
                                        }
                                }
                                ///////////// OBJECT RENDER ///////////////////

                                if (timer % APPEARANCE && timer > START_APPEAR) {
                                        num_lines++;
                                        Color color_line = {(unsigned short)random_int(0, 255),
                                                (unsigned short)random_int(0, 255),
                                                (unsigned short)random_int(0, 255),
                                                0};
                                        lines[num_lines].set_color(color_line);
                                        lines[num_lines].set_angle((float)random_int(0, 359));
                                        lines[num_lines].set_length(1);
                                        lines[num_lines].set_position(random_int(SCREEN_WIDTH/2 - 100, SCREEN_WIDTH/2 + 100), random_int(SCREEN_HEIGHT/2 - 200, SCREEN_HEIGHT/2 + 200));

                                        if (num_lines == capacity_lines)
                                        {
                                                expand_lines();
                                        }
                                } 
                                for (int i = 0; i < num_lines; i++) {
                                        lines[i].set_angle(lines[i].get_angle() + 0.25);
                                        lines[i].draw_line();
                                        if (zoom && timer % 100) {
                                                // zoom
                                                if (lines[i].get_x() > SCREEN_WIDTH/2)
                                                {
                                                        lines[i].set_position(lines[i].get_x() + ZOOM, lines[i].get_y());
                                                } 
                                                if (lines[i].get_x() < SCREEN_WIDTH/2)
                                                {
                                                        lines[i].set_position(lines[i].get_x() - ZOOM, lines[i].get_y());
                                                } 
                                                if (lines[i].get_y() > SCREEN_HEIGHT/2)
                                                {
                                                        lines[i].set_position(lines[i].get_x(), lines[i].get_y() + ZOOM);
                                                } 
                                                if (lines[i].get_y() < SCREEN_WIDTH/2)
                                                {
                                                        lines[i].set_position(lines[i].get_x(), lines[i].get_y() - ZOOM);
                                                }
                                                lines[i].set_length(lines[i].get_length() + STRETCH);
                                                        lines[i].set_alpha(lines[i].get_alpha()+1);
                                        }
                                }

                                SDL_RenderPresent( gRenderer );
                                timer++;
                        }
                }
        }
        close();
        delete []lines;
        return 0;
}

void expand_lines()
{
        Line *temp = new Line[capacity_lines*2];
        for (int i = 0; i < num_lines; i++)
        {
                temp[i] = lines[i];
        }
        delete []lines;
        lines = temp;
        capacity_lines = capacity_lines*2;
}
