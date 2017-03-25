/**********
 * gcc -lSDL2 test.c -o test
 * 
 * **********/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


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


/** GAME DATA **/
int ball_x;
int ball_y;
int ball_vx;
int ball_vy;

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
  if( SDL_Init( SDL_INIT_VIDEO) < 0 )
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
    
    
  }
}

void close_sdl()
{
  int i;
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
  ball_x=SCREEN_WIDTH/2;
  ball_y=SCREEN_HEIGHT/2;
  if(rand()%2==0)
  {
    ball_vx=5;
  }
  else
  {
    ball_vx=-5;
  }
  
  if(rand()%2==0)
  {
    ball_vy=5;
  }
  else
  {
    ball_vy=-5;
  }
}

void render()
{
  SDL_Rect sdl_rect;

  //Clear screen
  SDL_SetRenderDrawColor( sdl_renderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_RenderClear( sdl_renderer );
    
  // Update ball
  ball_x+=ball_vx;
  ball_y+=ball_vy;
      
  // Check collisions left wall
  if(ball_x<0)
  {
    ball_vx=5;
  }
  // Check collision right wall
  if(ball_x>SCREEN_WIDTH)
  {
    ball_vx=-1*ball_vx;
  }
  // Check collision top and bottom wall
  if(ball_y<0 || ball_y>SCREEN_HEIGHT)
  {
    ball_vy=-1*ball_vy;
  }
  
  
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
  
  SDL_Rect fillRect3 = {ball_x, ball_y, 10, 10 };
  SDL_RenderFillRect( sdl_renderer, &fillRect3 );
  
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
          || e->type == SDL_JOYBUTTONDOWN && e->jbutton.button == 8)
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
  
  
  
  
