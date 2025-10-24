#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"

#include "p2Point.h"
#include <math.h>
#include <box2d/box2d.h>

#define PIXELS_PER_METER 50.0f
#define METER_PER_PIXEL (1.0f / PIXELS_PER_METER)
#define METERS_TO_PIXELS(m) ((float)((m) * PIXELS_PER_METER))
#define PIXELS_TO_METERS(p) ((float)(p) / PIXELS_PER_METER)


#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f


ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    debug = true;
}

ModulePhysics::~ModulePhysics()
{

}

bool ModulePhysics::Start()
{
    LOG("Creating Physics 2D environment");

    // --- Create Box2D world ---
    b2Vec2 gravity(0.0f, 10.0f);
    world = new b2World(gravity);

    // --- Create static ground ---
    b2BodyDef groundDef;
    groundDef.position.Set(0, 0);
    b2Body* ground = world->CreateBody(&groundDef);

    // --- Paddle fixture ---
    b2PolygonShape paddleShape;
    paddleShape.SetAsBox(PIXELS_TO_METERS(50), PIXELS_TO_METERS(10)); // 100x20 pixels

    b2FixtureDef paddleFixture;
    paddleFixture.shape = &paddleShape;
    paddleFixture.density = 5.0f;
    paddleFixture.friction = 0.3f;

    // --- LEFT PADDLE ---
    // --- LEFT PADDLE ---
    b2BodyDef leftDef;
    leftDef.type = b2_dynamicBody;
    // Center of paddle is half-width to the right of pivot
    leftDef.position.Set(PIXELS_TO_METERS(150 + 50), PIXELS_TO_METERS(400));
    leftPaddle = world->CreateBody(&leftDef);
    leftPaddle->CreateFixture(&paddleFixture);

    b2RevoluteJointDef leftJointDef;
    leftJointDef.bodyA = ground;
    leftJointDef.bodyB = leftPaddle;

    // Pivot at left side of paddle (base)
    leftJointDef.localAnchorA.Set(PIXELS_TO_METERS(150), PIXELS_TO_METERS(400));
    leftJointDef.localAnchorB.Set(-PIXELS_TO_METERS(50), 0); // relative to paddle center

    leftJointDef.enableLimit = true;
    leftJointDef.lowerAngle = -15 * DEGTORAD; // resting downward
    leftJointDef.upperAngle = 30 * DEGTORAD;  // max upward flip

    leftJointDef.enableMotor = true;
    leftJointDef.maxMotorTorque = 100.0f;

    leftJoint = (b2RevoluteJoint*)world->CreateJoint(&leftJointDef);


    // --- RIGHT PADDLE ---
    b2BodyDef rightDef;
    rightDef.type = b2_dynamicBody;
    rightDef.position.Set(PIXELS_TO_METERS(650 - 50), PIXELS_TO_METERS(400)); // center half-width left
    rightPaddle = world->CreateBody(&rightDef);
    rightPaddle->CreateFixture(&paddleFixture);

    b2RevoluteJointDef rightJointDef;
    rightJointDef.bodyA = ground;
    rightJointDef.bodyB = rightPaddle;

    // Pivot at right side of paddle (base)
    rightJointDef.localAnchorA.Set(PIXELS_TO_METERS(650), PIXELS_TO_METERS(400));
    rightJointDef.localAnchorB.Set(PIXELS_TO_METERS(50), 0);

    rightJointDef.enableLimit = true;
    rightJointDef.lowerAngle = -30 * DEGTORAD; // upward flip
    rightJointDef.upperAngle = 15 * DEGTORAD;  // resting downward

    rightJointDef.enableMotor = true;
    rightJointDef.maxMotorTorque = 100.0f;

    rightJoint = (b2RevoluteJoint*)world->CreateJoint(&rightJointDef);

    leftJoint->EnableMotor(false);
    rightJoint->EnableMotor(false);

    return true;
}


update_status ModulePhysics::PreUpdate()
{
    world->Step(1.0f / 60.0f, 6, 2);

    // --- LEFT PADDLE ---
    // LEFT PADDLE
    if (IsKeyDown(KEY_LEFT))
    {
        leftJoint->EnableMotor(false);
        if (IsKeyPressed(KEY_LEFT))
            leftPaddle->ApplyAngularImpulse(-40.0f, true); // flip upward
    }
    else
    {
        float leftAngle = leftPaddle->GetAngle();
        float leftTarget = -30 * DEGTORAD;
        float leftSpeed = -(leftTarget - leftAngle) * 12.0f;
        leftJoint->EnableMotor(true);
        leftJoint->SetMotorSpeed(leftSpeed);
        leftJoint->SetMaxMotorTorque(50.0f);
    }

    // RIGHT PADDLE
    if (IsKeyDown(KEY_RIGHT))
    {
        rightJoint->EnableMotor(false);
        if (IsKeyPressed(KEY_RIGHT))
            rightPaddle->ApplyAngularImpulse(40.0f, true); // flip upward
    }
    else
    {
        float rightAngle = rightPaddle->GetAngle();
        float rightTarget = 30 * DEGTORAD;
        float rightSpeed = -(rightTarget - rightAngle) * 12.0f;
        rightJoint->EnableMotor(true);
        rightJoint->SetMotorSpeed(rightSpeed);
        rightJoint->SetMaxMotorTorque(50.0f);
    }


    return UPDATE_CONTINUE;
}



update_status ModulePhysics::PostUpdate()
{
    if (!debug)
        return UPDATE_CONTINUE;

    // --- Draw paddles ---
    b2Vec2 lPos = leftPaddle->GetPosition();
    float lAngle = leftPaddle->GetAngle() * RADTODEG;

    b2Vec2 rPos = rightPaddle->GetPosition();
    float rAngle = rightPaddle->GetAngle() * RADTODEG;

    DrawRectanglePro(
        Rectangle{ METERS_TO_PIXELS(lPos.x), METERS_TO_PIXELS(lPos.y), 100, 20 },
        Vector2{ 50, 10 },
        lAngle,
        RED
    );

    DrawRectanglePro(
        Rectangle{ METERS_TO_PIXELS(rPos.x), METERS_TO_PIXELS(rPos.y), 100, 20 },
        Vector2{ 50, 10 },
        rAngle,
        BLUE
    );

    return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
    LOG("Destroying physics world");

    delete world;
    world = nullptr;

    return true;
}
