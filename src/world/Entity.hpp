#pragma once
#include "Box2D/Box2D.h"
#include "raylib.h"

class Entity {
public:
    Entity(Rectangle rect, Color color) : m_rect{rect}, m_color{color} {};
    void draw();
    void update();
    void addToWorld(b2World& world, b2BodyType type, const Camera2D& camera,
                    float friction = 0.2f, float restitution = 0.f,
                    float density = 0.f);

    constexpr void setRotation(float rotation) {
        m_rotation = rotation;
    }

    constexpr Rectangle getRect() {
        return m_rect;
    }

    constexpr Vector2 position() {
        return {m_rect.x, m_rect.y};
    }

    Texture2D m_texture;

private:
    // bounds
    Rectangle m_rect;

    // color or tint
    Color m_color = BLACK;

    // rotation in degrees (note: box2d/liquidfun uses radians)
    float m_rotation = 0.f;

    b2Body* m_body = nullptr;
};
