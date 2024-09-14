#include "World.hpp"

World::World(float width, float height) : m_b2({0.f, 10.f}) {
    m_camera = {};
    m_camera.offset = {width / 2.f, height / 2.f};
    m_camera.zoom = 0.5f;

    // add floor
    Entity floor{{width / 2.f - 1000.f, height - 200.f, 1000.f, 30.f}, WHITE};
    m_entities.push_back(std::move(floor));
}

void World::update(float dt) {}

void World::draw() {
    BeginMode2D(m_camera);
    for (auto& entity : m_entities) {
        entity.draw();
    }
    EndMode2D();
}
