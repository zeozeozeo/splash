#include "Player.hpp"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "spdlog/spdlog.h"

void Player::addToWorld(b2World& world, b2BodyType type, float friction,
                        float restitution, float density) {
    b2BodyDef def;
    def.position.Set(m_rect.x + m_rect.width / 2.f,
                     m_rect.y + m_rect.height / 2.f);
    def.type = type;
    def.fixedRotation = true;
    // def.angle = DEG2RAD * m_rotation;

    // allocate body
    m_body = world.CreateBody(&def);
    assert(m_body != nullptr && "Failed to create body");
    if (m_body == nullptr)
        return;

    m_body->SetUserData(this);

    // define the shape, box extents are the half-widths of the rect
    b2PolygonShape shape;
    shape.SetAsBox(m_rect.width / 2.f, m_rect.height / 2.f);

    // add a fixture to the body
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = restitution;
    fixtureDef.density = 1.f;
    m_body->CreateFixture(&fixtureDef);

    // create sensors (total 6 fixtures): https://bhopkins.net/pages/mmphysics/
    // head
    createSensor({0.f, -m_rect.height / 2.f - 0.2f}, m_rect.width / 2.f, 0.2f,
                 SensorID::HEAD);
    // hitbox
    createSensor({0.f, 0.f}, m_rect.width / 2.f - 0.1f,
                 m_rect.height / 2.f - 0.1f, SensorID::HITBOX);
    // feet
    createSensor({0.f, m_rect.height / 2.f + 0.2f}, m_rect.width / 2.f, 0.4f,
                 SensorID::FEET);
    // left wall
    createSensor(
        {
            -m_rect.width / 2.f - 0.2f,
            0.f,
        },
        0.4f, m_rect.height / 2.f - 0.2f, SensorID::LEFT);
    // right wall
    auto sens = createSensor({m_rect.width / 2.f + 0.2f, 0.f}, 0.4f,
                             m_rect.height / 2.f - 0.2f, SensorID::RIGHT);
}

bool Player::canJump() {
    return m_jumpsLeft > 0;
}

void Player::update(float dt) {
    if (m_body == nullptr)
        return;

    b2Vec2 vel{m_body->GetLinearVelocity().x * dt * 133.f,
               m_body->GetLinearVelocity().y};
    if (canJump() && (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) ||
                      IsKeyPressed(KEY_SPACE))) {
        auto jumpStrength = 2400.f + (m_jumpsLeft * 300.f);
        vel.y = -jumpStrength * dt;
        --m_jumpsLeft;
    }
    if (IsKeyDown(KEY_A))
        vel.x = -3000.f * dt;
    if (IsKeyDown(KEY_D))
        vel.x = 3000.f * dt;

    m_body->SetLinearVelocity(vel);
}

b2Fixture* Player::createSensor(b2Vec2 center, float w, float h, SensorID id) {
    if (m_body == nullptr)
        return nullptr;

    b2PolygonShape shape;
    shape.SetAsBox(w, h, center, 0.f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true;
    // this is not actually a pointer, we're just storing the id in userdata so
    // we can retrieve it from the contact listener later
    fixtureDef.userData = reinterpret_cast<void*>(id);

    return m_body->CreateFixture(&fixtureDef);
}

void Player::onContact(SensorID id) {
    switch (id) {
    case SensorID::HEAD:
        spdlog::trace("HEAD");
        break;
    case SensorID::HITBOX:
        spdlog::trace("HITBOX");
        break;
    case SensorID::FEET:
        spdlog::trace("FEET");
        m_jumpsLeft = JUMP_LIMIT_RESET;
        break;
    case SensorID::LEFT:
        spdlog::trace("LEFT");
        break;
    case SensorID::RIGHT:
        spdlog::trace("RIGHT");
        break;
    case SensorID::NONE:
        spdlog::trace("NONE");
        break;
    }
}

void Player::onEndContact(SensorID id) {
    switch (id) {
    case SensorID::HEAD:
        spdlog::trace("END HEAD");
        break;
    case SensorID::HITBOX:
        spdlog::trace("END HITBOX");
        break;
    case SensorID::FEET:
        spdlog::trace("END FEET");
        break;
    case SensorID::LEFT:
        spdlog::trace("END LEFT");
        break;
    case SensorID::RIGHT:
        spdlog::trace("END RIGHT");
        break;
    case SensorID::NONE:
        spdlog::trace("END NONE");
        break;
    }
}
