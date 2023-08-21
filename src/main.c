#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>



const int WIDTH = 800;
const int HEIGHT = 600;

int BALL_SPEED = 120;
float BALL_SIZE = 16;

typedef struct Player {
   int score;
   float yPos;
} Player;

Player player;
Player player2;

const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 75;
const int PLAYER_MARGIN = 10;	// Pixels away from screen

float PLAYER_MOVE_SPEED = 150.0f;

typedef struct Ball {
   float x, y, xSpeed, ySpeed;
   int size;
} Ball;

bool served = false;
Ball ball;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

/** Main Functions **/
bool initialize(void);
void update(float elapsedTime);
// Static to prevent confusion between POSIX shutdown
static void shutdown(void);

/** Functions **/

// BALL
Ball makeBall(int size);
void updateBall(Ball *ball, float elapsedTime);
void renderBall(void);

// PLAYER
Player makePlayers(void);
void updatePlayers(float elapsedTime);
void renderPlayers();

// Score
void updateScore(int player, int points);

int main(int argc, char *argv[])
{
   srand((unsigned int)time(NULL));

   // Checks for arguments passed
   for(int i = 1; i < argc; i++){
      // Enables a faster game
      if(strcmp("speed", argv[1]) == 0){
         BALL_SPEED = 300;
         PLAYER_MOVE_SPEED = 300;
         printf("GOTTA GO FAST!\n");
         break;
      }

      // Increases the size of the ball
      if(strcmp("baller", argv[1]) == 0){
         BALL_SIZE = 64;
         printf("Thats a really big ball\n");
         break;
      }
   }

   // initializes initialize and if its unable to it'll exit
   if(!initialize()){
      exit(1);
   }

   SDL_Event event;
   
   bool quit = false;

   
   Uint32 lastTick = SDL_GetTicks();
   
   while(!quit)
   {
      /** EVENTS **/
      while(SDL_PollEvent(&event))
      {
         switch(event.type)
         {
            case SDL_QUIT: {
               quit = true;
            }break;
               
            case SDL_KEYDOWN: {
               switch(event.key.keysym.sym)
               {
                  case SDLK_ESCAPE: {
                     quit = true;
                  }break;

                  default: {
                     /** DEFAULT SWITCHCASE KEYSYM **/
                  }break;
               }
            }break;

            default: {
               /** EVENT TYPES DEFAULT **/
            }break;
         }
      }/** EVENTS **/

      // Calculates FPS
      Uint32 currentTicks = SDL_GetTicks();
      Uint32 diff = currentTicks - lastTick;
      float elapsedTime = diff / 1000.0f;
      update(elapsedTime);

      // Calculaes FPS
      lastTick = currentTicks;



   }/** Main Game Loop **/

   // cleans resources and shuts down SDL2
   shutdown();

   return 0;
}


bool initialize(void)
{
   
   // Initializes SDL2 VIDEO subsystem
   if(SDL_Init(SDL_INIT_VIDEO) < 0){
      fprintf(
         stderr,"FAILED TO INITIALIZE SDL_SUBSYSTEM.ERROR: %s\n",
         SDL_GetError());
      return false;
   }

   // Creates Window and Renderer
   SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, &window, &renderer);

   if(!window){
      fprintf(stderr, "Failed to initialize window. ERROR: %s\n", SDL_GetError());
      return false;
   }

   if(!renderer){
      fprintf(stderr, "Failed to initialize window. ERROR: %s\n", SDL_GetError());
      return false;
   }

   // Give the Window a title
   SDL_SetWindowTitle(window, "Pong | Press Spacebar to serve");

   // Makes the ball and players
   ball = makeBall(BALL_SIZE);
   player = makePlayers();
   player2 = makePlayers();

   return true;
}

// call this function upon every update
void update(float elapsedTime)
{
   // Renders onto the back buffer
   SDL_SetRenderDrawColor(renderer, 0, 100, 200, 0xFF);
   SDL_RenderClear(renderer);

   updateBall(&ball, elapsedTime);
   renderBall();
   
   updatePlayers(elapsedTime);
   renderPlayers();

   // Signals the double buffering to switch
   SDL_RenderPresent(renderer);
}

// Static to prevent confusion between POSIX shutdown
// cleans SDL2 related memory and shutsdown SDL2
static void shutdown(void)
{
   // If not NULL then cleanup
   if(window){
      SDL_DestroyWindow(window);
   }

   if(renderer){
      SDL_DestroyRenderer(renderer);   
   }
   
   SDL_Quit();
}

// PLAYER
Player makePlayers(void)
{
   Player player = {
      .yPos = (float)HEIGHT / 2,
   };

   return player;
}

