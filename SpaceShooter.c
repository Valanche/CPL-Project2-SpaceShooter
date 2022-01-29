//
// Created by block on 2022/1/22.
//
#include "raylib.h"

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define NUM_MAX_ENEMY1 4
#define NUM_MAX_ENEMY2 16
#define NUM_MAX_ENEMY3 8
#define NUM_PLAYER_SHOOT 48

#define PLAYER_SHOOT_FREQUENCY 40
#define ENEMY_SHOOT_FREQUENCY 300

#define NUM_PLAYER_HIVE 108
#define NUM_MAX_ENEMY_SHOOT 96
#define NUM_MAX_ENEMY_ARC 48
#define NUM_MAX_BOSS_SHOOT 96
#define BORDERCOLOR (Color){33, 77, 112, 255}

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { FIRST = 1, SECOND = 2, THIRD = 3 } GameStage;
typedef enum { NORMAL = 0, HIT, DROP } ColorFilter;
typedef enum { DEFUALT = 0, ARC, HIVE, EDEFAULT, EARC, BOSS} WeaponKind;
typedef enum { O = 0, A, H, S} StationKind;



typedef struct Player{
    Rectangle hitbox;
    Vector2 speed;
    WeaponKind weapon;
    Color color;
    ColorFilter state;
    Texture2D texture;

} Player;

typedef struct Enemy{
    Rectangle hitbox;
    Vector2 speed;
    bool active;
    Color color;
    WeaponKind type;
    int health;
    ColorFilter state;
    int shootRate;
} Enemy;

typedef struct Shoot{
    Rectangle hitbox;
    Vector2 speed;
    bool active;
    WeaponKind type;
    int damage;


} Shoot;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
    //entities
    static Player g_player = { 0 };
    static Enemy g_enemy1[NUM_MAX_ENEMY1] ={0};
    static Enemy g_enemy2[NUM_MAX_ENEMY2] ={0};
    static Enemy g_enemy3[NUM_MAX_ENEMY3] ={0};

    static Shoot g_playerShoot[NUM_PLAYER_SHOOT] = {0};
    static Shoot g_playerArc = {0};
    static Shoot g_hive[NUM_PLAYER_HIVE] = {0};
    static Shoot g_enemyShoot[NUM_MAX_ENEMY_SHOOT] = {0};
    static Shoot g_enemyArc[NUM_MAX_ENEMY_ARC] = {0};
    static Shoot g_bossShoot[NUM_MAX_BOSS_SHOOT] = {0};
    //numbers
    static int g_numOfArc, g_numOfHive, g_numOfShield;

    //counts
    static GameStage g_stage = FIRST;
    static int g_shootRate = 0;
    static float g_alpha = 0.0f;
    static int g_enemyKills = 0;

    //bools
    static bool g_isStarted = false;
    static bool g_isGameOver = false;
    static bool g_isPaused =  false;
    static bool g_smooth = false;
    static bool g_isWin =  false;
    //images
    Texture2D g_arcTexture, g_hiveTexture, g_shieldTexture,
              g_playerShootTexture, g_hiveShootTexture, g_arcShootTexture, g_bossShootTexture, g_enemyShootTexture,
              g_enemy1Texture, g_enemy2Texture, g_enemy3ATexture, g_enemy3HTexture, g_enemy3STexture,
              g_bossTexture, g_playerTexture;


    //audios

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UpdateShoot(Shoot *shoot, int maxShoot, int frequency, int flag);

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
    while (999)
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{
    //Load images
    g_arcTexture = LoadTexture("Textures/Arc.png");
    g_hiveTexture = LoadTexture("Textures/Hive.png");
    g_shieldTexture = LoadTexture("Textures/Shield.png");
    g_playerTexture = LoadTexture("Textures/Player.png");
    g_enemyShootTexture = LoadTexture("Textures/EnemyShoot.png");

    g_enemy1Texture = LoadTexture("Textures/Enemy1.png");

    g_playerShootTexture = LoadTexture("Textures/PlayerShoot.png");
    // Initialize game variables
    g_shootRate = PLAYER_SHOOT_FREQUENCY;
    g_alpha = 0;

    g_numOfArc = 0;
    g_numOfShield = 3;
    g_numOfHive = 0;

    g_isStarted = false;
    g_isGameOver = false;
    g_isPaused = false;
    g_isWin = false;

    // Initialize player
    g_player.hitbox.x = 440;
    g_player.hitbox.y = 620;
    g_player.hitbox.width = 68;
    g_player.hitbox.height = 64;
    g_player.speed.x = 8;
    g_player.speed.y = 8;
    g_player.state = NORMAL;
    g_player.color = WHITE;
    g_player.texture = g_playerTexture;
    g_player.weapon = DEFUALT;

    // Initialize enemies

    for (int i = 0; i < NUM_MAX_ENEMY1; i++) {
        g_enemy1[i].active = false;
        g_enemy1[i].hitbox.width = 68;
        g_enemy1[i].hitbox.height = 96;
        g_enemy1[i].hitbox.x = GetRandomValue(40, 850);
        g_enemy1[i].hitbox.y = GetRandomValue(-1000,-100);
        g_enemy1[i].type = EDEFAULT;
        g_enemy1[i].state = NORMAL;
        g_enemy1[i].speed.y = 4;
        g_enemy1[i].color = WHITE;
        g_enemy1[i].health = 3;
        g_enemy1[i].shootRate = 0;
    }


    // Initialize shoots
    for (int i = 0; i < NUM_PLAYER_SHOOT; i++) {
        g_playerShoot[i].hitbox.width = 12;
        g_playerShoot[i].hitbox.height = 16;
        g_playerShoot[i].speed.y = 12;
        g_playerShoot[i].speed.x = 0;
        g_playerShoot[i].type = DEFUALT;
        g_playerShoot[i].active = false;
        g_playerShoot[i].damage = 1;

    }

    for (int i = 0; i < NUM_MAX_ENEMY_SHOOT; i++) {
        g_enemyShoot[i].hitbox.width = 12;
        g_enemyShoot[i].hitbox.height = 16;
        g_enemyShoot[i].speed.y = -6;
        g_enemyShoot[i].speed.x = 0;
        g_enemyShoot[i].type = EDEFAULT;
        g_enemyShoot[i].active = false;
        g_enemyShoot[i].damage = 1;

    }
}

