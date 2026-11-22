#include "rlgl.h"

#include "raylib.h"
#include "raymath.h"
#include <cmath>

struct Car {
    Vector3 position = {4.0f, 0.0f, 5.0f};
    float heading = 0.0f;
    float speed = 0.0f;
};

void DrawBuilding(Vector3 p, Vector3 s, Color wall) {
    DrawCubeV(p, s, wall);
    DrawCubeV(
        {p.x, p.y + s.y/2 + 0.18f, p.z},
        {s.x + 0.35f, 0.35f, s.z + 0.35f},
        DARKGRAY
    );

    Color glass = {105, 165, 190, 255};

    int floors = (int)(s.y / 3.0f);
    int columns = (int)(s.x / 2.5f);

    for (int f = 0; f < floors; f++) {
        float y = p.y - s.y/2 + 1.7f + f*3.0f;

        for (int c = 0; c < columns; c++) {
            float x = p.x - s.x/2 + 1.25f + c*2.5f;

            DrawCubeV(
                {x, y, p.z - s.z/2 - 0.03f},
                {1.15f, 1.05f, 0.08f},
                glass
            );

            DrawCubeV(
                {x, y, p.z + s.z/2 + 0.03f},
                {1.15f, 1.05f, 0.08f},
                glass
            );
        }
    }
}

void DrawCar(Vector3 p, float heading) {
    rlPushMatrix();

    rlTranslatef(p.x, p.y, p.z);
    rlRotatef(heading * RAD2DEG, 0, 1, 0);

    DrawCubeV(
        {0, 0.75f, 0},
        {2.25f, 0.75f, 4.2f},
        RED
    );

    DrawCubeV(
        {0, 1.35f, -0.25f},
        {1.75f, 0.72f, 2.1f},
        (Color){65, 90, 105, 255}
    );

    for (int side = -1; side <= 1; side += 2) {
        for (int front = -1; front <= 1; front += 2) {
            DrawCylinder(
                {side*1.08f, 0.48f, front*1.35f},
                0.43f, 0.43f, 0.28f, 12, BLACK
            );
        }
    }

    DrawCubeV(
        {-0.72f, 0.9f, 2.13f},
        {0.38f, 0.2f, 0.08f},
        YELLOW
    );

    DrawCubeV(
        {0.72f, 0.9f, 2.13f},
        {0.38f, 0.2f, 0.08f},
        YELLOW
    );

    rlPopMatrix();
}

void DrawPlayer(Vector3 p, float heading, float walkTime) {
    rlPushMatrix();

    rlTranslatef(p.x, p.y, p.z);
    rlRotatef(heading * RAD2DEG, 0, 1, 0);

    float swing = sinf(walkTime * 9.0f) * 0.2f;

    DrawCubeV(
        {0, 1.05f, 0},
        {0.72f, 0.95f, 0.38f},
        BLUE
    );

    DrawSphere(
        {0, 1.82f, 0},
        0.28f,
        BEIGE
    );

    DrawCylinderEx(
        {-0.22f, 0.72f, 0},
        {-0.22f, 0.18f + swing, 0},
        0.13f, 0.09f, 8, DARKGRAY
    );

    DrawCylinderEx(
        {0.22f, 0.72f, 0},
        {0.22f, 0.18f - swing, 0},
        0.13f, 0.09f, 8, DARKGRAY
    );

    rlPopMatrix();
}

void DrawStreetLight(float x, float z) {
    DrawCylinder(
        {x, 2.7f, z},
        0.09f, 0.09f, 5.4f, 8, DARKGRAY
    );

    DrawCylinder(
        {x + 0.65f, 5.25f, z},
        0.07f, 0.07f, 1.3f, 8, DARKGRAY
    );

    DrawCubeV(
        {x + 1.25f, 5.18f, z},
        {0.75f, 0.22f, 0.35f},
        YELLOW
    );
}

