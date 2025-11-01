#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

#define PIXELS_PER_METER 50.0f
#define METER_PER_PIXEL (1.0f / PIXELS_PER_METER)
#define METERS_TO_PIXELS(m) ((float)((m) * PIXELS_PER_METER))
#define PIXELS_TO_METERS(p) ((float)(p) / PIXELS_PER_METER)


#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body)
		, listener(_listener)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

protected:
	PhysBody* body;
	Module* listener;
};

class Ball : public PhysicEntity
{
public:
	Ball(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateBall(_x, _y, 12.5), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}


private:
	Texture2D texture;

};

class Bumper : public PhysicEntity
{
public:
	Bumper(ModulePhysics* physics, int _x, int _y, int _rad, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateBumper(_x, _y, _rad), _listener)
		, texture(_texture)
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		posX = position.x - texture.width / 2;
		posY = position.y - texture.height / 2;
	}

	float posX;
	float posY;


	void Update() override
	{
		
		DrawTexture(texture, posX, posY, WHITE);
	}
private:
	Texture2D texture;

};

class Background : public PhysicEntity
{
public:
	Background(ModulePhysics* physics, int _x, int _y, const int* points, int size, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(_x, _y, points, size), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		DrawTexture(texture,0, 0, WHITE);
	}
private:
	Texture2D texture;

};

class Triangle : public PhysicEntity
{
public:
	Triangle(ModulePhysics* physics, int _x, int _y, const int* points, int size, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChainTriangle(_x, _y, points, size), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		DrawTexture(texture, 0, 0, WHITE);
	}
private:
	Texture2D texture;

};

class Flipper : public PhysicEntity
{
public:
	Flipper(ModulePhysics* physics, int height, int width, float density, float friction, int x, int y, Module* _listener, Texture2D _texture, int id)
		: PhysicEntity(physics->CreateFlipper(height, width, density, friction, x, y, id), _listener)
		, texture(_texture) 
	{
		PhysBody* pbody = static_cast<PhysBody*>(body);
		_id = id;
		if (_id == 1) {
			leftPaddle = pbody->body;
			leftJoint = pbody->joint;

		}
		else if (_id == 2) {
			rightPaddle = pbody->body;
			rightJoint = pbody->joint;
		}

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
		float rotation = body->GetRotation() * RAD2DEG;
		if (_id == 1) rotation += 75.0f;
		if (_id == 2) rotation -= 75.0f;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);

		Move();
	}

	void Move() {
		if (_id == 1) {
			if (IsKeyDown(KEY_LEFT))
			{
				leftJoint->EnableMotor(false);
				if (IsKeyPressed(KEY_LEFT))
					leftPaddle->ApplyAngularImpulse(-40.0f, true); // flip upward
			}
			else
			{
				float leftAngle = leftPaddle->GetAngle();
				float leftTarget = -120 * DEGTORAD;
				float leftSpeed = -(leftTarget - leftAngle) * 12.0f;
				leftJoint->EnableMotor(true);
				leftJoint->SetMotorSpeed(leftSpeed);
				leftJoint->SetMaxMotorTorque(50.0f);
			}
		}

		if (_id == 2) {
			if (IsKeyDown(KEY_RIGHT))
			{
				rightJoint->EnableMotor(false);
				if (IsKeyPressed(KEY_RIGHT))
					rightPaddle->ApplyAngularImpulse(40.0f, true); // flip upward
			}
			else
			{
				float rightAngle = rightPaddle->GetAngle();
				float rightTarget = 120 * DEGTORAD;
				float rightSpeed = -(rightTarget - rightAngle) * 12.0f;
				rightJoint->EnableMotor(true);
				rightJoint->SetMotorSpeed(rightSpeed);
				rightJoint->SetMaxMotorTorque(50.0f);
			}
		}
	}
private:
	Texture2D texture;
	int _id;
	b2Body* leftPaddle = nullptr;
	b2Body* rightPaddle = nullptr;
	b2RevoluteJoint* leftJoint = nullptr;
	b2RevoluteJoint* rightJoint = nullptr;
};

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
	ballTex = LoadTexture("Assets/ball.png");
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
		if (IsKeyPressed(KEY_ONE))
	{
		entities.emplace_back(new Ball(App->physics, GetMouseX(), GetMouseY(), this, ballTex));

	}
	 
	for (PhysicEntity* entity : entities)
	{
		entity->Update();
	}

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
	164, 627,
	139, 630,
	115, 640,
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

static constexpr int triangle1[10] = {
	396, 394,
	409, 395,
	445, 465,
	367, 551,
	354, 543
};

static constexpr int triangle2[10] = {
	94, 400,
	109, 399,
	148, 544,
	138, 554,
	54, 468
};

static constexpr int triangle3[10] = {
	454, 313,
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
	entities.emplace_back(new Background(App-> physics, 0, 0, outerBackground, 102, this, outerBackgroundTex));

	//bumpers
	entities.emplace_back(new Bumper(App->physics, bumper1Pos.x, bumper1Pos.y, 21, this, bumperTex));
	entities.emplace_back(new Bumper(App->physics, bumper2Pos.x, bumper2Pos.y, 21, this, bumperTex));
	entities.emplace_back(new Bumper(App->physics, bumper3Pos.x, bumper3Pos.y, 21, this, bumperTex));
	entities.emplace_back(new Bumper(App->physics, bumper4Pos.x, bumper4Pos.y, 11, this, bumperMiniTex));
	entities.emplace_back(new Bumper(App->physics, bumper5Pos.x, bumper5Pos.y, 11, this, bumperMiniTex));
	entities.emplace_back(new Bumper(App->physics, bumper6Pos.x, bumper6Pos.y, 11, this, bumperMiniTex));

	//triangles
	entities.emplace_back(new Triangle(App->physics, -5, 0, triangle1, 10, this, triangle1Tex));
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle2, 10, this, triangle2Tex));
	entities.emplace_back(new Triangle(App->physics, -5, 0, triangle3, 10, this, triangle3Tex));
	entities.emplace_back(new Triangle(App->physics, 0, 0, triangle4, 10, this, triangle4Tex));

	//flippers
	entities.emplace_back(new Flipper(App->physics, 50, 10, 5.0f, 0.3f, 185, 685, this, paddleLeftTex, 1));
	entities.emplace_back(new Flipper(App->physics, 50, 10, 5.0f, 0.3f, 310, 685, this, paddleRightTex, 2));

	//deathzone
	deathZone = App->physics->CreateDeathZone();
	//entities.emplace_back(deathZone, deathZone->listener);
}

//Module game should call activate on the flipper. All input on ModuleGame. We can create a Flipper file where we define the functions that make
// the flippers move