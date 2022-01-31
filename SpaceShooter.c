//
// Created by block on 2022/1/22.
//
#include "raylib.h"

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------

// MAX nums

#define NUM_PLAYER_SHOOT 48
#define NUM_PLAYER_HIVE 108
#define NUM_PLAYER_ARC 16

#define NUM_MAX_ENEMY1 3
#define NUM_MAX_ENEMY2 4
#define NUM_MAX_ENEMY3 1

#define NUM_MAX_ENEMY_SHOOT 96
#define NUM_MAX_ENEMY_ARC 24
#define NUM_MAX_BOSS_SHOOT 96

#define NUM_MAX_ITEM 3

// balance stats
#define PLAYER_SHOOT_FREQUENCY 8
#define PLAYER_HIVE_FREQUENCY 6
#define PLAYER_HEAT_MAX 96
#define PLAYER_INIT_SHIELDS 3
#define PLAYER_MAX_SHIELDS 9
#define PLAYER_INIT_HIVE 3
#define PLAYER_INIT_ARC 3
#define PLAYER_ARC_DURATION 512
#define PLAYER_HIVE_DURATION 256


#define ENEMY1_HEAT_MAX 4
#define ENEMY1_HP 3

#define ENEMY2_HEAT_MAX 12
#define ENEMY2_HP 5

#define ENEMY3_HEAT_MAX 4
#define ENEMY3_HP 12

#define ENEMY_SHOOT_FREQUENCY 100
#define ENEMY_ARC_FOCUS 128
#define ENEMY_ARC_DURATION 512
#define ENEMY_ARC_COOLDOWN 256

// other stats
#define FIRST_REQUIREMENT 16
#define SECOND_REQUIREMENT 48



#define BORDERCOLOR (Color){33, 77, 112, 255}

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { FIRST = 1, SECOND = 2, THIRD = 3 } GameStage;
typedef enum { NORMAL = 0, HIT, DROP } ColorFilter;
typedef enum { DEFUALT = 0, ARC, HIVE, EDEFAULT, EARC, BOSS, O, A, S, H} WeaponKind;

typedef struct Player{
    Rectangle hitbox;
    Vector2 speed;
    WeaponKind weapon;
    Color color;
    ColorFilter state;
    Texture2D texture;
    float heat;

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
    float heat;
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

    static Enemy g_items[NUM_MAX_ITEM] = {0};

    static Shoot g_playerShoot[NUM_PLAYER_SHOOT] = {0};
    static Shoot g_playerArc[NUM_PLAYER_ARC] = {0};
    static Shoot g_hiveL[NUM_PLAYER_HIVE] = {0};
    static Shoot g_hiveR[NUM_PLAYER_HIVE] = {0};
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

    static int g_arcTimer = 0;
    static int g_hiveTimer = 0;

    //bools
    static bool g_isStarted = false;
    static bool g_isGameOver = false;
    static bool g_isPaused =  false;
    static bool g_smooth = false;
    static bool g_isWin =  false;
    static bool g_retry = false;

    static bool g_arcActive = false;
    static bool g_hiveActive = false;

    //images
    Texture2D g_arcTexture, g_hiveTexture, g_shieldTexture,
              g_playerShootTexture, g_hiveShootTexture, g_arcHeadTexture, g_arcBodyTexture, //player shoot
              g_enemyShootTexture, g_EArcHeadTexture, g_EArcBodyTexture, g_bossShootTexture, //enemy shoot
              g_enemy1Texture, g_enemy2Texture,
              g_enemy3OTexture, g_enemy3ATexture, g_enemy3HTexture, g_enemy3STexture,
              g_bossTexture, g_playerTexture;


    //audios

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void LoadImgs(void);//Load textures
static void InitGame(void);// Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void UpdateShoot(Shoot *shoot, int maxShoot, int frequency, int flag);
static void UpdateArc(Shoot * shoot, int maxShoot, int frequency, int flag);
static void UpdateEnemy1(Enemy * enemy, Shoot * shoot, int maxEnemy);
static void UpdateEnemy2(Enemy * enemy, Shoot * shoot, int maxEnemy);
static void UpdateEnemy3(Enemy * enemy, Shoot * shoot, int maxEnemy);
static void UpdateItems(Enemy * enemy, Shoot * shoot, int maxEnemy);

static void DrawGame(void);         // Draw game (one frame)
static void DrawUI(void);

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
    LoadImgs();
    retry:
    InitGame();

    // Main game loop
    while (999)
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        if(g_retry){
            goto retry;
        }
        //----------------------------------------------------------------------------------
    }
    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------
