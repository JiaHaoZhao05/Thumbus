#pragma once

#include "Globals.h"
#include "Module.h"
#include "timer.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>
class PhysicEntity
{
public:

	PhysicEntity(PhysBody* _body, Module* _listener)
		: physBody(_body)
		, listener(_listener)
	{
		physBody->listener = listener;
	}

	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}


	PhysBody* physBody;
	Module* listener;
	int type;
	bool isSwitched = false;

};

class Ball : public PhysicEntity
{
public:
	Ball(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateBall(_x, _y, 12.5), _listener)
		, texture(_texture)
	{
		type = 0;
	}

	void Update() override
	{
		int x, y;
		physBody->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = physBody->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}

private:
	Texture2D texture;

};

class Bumper : public PhysicEntity
{
public:
	Bumper(ModulePhysics* physics, int _x, int _y, int _rad, Module* _listener, Texture2D _texture, Texture2D _textureAux)
		: PhysicEntity(physics->CreateBumper(_x, _y, _rad), _listener)
		, texture(_texture), textureAux(_textureAux)
	{
		type = 1;
		int x, y;
		physBody->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		posX = position.x - texture.width / 2;
		posY = position.y - texture.height / 2;
	}

	float posX;
	float posY;


	void Update() override
	{
		if (isSwitched) {
			DrawTexture(texture, posX, posY, WHITE);
			if (time.ReadSec() > 0.25) isSwitched = false;
		}
		else {
			DrawTexture(textureAux, posX, posY, WHITE);
			time.Start();
		}
	}
private:
	Texture2D texture;
	Texture2D textureAux;
	Timer time;
};

class Background : public PhysicEntity
{
public:
	Background(ModulePhysics* physics, int _x, int _y, const int* points, int size, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(_x, _y, points, size), _listener)
		, texture(_texture)
	{
		type = 3;
	}

	void Update() override
	{
		DrawTexture(texture, 0, 0, WHITE);
	}
private:
	Texture2D texture;

};

class Triangle : public PhysicEntity
{
public:
	Triangle(ModulePhysics* physics, int _x, int _y, const int* points, int size, Module* _listener, Texture2D _texture, Texture2D _textureAux)
		: PhysicEntity(physics->CreateChainTriangle(_x, _y, points, size), _listener)
		, texture(_texture), textureAux(_textureAux)
	{
		type = 2;
	}

	void Update() override
	{
		if (isSwitched) {
			DrawTexture(textureAux, 0, 0, WHITE);
			if (time.ReadSec() > 0.25) isSwitched = false;
		}
		else {
			DrawTexture(texture, 0, 0, WHITE);
			time.Start();
		}
	}
private:
	Texture2D texture;
	Texture2D textureAux;
	Timer time;

};

class Flipper : public PhysicEntity
{
public:
	Flipper(ModulePhysics* physics, int height, int width, float density, float friction, int x, int y, Module* _listener, Texture2D _texture, int id)
		: PhysicEntity(physics->CreateFlipper(height, width, density, friction, x, y, id), _listener)
		, texture(_texture)
	{
		PhysBody* pbody = static_cast<PhysBody*>(physBody);
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
		physBody->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = physBody->GetRotation() * RAD2DEG;
		if (_id == 1) rotation -= 13;
		if (_id == 2) rotation += 13;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);

		Move();
	}

	void Move() {
		if (_id == 1) {
			if (IsKeyDown(KEY_LEFT))
			{
				leftJoint->EnableMotor(false);
				if (IsKeyPressed(KEY_LEFT))
					leftPaddle->ApplyAngularImpulse(-60.0f, true); // flip upward
			}
			else
			{
				float leftAngle = leftPaddle->GetAngle();
				float leftTarget = -30 * DEGTORAD;
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
					rightPaddle->ApplyAngularImpulse(60.0f, true); // flip upward

			}
			else
			{
				float rightAngle = rightPaddle->GetAngle();
				float rightTarget = 30 * DEGTORAD;
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

class Spring : public PhysicEntity
{
public:
	Spring(ModulePhysics* physics, int height, int width, float density, float friction, int x, int y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateSpring(height, width, density, friction, x, y), _listener)
		, texture(_texture)
	{
		springBody = static_cast<PhysBody*>(physBody);
		springPrismatic = springBody->prismaticJoint;
	}

	void Update() {
		//Draw
		int x, y;
		physBody->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y-58.0f};
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = (physBody->GetRotation() * RAD2DEG);
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
			if (currentTranslation > PIXEL_TO_METERS(0.5f)) {
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