// Run in each frame
void updatePlayers(float elapsedTime)
{
   // Keyboard
   const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
   if(keyboardState[SDL_SCANCODE_SPACE]){
      served = true;
   }

   if(keyboardState[SDL_SCANCODE_W]){
      player.yPos -= PLAYER_MOVE_SPEED * elapsedTime;
   }

   if(keyboardState[SDL_SCANCODE_S]){
      player.yPos += PLAYER_MOVE_SPEED * elapsedTime;
   }

   if(keyboardState[SDL_SCANCODE_UP]){
      player2.yPos -= PLAYER_MOVE_SPEED * elapsedTime;
   }

   if(keyboardState[SDL_SCANCODE_DOWN]){
      player2.yPos += PLAYER_MOVE_SPEED * elapsedTime;
   }

   // Check if ball rect overlaps with player rect
   SDL_Rect ballRect = {
      .x = ball.x - BALL_SIZE / 2,
      .y = ball.y - BALL_SIZE / 2,
      .w = BALL_SIZE,
      .h = BALL_SIZE
   };

   SDL_Rect player1Rect = {
      .x = PLAYER_MARGIN,
      .y = (int)player.yPos - PLAYER_HEIGHT / 2,
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT
   };

   if(player.yPos < (float)PLAYER_HEIGHT / 2){
      player.yPos = (float)PLAYER_HEIGHT / 2;
   }

   if(player.yPos > HEIGHT - (float)PLAYER_HEIGHT / 2){
      player.yPos = HEIGHT - (float)PLAYER_HEIGHT / 2;
   }

   if(SDL_HasIntersection(&ballRect, &player1Rect)){
      ball.xSpeed = fabs(ball.xSpeed);	// Ball goes right
   }

   SDL_Rect player2Rect = {
      .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
      .y = (int)player2.yPos - PLAYER_HEIGHT / 2,
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT
   };

   if(player2.yPos < (float)PLAYER_HEIGHT / 2){
      player2.yPos = (float)PLAYER_HEIGHT / 2;
   }

   if(player2.yPos > HEIGHT - (float)PLAYER_HEIGHT / 2){
      player2.yPos = HEIGHT - (float)PLAYER_HEIGHT / 2;
   }
   
   if(SDL_HasIntersection(&ballRect, &player2Rect)){
      ball.xSpeed = -fabs(ball.xSpeed);	// Ball goes left
   }
}

// Renders every player on the screen
void renderPlayers(void)
{
   // Render player 1 (left, green)
   SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
   SDL_Rect player1Rect = {
      .x = PLAYER_MARGIN,
      .y = (int)player.yPos - PLAYER_HEIGHT / 2,
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT
   };
   SDL_RenderFillRect(renderer, &player1Rect);

   // Render player 2 (right, red)
   SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

   SDL_Rect player2Rect = {
      .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
      .y = (int)player2.yPos - PLAYER_HEIGHT / 2,
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT
   };
   SDL_RenderFillRect(renderer, &player2Rect);
}

// BALL
bool coinFlip(void)
{
   return rand() % 2 == 1 ? true : false;
}

Ball makeBall(int size)
{
   const float SPEED = BALL_SPEED;
   Ball ball = {
      .x = (float)WIDTH / 2 - (float)size / 2,
      .y = (float)HEIGHT / 2 - (float)size / 2,
      .size = size,
      .xSpeed = SPEED * (coinFlip() ? 1 : -1),
      .ySpeed = SPEED * (coinFlip() ? 1 : -1),
   };

   return ball;
}

// Updates ball every frame
void updateBall(Ball *ball, float elapsedTime)
{
   if(!served){
      ball->x = (float)WIDTH / 2;
      ball->y = (float)HEIGHT / 2;
      return;
   }

   ball->x += ball->xSpeed * elapsedTime;
   ball->y += ball->ySpeed * elapsedTime;

   if(ball->x < BALL_SIZE / 2){
//         ball->xSpeed = fabs(ball->xSpeed);
      updateScore(2, 100);
   }

   if(ball->x > WIDTH - BALL_SIZE / 2){
//         ball->xSpeed = -fabs(ball->xSpeed);
      updateScore(1, 100);
   }

   if(ball->y < BALL_SIZE / 2){
      ball->ySpeed = fabs(ball->ySpeed);
   }

   if(ball->y > HEIGHT - BALL_SIZE / 2){
      ball->ySpeed = -fabs(ball->ySpeed);

   }
}

// renders ball
void renderBall(void)
{
   int size = ball.size;
   int halfSize = size / 2;
   SDL_Rect rect = {
      .x = ball.x - halfSize,
      .y = ball.y - halfSize,
      .w = size,
      .h = size,
   };

   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
   SDL_RenderFillRect(renderer, &rect);
}

// Updates the score
void updateScore(int thePlayer, int points)
{
   // if a score happens set serving to false and update the score depending on thePlayer
   served = false;
   if(thePlayer == 1){
      player.score += points;
   }

   if(thePlayer == 2){
      player2.score += points;
   }

   // Safely format a string to the appropiate size and set the title to the new buffer string
   char *fmt = "Player: %d | Player2: %d";
   int len = snprintf(NULL, 0, fmt, player.score, player2.score);
   char buffer[len + 1];
   snprintf(buffer, len + 1, fmt, player.score, player2.score);
   SDL_SetWindowTitle(window, buffer);
   
}