//Load textures
void LoadImgs(void){
    //Load images
    g_arcTexture = LoadTexture("Textures/Arc.png");
    g_hiveTexture = LoadTexture("Textures/Hive.png");
    g_shieldTexture = LoadTexture("Textures/Shield.png");

    g_playerTexture = LoadTexture("Textures/Player.png");

    g_playerShootTexture = LoadTexture("Textures/PlayerShoot.png");
    g_arcHeadTexture = LoadTexture("Textures/ArcHead.png");
    g_arcBodyTexture = LoadTexture("Textures/ArcBody.png");
    g_hiveShootTexture = LoadTexture("Textures/HiveShoot.png");

    g_enemy1Texture = LoadTexture("Textures/Enemy1.png");
    g_enemy2Texture = LoadTexture("Textures/Enemy2.png");
    g_enemy3OTexture = LoadTexture("Textures/Enemy3O.png");
    g_enemy3ATexture = LoadTexture("Textures/Enemy3A.png");
    g_enemy3HTexture = LoadTexture("Textures/Enemy3H.png");
    g_enemy3STexture = LoadTexture("Textures/Enemy3S.png");

    g_enemyShootTexture = LoadTexture("Textures/EnemyShoot.png");
    g_EArcHeadTexture = LoadTexture("Textures/EArcHead.png");
    g_EArcBodyTexture = LoadTexture("Textures/EArcBody.png");
    g_bossShootTexture = LoadTexture("Textures/BossShoot.png");

}

