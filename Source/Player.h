#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

	void RespawnBall();
public:
	int currentScore = 0;
	int previousScore = 0;
	int highScore = 0 ;
	int balls = 3;
	int currentBalls = 0;

	PhysBody* pbody = nullptr;
	Texture2D tex;
	float ballRadius = 12.5f;
	Vector2 startPos = {465,550};
};