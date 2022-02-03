//
// Created by block on 2022/1/22.
//
#include "raylib.h"

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------

// MAX nums

#define NUM_PLAYER_SHOOT 96
#define NUM_PLAYER_HIVE 108
#define NUM_PLAYER_ARC 16

#define NUM_ENEMY1_STAGE1 4
#define NUM_ENEMY1_STAGE2 2
#define NUM_ENEMY1_STAGE3 1
#define NUM_MAX_ENEMY2 3
#define NUM_MAX_ENEMY3 1

#define NUM_MAX_ENEMY_SHOOT 96
#define NUM_MAX_ENEMY_ARC 24
#define NUM_MAX_BOSS_SHOOT 256
#define NUM_MAX_BOSS_ARC 48

#define NUM_MAX_ITEM 3
#define NUM_STAR_DENSITY 12

// balance stats
#define PLAYER_SHOOT_FREQUENCY 8
#define PLAYER_HIVE_FREQUENCY 6
#define PLAYER_HEAT_MAX 96
#define PLAYER_INIT_SHIELDS 3
#define PLAYER_MAX_SHIELDS 9
#define PLAYER_INIT_HIVE 1
#define PLAYER_INIT_ARC 2
#define PLAYER_ARC_DURATION 512
#define PLAYER_HIVE_DURATION 256


#define ENEMY1_HEAT_MAX 4
#define ENEMY1_HP 4

#define ENEMY2_HEAT_MAX 8
#define ENEMY2_HP 6

#define ENEMY3_HEAT_MAX 4
#define ENEMY3_HP 12

#define BOSS_HEAT_MAX 8
#define BOSS_HP 512

#define ENEMY_SHOOT_FREQUENCY 80
#define ENEMY_ARC_FOCUS 128
#define ENEMY_ARC_DURATION 512
#define ENEMY_SHOOT_DURATION 256

#define BOSS_SHOOT_FREQUENCY 16
#define BOSS_ARC_FOCUS 108
#define BOSS_ARC_DURATION 256
#define BOSS_ARC_COOLDOWN 384
#define BOSS_SHOOT_DURATION 256
#define BOSS_SHOOT_COOLDOWN 256



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
typedef enum { FOLLOW = 0, SPREAD} BossShootType;

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
    static Enemy g_enemy1[NUM_ENEMY1_STAGE1] ={0};
    static Enemy g_enemy2[NUM_MAX_ENEMY2] ={0};
    static Enemy g_enemy3[NUM_MAX_ENEMY3] ={0};
    static Enemy g_boss = {0};

    static Enemy g_items[NUM_MAX_ITEM] = {0};
    static Enemy g_stars[NUM_STAR_DENSITY] = {0};
    static Enemy g_gate[2] = {0};


    static Shoot g_playerShoot[NUM_PLAYER_SHOOT] = {0};
    static Shoot g_playerArc[NUM_PLAYER_ARC] = {0};
    static Shoot g_hiveL[NUM_PLAYER_HIVE] = {0};
    static Shoot g_hiveR[NUM_PLAYER_HIVE] = {0};
    static Shoot g_enemyShoot[NUM_MAX_ENEMY_SHOOT] = {0};
    static Shoot g_enemyArc[NUM_MAX_ENEMY_ARC] = {0};
    static Shoot g_bossShoot[NUM_MAX_BOSS_SHOOT] = {0};
    static Shoot g_bossArc[NUM_MAX_BOSS_ARC] = {0};
    //numbers
    static int g_numOfArc, g_numOfHive, g_numOfShield;

    //counts
    static GameStage g_stage = FIRST;
    static int g_shootRate = 0;
    static float g_alpha = 0.0f;
    static int g_enemyKills = 0;

    static int g_arcTimer = 0;
    static int g_hiveTimer = 0;

    static BossShootType g_bossShootType = SPREAD;

    //bools
    static bool g_isStarted = false;
    static bool g_isGameOver = false;
    static bool g_isPaused =  false;
    static bool g_smooth = false;
    static bool g_isWin =  false;
    static bool g_retry = false;
    static bool g_exit = false;


    static bool g_arcActive = false;
    static bool g_hiveActive = false;


    //images
    Texture2D g_arcTexture, g_hiveTexture, g_shieldTexture,
              g_playerShootTexture, g_hiveShootTexture, g_arcHeadTexture, g_arcBodyTexture, //player shoot
              g_enemyShootTexture, g_EArcHeadTexture, g_EArcBodyTexture, g_bossShootTexture, g_bossArcHTexture, g_bossArcBTexture, //enemy shoot
              g_enemy1Texture, g_enemy2Texture,
              g_enemy3OTexture, g_enemy3ATexture, g_enemy3HTexture, g_enemy3STexture,
              g_bossTexture, g_playerTexture;


    //audios
    Sound g_bgm;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void LoadResources(void);//Load textures
