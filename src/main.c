#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"
#include <stdlib.h>

#define MAX_ENEMIES 50
#define PARTICLE_COUNT 1000
#define FLT_MAX 1000

typedef struct {
    Vector2 position;
    bool active;
} Enemy;

typedef struct {
    Vector2 position;
    Color color;
} Particle;

int main ()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1280, 800, "P.N.01");
    SearchAndSetResourceDir("resources");

    Vector2 playAreaCenter = { 0, 0 };
    Vector2 playerPosition = playAreaCenter;
    Vector2 playerDirection = { 0, 0 };
    float playerSpeed = 5.0f;
    float playerRadius = 20.0f;

    Enemy enemies[MAX_ENEMIES] = { 0 };
    int enemyCount = 0;
    float enemySpeed = 3.0f;

    Particle particles[PARTICLE_COUNT] = { 0 };
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].position = (Vector2){ GetRandomValue(-640, 640), GetRandomValue(-400, 400) };
        particles[i].color = DARKGRAY;
    }

    Camera2D camera = { 0 };
    camera.target = playAreaCenter;
    camera.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.zoom = 1.0f;

    while (!WindowShouldClose())
    {
        playerDirection.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
        playerDirection.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
        playerDirection = Vector2Normalize(playerDirection);
        playerDirection = Vector2Scale(playerDirection, playerSpeed);
        
        playerPosition.x += playerDirection.x;
        playerPosition.y += playerDirection.y;

        if (playerPosition.x < -640 + playerRadius) playerPosition.x = -640 + playerRadius;
        if (playerPosition.x > 640 - playerRadius) playerPosition.x = 640 - playerRadius;
        if (playerPosition.y < -400 + playerRadius) playerPosition.y = -400 + playerRadius;
        if (playerPosition.y > 400 - playerRadius) playerPosition.y = 400 - playerRadius;

        camera.target = Vector2Lerp(camera.target, playerPosition, 0.1f);

        if (enemyCount < MAX_ENEMIES && GetRandomValue(0, 100) < 2) {
            enemies[enemyCount].position = (Vector2){ playerPosition.x + GetRandomValue(-800, 800), playerPosition.y + GetRandomValue(-600, 600) };
            enemies[enemyCount].active = true;
            enemyCount++;
        }

        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].active) {
                Vector2 enemyDirection = Vector2Subtract(playerPosition, enemies[i].position);
                enemyDirection = Vector2Normalize(enemyDirection);
                enemyDirection = Vector2Scale(enemyDirection, enemySpeed);
                enemies[i].position = Vector2Add(enemies[i].position, enemyDirection);
            }
        }

        if (enemyCount > 0) {
            int nearestEnemyIndex = -1;
            float nearestDistance = FLT_MAX;
            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i].active) {
                    float distance = Vector2Distance(playerPosition, enemies[i].position);
                    if (distance < nearestDistance) {
                        nearestDistance = distance;
                        nearestEnemyIndex = i;
                    }
                }
            }

            if (nearestEnemyIndex != -1) {
                enemies[nearestEnemyIndex].active = false;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        DrawRectangleLines(-640, -400, 1280, 800, WHITE);

        for (int i = 0; i < PARTICLE_COUNT; i++) {
            DrawPixelV(particles[i].position, particles[i].color);
        }

        DrawCircleV(playerPosition, playerRadius, WHITE);

        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].active) {
                DrawCube((Vector3){enemies[i].position.x, enemies[i].position.y, 0}, 40, 40, 40, RED);
            }
        }

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}