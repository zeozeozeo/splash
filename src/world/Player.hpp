#pragma once
#include "Entity.hpp"

constexpr uint8_t JUMP_LIMIT_RESET = 2;

// uintptr_t because we store it in the sensors' userdata ptr
enum class SensorID : uintptr_t { NONE, HEAD, HITBOX, FEET, LEFT, RIGHT };
constexpr int NUM_SENSOR_IDS = 6;

class Player : public Entity {
public:
    Player() : Entity{{0.f, 0.f, 1.f, 1.6f}, WHITE} {};

    void addToWorld(b2World& world, b2BodyType type, float friction = 0.2f,
                    float restitution = 0.f, float density = 0.f) override;
    bool canJump();
    void update(float dt) override;
    b2Fixture* createSensor(b2Vec2 center, float w, float h,
                            SensorID id = SensorID::NONE);
    void onContact(SensorID id);
    void onEndContact(SensorID id);

private:
    uint8_t m_jumpsLeft = JUMP_LIMIT_RESET;
};
