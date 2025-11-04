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
	tyellowTex = LoadTexture("Assets/tyellow.png");
	tblueTex = LoadTexture("Assets/tblue.png");
	hyellowTex = LoadTexture("Assets/hyellow.png");
	hblueTex = LoadTexture("Assets/hblue.png");
	uyellowTex = LoadTexture("Assets/uyellow.png");
	ublueTex = LoadTexture("Assets/ublue.png");
	myellowTex = LoadTexture("Assets/myellow.png");
	mblueTex = LoadTexture("Assets/mblue.png");
	byellowTex = LoadTexture("Assets/byellow.png");
	bblueTex = LoadTexture("Assets/bblue.png");
	


	//load sounds
	bumperFX = App->audio->LoadFx("Assets/Sounds/bumperFX.wav");
	bumperFX2 = App->audio->LoadFx("Assets/Sounds/bumperFX2.wav");
	bumperFX3 = App->audio->LoadFx("Assets/Sounds/bumperFX3.wav");
	springFX = App->audio->LoadFx("Assets/Sounds/springFX.wav");
	flipperLFX = App->audio->LoadFx("Assets/Sounds/flipperLFX.wav");
	flipperRFX = App->audio->LoadFx("Assets/Sounds/flipperRFX.wav");
	deathFX = App->audio->LoadFx("Assets/Sounds/deathFX.wav");
	respawnFX = App->audio->LoadFx("Assets/Sounds/respawnFX.wav");
	gameOverFX = App->audio->LoadFx("Assets/Sounds/gameOverFX.wav");
	extraBallFX = App->audio->LoadFx("Assets/Sounds/extraBallFX.wav");
	sensorFX1 = App->audio->LoadFx("Assets/Sounds/sensorFX1.wav");
	sensorFX2 = App->audio->LoadFx("Assets/Sounds/sensorFX2.wav");
	sensorFX3 = App->audio->LoadFx("Assets/Sounds/sensorFX3.wav");
	sensorFX4 = App->audio->LoadFx("Assets/Sounds/sensorFX4.wav");
	sensorFX5 = App->audio->LoadFx("Assets/Sounds/sensorFX5.wav");
	pinballTheme = App->audio->LoadFx("Assets/Music/pinballTheme.wav");



	//music
	ResetMusic();

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
	for (PhysicEntity* entity : entities)
	{
		entity->Update();
	}
	if (musicTimer.ReadSec() > 76) {
		ResetMusic();
	}
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
	385, 681,
	362, 680,
	355, 667,
	377, 652,
	400, 632,
	415, 610,
	444, 566,
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
	393, 394,
	406, 395,
	435, 465,
	364, 545,
	352, 544
};

static constexpr int triangle2[10] = { //bottom left triangle
	94, 400,
	109, 399,
	148, 544,
	138, 550,
	54, 468
};

static constexpr int triangle3[10] = { //top right triangle
	445, 313,
	426, 357,
	418, 355,
	391, 265,
	401, 260
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

static constexpr int t[12] = {
	187, 404,
	230, 440,
	206, 451,
	170, 474,
	155, 455,
	178, 426
};

static constexpr int h[8] = {
	39, 345,
	80, 306,
	117, 349,
	77, 387
};

static constexpr int u[12] = {
	219, 254,
	268, 254,
	265, 297,
	247, 318,
	208, 316,
	202, 281
};

static constexpr int m[8] = {
	293, 138,
	351, 141,
	358, 209,
	277, 202
};

static constexpr int b[8] = {
	322, 306,
	367, 297,
	385, 361,
	337, 375
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


	//sensors
	entities.emplace_back(new Sensor(App->physics, 0, 0, t, 12, this, tblueTex, tyellowTex));
	entities.emplace_back(new Sensor(App->physics, 0, 0, h, 8, this, hblueTex, hyellowTex));
	entities.emplace_back(new Sensor(App->physics, 0, 0, u, 12, this, ublueTex, uyellowTex));
	entities.emplace_back(new Sensor(App->physics, 0, 0, m, 8, this, mblueTex, myellowTex));
	entities.emplace_back(new Sensor(App->physics, 0, 0, b, 8, this, bblueTex, byellowTex));

	//deathzone
	entities.emplace_back(new DeathZone(App->physics,this));

}
//Module game should call activate on the flipper. All input on ModuleGame. We can create a Flipper file where we define the functions that make
// the flippers move

void ModuleGame::PlayRandomSound() {
	int r = rand() % 6;

	switch (r)
	{
	case 0:
		App->audio->PlayFx(bumperFX-1);
		break;
	case 1:
		App->audio->PlayFx(bumperFX2-1);
		break;
	case 2:
		App->audio->PlayFx(bumperFX3-1);
		break;
	case 3:
		App->audio->PlayFx(bumperFX-1);
		break;
	case 4:
		App->audio->PlayFx(bumperFX2-1);
		break;
	case 5:
		App->audio->PlayFx(bumperFX - 1);
		break;
	}
}

void ModuleGame::ResetMusic() {
	App->audio->PlayFx(pinballTheme - 1, 1);
	musicTimer = Timer();
}