int main() {
    const int WIDTH = 1280;
    const int HEIGHT = 720;

    InitWindow(WIDTH, HEIGHT, "Mini GTA 3D");
    SetTargetFPS(60);
    DisableCursor();

    Vector3 player = {0, 0, 10};

    Car car;

    bool inCar = false;

    float cameraYaw = PI;
    float playerHeading = PI;
    float walkTime = 0;

    Vector3 cameraPosition = {0, 5, 20};

    Camera3D camera = {};
    camera.position = cameraPosition;
    camera.target = {0, 1, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 65;
    camera.projection = CAMERA_PERSPECTIVE;

    Color road = {48, 51, 56, 255};
    Color sidewalk = {145, 145, 140, 255};

    Color walls[] = {
        {174, 166, 151, 255},
        {125, 137, 145, 255},
        {190, 180, 160, 255},
        {145, 125, 112, 255},
        {155, 165, 158, 255}
    };

    while (!WindowShouldClose()) {
        float dt = fminf(GetFrameTime(), 0.04f);

        Vector2 mouse = GetMouseDelta();

        cameraYaw -= mouse.x * 0.0025f;

        cameraYaw = Wrap(cameraYaw, -PI, PI);

        Vector3 forward = {
            sinf(cameraYaw), 0, cosf(cameraYaw)
        };

        Vector3 right = {
            cosf(cameraYaw), 0, -sinf(cameraYaw)
        };

        if (!inCar) {
            Vector3 move = {0, 0, 0};

            if (IsKeyDown(KEY_W))
                move = Vector3Add(move, forward);

            if (IsKeyDown(KEY_S))
                move = Vector3Subtract(move, forward);

            if (IsKeyDown(KEY_D))
                move = Vector3Add(move, right);

            if (IsKeyDown(KEY_A))
                move = Vector3Subtract(move, right);

            bool moving = Vector3Length(move) > 0.01f;

            if (moving) {
                move = Vector3Normalize(move);

                player.x += move.x * 5.5f * dt;
                player.z += move.z * 5.5f * dt;

                playerHeading = atan2f(move.x, move.z);

                walkTime += dt;
            }

            Vector3 distance = Vector3Subtract(
                player, car.position
            );

            if (IsKeyPressed(KEY_E) &&
                Vector3Length(distance) < 4.0f) {

                inCar = true;
                car.speed = 0;
                cameraYaw = car.heading + PI;
            }
        }
        else {
            if (IsKeyDown(KEY_W))
                car.speed += 13.0f * dt;

            if (IsKeyDown(KEY_S))
                car.speed -= 16.0f * dt;

            car.speed *= powf(0.985f, dt * 60.0f);

            car.speed = Clamp(car.speed, -12, 27);

            float steer = 0;

            if (IsKeyDown(KEY_A))
                steer += 1;

            if (IsKeyDown(KEY_D))
                steer -= 1;

            car.heading += steer * dt *
                (1.8f * (0.25f + fabsf(car.speed)/20.0f)) *
                (car.speed >= 0 ? 1 : -1);

            car.position.x +=
                sinf(car.heading) * car.speed * dt;

            car.position.z +=
                cosf(car.heading) * car.speed * dt;

            if (IsKeyPressed(KEY_E) &&
                fabsf(car.speed) < 2.0f) {

                inCar = false;

                player = {
                    car.position.x + 2.4f,
                    0,
                    car.position.z
                };

                playerHeading = car.heading;
                cameraYaw = car.heading + PI;
            }
        }

        Vector3 focus = inCar ? car.position : player;

        float followYaw = inCar ? car.heading : cameraYaw;

        Vector3 behind = {
            -sinf(followYaw) * 8.5f,
            4.2f,
            -cosf(followYaw) * 8.5f
        };

        Vector3 desiredCamera = Vector3Add(focus, behind);

        cameraPosition = Vector3Lerp(
            cameraPosition,
            desiredCamera,
            1.0f - expf(-5.0f * dt)
        );

        camera.position = cameraPosition;

        camera.target = Vector3Add(
            focus,
            {0, inCar ? 1.0f : 1.25f, 0}
        );

        BeginDrawing();

        ClearBackground(
            (Color){135, 185, 220, 255}
        );

        BeginMode3D(camera);

        DrawPlane(
            {0, -0.12f, 0},
            {180, 180},
            (Color){92, 125, 88, 255}
        );

        // CITY ROADS

        for (int k = -1; k <= 1; k++) {
            float offset = k * 36.0f;

            DrawCubeV(
                {offset, 0, 0},
                {13, 0.12f, 180},
                road
            );

            DrawCubeV(
                {0, 0, offset},
                {180, 0.12f, 13},
                road
            );

            DrawCubeV(
                {offset - 7.2f, 0.04f, 0},
                {1.2f, 0.2f, 180},
                sidewalk
            );

            DrawCubeV(
                {offset + 7.2f, 0.04f, 0},
                {1.2f, 0.2f, 180},
                sidewalk
            );
        }

        // ROAD MARKINGS

        for (int z = -80; z <= 80; z += 8) {
            DrawCubeV(
                {0, 0.075f, (float)z},
                {0.18f, 0.025f, 3.5f},
                YELLOW
            );

            DrawCubeV(
                {36, 0.075f, (float)z},
                {0.18f, 0.025f, 3.5f},
                YELLOW
            );

            DrawCubeV(
                {-36, 0.075f, (float)z},
                {0.18f, 0.025f, 3.5f},
                YELLOW
            );

            DrawCubeV(
                {(float)z, 0.075f, 0},
                {3.5f, 0.025f, 0.18f},
                YELLOW
            );
        }

        // BUILDINGS

        int index = 0;

        for (int x = -1; x <= 1; x++) {
            for (int z = -1; z <= 1; z++) {
                if (x == 0 && z == 0)
                    continue;

                for (int sx = -1; sx <= 1; sx++) {
                    for (int sz = -1; sz <= 1; sz++) {

                        float bx = x*36.0f + sx*8.5f;
                        float bz = z*36.0f + sz*8.5f;

                        float height =
                            9.0f + (index % 5) * 3.0f;

                        float width =
                            6.8f + (index % 3) * 0.7f;

                        DrawBuilding(
                            {bx, height/2, bz},
                            {width, height, 7},
                            walls[index % 5]
                        );

                        index++;
                    }
                }
            }
        }

        // STREET LIGHTS

        for (int z = -72; z <= 72; z += 24) {
            DrawStreetLight(-8.5f, (float)z);
            DrawStreetLight(8.5f, (float)z);
        }

        for (int x = -72; x <= 72; x += 24) {
            DrawStreetLight((float)x, -8.5f);
            DrawStreetLight((float)x, 8.5f);
        }

        // VEHICLE AND PLAYER

        DrawCar(car.position, car.heading);

        if (!inCar) {
            DrawPlayer(
                player,
                playerHeading,
                walkTime
            );
        }

        EndMode3D();

        // INTERFACE

        DrawRectangle(
            18, 18, 440, 120,
            Fade(BLACK, 0.65f)
        );

        DrawText(
            "MINI GTA 3D",
            32, 28, 28, WHITE
        );

        DrawText(
            "WASD - Move / Drive",
            32, 65, 18, WHITE
        );

        DrawText(
            "Mouse - Camera",
            32, 89, 18, WHITE
        );

        DrawText(
            "E - Enter / Exit car",
            32, 113, 18, WHITE
        );

        if (inCar) {
            DrawText(
                TextFormat(
                    "SPEED: %03i",
                    (int)(fabsf(car.speed) * 5)
                ),
                32, 160, 24, YELLOW
            );
        }
        else {
            DrawText(
                "Find the red car and press E",
                32, 160, 20, YELLOW
            );
        }

        DrawFPS(WIDTH - 95, 18);

        EndDrawing();
    }

    EnableCursor();
    CloseWindow();

    return 0;
}
