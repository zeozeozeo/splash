#pragma once
#include "Entity.hpp"

class Player : public Entity {
public:
    Player() : Entity{{0.f, 0.f, 1.f, 1.f}, WHITE} {};

    void update(float dt) override;
};
