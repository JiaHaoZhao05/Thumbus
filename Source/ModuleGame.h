#pragma once

#include "Globals.h"
#include "Module.h"
#include "Timer.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

class PhysicEntity;
class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();

	void CreateWorld();
	void PlayRandomSound();
	void ResetMusic();

	std::vector<PhysicEntity*> entities;


	//textures
	Texture2D bumperTex;
	Texture2D bumperTexAux;
	Texture2D bumperMiniTex;
	Texture2D bumperMiniTexAux;
	Texture2D outerBackgroundTex;
	Texture2D triangle1Tex;
	Texture2D triangle1TexAux;
	Texture2D triangle2Tex;
	Texture2D triangle2TexAux;
	Texture2D triangle3Tex;
	Texture2D triangle3TexAux;
	Texture2D triangle4Tex;
	Texture2D triangle4TexAux;
	Texture2D tyellowTex;
	Texture2D tblueTex;
	Texture2D hyellowTex;
	Texture2D hblueTex;
	Texture2D uyellowTex;
	Texture2D ublueTex;
	Texture2D myellowTex;
	Texture2D mblueTex;
	Texture2D byellowTex;
	Texture2D bblueTex;

	//sounds
	unsigned int bumperFX;
	unsigned int bumperFX2;
	unsigned int bumperFX3;
	unsigned int springFX;
	unsigned int flipperLFX;
	unsigned int flipperRFX;
	unsigned int deathFX;
	unsigned int respawnFX;
	unsigned int gameOverFX;
	unsigned int extraBallFX;
	unsigned int sensorFX1;
	unsigned int sensorFX2;
	unsigned int sensorFX3;
	unsigned int sensorFX4;
	unsigned int sensorFX5;
	unsigned int pinballTheme;

	//positions
	Vector2 bumper1Pos = { 150, 300 };
	Vector2 bumper2Pos = { 275, 375 };
	Vector2 bumper3Pos = { 325, 250 };
	Vector2 bumper4Pos = { 350, 125 };
	Vector2 bumper5Pos = { 150, 150 };
	Vector2 bumper6Pos = { 250, 200 };
	Vector2 ballPos = { 463,560 };

	Timer musicTimer;
	
};
