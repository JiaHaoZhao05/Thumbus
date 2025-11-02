#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "ModuleAudio.h"
#include "ModuleGame.h"
#include "PhysicEntity.h"
#include "player.h"

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
    world->SetContactListener(this);

    // --- Create static ground ---
    b2BodyDef groundDef;
    groundDef.position.Set(0, 0);
    ground = world->CreateBody(&groundDef);

    // --- PLUNGER / SPRING BODY (rectï¿½ngulo vertical que se desliza) ---
    b2BodyDef springDef;
    springDef.type = b2_dynamicBody;
    springDef.position.Set(PIXELS_TO_METERS(463), PIXELS_TO_METERS(650));
    springDef.fixedRotation = true; // que no rote
    springBody = world->CreateBody(&springDef);

    b2PolygonShape springShape;
    springShape.SetAsBox(PIXELS_TO_METERS(10), PIXELS_TO_METERS(60)); // 20x120 pï¿½xeles

    b2FixtureDef springFixture;
    springFixture.shape = &springShape;
    springFixture.density = 1.0f;     // masa moderada para que pueda moverse
    springFixture.friction = 0.2f;
    springBody->CreateFixture(&springFixture);

    // --- Cuerpo estï¿½tico de anclaje superior ---
    b2BodyDef anchorDef;
    anchorDef.position.Set(PIXELS_TO_METERS(463), PIXELS_TO_METERS(590)); // punto fijo arriba
    b2Body* anchor = world->CreateBody(&anchorDef); 

    // --- Prismatic joint: solo movimiento vertical ---
    b2PrismaticJointDef prismaticDef;
    prismaticDef.Initialize(anchor, springBody, anchor->GetPosition(), b2Vec2(0.0f, 1.0f));
    prismaticDef.enableLimit = true;
    prismaticDef.lowerTranslation = 0.0f; // punto de reposo
    prismaticDef.upperTranslation = PIXELS_TO_METERS(80); // 80 px hacia abajo mï¿½ximo
    springPrismatic = (b2PrismaticJoint*)world->CreateJoint(&prismaticDef);
    springPrismatic->SetLimits(prismaticDef.lowerTranslation, prismaticDef.upperTranslation);


    return true;
}


update_status ModulePhysics::PreUpdate()
{
    world->Step(1.0f / 60.0f, 6, 2);

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

    //for (b2Contact* contact = world->GetContactList(); contact; contact = contact->GetNext())
    for (b2Contact* contact = world->GetContactList(); contact; contact = contact->GetNext())
    {
        if (contact->GetFixtureA()->IsSensor() && contact->IsTouching())
        {
            b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
            b2BodyUserData dataB = contact->GetFixtureA()->GetBody()->GetUserData();
            PhysBody* physA = (PhysBody*)dataA.pointer;
            PhysBody* physB = (PhysBody*)dataB.pointer;
            if (physA && physB && physA->listener)
                physA->listener->OnCollision(physA, physB);
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
    //Debug change parameters WS to select AD to mod
    if (IsKeyPressed(KEY_W)) {
        App->renderer->selector--;
        if (App->renderer->selector <= -1) {
            App->renderer->selector = 2;
        }

    }
    else if (IsKeyPressed(KEY_S)) {
        App->renderer->selector++;
        if (App->renderer->selector >= 3) {
            App->renderer->selector = 0;
        }
    }
    if (IsKeyPressed(KEY_A)) { 
        if (App->renderer->selector == 0) { //reduce gravity
            b2Vec2 temp = App->physics->world->GetGravity();
            temp.y -= 0.5f;
            App->physics->world->SetGravity(temp);
        }
        if (App->renderer->selector == 1) { //reduce friction
            App->player->friction -= 0.5f;
            if (App->player->friction < 0) App->player->friction = 0;
            App->player->ModedBallFriction(App->player->friction);
        }
        if (App->renderer->selector == 2) { //reduce fps
            int temp = GetFPS();
            temp -= 5;
            if (temp < 5) temp = 5;
            SetTargetFPS(temp);
        }
    }
    else if (IsKeyPressed(KEY_D)) {
        if (App->renderer->selector == 0) { //increase gravity
            b2Vec2 temp = App->physics->world->GetGravity();
            temp.y += 0.5f;
            App->physics->world->SetGravity(temp);
        }
        if (App->renderer->selector == 1) { //increase friction
            App->player->friction += 0.5f;
            App->player->ModedBallFriction(App->player->friction);
        }
        if (App->renderer->selector == 2) { //increase fps
            int temp = GetFPS();
            temp += 5;
            SetTargetFPS(temp);
        }
    }

    return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateBall(int x, int y, int radius, float friction)
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
    fixture.friction = friction;

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

    int rotationfactor2 = 30;
    int rotationfactor = rotationfactor2 + 5;

    // Create paddle shape and fixture
    b2PolygonShape paddleShape;
    paddleShape.SetAsBox(PIXELS_TO_METERS(50), PIXELS_TO_METERS(10));

    b2FixtureDef paddleFixture;
    paddleFixture.shape = &paddleShape;
    paddleFixture.density = density;
    paddleFixture.friction = friction;

    // Create paddle body
    b2BodyDef Def;
    Def.type = b2_dynamicBody;

    if (id == 2) {
        // Right flipper: shift left from anchor
        Def.position.Set(PIXELS_TO_METERS(x + width/2), PIXELS_TO_METERS(y));
    }
    else {
        // Left flipper: shift right from anchor
        Def.position.Set(PIXELS_TO_METERS(x -  width/2), PIXELS_TO_METERS(y));
    }

    b2Body* Paddle = world->CreateBody(&Def);
    Paddle->CreateFixture(&paddleFixture);

    // Create joint
    b2RevoluteJointDef JointDef;
    JointDef.bodyA = ground;
    JointDef.bodyB = Paddle;

    

    if (id == 2) {   // Right flipper: pivot on RIGHT end of paddle
        JointDef.localAnchorA.Set(PIXELS_TO_METERS(353), PIXELS_TO_METERS(y));
        JointDef.localAnchorB.Set(PIXELS_TO_METERS(50), 0);
        JointDef.lowerAngle = -rotationfactor * DEGTORAD;
        JointDef.upperAngle = rotationfactor2 * DEGTORAD;
    }
    else {         // Left flipper: pivot on LEFT end of paddle
        JointDef.localAnchorA.Set(PIXELS_TO_METERS(140), PIXELS_TO_METERS(y));
        JointDef.localAnchorB.Set(-PIXELS_TO_METERS(50), 0);
        JointDef.lowerAngle = -rotationfactor2 * DEGTORAD;
        JointDef.upperAngle = rotationfactor * DEGTORAD;
    }


    JointDef.enableLimit = true;
    JointDef.enableMotor = true;
    JointDef.maxMotorTorque = 100.0f;

    b2RevoluteJoint* joint = (b2RevoluteJoint*)world->CreateJoint(&JointDef);
    joint->EnableMotor(false);

    pbody->body = Paddle;
    pbody->joint = joint;

    return pbody;
}

PhysBody* ModulePhysics::CreateSpring(int height, int width, float density, float friction, int x, int y) {
    PhysBody* pbody = new PhysBody;
    
    b2BodyDef springDef;
    springDef.type = b2_dynamicBody;
    springDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
    springDef.fixedRotation = true; // que no rote
    springBody = world->CreateBody(&springDef);

    b2PolygonShape springShape;
    springShape.SetAsBox(PIXELS_TO_METERS(height), PIXELS_TO_METERS(width)); // 20x120 píxeles

    b2FixtureDef springFixture;
    springFixture.shape = &springShape;
    springFixture.density = density;     // masa moderada para que pueda moverse
    springFixture.friction = friction;
    springBody->CreateFixture(&springFixture);

    // --- Cuerpo estático de anclaje superior ---
    b2BodyDef anchorDef;
    anchorDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y-60)); // punto fijo arriba
    b2Body* anchor = world->CreateBody(&anchorDef);

    // --- Prismatic joint: solo movimiento vertical ---
    b2PrismaticJointDef prismaticDef;
    prismaticDef.Initialize(anchor, springBody, anchor->GetPosition(), b2Vec2(0.0f, 1.0f));
    prismaticDef.enableLimit = true;
    prismaticDef.lowerTranslation = 0.0f; // punto de reposo
    prismaticDef.upperTranslation = PIXELS_TO_METERS(80); // 80 px hacia abajo máximo
    springPrismatic = (b2PrismaticJoint*)world->CreateJoint(&prismaticDef);
    springPrismatic->SetLimits(prismaticDef.lowerTranslation, prismaticDef.upperTranslation);

    pbody->body = springBody;
    pbody->prismaticJoint = springPrismatic;

    return pbody;
}


