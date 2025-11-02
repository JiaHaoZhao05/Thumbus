#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "PhysicEntity.h"
#include "Player.h"

class Spring : public PhysicEntity
{
public:
	Spring(ModulePhysics* physics, int height, int width, float density, float friction, int x, int y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateSpring(height, width, density, friction, x, y), _listener)
		, texture(_texture)
	{
		springBody = static_cast<PhysBody*>(body);
		springPrismatic = springBody->prismaticJoint;
	}

	void Update() {
		//Draw
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = (body->GetRotation() * RAD2DEG) + 85;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);

		Move();
	}

	void Move() {
		float currentTranslation = springPrismatic->GetJointTranslation();
		static bool wasKeyDown = false;

		if (IsKeyDown(KEY_DOWN)) {
			wasKeyDown = true;

			springPrismatic->EnableMotor(true);

			if (currentTranslation < springPrismatic->GetUpperLimit()) {
				springPrismatic->SetMotorSpeed(5.0f);     // positive = move down on screen
				springPrismatic->SetMaxMotorForce(10.0f);
			}
			else {
				springPrismatic->SetMotorSpeed(0.0f);
			}
		}
		else if (wasKeyDown && IsKeyUp(KEY_DOWN)) {
			wasKeyDown = false;

			float compression = fabs(currentTranslation - springPrismatic->GetLowerLimit()); //Erik you need the difference that was just its current position
			float k = 15.0f; // constante elastica del resorte (ajustable)
			float force = -k * compression; //raylib negative == up

			springPrismatic->EnableMotor(false);

			// aplicar impulso hacia arriba
			springBody->body->ApplyLinearImpulseToCenter(b2Vec2(0.0f, force), true);
		}
		else { //si no estas haciendo nada, gravedad mueve el muelle, esto hace que vuelva a su posicion inicial, no lo quites Erik
			if (currentTranslation > PIXELS_TO_METERS(0.5f)) {
				springPrismatic->EnableMotor(true);
				springPrismatic->SetMotorSpeed(-10.0f);  // negative = move up on screen
				springPrismatic->SetMaxMotorForce(150.0f);
			}
			else {
				springPrismatic->EnableMotor(false);
			}
		}
	}

private:
	Texture2D texture;
	PhysBody* springBody;
	b2PrismaticJoint* springPrismatic;
};

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

static constexpr int outerBackground[102] = {
	241, 6,
	147, 25,
	81, 81,
	40, 151,
	13, 244,
	3, 340,
	8, 414,
	26, 477,
	31, 504,
	96, 576,
	111, 579,
	146, 582,
	155, 590,
	167, 595,
	180, 618,
	177, 628,
	160, 617,
	137, 620,
	97, 640,
	97, 706,
	82, 724,
	77, 745,
	68, 763,
	92, 771,
	111, 782,
	125, 798,
	126, 849,
	365, 853,
	371, 798,
	380, 784,
	395, 773,
	397, 709,
	380, 696,
	372, 658,
	352, 628,
	312, 620,
	325, 598,
	344, 581,
	381, 579,
	401, 577,
	448, 537,
	451, 784,
	478, 784,
	478, 457,
	491, 357,
	488, 270,
	472, 194,
	442, 118,
	398, 60,
	347, 25,
	286, 6
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
	entities.emplace_back(new Background(App->physics, 0, 0, outerBackground, 102, this, outerBackgroundTex));

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
	entities.emplace_back(new Flipper(App->physics, 100, 20, 5.0f, 0.3f, 175, 663, this, paddleLeftTex, 1));
	entities.emplace_back(new Flipper(App->physics, 100, 20, 5.0f, 0.3f, 250, 663, this, paddleRightTex, 2));

	//spring
	entities.emplace_back(new Spring(App->physics, 10, 60, 1.0f, 0.2f, 463, 650, this, paddleLeftTex));

	//deathzone
	deathZone = App->physics->CreateDeathZone();
	//entities.emplace_back(deathZone, deathZone->listener);
}
//Module game should call activate on the flipper. All input on ModuleGame. We can create a Flipper file where we define the functions that make
// the flippers move
