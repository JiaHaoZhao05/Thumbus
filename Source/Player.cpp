#include "Globals.h"
#include "Application.h"
#include "Player.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "raylib.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	pbody = App->physics->CreateBall(startPos.x, startPos.y, ballRadius);
	tex = LoadTexture("Assets/ball.png");
    currentScore = 0;
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
    if (pbody != nullptr){
        App->physics->world->DestroyBody(pbody->body);
        delete pbody;
        pbody = nullptr;
    }
	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
    if (pbody != nullptr){
        int x, y;
        pbody->GetPhysicPosition(x, y);
        DrawTexture(tex,x-ballRadius, y-ballRadius, WHITE);
    }


    return UPDATE_CONTINUE;
}

// Ball respawn as long as there are balls left
void ModulePlayer::RespawnBall() {
    if (balls > 0 && pbody != nullptr) {
        balls--;
        App->physics->world->DestroyBody(pbody->body);
        delete pbody;
        pbody = App->physics->CreateBall(startPos.x, startPos.y, ballRadius);
    }
}