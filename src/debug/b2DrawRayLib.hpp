#pragma once
#include "box2d/b2_draw.h"
#include "box2d/b2_math.h"
#include "raylib.h"

class b2DrawRayLib : public b2Draw {
public:
    explicit b2DrawRayLib(float scale = 1.0f) noexcept;
    void SetAllFlags() noexcept;
    void SetScale(float scale) noexcept;
    float GetScale() noexcept;
    void DrawPolygon(b2Vec2 const* vertices, int32 vertexCount,
                     b2Color const& color) noexcept;
    void DrawSolidPolygon(b2Vec2 const* vertices, int32 vertexCount,
                          b2Color const& color) noexcept;
    void DrawCircle(b2Vec2 const& center, float radius,
                    b2Color const& color) noexcept;
    void DrawSolidCircle(b2Vec2 const& center, float radius, b2Vec2 const& axis,
                         b2Color const& color) noexcept;
    void DrawParticles(const b2Vec2* centers, float radius,
                       const b2ParticleColor* colors, int32 count);
    void DrawSegment(b2Vec2 const& p1, b2Vec2 const& p2,
                     b2Color const& color) noexcept;
    void DrawTransform(b2Transform const& xf) noexcept;
    void DrawPoint(b2Vec2 const& p, float size, b2Color const& color) noexcept;

private:
    float M_ToPixels(float f) const noexcept;
    Vector2 M_ToPixels(b2Vec2 const& p) const noexcept;
    Color M_ConvertColor(b2ParticleColor const& color) const noexcept;
    Color M_ConvertColor(b2Color const& color) const noexcept;
    Color M_ConvertColor(b2Color const& color, float newAlpha) const noexcept;

    float m_scale;
};
