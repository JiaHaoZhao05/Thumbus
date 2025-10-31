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
    b2Vec2 gravity(GRAVITY_X, GRAVITY_Y);
    world = new b2World(gravity);

    // --- Create static ground ---
    b2BodyDef groundDef;
    groundDef.position.Set(0, 0);
    ground = world->CreateBody(&groundDef);

    //// --- Paddle fixture ---
    //b2PolygonShape paddleShape;
    //paddleShape.SetAsBox(PIXELS_TO_METERS(50), PIXELS_TO_METERS(10)); // 100x20 pixels

    //b2FixtureDef paddleFixture;
    //paddleFixture.shape = &paddleShape;
    //paddleFixture.density = 5.0f;
    //paddleFixture.friction = 0.3f;

    //// --- LEFT PADDLE ---
    //b2BodyDef leftDef;
    //leftDef.type = b2_dynamicBody;
    //// Center of paddle is half-width to the right of pivot
    //leftDef.position.Set(PIXELS_TO_METERS(140 + 50), PIXELS_TO_METERS(600));
    //leftPaddle = world->CreateBody(&leftDef);
    //leftPaddle->CreateFixture(&paddleFixture);

    //b2RevoluteJointDef leftJointDef;
    //leftJointDef.bodyA = ground;
    //leftJointDef.bodyB = leftPaddle;

    //// Pivot at left side of paddle (base)
    //leftJointDef.localAnchorA.Set(PIXELS_TO_METERS(140), PIXELS_TO_METERS(600));
    //leftJointDef.localAnchorB.Set(-PIXELS_TO_METERS(50), 0); // relative to paddle center

    //leftJointDef.enableLimit = true;
    //leftJointDef.lowerAngle = -15 * DEGTORAD; // resting downward
    //leftJointDef.upperAngle = 30 * DEGTORAD;  // max upward flip

    //leftJointDef.enableMotor = true;
    //leftJointDef.maxMotorTorque = 100.0f;

    //leftJoint = (b2RevoluteJoint*)world->CreateJoint(&leftJointDef);


    // --- RIGHT PADDLE ---
    //b2BodyDef rightDef;
    //rightDef.type = b2_dynamicBody;
    //rightDef.position.Set(PIXELS_TO_METERS(360 - 50), PIXELS_TO_METERS(600)); // center half-width left
    //rightPaddle = world->CreateBody(&rightDef);
    //rightPaddle->CreateFixture(&paddleFixture);

    //b2RevoluteJointDef rightJointDef;
    //rightJointDef.bodyA = ground;
    //rightJointDef.bodyB = rightPaddle;

    //// Pivot at right side of paddle (base)
    //rightJointDef.localAnchorA.Set(PIXELS_TO_METERS(360), PIXELS_TO_METERS(600));
    //rightJointDef.localAnchorB.Set(PIXELS_TO_METERS(50), 0);

    //rightJointDef.enableLimit = true;
    //rightJointDef.lowerAngle = -30 * DEGTORAD; // upward flip
    //rightJointDef.upperAngle = 15 * DEGTORAD;  // resting downward

    //rightJointDef.enableMotor = true;
    //rightJointDef.maxMotorTorque = 100.0f;

    //rightJoint = (b2RevoluteJoint*)world->CreateJoint(&rightJointDef);

    //leftJoint->EnableMotor(false);
    //rightJoint->EnableMotor(false);

    // --- PLUNGER / SPRING BODY (rectángulo vertical que se desliza) ---
    b2BodyDef springDef;
    springDef.type = b2_dynamicBody;
    springDef.position.Set(PIXELS_TO_METERS(463), PIXELS_TO_METERS(650));
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
    anchorDef.position.Set(PIXELS_TO_METERS(463), PIXELS_TO_METERS(590)); // punto fijo arriba
    b2Body* anchor = world->CreateBody(&anchorDef);

    // --- Prismatic joint: solo movimiento vertical ---
    b2PrismaticJointDef prismaticDef;
    prismaticDef.Initialize(anchor, springBody, anchor->GetPosition(), b2Vec2(0.0f, 1.0f));
    prismaticDef.enableLimit = true;
    prismaticDef.lowerTranslation = 0.0f; // punto de reposo
    prismaticDef.upperTranslation = PIXELS_TO_METERS(80); // 80 px hacia abajo máximo
    springPrismatic = (b2PrismaticJoint*)world->CreateJoint(&prismaticDef);
    springPrismatic->SetLimits(prismaticDef.lowerTranslation, prismaticDef.upperTranslation);


    return true;
}