// Update game (one frame)
void UpdateShoot(Shoot * shoot, int maxShoot, int frequency, int flag){
    for (int i = 0; i < maxShoot; i++)
    {
        if (shoot[i].active)
        {
            // Movement
            shoot[i].hitbox.y -= shoot[i].speed.y;
            shoot[i].hitbox.x += shoot[i].speed.x;

            // Collision with ?
            switch (flag) {
                case 0:
                    for (int j = 0; j < NUM_MAX_ENEMY1; j++)
                    {
                        if (g_enemy1[j].active)
                        {
                            if (CheckCollisionRecs(shoot[i].hitbox, g_enemy1[j].hitbox))
                            {
                                shoot[i].active = false;
                                g_enemy1[j]. state= HIT;
                                g_enemy1[j]. health -= shoot[i].damage;
                                g_shootRate -= frequency;

                            }
                        }

                    }
                    break;
                case 1:
                    if (CheckCollisionRecs(shoot[i].hitbox, g_player.hitbox))
                    {
                        shoot[i].active = false;
                        g_player. state= HIT;
                        g_numOfShield -= shoot[i].damage;

                    }
                    break;
            }


            //Missed
            if (shoot[i].hitbox.y <= 0 || shoot[i].hitbox.y > 700)
            {
                shoot[i].active = false;
                if(flag == 0){
                    g_shootRate -= frequency;
                }

            }
        }
    }

}

