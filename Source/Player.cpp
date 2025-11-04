#include "Globals.h"
#include "Application.h"
#include "Player.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
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
    xtraballTex = LoadTexture("Assets/xtraball.png");
    ball = new Ball(App->physics, startPos.x, startPos.y, this, ballTex, friction);
    currentScore = 0;

    paddleRightTex = LoadTexture("Assets/ThumbRight.png");
    paddleLeftTex = LoadTexture("Assets/ThumbLeft.png");
    springTex = LoadTexture("Assets/spring.png");
    //spring
    spring = new Spring(App->physics, 10, 60, 1.0f, 0.2f, 463, 650, this, springTex);
    //flippers
    paddleLeft = new Flipper(App->physics, 100, 20, 5.0f, 0.3f, 175, 677, this, paddleLeftTex, 1);
    paddleRight = new Flipper(App->physics, 100, 20, 5.0f, 0.3f, 250, 679, this, paddleRightTex, 2);

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
    ReadInputs();
    if (isDead) {
        balls--;
        isDead = false;
        if (!isExtraBall && balls <= 0) { //game over
            if (currentScore > highScore) highScore = currentScore;
            previousScore = currentScore;
            currentScore = 0;
            balls = 3;
            if (thumb > 0) {
                for (auto& pEntity : App->scene_intro->entities) {
                    if (pEntity->type == 4) pEntity->isSwitched = false;
                }
                thumb = 0;
            }
            App->audio->PlayFx(App->scene_intro->gameOverFX-1);
            App->scene_intro->ResetMusic();
        }
        if (isExtraBall && balls == 0);
        else RespawnBall();
    }
    if (thumb == 5 && !isExtraBall) {
        ExtraBall();
    }

    //udpate entites
    if (isExtraBall) extraBall->Update();
    ball->Update();
    paddleLeft->Update();
    paddleRight->Update();
    spring->Update();



    return UPDATE_CONTINUE;
}

// Ball respawn as long as there are balls left
void ModulePlayer::RespawnBall() {
    LOG("RESPAWNBALL");
    if (balls >= 0 && ball->physBody != nullptr) {
        App->audio->PlayFx(App->scene_intro->respawnFX-1);
        ball->physBody->body->SetLinearVelocity({ 0,0.1 });
        ball->physBody->body->SetFixedRotation(true);
        ball->physBody->body->SetFixedRotation(false);
        ball->physBody->body->SetTransform({ 0.02f * startPos.x,0.02f * startPos.y }, 0);
     
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
    if (isExtraBall && extraBall->physBody != nullptr) {
        int x, y;
        extraBall->physBody->GetPhysicPosition(x, y);
        App->physics->world->DestroyBody(extraBall->physBody->body);
        delete extraBall->physBody;
        extraBall = new Ball(App->physics, x, y, this, xtraballTex, friction);
    }
}


void ModulePlayer::ExtraBall() {
    currentScore += 1000;
    App->audio->PlayFx(App->scene_intro->extraBallFX-1);
    extraBall = new Ball(App->physics, startPosExtra.x, startPosExtra.y, this, xtraballTex, friction);

    isExtraBall = true;
}

void ModulePlayer::ReadInputs() {
    if (IsKeyDown(KEY_LEFT)) { //left paddle inputs
        paddleLeft->state = 1;
       if (IsKeyPressed(KEY_LEFT)) {
          paddleLeft->state = 2;
          App->audio->PlayFx(App->scene_intro->flipperLFX - 1);
        }
    }
    else paddleLeft->state = 0;

    if (IsKeyDown(KEY_RIGHT)) { //right paddle inputs
        paddleRight->state = 1;
        if (IsKeyPressed(KEY_RIGHT)) {
            paddleRight->state = 2;
            App->audio->PlayFx(App->scene_intro->flipperRFX - 1);
        }
    }
    else paddleRight->state = 0;

    if (IsKeyDown(KEY_DOWN)) { //spring inputs   
        spring->state = 1;
        wasKeyDown = true;
    }
    else if (wasKeyDown && IsKeyUp(KEY_DOWN)) {
        wasKeyDown = false;
        spring->state = 2;
        App->audio->PlayFx(App->scene_intro->springFX - 1);
    }
    else  spring->state = 0;
}

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB) {
}