update_status ModulePhysics::PreUpdate()
{
    world->Step(1.0f / 60.0f, 6, 2);

    //// LEFT PADDLE
    //if (IsKeyDown(KEY_LEFT))
    //{
    //    leftJoint->EnableMotor(false);
    //    if (IsKeyPressed(KEY_LEFT))
    //        leftPaddle->ApplyAngularImpulse(-40.0f, true); // flip upward
    //}
    //else
    //{
    //    float leftAngle = leftPaddle->GetAngle();
    //    float leftTarget = -30 * DEGTORAD;
    //    float leftSpeed = -(leftTarget - leftAngle) * 12.0f;
    //    leftJoint->EnableMotor(true);
    //    leftJoint->SetMotorSpeed(leftSpeed);
    //    leftJoint->SetMaxMotorTorque(50.0f);
    //}

    //// RIGHT PADDLE
    //if (IsKeyDown(KEY_RIGHT))
    //{
    //    rightJoint->EnableMotor(false);
    //    if (IsKeyPressed(KEY_RIGHT))
    //        rightPaddle->ApplyAngularImpulse(40.0f, true); // flip upward
    //}
    //else
    //{
    //    float rightAngle = rightPaddle->GetAngle();
    //    float rightTarget = 30 * DEGTORAD;
    //    float rightSpeed = -(rightTarget - rightAngle) * 12.0f;
    //    rightJoint->EnableMotor(true);
    //    rightJoint->SetMotorSpeed(rightSpeed);
    //    rightJoint->SetMaxMotorTorque(50.0f);
    //}

    //SPRING CONTROL -> KEYDOWN
    float currentTranslation = springPrismatic->GetJointTranslation();
    static bool wasKeyDown = false;

    if (IsKeyDown(KEY_DOWN)){
        wasKeyDown = true;

        springPrismatic->EnableMotor(true);

        if (currentTranslation < springPrismatic->GetUpperLimit()){
            springPrismatic->SetMotorSpeed(5.0f);     // positive = move down on screen
            springPrismatic->SetMaxMotorForce(10.0f);
        }
        else{
            springPrismatic->SetMotorSpeed(0.0f);
        }
    }
    else if (wasKeyDown && IsKeyUp(KEY_DOWN)){
        wasKeyDown = false;

        float compression = fabs(currentTranslation - springPrismatic->GetLowerLimit()); //Erik you need the difference that was just its current position
        float k = 15.0f; // constante elastica del resorte (ajustable)
        float force = -k * compression; //raylib negative == up

        springPrismatic->EnableMotor(false);

        // aplicar impulso hacia arriba
        springBody->ApplyLinearImpulseToCenter(b2Vec2(0.0f, force), true);
    }
    else { //si no estas haciendo nada, gravedad mueve el muelle, esto hace que vuelva a su posicion inicial, no lo quites Erik
        if (currentTranslation > PIXELS_TO_METERS(0.5f)) {
            springPrismatic->EnableMotor(true);
            springPrismatic->SetMotorSpeed(-10.0f);  // negative = move up on screen
            springPrismatic->SetMaxMotorForce(150.0f);
        }
        else {
            springPrismatic->EnableMotor(false);
        }
    }
    return UPDATE_CONTINUE;
}



update_status ModulePhysics::PostUpdate()
{
    if (IsKeyPressed(KEY_F1))
        debug = !debug;

    if (!debug)
        return UPDATE_CONTINUE;

    //// --- Draw paddles ---
    //b2Vec2 lPos = leftPaddle->GetPosition();
    //float lAngle = leftPaddle->GetAngle() * RADTODEG;

    //b2Vec2 rPos = rightPaddle->GetPosition();
    //float rAngle = rightPaddle->GetAngle() * RADTODEG;


    // Bonus code: this will iterate all objects in the world and draw the circles
    // You need to provide your own macro to translate meters to pixels
    b2Body* body_clicked = nullptr;
    b2Vec2 mouse_position = b2Vec2{ PIXEL_TO_METERS(GetMousePosition().x), PIXEL_TO_METERS(GetMousePosition().y) };
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
        {
            switch (f->GetType())
            {
                // Draw circles ------------------------------------------------
            case b2Shape::e_circle:
            {
                b2CircleShape* shape = (b2CircleShape*)f->GetShape();
                b2Vec2 pos = f->GetBody()->GetPosition();

                DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{ 0, 0, 0, 128 });
            }
            break;

            // Draw polygons ------------------------------------------------
            case b2Shape::e_polygon:
            {
                b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
                int32 count = polygonShape->m_count;
                b2Vec2 prev, v;

                for (int32 i = 0; i < count; ++i)
                {
                    v = b->GetWorldPoint(polygonShape->m_vertices[i]);
                    if (i > 0)
                        DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);

                    prev = v;
                }

                v = b->GetWorldPoint(polygonShape->m_vertices[0]);
                DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);
            }
            break;

            // Draw chains contour -------------------------------------------
            case b2Shape::e_chain:
            {
                b2ChainShape* shape = (b2ChainShape*)f->GetShape();
                b2Vec2 prev, v;

                for (int32 i = 0; i < shape->m_count; ++i)
                {
                    v = b->GetWorldPoint(shape->m_vertices[i]);
                    if (i > 0)
                        DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
                    prev = v;
                }

                v = b->GetWorldPoint(shape->m_vertices[0]);
                DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
            }
            break;

            // Draw a single segment(edge) ----------------------------------
            case b2Shape::e_edge:
            {
                b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
                b2Vec2 v1, v2;

                v1 = b->GetWorldPoint(shape->m_vertex0);
                v1 = b->GetWorldPoint(shape->m_vertex1);
                DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), BLUE);
            }
            break;
            }
            //Select obj
            if (body_clicked == nullptr && mouse_joint == nullptr && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (f->TestPoint(mouse_position)) {
                    body_clicked = b;
                }

            }
        }
    }
    //Assign joint
    if (body_clicked != nullptr) {
        b2MouseJointDef def;
        def.bodyA = ground;
        def.bodyB = body_clicked;
        def.target = mouse_position;
        def.damping = 0.5f;
        def.stiffness = 2.0f;
        def.maxForce = 100.0f * body_clicked->GetMass();
        mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
    }
    //DESTROY joint
    if (mouse_joint != nullptr && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        mouse_joint->SetTarget(mouse_position);
        DrawLine(METERS_TO_PIXELS(mouse_joint->GetBodyB()->GetPosition().x), METERS_TO_PIXELS(mouse_joint->GetBodyB()->GetPosition().y), METERS_TO_PIXELS(mouse_position.x), METERS_TO_PIXELS(mouse_position.y), BLACK);
    }
    else if (mouse_joint != nullptr) {
        world->DestroyJoint(mouse_joint);
        mouse_joint = nullptr;
        body_clicked = nullptr;
    }

    return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateBall(int x, int y, int radius)
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
    fixture.density = 0.1f;

    b->CreateFixture(&fixture);

    pbody->body = b;
    pbody->width = pbody->height = radius;

    return pbody;
}


