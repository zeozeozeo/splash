#pragma once
#include "../debug/b2DrawRayLib.hpp"
#include "ContactListener.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "raylib.h"
#include <vector>

class World {
public:
    World(float width, float height);
    void update(float dt);
    void draw();

private:
    Camera2D m_camera;
    std::vector<Entity> m_entities;
    Player m_player;
    // note: b2World takes about 1OOK of stack memory, because it
    // contains a stack allocator (and this makes this class extremely large).
    // shouldn't be a problem though
    b2World m_b2;
    Texture m_steamhappy;
    b2DrawRayLib m_debugDrawer;
    MyContactListener m_contactListener;

    void updateCamera(float screenWidth, float screenHeight);
};
