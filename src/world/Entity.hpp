#pragma once
#include "Box2D/Box2D.h"
#include "raylib.h"

class Entity {
public:
    Entity(Rectangle rect, Color color) : m_rect{rect}, m_color{color} {};
    void draw();
    void addToWorld(b2World& world, b2BodyType type);

    constexpr void setRotation(float rotation) {
        m_rotation = rotation;
    }

    constexpr Rectangle getRect() {
        return m_rect;
    }

    constexpr Vector2 position() {
        return {m_rect.x, m_rect.y};
    }

private:
    // bounds
    Rectangle m_rect;

    // color or tint
    Color m_color;

    // rotation in degrees (note: box2d/liquidfun uses radians)
    float m_rotation;

    b2Body* m_body;
};
