#include "Player.hpp"

void Player::update(float dt) {
    if (m_body == nullptr)
        return;

    b2Vec2 vel{0.f, 0.f};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE))
        vel.y = -5000.f * dt;
    if (IsKeyDown(KEY_S))
        vel.y = 5000.f * dt;
    if (IsKeyDown(KEY_A))
        vel.x = -5000.f * dt;
    if (IsKeyDown(KEY_D))
        vel.x = 5000.f * dt;

    m_body->SetLinearVelocity(vel);
}

void Player::createSensor() {
    
}