void UpdateGame(void){
    if(g_isGameOver == false){

        if(g_isStarted && (g_isPaused == false)){



            //Player movement

            if(IsKeyDown('W')) g_player.hitbox.y -= g_player.speed.y;
            if(IsKeyDown('A')) g_player.hitbox.x -= g_player.speed.x;
            if(IsKeyDown('S')) g_player.hitbox.y += g_player.speed.y;
            if(IsKeyDown('D')) g_player.hitbox.x += g_player.speed.x;
            /*g_player.hitbox.x = GetMouseX() - 32;
            g_player.hitbox.y = GetMouseY() - 32;*/


            //WALL
            if(g_player.hitbox.x <=44) g_player.hitbox.x = 44;
            if(g_player.hitbox.x >=832) g_player.hitbox.x = 832;
            if(g_player.hitbox.y <= 4) g_player.hitbox.y = 4;
            if(g_player.hitbox.y >= 690) g_player.hitbox.y = 690;

            //player shoot
            if (IsKeyDown(KEY_SPACE))
            {
                g_shootRate += 5;

                switch (g_player.weapon) {
                    case DEFUALT:
                        for (int i = 0; i < NUM_PLAYER_SHOOT; i++)
                        {
                            if ((g_playerShoot[i].active == false) && (g_shootRate % PLAYER_SHOOT_FREQUENCY == 0))
                            {
                                g_playerShoot[i].hitbox.x = g_player.hitbox.x + 36;
                                g_playerShoot[i].hitbox.y = g_player.hitbox.y + 12;
                                g_playerShoot[i].active = true;
                                break;
                            }
                        }
                        break;
                    case ARC:
                        g_playerArc.hitbox.x = g_player.hitbox.x +  24;
                        break;
                    case HIVE:break;

                }

            }
            //gameover
            if(g_numOfShield < 0){
                g_isGameOver = true;
            }
            //enemies
                //enemy1
            for (int i = 0; i < NUM_MAX_ENEMY1; i++) {

                if(g_enemy1[i].active){
                    //color
                    switch (g_enemy1[i].state) {
                        case NORMAL:
                            g_enemy1[i].color = WHITE;
                            break;
                        case HIT:
                            g_enemy1[i].color = RED;
                            g_enemy1[i].state = NORMAL;
                            break;
                        case DROP:
                            g_enemy1[i].color = GREEN;
                            g_enemy1[i].active = false;
                            break;
                    }

                    //movement + shoot
                    if(g_enemy1[i].state != DROP){
                        if(g_enemy1[i].hitbox.y <= 120 + GetRandomValue(0,60)){
                            g_enemy1[i].hitbox.y += g_enemy1[i].speed.y;
                        } else{
                            g_enemy1[i].hitbox.y += 1;
                            g_enemy1[i].shootRate += 2;

                            if(g_enemy1[i].shootRate >= ENEMY_SHOOT_FREQUENCY){
                                for (int j = 0; j < NUM_MAX_ENEMY_SHOOT ; j++) {
                                    if((g_enemyShoot[j].active == false) &&((g_player.hitbox.x > g_enemy1[i].hitbox.x - 40)&& g_player.hitbox.x < g_enemy1[i].hitbox.x + 40)){
                                        g_enemyShoot[j].hitbox.x = g_enemy1[i].hitbox.x + 28;
                                        g_enemyShoot[j].hitbox.y = g_enemy1[i].hitbox.y + 96;
                                        g_enemyShoot[j].active = true;

                                        g_enemy1[i].shootRate = 0;
                                        break;

                                    }
                                }
                            }

                        }
                    }


                    if(CheckCollisionRecs(g_player.hitbox, g_enemy1[i].hitbox)){
                        g_enemy1[i].state = DROP;
                        g_player.state = HIT;
                        g_numOfShield -= 1;
                    }

                    if(g_enemy1[i].hitbox.y > 700){
                        g_numOfShield -= 1;
                        g_enemy1[i].hitbox.y = GetRandomValue(-600, -100);
                    }

                    if(g_enemy1[i].health <= 0) {
                        g_enemy1[i].state = DROP;
                        g_enemyKills += 1;
                    }

                } else if (g_alpha < 0){
                    g_enemy1[i].hitbox.x = GetRandomValue(40,850);
                    g_enemy1[i].hitbox.y = GetRandomValue(-600, -100);
                    g_enemy1[i].health = 3;
                    g_enemy1[i].state = NORMAL;
                    g_enemy1[i].active =true;

                }

            }

            // Shoot logic
            UpdateShoot(g_playerShoot, NUM_PLAYER_SHOOT,PLAYER_SHOOT_FREQUENCY, 0);

            UpdateShoot(g_enemyShoot, NUM_MAX_ENEMY_SHOOT,ENEMY_SHOOT_FREQUENCY,1);



            if(WindowShouldClose() || IsKeyPressed('P')) {
                g_isPaused = true;
            }

            if (!g_isPaused)
            {
                switch (g_stage)
                {
                    case FIRST:
                    {
                        if (!g_smooth)
                        {
                            g_alpha += 0.02f;

                            if (g_alpha >= 1.0f) g_smooth = true;
                        }

                        if (g_smooth) g_alpha -= 0.02f;

                        if (g_enemyKills == 20)
                        {
                            g_enemyKills = 0;
                            g_stage = SECOND;
                            g_smooth = false;
                            g_alpha = 0.0f;
                        }
                    } break;
                    case SECOND:
                    {
                        if (!g_smooth)
                        {
                            g_alpha += 0.02f;

                            if (g_alpha >= 1.0f) g_smooth = true;
                        }

                        if (g_smooth) g_alpha -= 0.02f;


                        if (g_enemyKills == 50)
                        {
                            g_enemyKills = 0;
                            g_stage = THIRD;
                            g_smooth = false;
                            g_alpha = 0.0f;
                        }

                    } break;
                    case THIRD:
                    {
                        if (!g_smooth)
                        {
                            g_alpha += 0.02f;

                            if (g_alpha >= 1.0f) g_smooth = true;
                        }

                        if (g_smooth) g_alpha -= 0.02f;
                    } break;
                    default: break;
                }

            }

        }else if(g_isPaused){

            if(IsKeyPressed('P') || WindowShouldClose()){
                g_isPaused = false;
            } else if(IsKeyPressed('Q')){
                CloseWindow();
            }

        }else if(g_isStarted == false){
            if(WindowShouldClose() || IsKeyPressed('P')){
                g_isPaused = true;
            } else if( IsKeyPressed('W') || IsKeyPressed('S') || IsKeyPressed('A') || IsKeyPressed('D')){
                g_isStarted = true;
            }
        }

    } else {
        if(WindowShouldClose()){
            CloseWindow();
        }
    }


}

