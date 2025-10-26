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

	Texture2D ballTex;
	Texture2D bumperTex;
	Vector2 bumperPos = { 300, 400 };
	
};
