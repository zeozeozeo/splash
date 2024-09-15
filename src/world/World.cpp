#include "World.hpp"
#include "Consts.hpp"
#include "Player.hpp"
#include <string>

World::World(float width, float height)
    : m_b2({0.f, 9.81f * 4.f}), m_player{}, m_debugDrawer{1.f} {
    updateCamera(width, height);

    // load assets
    m_steamhappy = LoadTexture("resources/steamhappy.png");

    // add floor
    Entity floor{{-20.f, 10.f, 80.f, 1.f}, WHITE};
    floor.addToWorld(m_b2, b2_staticBody);
    m_entities.push_back(std::move(floor));

    // add player
    m_player.addToWorld(m_b2, b2_kinematicBody);

    // debug drawer
    m_debugDrawer.SetAllFlags();
    m_b2.SetDebugDraw(&m_debugDrawer);
}

float lastSpawned = 0.f;

void World::update(float dt) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        GetTime() - lastSpawned > 0.1f) {
        auto pos = GetScreenToWorld2D(GetMousePosition(), m_camera);
        // m_camera.target = pos;

        Entity box{{pos.x, pos.y, 1.f, 1.f}, WHITE};
        box.m_texture = m_steamhappy;
        box.addToWorld(m_b2, b2_dynamicBody, 0.2f, 0.f, 0.5f);
        m_entities.push_back(std::move(box));
        lastSpawned = GetTime();
    }

    m_player.update(dt);

    // update physics world
    // we don't sync up entities here to avoid iterating over them twice
    m_b2.Step(dt, 8, 3);

    // we must extract the player state from the physics world
    m_player.syncWithPhysicsWorld();
    updateCamera(GetScreenWidth(), GetScreenHeight());
}

void World::updateCamera(float screenWidth, float screenHeight) {
    m_camera.offset = {screenWidth / 2.f, screenHeight / 2.f};
    m_camera.target = m_player.position();

    // Box2D uses meters and will go crazy if we measure everything in pixels
    // https://box2d.org/posts/2011/12/pixels/
    // https://seanballais.com/blog/box2d-and-the-pixel-per-meter-ratio/
    m_camera.zoom = screenHeight / HEIGHT_COEFF;
    // m_camera.zoom = screenWidth / WIDTH_COEFF;
}

void World::draw() {
    BeginMode2D(m_camera);
    for (auto& entity : m_entities) {
        entity.syncWithPhysicsWorld();
        entity.draw();
    }

    m_player.draw();
    m_b2.DebugDraw();

    auto mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_camera);
    auto text = std::to_string(static_cast<int>(mouseWorldPos.x)) + ", " +
                std::to_string(static_cast<int>(mouseWorldPos.y));

    EndMode2D();
    DrawFPS(10, 10);
    DrawText(text.c_str(), 10, 30, 20, WHITE);
}