PhysBody* ModulePhysics::CreateBumper(int x, int y, int radius)
{
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_staticBody;
    body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
    body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

    b2Body* b = world->CreateBody(&body);

    b2CircleShape shape;
    shape.m_radius = PIXEL_TO_METERS(radius);
    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.density = 1.0f;
    fixture.restitution = 1, 1;

    b->CreateFixture(&fixture);

    pbody->body = b;
    pbody->width = pbody->height = radius;

    return pbody;
}

PhysBody* ModulePhysics::CreateFlipper(int height, int width, float density, float friction, int x, int y, int id) {
    PhysBody* pbody = new PhysBody();

    b2PolygonShape paddleShape;
    paddleShape.SetAsBox(PIXELS_TO_METERS(width / 2), PIXELS_TO_METERS(height / 2));

    b2FixtureDef paddleFixture;
    paddleFixture.shape = &paddleShape;
    paddleFixture.density = density;
    paddleFixture.friction = friction;

    b2BodyDef Def;
    Def.type = b2_dynamicBody;
    Def.position.Set(PIXELS_TO_METERS(x + width / 2), PIXELS_TO_METERS(y));
    b2Body* Paddle = world->CreateBody(&Def);
    Paddle->CreateFixture(&paddleFixture);

    b2RevoluteJointDef JointDef;
    JointDef.bodyA = ground;
    JointDef.bodyB = Paddle;
    JointDef.localAnchorA.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
    JointDef.localAnchorB.Set(-PIXELS_TO_METERS(width / 2), 0);

    JointDef.enableLimit = true;
    JointDef.lowerAngle = -15 * DEGTORAD;
    JointDef.upperAngle = 30 * DEGTORAD;

    JointDef.enableMotor = true;
    JointDef.maxMotorTorque = 100.0f;

    b2RevoluteJoint* joint = (b2RevoluteJoint*)world->CreateJoint(&JointDef);

    pbody->body = Paddle;
    return pbody;
}


PhysBody* ModulePhysics::CreateDeathZone()
{
    int x = 250;
    int y = 650;
    int radius = 20;
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
    fixture.isSensor = true;

    b->CreateFixture(&fixture);

    pbody->body = b;
    pbody->width = pbody->height = radius;

    return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, const int* points, int size)
{
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_staticBody;
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

PhysBody* ModulePhysics::CreateChainTriangle(int x, int y, const int* points, int size)
{
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_staticBody;
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
    fixture.restitution = 0.9, 1;

    b->CreateFixture(&fixture);

    delete p;

    pbody->body = b;
    pbody->width = pbody->height = 0;

    return pbody;
}

PhysBody* ModulePhysics::CreateChainSensor(int x, int y, const int* points, int size)
{
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_staticBody;
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
    fixture.isSensor = true;

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

  
  //  if (physA == App->scene_intro->deathZone && physA == App->scene_intro->ball)
      //  physA->listener->OnCollision(physA, physB);

    if (physB && physB->listener != NULL)
        physB->listener->OnCollision(physB, physA);
}