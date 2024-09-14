#include "World.hpp"
#include "Consts.hpp"
#include <string>

World::World(float width, float height) : m_b2({0.f, 9.81f * 4.f}) {
    updateCamera(width, height);

    // add floor
    Entity floor{{-20.f, 10.f, 80.f, 1.f}, WHITE};
    floor.addToWorld(m_b2, b2_staticBody, m_camera);
    m_entities.push_back(std::move(floor));
}

float lastSpawned = 0.f;

void World::update(float dt) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        GetTime() - lastSpawned > 0.1f) {
        auto pos = GetScreenToWorld2D(GetMousePosition(), m_camera);
        // m_camera.target = pos;

        Entity box{{pos.x, pos.y, 1.f, 1.f}, ORANGE};
        box.addToWorld(m_b2, b2_dynamicBody, m_camera, 0.2f, 1.0f, 1.f);
        m_entities.push_back(std::move(box));
        lastSpawned = GetTime();
    }

    // update camera
    updateCamera(GetScreenWidth(), GetScreenHeight());

    // update physics world
    // we don't sync up entities here to avoid iterating over them twice
    m_b2.Step(dt, 8, 3);
}

void World::updateCamera(float screenWidth, float screenHeight) {
    m_camera.offset = {screenWidth / 2.f, screenHeight / 2.f};

    // Box2D uses meters and will go crazy if we measure everything in pixels
    // https://box2d.org/posts/2011/12/pixels/
    // https://seanballais.com/blog/box2d-and-the-pixel-per-meter-ratio/
    m_camera.zoom = screenHeight / HEIGHT_COEFF;
    // m_camera.zoom = screenWidth / WIDTH_COEFF;
}

void World::draw() {
    BeginMode2D(m_camera);
    for (auto& entity : m_entities) {
        entity.update(); // sync up with physics world
        entity.draw();
    }

    auto mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_camera);
    auto text = std::to_string(static_cast<int>(mouseWorldPos.x)) + ", " +
                std::to_string(static_cast<int>(mouseWorldPos.y));

    EndMode2D();
    DrawText(text.c_str(), 10, 10, 20, WHITE);
}
