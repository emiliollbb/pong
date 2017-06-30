/**********
 * gcc -lSDL2 test.c -o test
 * 
 * **********/

#define BALLS_NUM 150

#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

struct sball
{
  int x, y, vx, vy;
};

void init();
void close_sdl();
void sync_render();
void render();
void init_ball();
void init_game();
void process_input(SDL_Event *e, int *quit);

//Screen dimension constants
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

//The window we'll be rendering to
SDL_Window *sdl_window;

//The window renderer
SDL_Renderer* sdl_renderer;

// Gamepads
SDL_Joystick *sdl_gamepad[2];


/** GAME DATA **/
struct sball balls[BALLS_NUM];

void init()
{
  int i;
  
  SCREEN_WIDTH = 720;
  SCREEN_HEIGHT = 480;
  //SCREEN_WIDTH = 1920;
  //SCREEN_HEIGHT = 1080;
  //SCREEN_WIDTH = 1366;
  //SCREEN_HEIGHT = 768;
  
  sdl_window=NULL;
  sdl_renderer = NULL;
  
  //Initialize SDL
  if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 )
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    exit(-1);
  }
  else
  {
    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "Warning: Linear texture filtering not enabled!" );
      exit(-1);
    }
    
    //Create window
    //sdl_window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    // Create window fullscreen 
    sdl_window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN );
    SDL_GetWindowSize(sdl_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    printf("screen: %d x %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);

    if( sdl_window == NULL )
    {
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
      exit(-1);
    }
    
    //Create renderer for window
    sdl_renderer = SDL_CreateRenderer( sdl_window, -1, SDL_RENDERER_ACCELERATED );
    if( sdl_renderer == NULL )
    {
      printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
      exit(-1);
    }
    
    //Initialize renderer color
    SDL_SetRenderDrawColor( sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    
    //Check for joysticks 
    if( SDL_NumJoysticks() < 1 ) 
    { 
      printf( "Warning: No joysticks connected!\n" ); 
    } 
    else 
    {
      printf("%d joysticks connected\n", SDL_NumJoysticks());
      for(i=0; i<SDL_NumJoysticks(); i++)
      {
        //Load joystick 
        sdl_gamepad[i] = SDL_JoystickOpen(i); 
        if(sdl_gamepad[i] == NULL ) 
        { 
            printf( "Warning: Unable to open game controller %d! SDL Error: %s\n", i, SDL_GetError() ); 
            
        }
      }
      
    }
    
    
  }
}

void close_sdl()
{
  int i;
  
  // Close gamepads
  for(i=0; i<SDL_NumJoysticks(); i++)
  {
    SDL_JoystickClose(sdl_gamepad[i]);
    sdl_gamepad[i]=NULL;
  }
  
  //Destroy renderer  
  if(sdl_renderer!=NULL)
  {
    SDL_DestroyRenderer( sdl_renderer );
    sdl_renderer=NULL;
  }
  
  if(sdl_window != NULL)
  {
    // Destroy window
    SDL_DestroyWindow( sdl_window );
    sdl_window=NULL;
  }
    
  // Exit SDL
  SDL_Quit();
}



void sync_render()
{
  unsigned int ticks; 
  long remaining;
  
  ticks = SDL_GetTicks();
  
  render(); 
  
  remaining = ticks;
  remaining = remaining + 16 - SDL_GetTicks();
  
  if(remaining > 0)
  {
    //remaining = 1;
    SDL_Delay(remaining);
  }
  else
  {
    printf("%d remaining!!!\n", remaining);
  }

}  

void init_game()
{
  init_ball();
}

void init_ball()
{
  int i;
  double aux;
  
  for(i=0; i<BALLS_NUM; i++)
  {
    balls[i].x=rand()%SCREEN_WIDTH;
    balls[i].y=rand()%SCREEN_HEIGHT;
    balls[i].vx=rand()%40-20;
    balls[i].vy=rand()%19+1;
  }
}

void render()
{
  int i;
  SDL_Rect sdl_rect;

  //Clear screen
  SDL_SetRenderDrawColor( sdl_renderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_RenderClear( sdl_renderer );
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    
  for(i=0; i<BALLS_NUM; i++)
  {
    // Update ball
    balls[i].x+=balls[i].vx;
    balls[i].y+=balls[i].vy;
        
    // Check collision top and bottom wall
    if(balls[i].y>SCREEN_HEIGHT || balls[i].y<0)
    {
      balls[i].vy*=-1;
    }
    // Check collision left and right wall
    if(balls[i].x>SCREEN_WIDTH || balls[i].x<0)
    {
      balls[i].vx*=-1;
    }
    
    // Draw ball
    sdl_rect.x=balls[i].x;
    sdl_rect.y=balls[i].y;
    sdl_rect.w=10;
    sdl_rect.h=10;
    SDL_RenderFillRect(sdl_renderer, &sdl_rect);
  
  }  
  
  //Update screen
  SDL_RenderPresent( sdl_renderer );
}

void process_input(SDL_Event *e, int *quit)
{
      //User requests quit
      if(e->type == SDL_QUIT 
          // User press ESC or q
          || e->type == SDL_KEYDOWN && (e->key.keysym.sym=='q' || e->key.keysym.sym == 27)
          // User press select
          || e->type == SDL_JOYBUTTONDOWN)
      {
        *quit = 1;
      }
      
}

int main( int argc, char* args[] )
{
  //Main loop flag
  int quit=0;
  
  //Event handler
  SDL_Event e;
  
  // Initialize random seed
  srand(time(NULL));
  
  
  // Start up SDL and create window
  init();
     
  // Init game data
  init_game();
  
  // Main game loop
  while(!quit)
  {
    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
      process_input(&e, &quit);
    }
    // Render
    sync_render();
  }


  close_sdl();
  return 0;
}
  
  
  
  
