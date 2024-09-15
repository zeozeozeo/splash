#pragma once
#include "box2d/b2_world_callbacks.h"

class MyContactListener : public b2ContactListener {
private:
    // called when 2 fixtures begin to touch
    void BeginContact(b2Contact* contact) override;

    // called when 2 fixtures cease to touch
    void EndContact(b2Contact* contact) override;
};