// Draw game (one frame)

void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    //Draw UI
    DrawRectangle(0, 830 , 960, 160, BORDERCOLOR);
    DrawRectangle(0, 0 , 40, 960, BORDERCOLOR);
    DrawRectangle(920, 0 , 40, 960, BORDERCOLOR);
    DrawPoly((Vector2){480,1120}, 6 , 360, 90, BORDERCOLOR);
    DrawRectangle(10, 860 , 940, 145, (Color){33,110,112,255});
    DrawPoly((Vector2){480,1120}, 6 , 350, 90, (Color){33,110,112,255});
    DrawRectangle(0, 950 , 960, 10, BORDERCOLOR);


    DrawTextureEx(g_arcTexture, (Vector2){214, 870}, 0, 0.25f, WHITE);//24x8x0.5= 96pixels
    DrawText(TextFormat(" x%d",g_numOfArc), 216, 920, 24, WHITE);

    DrawTextureEx(g_hiveTexture, (Vector2){698, 870}, 0, 0.25f, WHITE);
    DrawText(TextFormat(" x%d",g_numOfHive), 700, 920, 24, WHITE);

    DrawTextureEx(g_shieldTexture, (Vector2){310, 820}, 0, 0.25f, WHITE);
    DrawRectangle(365, 835, 285, 20, (Color){0,20,20,128});
    for (int i = 1; i <= g_numOfShield && i <= 9; i++) {
        DrawRectangle(370 + (i-1)*31, 840, 27, 10, BLUE);
    }
    for (int i = g_numOfShield +1; 1<= i && i <= 9; i++) {
        DrawRectangle(370 + (i-1)*31, 840, 27, 10, GRAY);
    }

    DrawTextureEx(g_player.texture, (Vector2){g_player.hitbox.x - 32, g_player.hitbox.y - 32}, 0, 0.5f, g_player.color);
    //DrawRectangle(g_player.hitbox.x, g_player.hitbox.y, 96, 96, g_player.color );
    DrawRectangle(g_player.hitbox.x, g_player.hitbox.y, 4, 4, GREEN );
    DrawRectangle(g_player.hitbox.x + 64, g_player.hitbox.y, 4, 4, GREEN );
    DrawRectangle(g_player.hitbox.x, g_player.hitbox.y + 60, 4, 4, GREEN );
    DrawRectangle(g_player.hitbox.x + 64, g_player.hitbox.y + 60, 4, 4, GREEN );




    if(g_isStarted){
        //Draw background;
        if (g_stage == FIRST) DrawText("STAGE 1", 480 - MeasureText("STAGE 1", 40)/2, 480 - 40, 40, Fade(RED, g_alpha));
        else if (g_stage == SECOND) DrawText("STAGE 2", 480 - MeasureText("STAGE 2", 40)/2, 480 - 40, 40, Fade(RED, g_alpha));
        else if (g_stage == THIRD) DrawText("STAGE 3", 480 - MeasureText("STAGE 3", 40)/2, 480 - 40, 40, Fade(RED , g_alpha));

        //Draw enemies
        for (int i = 0; i < NUM_MAX_ENEMY1; i++) {
            if(g_enemy1[i].active){
                DrawTextureEx(g_enemy1Texture,(Vector2){g_enemy1[i].hitbox.x - 32, g_enemy1[i].hitbox.y - 16}, 0, 0.5f, g_enemy1[i].color);

            }
        }

        //Draw shoots
        for (int i = 0; i < NUM_PLAYER_SHOOT; i++) {
            if(g_playerShoot[i].active){
                DrawTextureEx(g_playerShootTexture, (Vector2){g_playerShoot[i].hitbox.x, g_playerShoot[i].hitbox.y}, 0, 0.5f, WHITE);

            }
        }

        for (int i = 0; i < NUM_MAX_ENEMY_SHOOT; i++) {
            if(g_enemyShoot[i].active){
                DrawTextureEx(g_enemyShootTexture, (Vector2){g_enemyShoot[i].hitbox.x, g_enemyShoot[i].hitbox.y}, 0, 0.5f, WHITE);

            }
        }


    }else{
        DrawText("[ Move to Start ]", 480 - MeasureText("[ Move to Start ]",32)/2, 440, 32, GRAY);
    }

    //Paused
    if(g_isPaused){
        DrawRectangle(0,0,960,960,CLITERAL(Color){ 0, 0, 0, 180 });
        DrawText("--- GAME PAUSED ---", 480 - MeasureText("--- GAME PAUSED ---", 48)/2, 400, 48, RED);
        DrawText("press 'P' to continue", 480 - MeasureText("press 'P' to continue", 32)/2, 480, 32, RED);
        DrawText("press 'Q' to exit game", 480 - MeasureText("press 'Q' to exit game", 32)/2, 520, 32, RED);
    }

    if(g_isGameOver){
        DrawRectangle(0,0,960,960,CLITERAL(Color){ 0, 0, 0, 180 });
        DrawText("--- GAME OVER ---", 480 - MeasureText("--- GAME OVER ---", 48)/2, 400, 48, RED);
        DrawText("press [ENTER] to play again", 480 - MeasureText("press [ENTER] to play again", 32)/2, 480, 32, RED);
        DrawText("press [ESC] to exit", 480 - MeasureText("press [ESC] to exit", 32)/2, 520, 32, RED);
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
