#pragma once
#include "box2d/box2d.h"

class Flipper {
public:

	Flipper(int height, int width, float density, float friction, int x, int y, b2World* world, b2Body* ground, b2Body* Paddle, b2RevoluteJoint* Joint);

private:



};