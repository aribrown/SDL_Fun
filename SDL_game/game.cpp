// SDL Game
// BY: Ari Brown
//
#include<iostream>
#include<stdlib.h>
#include<SDL2/SDL.h>
#include<SDL2_image/SDL_image.h>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// GLOBALS /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 500;
const int MOVE_RL = 15;
const int MOVE_UB = 10;
const int GRASS_LEVEL = 420;
const int METER = 30; // pixels

const int RECTANGLES = 100;
const int BIRDS = 5;
int screen;
const float GRAVITY = 9.8;

int wrap_pos_r = -60; 
int wrap_pos_l = SCREEN_WIDTH - 80/4;

const int TIMER = 300;

bool init();
bool loadMedia();
void close();
SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// FUNCTIONS ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

        gTexture = loadTexture( "game_visuals/Environment.png");
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// CLASSES /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////// General Square Structs //////////////////////////////////////
struct Color {
        unsigned short r;
        unsigned short g;
        unsigned short b;
        unsigned short a;
};

struct Position {
        int x;
        int y;
        int w;
        int h;
};

/////////////////////////////////////////////// RECTANGLE ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Rectangle {
        public:
                Rectangle();
                Rectangle(bool random, bool outline, Position p, Color c);

                void draw_rectangle();
        private: 
                SDL_Rect rect;
                Color color;
                Position pp;
                bool o;
};


Rectangle::Rectangle()
{
}

Rectangle::Rectangle(bool random, bool outline, Position p, Color c)
{
        if (random){
                int num;
                if (SCREEN_WIDTH >= SCREEN_HEIGHT) {
                        num = SCREEN_WIDTH;
                }
                else {num = SCREEN_HEIGHT;}
                pp.x = random_int(0,num);
                pp.y = random_int(0,num);
                num = num/10;
                pp.w = random_int(0,num);
                pp.h = random_int(0,num);
                color.r = random_int(0,255);
                color.g = random_int(0,255);
                color.b = random_int(0,255);
                color.a = random_int(0,255);
                o = outline;
        }
        else {
                pp.x = p.x;
                pp.y = p.y;
                pp.w = p.w;
                pp.h = p.h;
                color.r = c.r;
                color.g = c.g; 
                color.b = c.b;
                color.a = c.a;
                o = outline;
        }
        draw_rectangle();
}
void Rectangle::draw_rectangle()
{
        rect.x = pp.x;
        rect.y = pp.y;
        rect.w = pp.w;
        rect.h = pp.h;
        SDL_SetRenderDrawColor( gRenderer, color.r, color.g, color.b, color.a);
        if (o) {
                SDL_RenderDrawRect(gRenderer, &rect);       
        }
        else {
                SDL_RenderFillRect(gRenderer, &rect);
        }
}



/////////////////////////////////////////////// PERSON //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Person{
        public:
                Person();
                void move(std::string s, bool wrap_r, bool wrap_l);
                void print_person();
                void switch_walk();
                int get_x();
                int get_y();
        private:
                Position p;
                SDL_Rect bubble;
                void move_left();
                void move_right();
                void move_forward();
                void move_back();
                SDL_Texture* person_fig;
                SDL_Texture* person_fig1;
                bool walk;
};

Person::Person()
{
        p.x = SCREEN_WIDTH/2 - (135/2);
        p.y = GRASS_LEVEL-258;
        p.w = 80;
        p.h = 258;
        bubble.x = p.x;
        bubble.y = p.y;
        bubble.w = p.w;
        bubble.h = p.h;
        person_fig1 = loadTexture("game_visuals/man.png");
        person_fig = loadTexture("game_visuals/man_walk.png");
        walk = false;
        print_person();
}

void Person::move(std::string s, bool wrap_r, bool wrap_l)
{
        if (s == "r") {move_right();}
        if (s == "l") {move_left();}
        if (s == "f") {move_forward();}
        if (s == "b") {move_back();}

        //wrap
        if (bubble.x + (bubble.w/4) > SCREEN_WIDTH || wrap_r) {
                bubble.x = wrap_pos_r;
                screen++;
        }
        if (bubble.x < -1*3*bubble.w/4 - 1 || wrap_l) {
                bubble.x = wrap_pos_l;
                screen--;
        }

}

