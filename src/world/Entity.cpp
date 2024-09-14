#include "Entity.hpp"

void Entity::draw() {
    DrawRectanglePro(m_rect, {0.f, 0.f}, m_rotation, m_color);
}

void Entity::addToWorld(b2World& world, b2BodyType type) {
    b2BodyDef def;
    def.position.Set(m_rect.x, m_rect.y);
    def.type = type;
    def.angle = DEG2RAD * m_rotation;

    // allocate body
    m_body = world.CreateBody(&def);
    assert(m_body != nullptr && "Failed to create body");

    // define the shape
    b2PolygonShape shape;
    // box extents are the half-widths of the rect
    shape.SetAsBox(m_rect.width / 2.f, m_rect.height / 2.f);

    // add a fixture
    m_body->CreateFixture(
        &shape, type == b2_staticBody ? 0.f : 1.f /* TODO: density */);
}
