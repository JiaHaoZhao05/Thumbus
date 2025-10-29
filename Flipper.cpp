#include "Flipper.h"
#include "box2d/box2d.h"

#define PIXELS_PER_METER 50.0f
#define METER_PER_PIXEL (1.0f / PIXELS_PER_METER)
#define METERS_TO_PIXELS(m) ((float)((m) * PIXELS_PER_METER))
#define PIXELS_TO_METERS(p) ((float)(p) / PIXELS_PER_METER)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

Flipper::Flipper(int height, int width, float density, float friction, int x, int y, b2World *world, b2Body* ground, b2Body* Paddle, b2RevoluteJoint* Joint) {
    b2PolygonShape paddleShape;
    paddleShape.SetAsBox(PIXELS_TO_METERS(50), PIXELS_TO_METERS(10)); // 100x20 pixels

    b2FixtureDef paddleFixture;
    paddleFixture.shape = &paddleShape;
    paddleFixture.density = density;
    paddleFixture.friction = friction;

    // --- LEFT PADDLE ---
    b2BodyDef leftDef;
    leftDef.type = b2_dynamicBody;
    // Center of paddle is half-width to the right of pivot
    leftDef.position.Set(PIXELS_TO_METERS(140 + 50), PIXELS_TO_METERS(600));
    Paddle = world->CreateBody(&leftDef);
    Paddle->CreateFixture(&paddleFixture);

    b2RevoluteJointDef leftJointDef;
    leftJointDef.bodyA = ground;
    leftJointDef.bodyB = Paddle;

    // Pivot at left side of paddle (base)
    leftJointDef.localAnchorA.Set(PIXELS_TO_METERS(140), PIXELS_TO_METERS(600));
    leftJointDef.localAnchorB.Set(-PIXELS_TO_METERS(50), 0); // relative to paddle center

    leftJointDef.enableLimit = true;
    leftJointDef.lowerAngle = -15 * DEGTORAD; // resting downward
    leftJointDef.upperAngle = 30 * DEGTORAD;  // max upward flip

    leftJointDef.enableMotor = true;
    leftJointDef.maxMotorTorque = 100.0f;

    Joint = (b2RevoluteJoint*)world->CreateJoint(&leftJointDef);
}

