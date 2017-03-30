#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

struct TTF_texture
{
  SDL_Texture* texture;
  int width;
  int height;
};

void init();
void close_sdl();
void sync_render();
void render();
void loadTFTTexture(struct TTF_texture *texture, TTF_Font *font, char* text);
void process_input(SDL_Event *e, int *quit);
void init_ball();

//Screen dimension constants
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

//The window we'll be rendering to
SDL_Window *sdl_window;

//The window renderer
SDL_Renderer* sdl_renderer;

// Display mode
SDL_DisplayMode sdl_display_mode;

//Game Controller 1 handler 
SDL_Joystick *sdl_gamepad[2];

//Globally used font 
TTF_Font *number_font = NULL;

struct TTF_texture texture_text_p1;
struct TTF_texture texture_text_p2;

/** GAME DATA **/
int p1_y;
int p2_y;
int p1_x;
int p2_x;
int height_p1;
int height_p2;
int width;
int pvy[2];
int ball_x;
int ball_y;
int ball_vx;
int ball_vy;
int p_v;
int p1_score;
char p1_score_s[10];
int p2_score;
char p2_score_s[10];
int game_over;

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
  sdl_gamepad[0] = NULL;
  sdl_gamepad[1] = NULL;
  
  //Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
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
    
    // Get display mode
    if (SDL_GetDesktopDisplayMode(0, &sdl_display_mode) != 0) {
      printf("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
      exit(-1);
    }
    SCREEN_WIDTH=sdl_display_mode.w;
    SCREEN_HEIGHT=sdl_display_mode.h;

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
    
    //Initialize SDL_ttf 
    if( TTF_Init() == -1 ) 
    {
      printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() ); 
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
  
  for(i=0; i<SDL_NumJoysticks(); i++)
  {
    SDL_JoystickClose(sdl_gamepad[i]);
    sdl_gamepad[i]=NULL;
  }
  
  // Exit SDL
  TTF_Quit();
  SDL_Quit();
}

void load_media()
{
  //Open the font 
  number_font = TTF_OpenFont( "DSEG7Classic-Bold.ttf", 50 ); 
  if( number_font == NULL ) 
  { 
    printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
    exit(-1);
  }
}

