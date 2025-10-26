#pragma once

#include "Globals.h"
#include "Module.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicEntity;


class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();

public:
	void ModuleGame::CreateWorld();
	std::vector<PhysicEntity*> entities;
	PhysicEntity* ball;

	PhysBody* deathZone;

	//textures
	Texture2D ballTex;
	Texture2D bumperTex;
	Texture2D bumperMiniTex;
	Texture2D outerBackgroundTex;
	Texture2D triangle1Tex;
	Texture2D triangle2Tex;
	Texture2D triangle3Tex;
	Texture2D triangle4Tex;

	//positions
	Vector2 bumper1Pos = { 150, 300 };
	Vector2 bumper2Pos = { 275, 375 };
	Vector2 bumper3Pos = { 325, 250 };
	Vector2 bumper4Pos = { 350, 125 };
	Vector2 bumper5Pos = { 150, 150 };
	Vector2 bumper6Pos = { 250, 200 };
	Vector2 ballPos = { 463,560 };

	//score and balls
	int currentScore = 0;
	int previousScore = 0;
	int highScore = 0;
	int balls = 3;
	int currentBalls = 0;
	
};