PhysBody* ModulePhysics::CreateDeathZone()
{
    int x = 250;
    int y = 675;
    int width = 500;
    int height = 300;
    PhysBody* pbody = new PhysBody();

    b2BodyDef body;
    body.type = b2_dynamicBody;
    body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
    body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

    b2Body* b = world->CreateBody(&body);
    b2PolygonShape shape;
    shape.SetAsBox(width, height);
    b2FixtureDef fixture;
    fixture.shape = &shape;
    fixture.density = 0.0f;
    fixture.isSensor = true;

    b->CreateFixture(&fixture);

    pbody->body = b;
    pbody->width = width ;
    pbody->height = height ;

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
    LOG("CONTACT");
    b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
    b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

    PhysBody* physA = (PhysBody*)dataA.pointer;
    PhysBody* physB = (PhysBody*)dataB.pointer;

    if (physA && physA->listener != NULL)
        physA->listener->OnCollision(physA, physB);
    if (physB && physB->listener != NULL)
        physB->listener->OnCollision(physB, physA);


    PhysBody* ball = App->player->ball->physBody;
    for (auto& pEntity : App->scene_intro->entities) {
        if (physA == pEntity->physBody && physB == ball) {
            
            if (pEntity->type == 1) { //check bumpers
                App->player->currentScore += 75;
                pEntity->isSwitched = true;
                App->audio->PlayFx(App->scene_intro->bumperFX);
            }
            if (pEntity->type == 2) { //check triangles
                App->player->currentScore += 50;
                pEntity->isSwitched = true;
                App->audio->PlayFx(App->scene_intro->bumperFX);
            }
            if (pEntity->type == 3) { //check background

            }
        }
    }
    /*
    PhysBody* physB = (PhysBody*)dataB.pointer;

    if (physA && physA->listener != NULL)
        physA->listener->OnCollision(physA, physB);
    if (physB && physB->listener != NULL)
        physB->listener->OnCollision(physB, physA);
  

    p2List_item<PhysBody*>* c = App->scene_intro->balls.getFirst();
    while (c != NULL)
   // if (physA == App->scene_intro->ball && physB == App->scene_intro->ball)
      //  physA->listener->OnCollision(physA, physB);

    if (physB && physB->listener != NULL)
        physB->listener->OnCollision(physB, physA);
        */
}