void Person::move_forward()
{
        if (bubble.y + 258 < SCREEN_HEIGHT)
        {
                bubble.y += MOVE_UB;
        }
        print_person();
}
void Person::move_back()
{
        if (bubble.y + 258 - 5 > GRASS_LEVEL)
        {
                bubble.y -= MOVE_RL;
        }
        print_person();
}
void Person::move_right()
{
                bubble.x += MOVE_RL;
                print_person();
}

void Person::move_left()
{
                bubble.x -= MOVE_RL;
                print_person();
}

void Person::print_person()
{
        if (walk == false) {
                SDL_RenderCopy( gRenderer, person_fig, NULL, &bubble);
        };
        if (walk == true) {
                SDL_RenderCopy( gRenderer, person_fig1, NULL, &bubble);
        };
}

void Person::switch_walk()
{
        walk = !walk;
}

int Person::get_x()
{
        return bubble.x;
}
int Person::get_y()
{
        return bubble.y;
}

/////////////////////////////////////////////// OBJECT //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Object{
        public:
                Object();
                Object(std::string s, Position pos);
                void print_object();
        private:
                Position p;
                SDL_Rect area;
                SDL_Texture* object_area;
};

Object::Object(){}

Object::Object(std::string s, Position pos)
{
        p.x = pos.x;
        p.y = pos.y;
        p.w = pos.w;
        p.h = pos.h;
        area.x = p.x;
        area.y = p.y;
        area.w = p.w;
        area.h = p.h;
        object_area = loadTexture(s);
        print_object();
}


void Object::print_object()
{
                SDL_RenderCopy( gRenderer, object_area, NULL, &area);
}


/////////////////////////////////////////////// DYNAMIC OBJECT //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DynamicObject{
        public:
                DynamicObject();
                DynamicObject(std::string s, Position pos, float m);
                void print_object();
                void constant_speed(float dx, float dy, float velocity);
                void constant_force(float xnewtons, float ynewtons, int time);
                void set_x(int x);
                void set_y(int y);
                int get_x();
                int get_y();
                void set_float_y(float y);
        
        private:
                Position p;
                SDL_Rect area;
                SDL_Texture* object_area[4];
                float float_x;
                float float_y;
                float angular_w;
                int radius;
                void spin();
                int texture_num;
                int angular_timer;
                float mass;
};

DynamicObject::DynamicObject(){}

DynamicObject::DynamicObject(std::string s, Position pos, float m)
{
        texture_num = 0;
        angular_timer = 0;
        angular_w = 0.0;
        mass = m;
        p.x = pos.x;
        p.y = pos.y;
        p.w = pos.w;
        p.h = pos.h;
        area.x = p.x;
        area.y = p.y;
        area.w = p.w;
        area.h = p.h;
        radius = area.w/2;
        float_x = (float) area.x;
        float_y = (float) area.y;
        object_area[texture_num] = loadTexture(s);
        object_area[1] = loadTexture("game_visuals/soccer_ball_rotate90.png");
        object_area[2] = loadTexture("game_visuals/soccer_ball_rotate180.png");
        object_area[3] = loadTexture("game_visuals/soccer_ball_rotate270.png");
        print_object();
}


void DynamicObject::print_object()
{
        SDL_RenderCopy( gRenderer, object_area[texture_num], NULL, &area);
}

// position is in number of pixels
/////// velocity in pixels / integer count
// acceleration is change of pixels per integer count
// 
//
//
// everything force is a velocity, the only real force is constant gravity
//
void DynamicObject::set_x(int x)
{
        area.x = x;
}
void DynamicObject::set_y(int y)
{
        area.y = y;
}
int DynamicObject::get_x()
{
        return area.x;
}
int DynamicObject::get_y()
{
        return area.y;
}
void DynamicObject::set_float_y(float y)
{
        float_y = y;    
}
// accepts a vector to change into a unit vector
//  velocities are scalars of the unit vector
void DynamicObject::constant_speed(float dx, float dy, float velocity)
{
        // V = wr, where w is the angular velocity, the amount of times the spin
        // function is called
        angular_w = velocity/radius;
        angular_timer++;
        if (angular_timer > angular_w * 250){        
                spin();
                angular_timer = 0;
        }

        float mag = sqrt( pow(dx, 2) + pow(dy, 2) );
        float unit_x = dx/mag;
        float unit_y = dy/mag;
        

        // for velocity of 1, about METER pixels per second
        velocity = velocity / (250 / METER);

        
        float_x += unit_x * velocity;
        float_y += unit_y * velocity;

        area.x = (int) float_x;
        area.y = (int) float_y;
}