// Initialize game variables
void InitGame(void)
{
    // Initialize game variables
    g_stage = FIRST;
    g_shootRate = PLAYER_SHOOT_FREQUENCY;
    g_alpha = 0.0f;

    g_numOfArc = PLAYER_INIT_ARC;
    g_numOfShield = PLAYER_INIT_SHIELDS;
    g_numOfHive = PLAYER_INIT_HIVE;
    g_enemyKills = 0;

    g_isStarted = false;
    g_isGameOver = false;
    g_isPaused = false;
    g_smooth = false;
    g_isWin =  false;
    g_retry = false;

    g_arcActive = false;

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
    g_player.heat = 0;

    // Initialize enemies

    for (int i = 0; i < NUM_MAX_ENEMY1; i++) {
        g_enemy1[i].active = false;
        g_enemy1[i].hitbox.width = 68;
        g_enemy1[i].hitbox.height = 128;
        g_enemy1[i].hitbox.x = GetRandomValue(40, 850);
        g_enemy1[i].hitbox.y = GetRandomValue(-1400,-300);
        g_enemy1[i].type = EDEFAULT;
        g_enemy1[i].state = NORMAL;
        g_enemy1[i].speed.y = 4;
        g_enemy1[i].color = WHITE;
        g_enemy1[i].health = ENEMY1_HP;
        g_enemy1[i].shootRate = 50;
        g_enemy1[i].heat = 0;
    }

    for (int i = 0; i < NUM_MAX_ENEMY2; i++) {
        g_enemy2[i].active = false;
        g_enemy2[i].hitbox.width = 76;
        g_enemy2[i].hitbox.height = 96;
        g_enemy2[i].hitbox.x = 78 + GetRandomValue(0,5) * 152;
        g_enemy2[i].hitbox.y = GetRandomValue(-2000,-600);
        g_enemy2[i].type = EARC;
        g_enemy2[i].state = NORMAL;
        g_enemy2[i].speed.y = 2;
        g_enemy2[i].color = WHITE;
        g_enemy2[i].health = ENEMY2_HP;
        g_enemy2[i].shootRate = 0;
        g_enemy2[i].heat = 0;
    }

    for (int i = 0; i < NUM_MAX_ENEMY3; i++) {
        g_enemy3[i].active = false;
        g_enemy3[i].hitbox.width = 96;
        g_enemy3[i].hitbox.height = 96;
        g_enemy3[i].hitbox.x = GetRandomValue(40, 850);
        g_enemy3[i].hitbox.y = GetRandomValue(-2000,-1400);
        g_enemy3[i].type = O;
        g_enemy3[i].state = NORMAL;
        g_enemy3[i].speed.y = 2;
        g_enemy3[i].color = WHITE;
        g_enemy3[i].health = ENEMY3_HP;
        g_enemy3[i].shootRate = 0;
        g_enemy3[i].heat = 0;
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
    for (int i = 0; i < NUM_PLAYER_HIVE; i++) {
        g_hiveL[i].hitbox.width = 12;
        g_hiveL[i].hitbox.height = 20;
        g_hiveL[i].speed.y = 8;
        g_hiveL[i].speed.x = 0;
        g_hiveL[i].type = HIVE;
        g_hiveL[i].active = false;
        g_hiveL[i].damage = 1;

    }
    for (int i = 0; i < NUM_PLAYER_HIVE; i++) {
        g_hiveR[i].hitbox.width = 12;
        g_hiveR[i].hitbox.height = 20;
        g_hiveR[i].speed.y = 8;
        g_hiveR[i].speed.x = 0;
        g_hiveR[i].type = HIVE;
        g_hiveR[i].active = false;
        g_hiveR[i].damage = 1;

    }
    for (int i = 0; i < NUM_PLAYER_ARC; i++) {
        g_playerArc[i].hitbox.width = 28;
        g_playerArc[i].hitbox.height = 700;
        g_playerArc[i].speed.y = 0;
        g_playerArc[i].speed.x = 0;
        g_playerArc[i].type = ARC;
        g_playerArc[i].active = false;
        g_playerArc[i].damage = 1;
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
    for (int i = 0; i < NUM_MAX_ENEMY_ARC; i++) {
        g_enemyArc[i].hitbox.width = 28;
        g_enemyArc[i].hitbox.height = 700;
        g_enemyArc[i].speed.y = 0;
        g_enemyArc[i].speed.x = 0;
        g_enemyArc[i].type = EARC;
        g_enemyArc[i].active = false;
        g_enemyArc[i].damage = 1;
    }

    // initialize items

    for (int i = 0; i < NUM_MAX_ITEM; i++) {
        g_items[i].active = false;
        g_items[i].hitbox.width = 48;
        g_items[i].hitbox.height = 48;
        g_items[i].hitbox.x = 0;
        g_items[i].hitbox.y = 0;
        g_items[i].type = O;
        g_items[i].state = NORMAL;
        g_items[i].speed.y = 1;
        g_items[i].color = WHITE;
        g_items[i].health = ENEMY3_HP;
        g_items[i].shootRate = 0;
        g_items[i].heat = 0;
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
                    for (int j = 0; j < NUM_MAX_ENEMY2; j++)
                    {
                        if (g_enemy2[j].active)
                        {
                            if (CheckCollisionRecs(shoot[i].hitbox, g_enemy2[j].hitbox))
                            {
                                shoot[i].active = false;
                                g_enemy2[j]. state= HIT;
                                g_enemy2[j]. health -= shoot[i].damage;
                                g_shootRate -= frequency;

                            }
                        }

                    }

                    for (int j = 0; j < NUM_MAX_ENEMY3; j++) {
                        if(g_enemy3[j].active){
                            if (CheckCollisionRecs(shoot[i].hitbox, g_enemy3[j].hitbox))
                            {
                                shoot[i].active = false;
                                g_enemy3[j]. state= HIT;
                                g_enemy3[j]. health -= shoot[i].damage;
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
void UpdateArc(Shoot * shoot, int maxShoot, int frequency, int flag){
    for (int i = 0; i < maxShoot; i++)
    {
        if (shoot[i].active){
            // Collision with ?

            switch (flag) {
                case 0:

                    for (int j = 0; j < NUM_MAX_ENEMY1; j++){
                        if (g_enemy1[j].active){
                            if (CheckCollisionRecs(shoot[i].hitbox, g_enemy1[j].hitbox)){
                                g_enemy1[j]. state= HIT;
                                g_enemy1[j]. heat += shoot[i].damage;

                            }
                            if(g_enemy1[j]. health <= 0){
                                //g_enemy1[j].active = false;
                            }
                        }

                    }

                    for (int j = 0; j < NUM_MAX_ENEMY2; j++){
                        if (g_enemy2[j].active){
                            if (CheckCollisionRecs(shoot[i].hitbox, g_enemy2[j].hitbox)){
                                g_enemy2[j]. state= HIT;
                                g_enemy2[j]. heat += shoot[i].damage;

                            }
                        }


                    }

                    for (int j = 0; j < NUM_MAX_ENEMY3; j++){
                        if (g_enemy3[j].active){
                            if (CheckCollisionRecs(shoot[i].hitbox, g_enemy3[j].hitbox)){
                                g_enemy3[j]. state= HIT;
                                g_enemy3[j]. heat += shoot[i].damage;

                            }
                        }

                    }
                    break;
                case 1:
                    if (CheckCollisionRecs(shoot[i].hitbox, g_player.hitbox))
                    {
                        g_player. state= HIT;
                        g_player.heat += shoot[i].damage;

                    }
                    break;
                default:break;
            }


            shoot[i].active = false;

        }
    }

}
void UpdateEnemy1(Enemy * enemy, Shoot * shoot, int maxEnemy){
    for (int i = 0; i < maxEnemy; i++) {

        if(enemy[i].active){
            // color
            switch (enemy[i].state) {
                case NORMAL:
                    enemy[i].color = WHITE;
                    break;
                case HIT:
                    enemy[i].color = RED;
                    enemy[i].state = NORMAL;
                    break;
                case DROP:
                    enemy[i].color = GREEN;
                    enemy[i].active = false;
                    break;
            }

            // movement + shoot
            if(enemy[i].state != DROP){
                if(enemy[i].hitbox.y <= 120 + GetRandomValue(0,60)){
                    enemy[i].hitbox.y += enemy[i].speed.y;
                } else{
                    enemy[i].hitbox.y += 1;
                    enemy[i].shootRate += 1;

                    if(enemy[i].shootRate >= ENEMY_SHOOT_FREQUENCY){
                        for (int j = 0; j < NUM_MAX_ENEMY_SHOOT ; j++) {
                            if((shoot[j].active == false) &&((g_player.hitbox.x > enemy[i].hitbox.x - 40)&& g_player.hitbox.x < enemy[i].hitbox.x + 40)){
                                shoot[j].hitbox.x = enemy[i].hitbox.x + 28;
                                shoot[j].hitbox.y = enemy[i].hitbox.y + 96;
                                shoot[j].active = true;

                                enemy[i].shootRate = 0;
                                break;

                            }
                        }
                    }

                }

                if(CheckCollisionRecs(g_player.hitbox, enemy[i].hitbox)){
                    enemy[i].state = DROP;
                    g_player.state = HIT;
                    g_numOfShield -= 1;
                }

                if(enemy[i].hitbox.y > 700){
                    g_numOfShield -= 1;
                    enemy[i].hitbox.y = GetRandomValue(-600, -100);
                }

                // response to arc
                if(enemy[i].heat > ENEMY1_HEAT_MAX){
                    enemy[i].state = HIT;
                    enemy[i].health -= 1;
                    enemy[i].heat = 0;
                }

                // die
                if(enemy[i].health <= 0) {
                    enemy[i].active = false;
                    g_enemyKills += 1;
                }








            }




        } else if (g_alpha < 0){
            enemy[i].hitbox.x = GetRandomValue(40,850);
            enemy[i].hitbox.y = GetRandomValue(-1400, -300);
            enemy[i].health = ENEMY1_HP;
            enemy[i].heat = 0;
            enemy[i].state = NORMAL;
            enemy[i].active =true;
            enemy[i].shootRate = 50;


        }

    }
}
void UpdateEnemy2(Enemy * enemy, Shoot * shoot, int maxEnemy){
    for (int i = 0; i < maxEnemy; i++) {

        if(enemy[i].active){
            // color
            switch (enemy[i].state) {
                case NORMAL:
                    enemy[i].color = WHITE;
                    break;
                case HIT:
                    enemy[i].color = RED;
                    enemy[i].state = NORMAL;
                    break;
                case DROP:
                    enemy[i].color = GREEN;
                    enemy[i].active = false;
                    break;
            }

            // movement + shoot
            if(enemy[i].state != DROP){
                if(enemy[i].hitbox.y < 40 ){
                    enemy[i].hitbox.y += enemy[i].speed.y;
                } else{
                    enemy[i].shootRate += 1;

                    if(enemy[i].shootRate >= ENEMY_ARC_FOCUS){
                        if(enemy[i].shootRate >=ENEMY_ARC_FOCUS + ENEMY_ARC_DURATION){
                            enemy[i].shootRate = 0 - ENEMY_ARC_COOLDOWN;
                        } else{
                            for (int j = 0; j < NUM_MAX_ENEMY_ARC ; j++) {
                                if((shoot[j].active == false)){
                                    shoot[j].hitbox.x = enemy[i].hitbox.x + 24;
                                    shoot[j].hitbox.y = enemy[i].hitbox.y + 124;
                                    shoot[j].active = true;

                                    break;
                                }
                            }
                        }

                    }

                }

                if(CheckCollisionRecs(g_player.hitbox, enemy[i].hitbox)){
                    enemy[i].state = DROP;
                    g_player.state = HIT;
                    g_numOfShield -= 1;
                }

                // response to arc
                if(enemy[i].heat > ENEMY2_HEAT_MAX){
                    enemy[i].state = HIT;
                    enemy[i].health -= 1;
                    enemy[i].heat = 0;
                }

                // die
                if(enemy[i].health <= 0) {
                    enemy[i].active = false;
                    g_enemyKills += 1;
                }

            }



        } else if (g_alpha < 0 && (g_stage >= SECOND)){
            enemy[i].hitbox.x = 78 + GetRandomValue(0,5) * 152;
            enemy[i].hitbox.y = GetRandomValue(-2000,-600);
            enemy[i].health = ENEMY2_HP;
            enemy[i].heat = 0;
            enemy[i].state = NORMAL;
            enemy[i].active =true;
            enemy[i].color = WHITE;
            enemy[i].shootRate = 0;


        }

    }
}
void UpdateEnemy3(Enemy * enemy, Shoot * shoot, int maxEnemy){
    for (int i = 0; i < maxEnemy; i++) {

        if(enemy[i].active) {
            // color
            switch (enemy[i].state) {
                case NORMAL:
                    enemy[i].color = WHITE;
                    break;
                case HIT:
                    enemy[i].color = RED;
                    enemy[i].state = NORMAL;
                    break;
                case DROP:
                    enemy[i].color = GREEN;
                    enemy[i].active = false;
                    break;

            }

            // movement
            if (enemy[i].state != DROP) {
                enemy[i].hitbox.y += enemy[i].speed.y;

                if (CheckCollisionRecs(g_player.hitbox, enemy[i].hitbox)) {
                    enemy[i].state = DROP;
                    g_player.state = HIT;
                    g_numOfShield -= 1;
                }


                // response to arc
                if (enemy[i].heat > ENEMY3_HEAT_MAX) {
                    enemy[i].state = HIT;
                    enemy[i].health -= 1;
                    enemy[i].heat = 0;
                }

                // die + item
                if (enemy[i].health <= 0) {
                    enemy[i].active = false;
                    g_enemyKills += 1;

                    for (int j = 0; j < NUM_MAX_ITEM; j++) {
                        if(g_items[j].active == false){
                            g_items[j].type = enemy[i].type;
                            g_items[j].hitbox.x = enemy[i].hitbox.x + 24;
                            g_items[j].hitbox.y = enemy[i].hitbox.y + 24;

                            g_items[j].active = true;
                            break;

                        }
                    }
                }

                if(enemy[i].hitbox.y > 900){
                    enemy[i].hitbox.x = GetRandomValue(40, 850);
                    enemy[i].hitbox.y = GetRandomValue(-1000, -700);
                    enemy[i].health = ENEMY3_HP;
                    enemy[i].heat = 0;
                    enemy[i].state = NORMAL;
                    enemy[i].active = true;
                    enemy[i].color = WHITE;
                    enemy[i].shootRate = 0;
                    enemy[i].type = GetRandomValue(6, 9);
                }

            }




        } else if (g_alpha < 0 && (g_stage >= SECOND)) {
            enemy[i].hitbox.x = GetRandomValue(40, 850);
            enemy[i].hitbox.y = GetRandomValue(-1000, -700);
            enemy[i].health = ENEMY3_HP;
            enemy[i].heat = 0;
            enemy[i].state = NORMAL;
            enemy[i].active = true;
            enemy[i].color = WHITE;
            enemy[i].shootRate = 0;
            enemy[i].type = GetRandomValue(6, 9);

        }
    }
}
void UpdateItems(Enemy * enemy, Shoot * shoot, int maxEnemy){
    for (int i = 0; i < maxEnemy; i++) {

        if(enemy[i].active) {
            // color
            switch (enemy[i].state) {
                case NORMAL:
                    enemy[i].color = WHITE;
                    enemy[i].state = HIT;
                    break;
                case HIT:
                    enemy[i].color = WHITE;
                    enemy[i].state = DROP;
                    break;
                case DROP:
                    enemy[i].color = GOLD;
                    enemy[i].state = NORMAL;
                    break;

            }

            // movement
            if (enemy[i].state != DROP) {
                enemy[i].hitbox.y += enemy[i].speed.y;
            }

            // get item
            if (CheckCollisionRecs(g_player.hitbox, enemy[i].hitbox)) {
                switch (enemy[i].type) {
                    case A:
                        g_numOfArc += 1;
                        break;
                    case S:
                        g_numOfShield += 1;
                        break;
                    case H:
                        g_numOfHive += 1;
                        break;
                    default:break;
                }

                enemy[i].active = false;

            }

            if(enemy[i].hitbox.y > 900){
                enemy[i].hitbox.x = GetRandomValue(40, 850);
                enemy[i].hitbox.y = GetRandomValue(-1000, -700);
                enemy[i].health = ENEMY3_HP;
                enemy[i].heat = 0;
                enemy[i].state = NORMAL;
                enemy[i].active = false;
                enemy[i].color = WHITE;
                enemy[i].shootRate = 0;
            }
        }
    }
}

void UpdateGame(void){
    if(g_isGameOver == false){

        if(g_isStarted && (g_isPaused == false)){

            //Player control

            if(IsKeyDown('W')) g_player.hitbox.y -= g_player.speed.y;
            if(IsKeyDown('A')) g_player.hitbox.x -= g_player.speed.x;
            if(IsKeyDown('S')) g_player.hitbox.y += g_player.speed.y;
            if(IsKeyDown('D')) g_player.hitbox.x += g_player.speed.x;
            /*g_player.hitbox.x = GetMouseX() - 32;
            g_player.hitbox.y = GetMouseY() - 32;*/

            if(IsKeyPressed('J') && g_numOfArc > 0) {
                g_arcActive = true;
                g_numOfArc -= 1;
            }
            if(IsKeyPressed('K') && g_numOfHive > 0) {
                g_hiveActive = true;
                g_numOfHive -= 1;
            }


            //WALL
            if(g_player.hitbox.x <=44) g_player.hitbox.x = 44;
            if(g_player.hitbox.x >=832) g_player.hitbox.x = 832;
            if(g_player.hitbox.y <= 4) g_player.hitbox.y = 4;
            if(g_player.hitbox.y >= 690) g_player.hitbox.y = 690;

            //player shoot
            g_shootRate += 1;

            if(g_arcActive){
                g_arcTimer += 1;

                for (int j = 0; j < NUM_PLAYER_ARC; j++) {
                    if(g_playerArc[j].active == false){
                        g_playerArc[j].hitbox.x = g_player.hitbox.x + 20;
                        g_playerArc[j].hitbox.y = g_player.hitbox.y - 152 - 700;
                        g_playerArc[j].active = true;
                        break;
                    }
                }


                if(g_arcTimer > PLAYER_ARC_DURATION){
                    g_arcActive = false;
                    g_arcTimer = 0;
                }
            } else{
                // shoot
                for (int i = 0; i < NUM_PLAYER_SHOOT; i++)
                {
                    if ((g_playerShoot[i].active == false) && (g_shootRate % PLAYER_SHOOT_FREQUENCY == 0))
                    {
                        g_playerShoot[i].hitbox.x = g_player.hitbox.x + 28;
                        g_playerShoot[i].hitbox.y = g_player.hitbox.y + 12;
                        g_playerShoot[i].active = true;
                        break;
                    }
                }
            }

            if(g_hiveActive){
                g_hiveTimer += 1;
                //L
                for (int i = 0; i < NUM_PLAYER_HIVE; i++) {
                    if ((g_hiveL[i].active == false) && (g_hiveTimer % PLAYER_HIVE_FREQUENCY == 0))
                    {
                        g_hiveL[i].hitbox.x = g_player.hitbox.x - 8;
                        g_hiveL[i].hitbox.y = g_player.hitbox.y + 16;
                        g_hiveL[i].active = true;
                        break;
                    }
                }
                //R
                for (int i = 0; i < NUM_PLAYER_HIVE; i++) {
                    if ((g_hiveR[i].active == false) && (g_hiveTimer % PLAYER_HIVE_FREQUENCY == 0))
                    {
                        g_hiveR[i].hitbox.x = g_player.hitbox.x + 60;
                        g_hiveR[i].hitbox.y = g_player.hitbox.y + 16;
                        g_hiveR[i].active = true;
                        break;
                    }
                }

                if(g_hiveTimer > PLAYER_HIVE_DURATION){
                    g_hiveActive = false;
                    g_hiveTimer = 0;
                }

            }




            switch (g_player.state) {
                case NORMAL:
                    g_player.color = WHITE;
                    break;
                case HIT:
                    g_player.color = RED;
                    g_player.state = NORMAL;
                    break;
                case DROP:
                    g_player.color = BLACK;
                    break;

            }

            // response to arc
            if(g_player.heat > 0){
                g_player.heat -= 0.1f;
            }
            if(g_player.heat > PLAYER_HEAT_MAX){
                g_numOfShield -= 1;
                g_player.heat = 0;
            }

            //gameover ///
            if(g_numOfShield < 0){
                g_numOfShield = 0;

            }

            // enemies

            UpdateEnemy1(g_enemy1, g_enemyShoot, NUM_MAX_ENEMY1);
            UpdateEnemy2(g_enemy2, g_enemyArc, NUM_MAX_ENEMY2);
            UpdateEnemy3(g_enemy3, g_enemyShoot, NUM_MAX_ENEMY3);

            UpdateItems(g_items, g_enemyShoot, NUM_MAX_ITEM);



            // Shoot logic
            UpdateShoot(g_playerShoot, NUM_PLAYER_SHOOT,PLAYER_SHOOT_FREQUENCY, 0);
            UpdateShoot(g_hiveL, NUM_PLAYER_HIVE, PLAYER_HIVE_FREQUENCY, 0);
            UpdateShoot(g_hiveR, NUM_PLAYER_HIVE, PLAYER_HIVE_FREQUENCY, 0);
            UpdateShoot(g_enemyShoot, NUM_MAX_ENEMY_SHOOT,ENEMY_SHOOT_FREQUENCY,1);
            UpdateArc(g_enemyArc, NUM_MAX_ENEMY_ARC, 0, 1);
            UpdateArc(g_playerArc, NUM_PLAYER_ARC, 0, 0);



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

                        if (g_enemyKills/2 == FIRST_REQUIREMENT)
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


                        if (g_enemyKills/2 == SECOND_REQUIREMENT)
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
        } else if(IsKeyPressed(KEY_ENTER)){
            g_retry = true;
        }
    }


}

// Draw game (one frame)
void DrawUI(void ){
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

    DrawTextureEx(g_shieldTexture, (Vector2){310, 825}, 0, 0.25f, WHITE);
    DrawRectangle(365, 840, 285, 20, (Color){0,20,20,128});
    for (int i = 1; i <= g_numOfShield && i <= 9; i++) {
        DrawRectangle(370 + (i-1)*31, 845, 27, 10, BLUE);
    }
    for (int i = g_numOfShield +1; 1<= i && i <= 9; i++) {
        DrawRectangle(370 + (i-1)*31, 845, 27, 10, GRAY);
    }

    if(g_player.heat > PLAYER_HEAT_MAX/2){
        DrawRectangle(365, 824, 266, 12, RED);
    } else{
        DrawRectangle(365, 824, 266, 12, (Color){0,20,20,128});
    }


    DrawRectangle(370, 826, (g_player.heat/PLAYER_HEAT_MAX) * 256, 8, ORANGE);



    DrawText(TextFormat("KILL: %d", g_enemyKills/2), 480 - MeasureText("KILL:100",48)/2, 880, 48, WHITE);

}
void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(BLACK);


    DrawTextureEx(g_player.texture, (Vector2){g_player.hitbox.x - 32, g_player.hitbox.y - 32}, 0, 0.5f, g_player.color);
    //DrawRectangle(g_player.hitbox.x, g_player.hitbox.y, 96, 96, g_player.color );
    DrawRectangle(g_player.hitbox.x, g_player.hitbox.y, 4, 4, GREEN );
    DrawRectangle(g_player.hitbox.x + g_player.hitbox.width - 4, g_player.hitbox.y, 4, 4, GREEN );
    DrawRectangle(g_player.hitbox.x, g_player.hitbox.y + g_player.hitbox.height - 4, 4, 4, GREEN );
    DrawRectangle(g_player.hitbox.x + g_player.hitbox.width - 4, g_player.hitbox.y + g_player.hitbox.height - 4, 4, 4, GREEN );




    if(g_isStarted){
        //Draw background;
        if (g_stage == FIRST) DrawText("STAGE 1", 480 - MeasureText("STAGE 1", 40)/2, 480 - 40, 40, Fade(RED, g_alpha));
        else if (g_stage == SECOND) DrawText("STAGE 2", 480 - MeasureText("STAGE 2", 40)/2, 480 - 40, 40, Fade(RED, g_alpha));
        else if (g_stage == THIRD) DrawText("STAGE 3", 480 - MeasureText("STAGE 3", 40)/2, 480 - 40, 40, Fade(RED , g_alpha));

        // Draw items
        Texture2D itemTexture;
        for (int i = 0; i < NUM_MAX_ITEM; i++) {
            if(g_items[i].active){

                switch (g_items[i].type) {
                    case A:
                        itemTexture = g_arcTexture;
                        break;
                    case S:
                        itemTexture = g_shieldTexture;
                        break;
                    case H:
                        itemTexture = g_hiveTexture;
                        break;
                }

                DrawTextureEx(itemTexture,(Vector2){g_items[i].hitbox.x, g_items[i].hitbox.y}, 0, 0.25f, g_items[i].color);

            }
        }

        //Draw shoots
        for (int i = 0; i < NUM_PLAYER_SHOOT; i++) {
            if(g_playerShoot[i].active){
                DrawTextureEx(g_playerShootTexture, (Vector2){g_playerShoot[i].hitbox.x, g_playerShoot[i].hitbox.y}, 0, 0.5f, WHITE);
            }
        }
        for (int i = 0; i < NUM_PLAYER_HIVE; i++) {
            if(g_hiveL[i].active){
                DrawTextureEx(g_hiveShootTexture, (Vector2){g_hiveL[i].hitbox.x, g_hiveL[i].hitbox.y}, 0, 0.5f, WHITE);
            }
            if(g_hiveR[i].active){
                DrawTextureEx(g_hiveShootTexture, (Vector2){g_hiveR[i].hitbox.x, g_hiveR[i].hitbox.y}, 0, 0.5f, WHITE);
            }
        }
        for (int i = 0; i < NUM_PLAYER_ARC; i++) {
            if(g_arcActive){

                DrawTextureEx(g_arcHeadTexture, (Vector2){g_player.hitbox.x - 32, g_player.hitbox.y - 152}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 32, g_player.hitbox.y - 280}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 32, g_player.hitbox.y - 408}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 32, g_player.hitbox.y - 536}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 32, g_player.hitbox.y - 664}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 32, g_player.hitbox.y - 782}, 0, 0.5f,
                              Fade(WHITE,32));
            }
        }

        for (int i = 0; i < NUM_MAX_ENEMY_SHOOT; i++) {
            if(g_enemyShoot[i].active){
                DrawTextureEx(g_enemyShootTexture, (Vector2){g_enemyShoot[i].hitbox.x, g_enemyShoot[i].hitbox.y}, 0, 0.5f, WHITE);

            }
        }

        //Draw enemies
        for (int i = 0; i < NUM_MAX_ENEMY1; i++) {
            if(g_enemy1[i].active){
                DrawTextureEx(g_enemy1Texture,(Vector2){g_enemy1[i].hitbox.x - 32, g_enemy1[i].hitbox.y - 16}, 0, 0.5f, g_enemy1[i].color);
            }
        }

        for (int i = 0; i < NUM_MAX_ENEMY2; i++) {
            if(g_enemy2[i].active){
                DrawTextureEx(g_enemy2Texture,(Vector2){g_enemy2[i].hitbox.x - 28, g_enemy2[i].hitbox.y}, 0, 0.5f, g_enemy2[i].color);
                if( (g_enemy2[i].shootRate > 0) && (g_enemy2[i].shootRate < ENEMY_ARC_FOCUS) ){
                    DrawRectangle(g_enemy2[i].hitbox.x + 36, g_enemy2[i].hitbox.y + 128, 4, 900, RED);
                }
                if( (g_enemy2[i].shootRate >= ENEMY_ARC_FOCUS) && ((g_enemy2[i].shootRate < ENEMY_ARC_FOCUS + ENEMY_ARC_DURATION))){
                    DrawTextureEx(g_EArcHeadTexture, (Vector2){g_enemy2[i].hitbox.x - 28, g_enemy2[i].hitbox.y + 128}, 0, 0.5f,
                                  Fade(WHITE,64));
                    DrawTextureEx(g_EArcBodyTexture, (Vector2){g_enemy2[i].hitbox.x - 28, g_enemy2[i].hitbox.y + 256}, 0, 0.5f,
                                  Fade(WHITE,64));
                    DrawTextureEx(g_EArcBodyTexture, (Vector2){g_enemy2[i].hitbox.x - 28, g_enemy2[i].hitbox.y + 384}, 0, 0.5f,
                                  Fade(WHITE,64));
                    DrawTextureEx(g_EArcBodyTexture, (Vector2){g_enemy2[i].hitbox.x - 28, g_enemy2[i].hitbox.y + 512}, 0, 0.5f,
                                  Fade(WHITE,64));
                    DrawTextureEx(g_EArcBodyTexture, (Vector2){g_enemy2[i].hitbox.x - 28, g_enemy2[i].hitbox.y + 640}, 0, 0.5f,
                                  Fade(WHITE,64));
                }
            }
        }


        Texture2D E3;
        for (int i = 0; i < NUM_MAX_ENEMY3; i++) {
            if(g_enemy3[i].active){
                switch (g_enemy3[i].type) {
                    case O:
                        E3 = g_enemy3OTexture;
                        break;
                    case A:
                        E3 = g_enemy3ATexture;
                        break;
                    case S:
                        E3 = g_enemy3STexture;
                        break;
                    case H:
                        E3 = g_enemy3HTexture;
                        break;

                }
                DrawTextureEx(E3, (Vector2){g_enemy3[i].hitbox.x - 16, g_enemy3[i].hitbox.y - 16}, 0, 0.5f, g_enemy3[i].color);
            }
        }



        DrawUI();


    }else{
        DrawUI();
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
