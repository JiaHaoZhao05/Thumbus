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
    b2BodyDef leftDef;
    leftDef.type = b2_dynamicBody;
    // Center of paddle is half-width to the right of pivot
    leftDef.position.Set(PIXELS_TO_METERS(190 + 50), PIXELS_TO_METERS(600));
    leftPaddle = world->CreateBody(&leftDef);
    leftPaddle->CreateFixture(&paddleFixture);

    b2RevoluteJointDef leftJointDef;
    leftJointDef.bodyA = ground;
    leftJointDef.bodyB = leftPaddle;

    // Pivot at left side of paddle (base)
    leftJointDef.localAnchorA.Set(PIXELS_TO_METERS(190), PIXELS_TO_METERS(600));
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
    rightDef.position.Set(PIXELS_TO_METERS(410 - 50), PIXELS_TO_METERS(600)); // center half-width left
    rightPaddle = world->CreateBody(&rightDef);
    rightPaddle->CreateFixture(&paddleFixture);

    b2RevoluteJointDef rightJointDef;
    rightJointDef.bodyA = ground;
    rightJointDef.bodyB = rightPaddle;

    // Pivot at right side of paddle (base)
    rightJointDef.localAnchorA.Set(PIXELS_TO_METERS(410), PIXELS_TO_METERS(600));
    rightJointDef.localAnchorB.Set(PIXELS_TO_METERS(50), 0);

    rightJointDef.enableLimit = true;
    rightJointDef.lowerAngle = -30 * DEGTORAD; // upward flip
    rightJointDef.upperAngle = 15 * DEGTORAD;  // resting downward

    rightJointDef.enableMotor = true;
    rightJointDef.maxMotorTorque = 100.0f;

    rightJoint = (b2RevoluteJoint*)world->CreateJoint(&rightJointDef);

    leftJoint->EnableMotor(false);
    rightJoint->EnableMotor(false);

    // --- PLUNGER / SPRING BODY (rectángulo vertical que se desliza) ---
    b2BodyDef springDef;
    springDef.type = b2_dynamicBody;
    springDef.position.Set(PIXELS_TO_METERS(500), PIXELS_TO_METERS(500));
    springDef.fixedRotation = true; // que no rote
    springBody = world->CreateBody(&springDef);

    b2PolygonShape springShape;
    springShape.SetAsBox(PIXELS_TO_METERS(10), PIXELS_TO_METERS(60)); // 20x120 píxeles

    b2FixtureDef springFixture;
    springFixture.shape = &springShape;
    springFixture.density = 1.0f;     // masa moderada para que pueda moverse
    springFixture.friction = 0.2f;
    springBody->CreateFixture(&springFixture);

    // --- Cuerpo estático de anclaje superior ---
    b2BodyDef anchorDef;
    anchorDef.position.Set(PIXELS_TO_METERS(500), PIXELS_TO_METERS(440)); // punto fijo arriba
    b2Body* anchor = world->CreateBody(&anchorDef);

    // --- Prismatic joint: solo movimiento vertical ---
    b2PrismaticJointDef prismaticDef;
    prismaticDef.Initialize(anchor, springBody, anchor->GetPosition(), b2Vec2(0.0f, 1.0f));
    prismaticDef.enableLimit = true;
    prismaticDef.lowerTranslation = -PIXELS_TO_METERS(80); // 80 px hacia abajo máximo
    prismaticDef.upperTranslation = 0.0f; // punto de reposo
    springPrismatic = (b2PrismaticJoint*)world->CreateJoint(&prismaticDef);



    return true;
}


