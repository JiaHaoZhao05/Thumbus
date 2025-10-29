#pragma once

class Flipper {
private:

	Flipper(int height, int width, float density, float friction, int x, int y, b2World* world, b2Body* ground, b2Body* Paddle, b2RevoluteJoint* Joint);
	~Flipper();

public:


};