static void InitGame(void);// Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void UpdateShoot(Shoot *shoot, int maxShoot, int frequency, int flag);
static void UpdateArc(Shoot * shoot, int maxShoot, int frequency, int flag);
static void UpdateEnemy1(Enemy * enemy, Shoot * shoot, int maxEnemy);
static void UpdateEnemy2(Enemy * enemy, Shoot * shoot, int maxEnemy);
static void UpdateEnemy3(Enemy * enemy, Shoot * shoot, int maxEnemy);
static void UpdateBoss(Enemy * enemy, Shoot * shoot);
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
    InitAudioDevice();
    SetMasterVolume(0.1f);

    SetTargetFPS(60);
    LoadResources();
    retry:
    InitGame();

    // Main game loop
    while (!g_exit)
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        if(g_retry){
            goto retry;
        }
        //----------------------------------------------------------------------------------
    }
    UnloadSound(g_bgm);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------
//Load
void LoadResources(void){
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
    g_bossTexture = LoadTexture("Textures/Boss.png");

    g_enemyShootTexture = LoadTexture("Textures/EnemyShoot.png");
    g_EArcHeadTexture = LoadTexture("Textures/EArcHead.png");
    g_EArcBodyTexture = LoadTexture("Textures/EArcBody.png");
    g_bossShootTexture = LoadTexture("Textures/BossShoot.png");
    g_bossArcHTexture = LoadTexture("Textures/BArcHead.png");
    g_bossArcBTexture = LoadTexture("Textures/BArcBody.png");

    // Load BGM
    g_bgm = LoadSound("Audios/HSD.wav");

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
    g_exit = false;


    g_arcActive = false;

    // Initialize player
    g_player.hitbox.x = 454;
    g_player.hitbox.y = 620;
    g_player.hitbox.width = 52;
    g_player.hitbox.height = 52;
    g_player.speed.x = 8;
    g_player.speed.y = 8;
    g_player.state = NORMAL;
    g_player.color = WHITE;
    g_player.texture = g_playerTexture;
    g_player.weapon = DEFUALT;
    g_player.heat = 0;

    // Initialize enemies

    for (int i = 0; i < NUM_ENEMY1_STAGE1; i++) {
        g_enemy1[i].active = false;
        g_enemy1[i].hitbox.width = 68;
        g_enemy1[i].hitbox.height = 96;
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

    g_boss.active = false;
    g_boss.hitbox.width = 256;
    g_boss.hitbox.height = 176;
    g_boss.hitbox.x = 352;
    g_boss.hitbox.y = 12;
    g_boss.type = BOSS;
    g_boss.state = NORMAL;
    g_boss.speed.y = 1;
    g_boss.color = WHITE;
    g_boss.health = BOSS_HP;
    g_boss.shootRate = 0;
    g_boss.heat = 0;


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
    for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
        g_bossShoot[i].hitbox.width = 12;
        g_bossShoot[i].hitbox.height = 12;
        g_bossShoot[i].speed.y = 0;
        g_bossShoot[i].speed.x = 0;
        g_bossShoot[i].type = BOSS;
        g_bossShoot[i].active = false;
        g_bossShoot[i].damage = 1;

    }
    for (int i = 0; i < NUM_MAX_BOSS_ARC; i++) {
        g_bossArc[i].hitbox.width = 256;
        g_bossArc[i].hitbox.height = 700;
        g_bossArc[i].speed.y = 0;
        g_bossArc[i].speed.x = 0;
        g_bossArc[i].type = EARC;
        g_bossArc[i].active = false;
        g_bossArc[i].damage = 1;
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
    for (int i = 0; i < NUM_STAR_DENSITY; i++) {
        g_stars[i].active = false;
        g_stars[i].hitbox.width = GetRandomValue(2,4);
        g_stars[i].hitbox.height = GetRandomValue(2,4);
        g_stars[i].hitbox.x = 0;
        g_stars[i].hitbox.y = 0;
        g_stars[i].type = DEFUALT;
        g_stars[i].state = NORMAL;
        g_stars[i].speed.y = GetRandomValue(4,6);
        g_stars[i].color = WHITE;
        g_stars[i].health = 0;
        g_stars[i].shootRate = 0;
        g_stars[i].heat = 0;
    }
    for (int i = 0; i < 2; i++) {
        g_gate[i].active = false;
        g_gate[i].hitbox.width = 420;
        g_gate[i].hitbox.height = 960;
        g_gate[i].type = DEFUALT;
        g_gate[i].state = NORMAL;
        g_gate[i].speed.x = 3;
        g_gate[i].color = WHITE;
        g_gate[i].health = 0;
        g_gate[i].shootRate = 0;
        g_gate[i].heat = 0;
    }
    g_gate[0].hitbox.x = 0;
    g_gate[0].hitbox.y = 0;
    g_gate[1].hitbox.x = 540;
    g_gate[1].hitbox.y = 0;
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

            int numOfEnemy1 = 0;
            switch (g_stage) {
                case FIRST:
                    numOfEnemy1 = NUM_ENEMY1_STAGE1;
                    break;
                case SECOND:
                    numOfEnemy1 = NUM_ENEMY1_STAGE2;
                    break;
                case THIRD:
                    numOfEnemy1 = NUM_ENEMY1_STAGE3;
                    break;

            }

            // Collision with ?
            switch (flag) {
                case 0:
                    for (int j = 0; j < numOfEnemy1; j++)
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

                    if(g_boss.active){
                        if(CheckCollisionRecs(shoot[i].hitbox, g_boss.hitbox)){
                            shoot[i].active = false;
                            g_boss. state= HIT;
                            g_boss. health -= shoot[i].damage;
                            g_shootRate -= frequency;
                        }
                    }

                    if(shoot[i].type == HIVE){
                        for (int j = 0; j < NUM_MAX_ENEMY_SHOOT; j++) {
                            if(CheckCollisionRecs(shoot[i].hitbox, g_enemyShoot[j].hitbox)){
                                g_enemyShoot[j].active = false;

                            }
                        }
                        for (int j = 0; j < NUM_MAX_BOSS_SHOOT; j++) {
                            if(CheckCollisionRecs(shoot[i].hitbox, g_bossShoot[j].hitbox)){
                                g_bossShoot[j].active = false;

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
            if (shoot[i].hitbox.y <= 0 || shoot[i].hitbox.y > 800)
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

                    for (int j = 0; j < NUM_ENEMY1_STAGE1; j++){
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
                    if(g_boss.active){
                        if (CheckCollisionRecs(shoot[i].hitbox, g_boss.hitbox)){
                            g_boss. state= HIT;
                            g_boss. heat += shoot[i].damage;

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
                    g_enemyKills += 1;
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
                    g_enemyKills += 1;
                    enemy[i].active = false;

                }

            }


        } else if (g_alpha < 0 && g_stage == FIRST){
            enemy[i].hitbox.x = GetRandomValue(40,850);
            enemy[i].hitbox.y = GetRandomValue(-1400, -500);
            enemy[i].health = ENEMY1_HP;
            enemy[i].heat = 0;
            enemy[i].state = NORMAL;
            enemy[i].active =true;
            enemy[i].shootRate = 50;

        } else if(g_alpha < 0 && g_stage == SECOND){
            enemy[i].hitbox.x = GetRandomValue(40,850);
            enemy[i].hitbox.y = GetRandomValue(-1200, -400);
            enemy[i].health = ENEMY1_HP;
            enemy[i].heat = 0;
            enemy[i].state = NORMAL;
            enemy[i].active =true;
            enemy[i].shootRate = 50;

        } else if(g_alpha < 0 && g_stage == THIRD){
            enemy[i].hitbox.x = GetRandomValue(40,850);
            enemy[i].hitbox.y = GetRandomValue(-1200, -400);
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
                    g_enemyKills += 1;
                    break;
            }

            // movement + shoot
            if(enemy[i].state != DROP){
                if(enemy[i].hitbox.y < 140  && (enemy[i].shootRate == 0)){
                    enemy[i].hitbox.y += enemy[i].speed.y;
                } else{
                    enemy[i].shootRate += 1;

                    if(enemy[i].shootRate >= ENEMY_ARC_FOCUS){
                        if(enemy[i].shootRate >=ENEMY_ARC_FOCUS + ENEMY_ARC_DURATION){
                            enemy[i].shootRate = 0 - ENEMY_SHOOT_DURATION;
                            enemy[i].hitbox.y -= enemy[i].speed.y;
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

                    } else if (enemy[i].shootRate < 0){
                        enemy[i].hitbox.y -= 2 * enemy[i].speed.y;
                        if(enemy[i].hitbox.y < 200){
                            enemy[i].hitbox.x = g_player.hitbox.x + GetRandomValue(-40,40);
                            if(g_player.hitbox.x < 78){
                                enemy[i].hitbox.x = 78;
                            } else if(g_player.hitbox.x > 840){
                                enemy[i].hitbox.x = 840;
                            }
                            enemy[i].hitbox.y = GetRandomValue(-1000,-500);
                            enemy[i].heat = 0;
                            enemy[i].state = NORMAL;
                            enemy[i].active =true;
                            enemy[i].color = WHITE;
                            enemy[i].shootRate = 0;
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
            enemy[i].hitbox.x = g_player.hitbox.x + GetRandomValue(-40,40);
            if(g_player.hitbox.x < 78){
                enemy[i].hitbox.x = 78;
            } else if(g_player.hitbox.x > 840){
                enemy[i].hitbox.x = 840;
            }
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
            enemy[i].hitbox.y = GetRandomValue(-800, -600);
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
void UpdateBoss(Enemy * enemy, Shoot * shoot){
    if(enemy->active){
        // color
        switch (enemy->state) {
            case NORMAL:
                enemy->color = WHITE;
                break;
            case HIT:
                enemy->color = RED;
                enemy->state = NORMAL;
                break;
            case DROP:
                enemy->color = GREEN;
                enemy->active = false;
                break;
        }

        // movement + shoot
        if(enemy->state != DROP){
            if(enemy->hitbox.y < 20 ){
                enemy->hitbox.y += enemy->speed.y;
            } else{
                enemy->shootRate += 1;
                if(0 < enemy->shootRate){
                    if(enemy->shootRate >= BOSS_SHOOT_DURATION){
                        if( (BOSS_SHOOT_DURATION + BOSS_SHOOT_COOLDOWN + BOSS_ARC_FOCUS <= enemy->shootRate) && ( enemy->shootRate <= BOSS_SHOOT_DURATION + BOSS_SHOOT_COOLDOWN + BOSS_ARC_FOCUS + BOSS_ARC_DURATION)){
                            for (int j = 0; j < NUM_MAX_BOSS_ARC ; j++) {
                                if(shoot[j].active == false){
                                    shoot[j].hitbox.x = enemy->hitbox.x;
                                    shoot[j].hitbox.y = enemy->hitbox.y + 192;
                                    shoot[j].active = true;

                                    break;
                                }
                            }
                        } else if (enemy->shootRate >= BOSS_SHOOT_DURATION + BOSS_SHOOT_COOLDOWN + BOSS_ARC_FOCUS + BOSS_ARC_DURATION){
                            enemy->shootRate = 0 - BOSS_ARC_COOLDOWN;
                            g_bossShootType = GetRandomValue(0, 1);
                        }

                    } else if(enemy->shootRate <= BOSS_SHOOT_DURATION ){

                        if(g_bossShootType == FOLLOW){
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 10 * (g_player.hitbox.x + 24 - shoot[i].hitbox.x) / ( g_player.hitbox.y + 32 - shoot[i].hitbox.y );
                                    shoot[i].speed.y = -10;
                                    shoot[i].active = true;
                                    if((g_player.hitbox.x + 24 > shoot[i].hitbox.x) && ( g_player.hitbox.y + 32 < shoot[i].hitbox.y + 80 )){
                                        shoot[i].active = false;
                                    }
                                    break;
                                }
                            }

                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 8 * (g_player.hitbox.x + 24 - shoot[i].hitbox.x) / ( g_player.hitbox.y + 32 - shoot[i].hitbox.y );
                                    shoot[i].speed.y = -8;
                                    shoot[i].active = true;
                                    if((g_player.hitbox.x + 24 < shoot[i].hitbox.x) && ( g_player.hitbox.y + 32 < shoot[i].hitbox.y + 80 )){
                                        shoot[i].active = false;
                                    }
                                    break;
                                }
                            }

                        } else if(g_bossShootType == SPREAD){
                            //L

                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -4;
                                    shoot[i].speed.y = -3;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -4;
                                    shoot[i].speed.y = -4;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -3;
                                    shoot[i].speed.y = -4;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -2;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -1;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 0;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            //Lr
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 1;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 2;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 3;
                                    shoot[i].speed.y = -4;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 4;
                                    shoot[i].speed.y = -4;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 28;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 4;
                                    shoot[i].speed.y = -3;
                                    shoot[i].active = true;
                                    break;
                                }
                            }


                            //R

                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -4;
                                    shoot[i].speed.y = -3;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -4;
                                    shoot[i].speed.y = -4;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -3;
                                    shoot[i].speed.y = -4;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -2;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = -1;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 0;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            //Rr
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 1;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 2;
                                    shoot[i].speed.y = -5;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 3;
                                    shoot[i].speed.y = -4;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = 4;
                                    shoot[i].speed.y = -4;
                                    shoot[i].active = true;
                                    break;
                                }
                            }
                            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                                if(shoot[i].active == false && (enemy->shootRate % BOSS_SHOOT_FREQUENCY == 0)){
                                    shoot[i].hitbox.x = enemy->hitbox.x + 216;
                                    shoot[i].hitbox.y = enemy->hitbox.y + 108;
                                    shoot[i].speed.x = +4;
                                    shoot[i].speed.y = -3;
                                    shoot[i].active = true;
                                    break;
                                }
                            }

                        }

                    }
                }


            }

            if(CheckCollisionRecs(g_player.hitbox, enemy->hitbox)){
                enemy->state = HIT;
                enemy->health -= 2;
                g_player.state = HIT;
                g_numOfShield -= 1;
            }

            // response to arc
            if(enemy->heat > BOSS_HEAT_MAX){
                enemy->state = HIT;
                enemy->health -= 1;
                enemy->heat = 0;
            }

            // die
            if(enemy->health <= 0) {
                enemy->active = false;
                g_enemyKills += 1;
                g_isWin = true;
            }

        }



    } else if (g_alpha < 0 && (g_stage == THIRD)){
        g_boss.active = true;
        g_boss.hitbox.width = 256;
        g_boss.hitbox.height = 176;
        g_boss.hitbox.x = 352;
        g_boss.hitbox.y = -300;
        g_boss.type = BOSS;
        g_boss.state = NORMAL;
        g_boss.speed.y = 1;
        g_boss.color = WHITE;
        g_boss.health = BOSS_HP;
        g_boss.shootRate = 0;
        g_boss.heat = 0;


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
                        g_numOfShield += 2;
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
void UpdateStars(void){
    for (int i = 0; i < NUM_STAR_DENSITY; i++) {
        if(g_stars[i].active){
            g_stars[i].hitbox.y += g_stars[i].speed.y;
            if(g_stars[i].hitbox.y > 900){
                /*g_stars[i].hitbox.width = GetRandomValue(2,4);
            g_stars[i].hitbox.height = GetRandomValue(2,4);*/
                g_stars[i].hitbox.x = GetRandomValue(40,920);
                g_stars[i].hitbox.y = GetRandomValue(-200,-20);
                g_stars[i].speed.y = GetRandomValue(4,6);
            }
        } else {
            g_stars[i].active = true;
            /*g_stars[i].hitbox.width = GetRandomValue(2,4);
            g_stars[i].hitbox.height = GetRandomValue(2,4);*/
            g_stars[i].hitbox.x = GetRandomValue(40,920);
            g_stars[i].hitbox.y = GetRandomValue(-200,-20);
            g_stars[i].speed.y = GetRandomValue(4,6);
        }


    }
}
void UpdateGate(void){
    if(g_gate[0].hitbox.x > -420){
        g_gate[0].hitbox.x -= g_gate[0].speed.x;
    }
    if(g_gate[1].hitbox.x < 960){
        g_gate[1].hitbox.x += g_gate[1].speed.x;
    }



}
void UpdateGame(void){
    if(g_isGameOver == false){

        if(g_isStarted && (g_isPaused == false) && (g_isWin == false)){

            //Player control
            if(IsKeyDown('W')) g_player.hitbox.y -= g_player.speed.y;
            if(IsKeyDown('A')) g_player.hitbox.x -= g_player.speed.x;
            if(IsKeyDown('S')) g_player.hitbox.y += g_player.speed.y;
            if(IsKeyDown('D')) g_player.hitbox.x += g_player.speed.x;
            /*g_player.hitbox.x = GetMouseX() - 32;
            g_player.hitbox.y = GetMouseY() - 32;*/



            if(IsKeyPressed('J') && (g_numOfArc > 0) && (g_arcActive == false)) {
                g_arcActive = true;
                g_numOfArc -= 1;
            }
            if(IsKeyPressed('K') && (g_numOfHive > 0) && (g_hiveActive == false)) {
                g_hiveActive = true;
                g_numOfHive -= 1;
            }


            //WALL
            if(g_player.hitbox.x <=44) g_player.hitbox.x = 44;
            if(g_player.hitbox.x >=864) g_player.hitbox.x = 864;
            if(g_player.hitbox.y <= 4) g_player.hitbox.y = 4;
            if(g_player.hitbox.y >= 760) g_player.hitbox.y = 760;

            //player shoot
            g_shootRate += 1;

            if(g_arcActive){
                g_arcTimer += 1;

                for (int j = 0; j < NUM_PLAYER_ARC; j++) {
                    if(g_playerArc[j].active == false){
                        g_playerArc[j].hitbox.x = g_player.hitbox.x + 12;
                        g_playerArc[j].hitbox.y = g_player.hitbox.y - 24 - 700;
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
                        g_playerShoot[i].hitbox.x = g_player.hitbox.x + 20;
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
                        g_hiveR[i].hitbox.x = g_player.hitbox.x + 48;
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
                g_isGameOver = true;

            }

            // enemies

            switch (g_stage) {
                case FIRST:
                    UpdateEnemy1(g_enemy1, g_enemyShoot, NUM_ENEMY1_STAGE1);
                    break;
                case SECOND:
                    UpdateEnemy1(g_enemy1, g_enemyShoot, NUM_ENEMY1_STAGE2);
                    break;
                case THIRD:
                    UpdateEnemy1(g_enemy1, g_enemyShoot, NUM_ENEMY1_STAGE3);
                    break;

            }
            UpdateEnemy2(g_enemy2, g_enemyArc, NUM_MAX_ENEMY2);
            UpdateEnemy3(g_enemy3, g_enemyShoot, NUM_MAX_ENEMY3);
            if(g_boss.shootRate >= BOSS_SHOOT_DURATION){
                UpdateBoss(&g_boss, g_bossArc);
            } else{
                UpdateBoss(&g_boss, g_bossShoot);
            }


            UpdateItems(g_items, g_enemyShoot, NUM_MAX_ITEM);
            UpdateStars();
            UpdateGate();



            // Shoot logic
            UpdateShoot(g_playerShoot, NUM_PLAYER_SHOOT,PLAYER_SHOOT_FREQUENCY, 0);
            UpdateShoot(g_hiveL, NUM_PLAYER_HIVE, PLAYER_HIVE_FREQUENCY, 0);
            UpdateShoot(g_hiveR, NUM_PLAYER_HIVE, PLAYER_HIVE_FREQUENCY, 0);
            UpdateShoot(g_enemyShoot, NUM_MAX_ENEMY_SHOOT,ENEMY_SHOOT_FREQUENCY,1);
            UpdateShoot(g_bossShoot, NUM_MAX_BOSS_SHOOT, BOSS_SHOOT_FREQUENCY, 1);
            UpdateArc(g_enemyArc, NUM_MAX_ENEMY_ARC, 0, 1);
            UpdateArc(g_playerArc, NUM_PLAYER_ARC, 0, 0);
            UpdateArc(g_bossArc, NUM_MAX_ENEMY_ARC, 0, 1);



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

                        if (g_enemyKills == FIRST_REQUIREMENT)
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


                        if (g_enemyKills == SECOND_REQUIREMENT)
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
            PauseSound(g_bgm);
            if(IsKeyPressed('P') || WindowShouldClose()){
                g_isPaused = false;
                ResumeSound(g_bgm);

            } else if(IsKeyPressed('Q')){
                g_exit = true;
                StopSound(g_bgm);
            }

        }else if(g_isStarted == false){
            if(WindowShouldClose() || IsKeyPressed('P')){
                g_isPaused = true;
            } else if( IsKeyPressed('W') || IsKeyPressed('S') || IsKeyPressed('A') || IsKeyPressed('D')){
                g_isStarted = true;
                PlaySound(g_bgm);
            }
        } else if(g_isWin){
            StopSound(g_bgm);
            if(WindowShouldClose()){
                g_exit = true;
            } else if(IsKeyPressed(KEY_ENTER)){
                g_retry = true;

            }
        }

    } else {
        StopSound(g_bgm);
        if(WindowShouldClose()){
            g_exit = true;

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



    DrawText(TextFormat("KILL: %d", g_enemyKills), 480 - MeasureText("KILL:100",48)/2, 880, 48, WHITE);

}
void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    for (int i = 0; i < NUM_STAR_DENSITY; i++) {
        DrawRectangleRec(g_stars[i].hitbox, WHITE);
    }
    for (int i = 0; i < 2; i++) {
        DrawRectangleRec(g_gate[i].hitbox, GRAY);
        DrawRectangle(g_gate[i].hitbox.x + 20, g_gate[i].hitbox.y + 10, 380, 960, LIGHTGRAY);
    }


    DrawTextureEx(g_player.texture, (Vector2){g_player.hitbox.x - 40, g_player.hitbox.y - 32}, 0, 0.5f, g_player.color);
    //DrawRectangle(g_player.hitbox.x, g_player.hitbox.y, 96, 96, g_player.color );
    if(g_numOfShield > 0){
        DrawRectangle(g_player.hitbox.x, g_player.hitbox.y, 52, 52, Fade(LIME, 0.4f) );
    }






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

                DrawTextureEx(g_arcHeadTexture, (Vector2){g_player.hitbox.x - 40, g_player.hitbox.y - 152}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 40, g_player.hitbox.y - 280}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 40, g_player.hitbox.y - 408}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 40, g_player.hitbox.y - 536}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 40, g_player.hitbox.y - 664}, 0, 0.5f,
                              Fade(WHITE,32));
                DrawTextureEx(g_arcBodyTexture, (Vector2){g_player.hitbox.x - 40, g_player.hitbox.y - 782}, 0, 0.5f,
                              Fade(WHITE,32));
            }
        }

        for (int i = 0; i < NUM_MAX_ENEMY_SHOOT; i++) {
            if(g_enemyShoot[i].active){
                DrawTextureEx(g_enemyShootTexture, (Vector2){g_enemyShoot[i].hitbox.x, g_enemyShoot[i].hitbox.y}, 0, 0.5f, WHITE);
            }
        }

        //Draw enemies
        int numOfEnemy1 = 0;
        switch (g_stage) {
            case FIRST:
                numOfEnemy1 = NUM_ENEMY1_STAGE1;
                break;
            case SECOND:
                numOfEnemy1 = NUM_ENEMY1_STAGE2;
                break;
            case THIRD:
                numOfEnemy1 = NUM_ENEMY1_STAGE3;
                break;

        }
        for (int i = 0; i < numOfEnemy1; i++) {
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

        if(g_boss.active){
            DrawRectangle(100,790,760,20, DARKGREEN);
            DrawRectangle(105,794,750 * (g_boss.health * 1.0f / BOSS_HP),12, RED);
            DrawTextureEx(g_bossTexture, (Vector2){g_boss.hitbox.x, g_boss.hitbox.y}, 0, 0.5f, g_boss.color);
            for (int i = 0; i < NUM_MAX_BOSS_SHOOT; i++) {
                if(g_bossShoot[i].active){
                    DrawTextureEx(g_bossShootTexture, (Vector2){g_bossShoot[i].hitbox.x, g_bossShoot[i].hitbox.y - 4}, 0, 0.5f, WHITE);
                }
            }
            if( (g_boss.shootRate > BOSS_SHOOT_DURATION + BOSS_SHOOT_COOLDOWN) && (g_boss.shootRate < BOSS_SHOOT_DURATION + BOSS_SHOOT_COOLDOWN + BOSS_ARC_FOCUS + BOSS_SHOOT_DURATION) ){
                DrawRectangle(g_boss.hitbox.x + 124, g_boss.hitbox.y + 176, 8, 900, RED);
            }
            if( (g_boss.shootRate >= BOSS_SHOOT_DURATION+ BOSS_SHOOT_COOLDOWN + BOSS_ARC_FOCUS) && (g_boss.shootRate < BOSS_SHOOT_DURATION + BOSS_SHOOT_COOLDOWN + BOSS_ARC_FOCUS + BOSS_ARC_DURATION)){

                DrawTextureEx(g_bossArcHTexture, (Vector2){g_boss.hitbox.x, g_boss.hitbox.y + 184}, 0, 0.5f,
                              Fade(WHITE,64));
                DrawTextureEx(g_bossArcBTexture, (Vector2){g_boss.hitbox.x, g_boss.hitbox.y + 184 + 192}, 0, 0.5f,
                              Fade(WHITE,64));
                DrawTextureEx(g_bossArcBTexture, (Vector2){g_boss.hitbox.x, g_boss.hitbox.y + 184 + 2*192}, 0, 0.5f,
                              Fade(WHITE,64));
                DrawTextureEx(g_bossArcBTexture, (Vector2){g_boss.hitbox.x, g_boss.hitbox.y + 184 + 3*192}, 0, 0.5f,
                              Fade(WHITE,64));

            }
        }



        DrawUI();


    }else{


        DrawUI();
        DrawText("GUIDE", 80, 55, 48, BLACK);

        DrawText("Controls", 80, 120, 32, BLACK);
        DrawText("[WASD] : move", 80, 170, 32, BLACK);
        DrawText("[J] : Laser", 80, 210, 32, BLACK);
        DrawText("[K] : Missiles", 80, 250, 32, BLACK);
        DrawText("[P]/[esc] : Pause", 80, 290, 32, BLACK);

        DrawText("UI", 80, 390, 32, BLACK);

        DrawText("Exposing to enemy", 80, 430, 32, BLACK);
        DrawText("laser increase your", 80, 460, 32, BLACK);
        DrawText("'heat'", 80, 490, 32, BLACK);
        DrawRectangle(180, 500, 200,8,YELLOW );
        DrawRectangle(380, 500, 8,320,YELLOW );
        DrawText("HEAT",390,800,24,WHITE);

        DrawText("When it's full, ", 80, 530, 32, BLACK);
        DrawText("you lost 1 HP", 80, 560, 32, BLACK);
        DrawRectangle(300, 570, 50,8,YELLOW );
        DrawRectangle(350, 570, 8,274,YELLOW );
        DrawRectangle(350, 844, 12,8,YELLOW );
        DrawText("HP",350,860,24,WHITE);


        DrawText("The green square", 80, 600, 32, BLACK);
        DrawText("is your hitbox,", 80, 630, 32, BLACK);
        DrawText("appears when HP>0", 80, 660, 32, BLACK);
        DrawRectangle(314, 644, 136,8, ORANGE );

        DrawText("Enemies", 580, 120, 32, BLACK);
        DrawTextureEx(g_enemy1Texture,(Vector2){580,150},0,0.25f,WHITE);
        DrawText("Enemy1", 650, 160, 32, BLACK);
        DrawTextureEx(g_enemy2Texture,(Vector2){580,210},0,0.25f,WHITE);
        DrawText("Enemy2", 650, 220, 32, BLACK);
        DrawTextureEx(g_enemy3OTexture,(Vector2){580,270},0,0.25f,WHITE);
        DrawText("Enemy3", 650, 280, 32, BLACK);
        DrawTextureEx(g_bossTexture,(Vector2){580,330},0,0.25f,WHITE);
        DrawText("Boss", 720, 360, 32, BLACK);
        DrawText("You lost 1 HP if you", 580, 420, 32, BLACK);
        DrawText("let Enemy1 escape.", 580, 450, 32, BLACK);
        DrawText("Collision with enemies", 580, 490, 32, BLACK);
        DrawText("takes you 1 HP.", 580, 520, 32, BLACK);
        DrawText("Colored Enemy3", 580, 560, 32, BLACK);
        DrawText("drops items,", 580, 590, 32, BLACK);
        DrawText("touch to get them.", 580, 620, 32, BLACK);
        DrawText("If HP<0, you lose.", 580, 660, 32, RED);

        DrawText("Move", 480- MeasureText("Move",32)/2, 380, 32, GRAY);
        DrawText("to", 480- MeasureText("to",32)/2, 420, 32, GRAY);
        DrawText("Start", 480- MeasureText("Start",32)/2, 460, 32, GRAY);






























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

    if(g_isWin){
        DrawText("--- YOU WIN ---", 480 - MeasureText("--- YOU WIN ---", 48)/2, 400, 48, RED);
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