void DynamicObject::spin()
{
        if (texture_num < 3) {
                texture_num++;
        }
        if (texture_num == 3) {
                texture_num = 0;
        }
}

void DynamicObject::constant_force(float xnewtons, float ynewtons, int time)
{
        area.x += (int)((xnewtons*1.5/mass) * time/250 * time/250);
        area.y += (int)((ynewtons*1.5/mass) * time/250 * time/250);
}
/////////////////////////////////////////////// BIRD ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Bird{
        public:
                Bird();
                void print_bird();
                void move();
        private:
                Position p;
                void switch_flap();
                SDL_Rect area;
                SDL_Texture* bird;
                SDL_Texture* bird_flap;
                bool flap;
};

Bird::Bird()
{
        flap = false;
        bird = loadTexture("game_visuals/bird.png");
        bird_flap = loadTexture("game_visuals/bird_flap.png");
        area.x = random_int(0,SCREEN_WIDTH);
        area.y = random_int(0, 100);
        area.w = 80;
        area.h = 80;
        print_bird();

}

void Bird::print_bird()
{
        if (flap == false) {
                SDL_RenderCopy( gRenderer, bird, NULL, &area);
        };
        if (flap == true) {
                SDL_RenderCopy( gRenderer, bird_flap, NULL, &area);
        };
}

