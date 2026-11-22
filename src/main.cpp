#include "raylib.h"
#include "raymath.h"
#include <vector>

struct Block
{
    Vector3 position;
    Vector3 size;
};

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "My 3D Game");
    SetTargetFPS(120);

    Camera3D camera = {};
    camera.position = { 0.0f, 2.0f, 6.0f };
    camera.target = { 0.0f, 1.5f, 5.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 playerPosition = { 0.0f, 1.0f, 0.0f };

    float yaw = 0.0f;
    float pitch = 0.0f;

    const float playerHeight = 2.0f;
    const float moveSpeed = 6.0f;
    const float mouseSensitivity = 0.003f;

    float verticalVelocity = 0.0f;
    bool grounded = true;

    std::vector<Block> blocks =
    {
        {{ 0.0f, 1.0f, -8.0f }, { 4.0f, 2.0f, 2.0f }},
        {{ 6.0f, 1.0f, -14.0f }, { 3.0f, 2.0f, 3.0f }},
        {{-6.0f, 1.5f, -20.0f }, { 4.0f, 3.0f, 4.0f }},
        {{ 0.0f, 0.5f, -28.0f }, { 12.0f, 1.0f, 2.0f }}
    };

    DisableCursor();

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();

        // -------------------------
        // MOUSE LOOK
        // -------------------------

        Vector2 mouseDelta = GetMouseDelta();

        yaw -= mouseDelta.x * mouseSensitivity;
        pitch -= mouseDelta.y * mouseSensitivity;

        pitch = Clamp(pitch, -1.45f, 1.45f);

        Vector3 forward =
        {
            sinf(yaw) * cosf(pitch),
            sinf(pitch),
            cosf(yaw) * cosf(pitch)
        };

        forward = Vector3Normalize(forward);

        Vector3 flatForward =
        {
            sinf(yaw),
            0.0f,
            cosf(yaw)
        };

        Vector3 right =
        {
            cosf(yaw),
            0.0f,
            -sinf(yaw)
        };

        // -------------------------
        // MOVEMENT
        // -------------------------

        Vector3 movement = { 0, 0, 0 };

        if (IsKeyDown(KEY_W))
            movement = Vector3Add(movement, flatForward);

        if (IsKeyDown(KEY_S))
            movement = Vector3Subtract(movement, flatForward);

        if (IsKeyDown(KEY_D))
            movement = Vector3Add(movement, right);

        if (IsKeyDown(KEY_A))
            movement = Vector3Subtract(movement, right);

        if (Vector3Length(movement) > 0.0f)
        {
            movement = Vector3Normalize(movement);

            playerPosition.x += movement.x * moveSpeed * delta;
            playerPosition.z += movement.z * moveSpeed * delta;
        }

        // -------------------------
        // JUMP / GRAVITY
        // -------------------------

        if (IsKeyPressed(KEY_SPACE) && grounded)
        {
            verticalVelocity = 7.0f;
            grounded = false;
        }

        verticalVelocity -= 18.0f * delta;
        playerPosition.y += verticalVelocity * delta;

        if (playerPosition.y <= 1.0f)
        {
            playerPosition.y = 1.0f;
            verticalVelocity = 0.0f;
            grounded = true;
        }

        // -------------------------
        // CAMERA
        // -------------------------

        Vector3 cameraOffset =
        {
            0.0f,
            1.0f,
            0.0f
        };

        camera.position = Vector3Add(playerPosition, cameraOffset);

        camera.target = Vector3Add(
            camera.position,
            forward
        );

        // -------------------------
        // DRAW
        // -------------------------

        BeginDrawing();

        ClearBackground(SKYBLUE);

        BeginMode3D(camera);

        // Ground
        DrawPlane(
            { 0.0f, 0.0f, -15.0f },
            { 100.0f, 100.0f },
            DARKGREEN
        );

        // Grid
        DrawGrid(100, 1.0f);

        // Blocks
        for (const Block& block : blocks)
        {
            DrawCube(
                block.position,
                block.size.x,
                block.size.y,
                block.size.z,
                GRAY
            );

            DrawCubeWires(
                block.position,
                block.size.x,
                block.size.y,
                block.size.z,
                DARKGRAY
            );
        }

        // Goal
        DrawCube(
            { 0.0f, 1.0f, -35.0f },
            3.0f,
            2.0f,
            3.0f,
            GOLD
        );

        DrawCubeWires(
            { 0.0f, 1.0f, -35.0f },
            3.0f,
            2.0f,
            3.0f,
            ORANGE
        );

        EndMode3D();

        // UI
        DrawText(
            "MY 3D GAME",
            20,
            20,
            30,
            WHITE
        );

        DrawText(
            "WASD - Move",
            20,
            60,
            20,
            WHITE
        );

        DrawText(
            "SPACE - Jump",
            20,
            85,
            20,
            WHITE
        );

        DrawText(
            "Mouse - Look",
            20,
            110,
            20,
            WHITE
        );

        DrawText(
            "Reach the golden cube!",
            20,
            145,
            20,
            YELLOW
        );

        EndDrawing();
    }

    EnableCursor();
    CloseWindow();

    return 0;
}
