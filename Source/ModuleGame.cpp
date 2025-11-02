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
	SetTargetFPS(60);
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//load textures
	paddleRightTex = LoadTexture("Assets/ThumbRight.png");
	paddleLeftTex = LoadTexture("Assets/ThumbLeft.png");
	bumperTex = LoadTexture("Assets/bumper2.png");
	bumperMiniTex = LoadTexture("Assets/bumperMini2.png");
	bumperTexAux = LoadTexture("Assets/bumper.png");
	bumperMiniTexAux = LoadTexture("Assets/bumperMini.png");
	outerBackgroundTex = LoadTexture("Assets/Map/outerBackground.png");
	triangle1Tex = LoadTexture("Assets/Map/triangle1.png");
	triangle2Tex = LoadTexture("Assets/Map/triangle2.png");
	triangle3Tex = LoadTexture("Assets/Map/triangle3.png");
	triangle4Tex = LoadTexture("Assets/Map/triangle4.png");
	triangle1TexAux = LoadTexture("Assets/Map/triangle12.png");
	triangle2TexAux = LoadTexture("Assets/Map/triangle22.png");
	triangle3TexAux = LoadTexture("Assets/Map/triangle32.png");
	triangle4TexAux = LoadTexture("Assets/Map/triangle42.png");

	//load sounds
	bumperFX = App->audio->LoadFx("Assets/sound.wav");
	bumperFX2 = App->audio->LoadFx("Assets/sound.wav");

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

static constexpr int outerBackground[92] = {
	241, 6,
	165, 17,
	123, 38,
	80, 78,
	49, 128,
	27, 183,
	12, 246,
	5, 310,
	9, 407,
	18, 485,
	28, 536,
	58, 590,
	88, 627,
	116, 651,
	137, 665,
	112, 686,
	108, 756,
	95, 763,
	90, 778,
	110, 779,
	143, 784,
	166, 802,
	158, 921,
	366, 919,
	369, 801,
	379, 789,
	390, 785,
	394, 699,
	387, 685,
	374, 683,
	354, 670,
	378, 653,
	400, 632,
	415, 610,
	445, 570,
	452, 785,
	478, 785,
	476, 464,
	488, 404,
	492, 328,
	485, 244,
	466, 173,
	435, 105,
	396, 57,
	358, 29,
	301, 9
};

static constexpr int triangle1[10] = { //bottom right triangle
	396, 394,
	409, 395,
	435, 465,
	367, 545,
	355, 544
};

static constexpr int triangle2[10] = { //bottom left triangle
	94, 400,
	109, 399,
	148, 544,
	138, 545,
	54, 468
};

static constexpr int triangle3[10] = { //top right triangle
	445, 313,
	429, 357,
	420, 355,
	395, 265,
	403, 260
};

static constexpr int triangle4[10] = { //top left triangle
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
	entities.emplace_back(new Background(App->physics, 0, 0, outerBackground, 92, this, outerBackgroundTex));

	//bumpers
	entities.emplace_back(new Bumper(App->physics, bumper1Pos.x, bumper1Pos.y, 30, this, bumperTex, bumperTexAux));
	entities.emplace_back(new Bumper(App->physics, bumper2Pos.x, bumper2Pos.y, 30, this, bumperTex, bumperTexAux));
	entities.emplace_back(new Bumper(App->physics, bumper3Pos.x, bumper3Pos.y, 30, this, bumperTex, bumperTexAux));
	entities.emplace_back(new Bumper(App->physics, bumper4Pos.x, bumper4Pos.y, 11, this, bumperMiniTex, bumperMiniTexAux));
	entities.emplace_back(new Bumper(App->physics, bumper5Pos.x, bumper5Pos.y, 11, this, bumperMiniTex, bumperMiniTexAux));
	entities.emplace_back(new Bumper(App->physics, bumper6Pos.x, bumper6Pos.y, 11, this, bumperMiniTex, bumperMiniTexAux));

	//triangles
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle1, 10, this, triangle1Tex, triangle1TexAux));
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle2, 10, this, triangle2Tex, triangle2TexAux));
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle3, 10, this, triangle3Tex, triangle3TexAux));
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle4, 10, this, triangle4Tex, triangle4TexAux));

	//flippers
	entities.emplace_back(new Flipper(App->physics, 100, 20, 5.0f, 0.3f, 175, 677, this, paddleLeftTex, 1));
	entities.emplace_back(new Flipper(App->physics, 100, 20, 5.0f, 0.3f, 250, 677, this, paddleRightTex, 2));

	//spring
	//entities.emplace_back(new Spring(App->physics, 10, 60, 1.0f, 0.2f, 463, 650, this, paddleLeftTex));

	//deathzone
	deathZone = App->physics->CreateDeathZone();
	//entities.emplace_back(deathZone, deathZone->listener);
}
//Module game should call activate on the flipper. All input on ModuleGame. We can create a Flipper file where we define the functions that make
// the flippers move
