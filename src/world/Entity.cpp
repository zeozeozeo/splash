#include "Entity.hpp"

void Entity::draw() {
    if (m_texture.id != 0) {
        DrawTexturePro(m_texture,
                       {0.f, 0.f, static_cast<float>(m_texture.width),
                        static_cast<float>(m_texture.height)},
                       m_rect, {0.f, 0.f}, m_rotation, m_color);
    } else {
        DrawRectanglePro(m_rect, {0.f, 0.f}, m_rotation, m_color);
    }
}

void Entity::syncWithPhysicsWorld() {
    if (m_body == nullptr)
        return;

    auto pos =
        m_body->GetWorldPoint({-m_rect.width / 2.f, -m_rect.height / 2.f});
    m_rect.x = pos.x;
    m_rect.y = pos.y;
    m_rotation = RAD2DEG * m_body->GetAngle();
}

void Entity::addToWorld(b2World& world, b2BodyType type,
                        float friction, float restitution, float density) {
    b2BodyDef def;
    def.position.Set(m_rect.x + m_rect.width / 2.f,
                     m_rect.y + m_rect.height / 2.f);
    def.type = type;
    def.angle = DEG2RAD * m_rotation;

    // allocate body
    m_body = world.CreateBody(&def);
    assert(m_body != nullptr && "Failed to create body");

    if (m_body != nullptr) {
        // define the shape
        b2PolygonShape shape;
        // box extents are the half-widths of the rect
        shape.SetAsBox(m_rect.width / 2.f, m_rect.height / 2.f);

        // add a fixture to the body
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.friction = friction;
        fixtureDef.restitution = restitution;
        fixtureDef.density = density;
        m_body->CreateFixture(&fixtureDef);
    }
}
