#include "Globals.h"
#include "Application.h"
#include "Player.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "PhysicEntity.h"
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
    ballTex = LoadTexture("Assets/ball.png");
    ball = new Ball(App->physics, startPos.x, startPos.y, this, ballTex, friction);
    currentScore = 0;
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
    if (ball->physBody != nullptr){
        App->physics->world->DestroyBody(ball->physBody->body);
        delete ball->physBody;
        ball->physBody = nullptr;
    }
	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
    ball->Update();


    return UPDATE_CONTINUE;
}

// Ball respawn as long as there are balls left
void ModulePlayer::RespawnBall() {
    LOG("RESPAWNBALL");
    if (balls > 0 && ball->physBody != nullptr) {
        balls--;
        App->physics->world->DestroyBody(ball->physBody->body);
        delete ball->physBody;
        ball = new Ball(App->physics, startPos.x, startPos.y, this, ballTex, friction);
    }
}

//reapear a ball at same position with different friction
void::ModulePlayer::ModedBallFriction(float friction) {
    if (ball->physBody != nullptr) {
        int x, y;
        ball->physBody->GetPhysicPosition(x, y);
        App->physics->world->DestroyBody(ball->physBody->body);
        delete ball->physBody;
        ball = new Ball(App->physics, x, y, this, ballTex, friction);
    }
}


void ModulePlayer::ExtraBall() {

}

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB) {
    //currentScore++;
}