void Bird::switch_flap()
{
        flap = !flap;
}
void Bird::move()
{
        switch_flap();
                area.x += MOVE_RL/3;
                print_bird();
        //wrap
        if (area.x + (area.w/4) > SCREEN_WIDTH) {
                area.x = -1*3*area.w/4;
        }
        if (area.x < -1*3*area.w/4 - 1) {
                area.x = SCREEN_WIDTH - area.w/4;
        }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// MAIN IMPLEMENTATION /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char* args[])
{

        if (!init()){
                cout << "couldn't init";
        }
        else{
                if( !loadMedia() ) {
                        cout << "failed to load media";
                }               
                else {

                        bool quit = false;
                        screen = 1;
                        SDL_Event e;

                        ///////////// CONTENTS ///////////////////////
                        
                        // real grass level is grass level minus content height
                                                        
                        // man
                        Person man;
                        
                        // room
                        Position room1_p = {0,0, SCREEN_WIDTH, SCREEN_HEIGHT};
                        Object room1("game_visuals/room1.png", room1_p);                      
                        
                        // soccer ball
                        Position soccerball_p = {SCREEN_WIDTH-100, GRASS_LEVEL, 30, 30};
                        DynamicObject soccerball("game_visuals/soccer_ball.png", soccerball_p, 1.0);

                        // tree
                        Position tree_p = {SCREEN_WIDTH - 300, GRASS_LEVEL- 340, 200, 330};
                        Object tree("game_visuals/tree.png", tree_p);
                        
                        Position house_p = {SCREEN_WIDTH-400, 100, 400, 400};
                        Object house("game_visuals/house.png", house_p);                        
                        
                        // birds
                        Bird bird_array[BIRDS];
                        for (int i = 0; i < BIRDS; i++){
                                Bird bird;
                                bird_array[i] = bird;
                                bird_array[i].print_bird();
                        }
                        
                        Position bench_p = {100, GRASS_LEVEL-130, 250, 150};
                        Object bench("game_visuals/bench.png", bench_p);


                        Rectangle array[RECTANGLES];
                        Position rect_p = {0,0,0,0};
                        Color rect_c = {0,0,0,0};
                        for (int i = 0; i < RECTANGLES; i++) {
                                Rectangle rect(true,false, rect_p, rect_c);
                                array[i] = rect;
                                array[i].draw_rectangle();
                        }

                        int wrap_original_l = wrap_pos_l;
                        int wrap_original_r = wrap_pos_r;
                        bool wr = false, wl = false;
                        bool indoors = false;

                        bool soccer = false;
                        bool kick_left = false;
                        bool kick_right = false;


                        // TIMERS
                        int soccer_time = 0;
                        int bird_time = 0;
                        // int square_time = 0;



                        // Comments: 
                        // Set wrap_pos_r to be whatever position the man returns to on the screen
                        // use an if statement to trigger early wraps, wr, and wl.
                        // 
                        // set indoors trait of each screen
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
                                                        man.move("r", wr, wl);
                                                        man.switch_walk();
                                                        break;

                                                        case SDLK_LEFT:
                                                        man.move("l", wr, wl);
                                                        man.switch_walk();
                                                        break;
                                                        
                                                        case SDLK_UP:
                                                        man.move("b", wr, wl);
                                                        man.switch_walk();
                                                        break;
                                                        
                                                        case SDLK_DOWN:
                                                        man.move("f", wr, wl);
                                                        man.switch_walk();
                                                        break;

                                                }
                                        }
                                }

                                // REINITIALIZERS
                                wrap_pos_l = wrap_original_l; // returners
                                wrap_pos_r = wrap_original_r;
                                if (wr) {wr= false;} // senders
                                if (wl) {wl = false;}
                               

                                // TESTERS
                                //
                                // cout << man.get_x() << ", " << man.get_y() << endl;
                                // cout << soccerball.get_x() << " " << soccerball.get_y() << endl;
                                        
                                if(screen == 0 || screen == -1) {
                                        soccer = true;
                                        indoors = false;
                                        soccerball.print_object();
                                        
                                        if (man.get_y() - soccerball.get_y() < 10 && man.get_x() - soccerball.get_x() < 10){
                                                soccer_time = 0;
                                                soccerball.set_y(GRASS_LEVEL-1);
                                                soccerball.set_float_y(GRASS_LEVEL-1);
                                                kick_left = true;
                                        }
                                        
                                        // above ground
                                        if(soccerball.get_y() < GRASS_LEVEL && kick_left) {
                                                soccerball.constant_speed(-5.0, -9.0, 4);
                                        } 
                                        // always
                                        soccerball.constant_force(0.0, GRAVITY, soccer_time);

                                        // normal force of ground
                                        if (soccerball.get_y() > GRASS_LEVEL) {
                                                soccerball.constant_force(0.0, -1*GRAVITY, soccer_time);
                                        }
                                }        
                                
                                if(screen == 1)
                                {
                                        indoors = false;
                                        tree.print_object();
                                        bench.print_object();        
                                }
                                if(screen == 2)
                                {
                                        indoors = false;
                                        house.print_object();
                                        if ((man.get_x() <= 480 && man.get_x() >= 450) 
                                                        && (man.get_y() <= 210 && man.get_y() >= 190)) {
                                                wr = true;
                                        }
                                        wrap_pos_r = 20;
                                }
                                if(screen == 3)
                                {
                                        indoors = true;
                                        if (man.get_x() <= 15) {
                                                wl = true;
                                        }
                                        room1.print_object();
                                        wrap_pos_l = 440;
                                }


                                // BIRDS
                                if (!indoors) {
                                        bool move = false;
                                        for (int i = 0; i < BIRDS; i++){
                                                bird_array[i].print_bird();
                                                if (bird_time > TIMER/4) {
                                                        bird_array[i].move();
                                                        move = true;
                                                }
                                        }
                                        if (move){bird_time = 0;}
                                }


                                /*
                                // array of rectangles
                                for (int i = 0; i < RECTANGLES; i++) {
                                        array[i].draw_rectangle();
                                }
                                if (square_time > TIMER){
                                        for (int i = 0; i < RECTANGLES; i++) {
                                                Rectangle rect(true,false, rect_p, rect_c);
                                                array[i] = rect;
                                                array[i].draw_rectangle();
                                        }
                                        square_time = 0;
                                }
                                */


                                man.print_person();


                                // TIMER UPDATES
                                
                                if (soccer){soccer_time++;}
                                if (soccer_time > 10000) {soccer_time = 0;}
                                bird_time++;
                                // square_time++;

                                SDL_RenderPresent( gRenderer );
                        }
                }
        }
        close();
        return 0;
}

