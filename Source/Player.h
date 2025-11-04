#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"




class PhysicEntity;
class ModulePlayer : public Module
{
public:

	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	bool CleanUp();

	void RespawnBall();
	void ModedBallFriction(float friction);
	void ExtraBall();
	void ReadInputs();

public:

	int currentScore = 0;
	int previousScore = 0;
	int highScore = 0 ;
	int balls = 3;
	int currentBalls = 0;
	bool isDead = false;

	int thumb = 0;
	bool isExtraBall = false;

	PhysicEntity* ball;
	PhysicEntity* extraBall;
	Texture2D ballTex;
	Texture2D xtraballTex;
	float ballRadius = 12.5f;
	float friction = 0.1f;
	Vector2 startPos = {465,550};
	Vector2 startPosExtra = { 230,30 };

	PhysicEntity* paddleLeft;
	PhysicEntity* paddleRight;
	PhysicEntity* spring;
	Texture2D paddleLeftTex;
	Texture2D paddleRightTex;
	Texture2D springTex;
	bool wasKeyDown = false;
};