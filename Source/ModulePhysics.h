#pragma once

#include "Module.h"
#include "Globals.h"

#include "box2d\box2d.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y 7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{
	}

	//void GetPosition(int& x, int& y) const;
	void GetPhysicPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Module* listener;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateBall(int x, int y, int radius);
	PhysBody* CreateBumper(int x, int y, int radius);
	PhysBody* CreateDeathZone();
	PhysBody* CreateChain(int x, int y, const int* points, int size);
	PhysBody* CreateChainTriangle(int x, int y, const int* points, int size);
	PhysBody* CreateChainSensor(int x, int y, const int* points, int size);
	
	// Paddles and joints
	b2Body* leftPaddle = nullptr;
	b2Body* rightPaddle = nullptr;
	b2RevoluteJoint* leftJoint = nullptr;
	b2RevoluteJoint* rightJoint = nullptr;

	

private:

	bool debug;

	b2World* world = nullptr;

	// Paddles and joints
	b2Body* leftPaddle = nullptr;
	b2Body* rightPaddle = nullptr;
	b2RevoluteJoint* leftJoint = nullptr;
	b2RevoluteJoint* rightJoint = nullptr;

	// Plunger / spring
	b2Body* springBody = nullptr;
	b2PrismaticJoint* springPrismatic = nullptr;
	float springForce = 0.0f;
	float maxSpringForce = 30.0f;

	

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

	
};