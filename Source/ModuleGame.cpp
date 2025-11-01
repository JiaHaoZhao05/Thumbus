#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "PhysicEntity.h"
#include "Player.h"

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	paddleRightTex = LoadTexture("Assets/ThumbRight.png");
	paddleLeftTex = LoadTexture("Assets/ThumbLeft.png");
	bumperTex = LoadTexture("Assets/bumper.png");
	bumperMiniTex = LoadTexture("Assets/bumperMini.png");
	outerBackgroundTex = LoadTexture("Assets/Map/outerBackground.png");
	triangle1Tex = LoadTexture("Assets/Map/triangle1.png");
	triangle2Tex = LoadTexture("Assets/Map/triangle2.png");
	triangle3Tex = LoadTexture("Assets/Map/triangle3.png");
	triangle4Tex = LoadTexture("Assets/Map/triangle4.png");

	//rick = LoadTexture("Assets/rick_head.png");

	//bonus_fx = App->audio->LoadFx("Assets/bonus.wav");

	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	CreateWorld();
	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
	/*
	//ball reset
	if (IsKeyDown(KEY_ONE)) {
		ball->physBody->body->SetTransform({ PIXEL_TO_METERS(ballPos.x),PIXEL_TO_METERS(ballPos.y) }, 0);
		ball->physBody->body->SetLinearVelocity({0,0.1});
		ball->physBody->body->SetFixedRotation(true);
		ball->physBody->body->SetFixedRotation(false);
		*/

	if (IsKeyPressed(KEY_ONE))
	{
		App->player->RespawnBall(); //As long as there are balls left pressing 1 will respawn the ball

	}
	
	for (PhysicEntity* entity : entities)
	{
		entity->Update();
	}
	//ball->Update();

	return UPDATE_CONTINUE;
}

static constexpr int outerBackground[48] = {
	241, 6,
	147, 25,
	81, 81,
	40, 151,
	13, 244,
	3, 340,
	8, 414,
	26, 477,
	45, 522,
	133, 585,
	90, 637,
	86, 708,
	397, 716,
	398, 634,
	368, 584,
	443, 530,
	445, 786,
	479, 783,
	486, 419,
	494, 316,
	482, 219,
	455, 142,
	414, 78,
	346, 22
};

static constexpr int triangle1[10] = {
	396, 394,
	409, 395,
	435, 465,
	367, 545,
	345, 544
};

static constexpr int triangle2[10] = {
	94, 400,
	109, 399,
	148, 544,
	138, 545,
	54, 468
};

static constexpr int triangle3[10] = {
	445, 313,
	429, 357,
	420, 355,
	395, 265,
	403, 260
};

static constexpr int triangle4[10] = {
	75, 288,
	67, 290,
	40, 246,
	92, 191,
	101, 197
};

static constexpr int thumbusRight[28] = {
	263, 698,
	260, 688,
	267, 677,
	285, 667,
	314, 663,
	329, 656,
	339, 657,
	348, 665,
	354, 679,
	350, 694,
	339, 695,
	317, 689,
	289, 690,
	273, 697
};
static constexpr int thumbusLeft[28] = {
	231, 690,
	227, 698,
	217, 697,
	198, 689,
	174, 689,
	150, 695,
	143, 694,
	136, 683,
	139, 669,
	152, 655,
	175, 662,
	195, 665,
	216, 673,
	229, 684
};

void ModuleGame::CreateWorld() {

	//background
	entities.emplace_back(new Background(App-> physics, 0, 0, outerBackground, 48, this, outerBackgroundTex));

	//bumpers
	entities.emplace_back(new Bumper(App->physics, bumper1Pos.x, bumper1Pos.y, 30, this, bumperTex));
	entities.emplace_back(new Bumper(App->physics, bumper2Pos.x, bumper2Pos.y, 30, this, bumperTex));
	entities.emplace_back(new Bumper(App->physics, bumper3Pos.x, bumper3Pos.y, 30, this, bumperTex));
	entities.emplace_back(new Bumper(App->physics, bumper4Pos.x, bumper4Pos.y, 11, this, bumperMiniTex));
	entities.emplace_back(new Bumper(App->physics, bumper5Pos.x, bumper5Pos.y, 11, this, bumperMiniTex));
	entities.emplace_back(new Bumper(App->physics, bumper6Pos.x, bumper6Pos.y, 11, this, bumperMiniTex));

	//triangles
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle1, 10, this, triangle1Tex));
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle2, 10, this, triangle2Tex));
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle3, 10, this, triangle3Tex));
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle4, 10, this, triangle4Tex));

	//deathzone
	deathZone = App->physics->CreateDeathZone();
	//entities.emplace_back(deathZone, deathZone->listener);

	//ball
	//ball = new Ball(App->physics, ballPos.x, ballPos.y, this, ballTex);
}

