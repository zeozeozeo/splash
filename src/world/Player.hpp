#pragma once
#include "Entity.hpp"

// uintptr_t because we store it in the sensors' userdata ptr
enum class SensorID : uintptr_t { NONE, HEAD, HITBOX, FEET, LEFT, RIGHT };

class Player : public Entity {
public:
    Player() : Entity{{0.f, 0.f, 1.f, 1.6f}, WHITE} {};

    void addToWorld(b2World& world, b2BodyType type, float friction = 0.2f,
                    float restitution = 0.f, float density = 0.f) override;
    void update(float dt) override;
    b2Fixture* createSensor(b2Vec2 center, float w, float h,
                            SensorID id = SensorID::NONE);
    void onContact(SensorID id);
};
