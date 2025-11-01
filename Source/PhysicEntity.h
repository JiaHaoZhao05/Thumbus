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