void loadTFTTexture(struct TTF_texture *texture, TTF_Font *font, char* text)
{
  //The final texture
  texture->texture = NULL;
  // Text color
  SDL_Color textColor = { 255, 255, 255 };

  //Load image at specified path
  SDL_Surface *loadedSurface = TTF_RenderText_Solid( font, text, textColor );
  if( loadedSurface == NULL )
  {
    printf( "Unable to render text! SDL_image Error: %s\n", TTF_GetError() );
    exit(-1);
  }
  else
  {
    //printf("Surface: %d, %d\n", loadedSurface->w, loadedSurface->h);
    
    //Create texture from surface pixels
        texture->texture = SDL_CreateTextureFromSurface(sdl_renderer, loadedSurface );
    if( texture->texture == NULL )
    {
      printf( "Unable to create texture! SDL Error: %s\n", SDL_GetError() );
      exit(-1);
    }
    else {
      //Get image dimensions 
      texture->width = loadedSurface->w; 
      texture->height = loadedSurface->h;      
    }

    //Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

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
  height_p1=70;
  height_p2=70;
  width=15;
  p1_x=10;
  p2_x=SCREEN_WIDTH-10-width;
  p1_y=0;
  p2_y=0;
  pvy[0]=0;
  pvy[1]=0;
  init_ball();
  p_v=10;
  p1_score=10;
  p2_score=10;
  game_over=0;
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
  
  // Update p1
  p1_y+=pvy[0];
  if(p1_y>SCREEN_HEIGHT-height_p1)
  {
    p1_y=SCREEN_HEIGHT-height_p1;
  }
  if(p1_y<0)
  {
    p1_y=0;
  }
  
  // Update p2
  p2_y+=pvy[1];
  if(p2_y>SCREEN_HEIGHT-height_p2)
  {
    p2_y=SCREEN_HEIGHT-height_p2;
  }
  if(p2_y<0)
  {
    p2_y=0;
  }
  
  // Update ball
  if(!game_over)
  {
    ball_x+=ball_vx;
    ball_y+=ball_vy;
  }
    
  // Check collisions p1
  if(ball_x>=p1_x && ball_x<=p1_x+width &&  ball_y>=p1_y && ball_y<=p1_y+height_p1)
  {
    ball_vx=-1*ball_vx;
  }
  // Check collisions p2
  if(ball_x>=p2_x && ball_x<=p2_x+width &&  ball_y>=p2_y && ball_y<=p2_y+height_p2)
  {
    ball_vx=-1*ball_vx;
  }
  // Check collision top and bottom wall
  if(ball_y<0 || ball_y>SCREEN_HEIGHT)
  {
    ball_vy=-1*ball_vy;
  }
  // Check collision left wall
  if(ball_x<0)
  {
    p1_score--;
    init_ball();
  }
  // Check collision right wall
  if(ball_x>SCREEN_WIDTH)
  {
    p2_score--;
    init_ball();
  }
  else if(ball_x>SCREEN_WIDTH) 
  {
    ball_vx=-1*ball_vx;
  }

  if(p1_score==0 || p2_score==0)
  {
    game_over=1;
  }
  
  
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
  
  // Draw p1
  sdl_rect.x=p1_x;
  sdl_rect.y=p1_y;
  sdl_rect.w=width;
  sdl_rect.h=height_p1;
  SDL_RenderFillRect(sdl_renderer, &sdl_rect);
  
  // Draw p2
  sdl_rect.x=p2_x;
  sdl_rect.y=p2_y;
  sdl_rect.w=width;
  sdl_rect.h=height_p2;
  SDL_RenderFillRect(sdl_renderer, &sdl_rect);
  
  
  // Draw ball
  if(!game_over)
  {
    sdl_rect.x=ball_x;
    sdl_rect.y=ball_y;
    sdl_rect.w=10;
    sdl_rect.h=10;
    SDL_RenderFillRect(sdl_renderer, &sdl_rect);
  }
  
  // Draw counter player 1
  sprintf(p1_score_s, "%02d", p1_score);
  loadTFTTexture(&texture_text_p1, number_font, p1_score_s);
  SDL_Rect number_rect = {50,50, texture_text_p1.width, texture_text_p1.height};
  SDL_RenderCopy(sdl_renderer, texture_text_p1.texture, NULL, &number_rect);
  SDL_DestroyTexture(texture_text_p1.texture);

  
  // Draw counter player 2
  sprintf(p2_score_s, "%02d", p2_score);
  loadTFTTexture(&texture_text_p2, number_font, p2_score_s);
  SDL_Rect number_rect2 = {SCREEN_WIDTH-200,50, texture_text_p2.width, texture_text_p2.height};
  SDL_RenderCopy(sdl_renderer, texture_text_p2.texture, NULL, &number_rect2);
  SDL_DestroyTexture(texture_text_p2.texture);
  
  //Update screen
  SDL_RenderPresent( sdl_renderer );
}

void process_input(SDL_Event *e, int *quit)
{
      //User requests quit
      if(e->type == SDL_QUIT 
          // User press ESC or q
          || e->type == SDL_KEYDOWN && (e->key.keysym.sym=='q' || e->key.keysym.sym == 27)
          // User 1 press select
          || e->type == SDL_JOYBUTTONDOWN && e->jbutton.which==0 && e->jbutton.button == 8)
      {
        *quit = 1;
      }
      // Axis 0 controls player velocity
      else if( e->type == SDL_JOYAXISMOTION && e->jaxis.axis == 1)
      {
        //printf("controller: %d, axis: %d, value: %d\n", e->jaxis.which, e->jaxis.axis, e->jaxis.value);
        pvy[e->jaxis.which]=p_v*e->jaxis.value/32767; 
      }
      // Buttons
      else if( e->type == SDL_JOYBUTTONDOWN && e->jbutton.button<8)
      {
        //printf("controller: %d button: %d\n",e->jbutton.which, e->jbutton.button);
        switch( e->jbutton.button) 
        {
          case 0: ; break;
          case 1: ; break;
          case 2: ; break;
          case 3: ; break;
          case 4: ; break;
        }
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
  
  // Load Media
  load_media();
  
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
  
  
  
  
