//
// Created by block on 2022/1/22.
//
#include "raylib.h"

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define BORDERCOLOR (Color){33, 77, 112, 255}

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { FIRST = 0, SECOND, THIRD } EnemyWave;

typedef struct Player{
    Rectangle rec;
    Vector2 speed;
    Color color;
} Player;

typedef struct Enemy{
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
} Enemy;

typedef struct Shoot{
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
} Shoot;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
    //Numbers
    int g_numOfArc, g_numOfHive, g_numOfShield;

    //images
    Texture2D g_Arc, g_Hive, g_Shield,
              g_HShoot, g_AShoot, g_BShoot, g_EShoot,
              g_Enemy1, g_Enemy2, g_Enemy3A, g_Enemy3H, g_Enemy3S,
              g_Boss;

    //audios

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(960, 960, "Space Shooter");

    SetTargetFPS(60);
    InitGame();

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{
    //Load images
    g_Arc = LoadTexture("Arc.png");
    g_Hive = LoadTexture("Hive.png");
    g_Shield = LoadTexture("Shield.png");
    // Initialize game variables
    g_numOfArc = 0;
    g_numOfShield = 3;
    g_numOfHive = 0;

    // Initialize player


    // Initialize enemies


    // Initialize shoots

}

// Update game (one frame)
void UpdateGame(void)
{
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    //Draw UI
    DrawRectangle(0, 800 , 960, 160, BORDERCOLOR);
    DrawRectangle(0, 0 , 40, 960, BORDERCOLOR);
    DrawRectangle(920, 0 , 40, 960, BORDERCOLOR);
    DrawPoly((Vector2){480,1070}, 6 , 360, 90, BORDERCOLOR);
    DrawRectangle(10, 810 , 940, 145, (Color){33,110,112,255});
    DrawPoly((Vector2){480,1070}, 6 , 350, 90, (Color){33,110,112,255});
    DrawRectangle(0, 950 , 960, 10, BORDERCOLOR);


    DrawTextureEx(g_Arc, (Vector2){190,820}, 0, 0.5f, WHITE);//24x8x0.5= 96pixels
    DrawText(TextFormat("x%d",g_numOfArc), 216, 920, 32, WHITE);

    DrawTextureEx(g_Hive, (Vector2){674,820}, 0, 0.5f, WHITE);
    DrawText(TextFormat("x%d",g_numOfHive), 700, 920, 32, WHITE);

    DrawTextureEx(g_Shield, (Vector2){310,770}, 0, 0.25f, WHITE);
    DrawRectangle(365, 785, 285, 20, (Color){0,20,20,128});
    for (int i = 1; i <= g_numOfShield && i <= 9; i++) {
        DrawRectangle(370 + (i-1)*31, 790, 27, 10, BLUE);
    }
    for (int i = g_numOfShield +1; 1<= i && i <= 9; i++) {
        DrawRectangle(370 + (i-1)*31, 790, 27, 10, GRAY);
    }

    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