update_status ModulePhysics::PreUpdate()
{
    world->Step(1.0f / 60.0f, 6, 2);

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

    // --- Control del plunger (spring) ---
    float currentTranslation = springPrismatic->GetJointTranslation();

    // --- Tirar hacia abajo ---
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        // aplicamos impulso hacia abajo solo si no llega al tope
        if (currentTranslation > springPrismatic->GetLowerLimit())
        {
            springBody->ApplyLinearImpulseToCenter(b2Vec2(0.0f, 5.0f), true);
        }
    }
    // --- Soltar: impulsa hacia arriba proporcional a la distancia ---
    else if (currentTranslation < -PIXELS_TO_METERS(2))
    {
        float compression = fabs(currentTranslation);  // cuánto bajó el plunger
        float k = 250.0f; // constante elastica del resorte (ajustable)
        float force = k * compression;

        // aplicar impulso hacia arriba
        springBody->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -force), true);
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
    
    //Draw spring
    b2Vec2 pos = springBody->GetPosition();

    DrawRectanglePro(
        Rectangle{ METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), 20, 120 },
        Vector2{ 10, 60 },
        0.0f,
        ORANGE
    );


    return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius)
{
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_dynamicBody;
    body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
    body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

    b2Body* b = world->CreateBody(&body);

    b2CircleShape shape;
    shape.m_radius = PIXEL_TO_METERS(radius);
    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.density = 1.0f;

    b->CreateFixture(&fixture);

    pbody->body = b;
    pbody->width = pbody->height = radius;

    return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_dynamicBody;
    body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
    body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

    b2Body* b = world->CreateBody(&body);
    b2PolygonShape box;
    box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

    b2FixtureDef fixture;
    fixture.shape = &box;
    fixture.density = 1.0f;

    b->CreateFixture(&fixture);

    pbody->body = b;
    pbody->width = (int)(width * 0.5f);
    pbody->height = (int)(height * 0.5f);

    return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_staticBody;
    body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
    body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

    b2Body* b = world->CreateBody(&body);

    b2PolygonShape box;
    box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

    b2FixtureDef fixture;
    fixture.shape = &box;
    fixture.density = 1.0f;
    fixture.isSensor = true;

    b->CreateFixture(&fixture);

    pbody->body = b;
    pbody->width = width;
    pbody->height = height;

    return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, const int* points, int size)
{
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_dynamicBody;
    body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
    body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

    b2Body* b = world->CreateBody(&body);

    b2ChainShape shape;
    b2Vec2* p = new b2Vec2[size / 2];

    for (int i = 0; i < size / 2; ++i)
    {
        p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
        p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
    }

    shape.CreateLoop(p, size / 2);

    b2FixtureDef fixture;
    fixture.shape = &shape;

    b->CreateFixture(&fixture);

    delete p;

    pbody->body = b;
    pbody->width = pbody->height = 0;

    return pbody;
}

bool ModulePhysics::CleanUp()
{
    LOG("Destroying physics world");

    delete world;
    world = nullptr;

    return true;
}

void PhysBody::GetPhysicPosition(int& x, int& y) const
{
    b2Vec2 pos = body->GetPosition();
    x = METERS_TO_PIXELS(pos.x);
    y = METERS_TO_PIXELS(pos.y);
}

float PhysBody::GetRotation() const
{
    return body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
    b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

    const b2Fixture* fixture = body->GetFixtureList();

    while (fixture != NULL)
    {
        if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
            return true;
        fixture = fixture->GetNext();
    }

    return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
    int ret = -1;

    b2RayCastInput input;
    b2RayCastOutput output;

    input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
    input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
    input.maxFraction = 1.0f;

    const b2Fixture* fixture = body->GetFixtureList();

    while (fixture != NULL)
    {
        if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
        {
            // do we want the normal ?

            float fx = (float)(x2 - x1);
            float fy = (float)(y2 - y1);
            float dist = sqrtf((fx * fx) + (fy * fy));

            normal_x = output.normal.x;
            normal_y = output.normal.y;

            return (int)(output.fraction * dist);
        }
        fixture = fixture->GetNext();
    }

    return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
    b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
    b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

    PhysBody* physA = (PhysBody*)dataA.pointer;
    PhysBody* physB = (PhysBody*)dataB.pointer;

    if (physA && physA->listener != NULL)
        physA->listener->OnCollision(physA, physB);

    if (physB && physB->listener != NULL)
        physB->listener->OnCollision(physB, physA);
}