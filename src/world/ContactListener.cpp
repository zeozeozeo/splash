#include "ContactListener.hpp"
#include "Player.hpp"
#include "box2d/b2_contact.h"
#include "spdlog/spdlog.h"

void MyContactListener::BeginContact(b2Contact* contact) {
    auto fixtureA = contact->GetFixtureA();
    auto fixtureB = contact->GetFixtureB();
    auto sensorA = fixtureA->IsSensor();
    auto sensorB = fixtureB->IsSensor();

    // make sure only one of the fixtures was a sensor
    if (!(sensorA ^ sensorB))
        return;

    // determine which sensor was touched and tell that to the player
    if (sensorA)
        (static_cast<Player*>(fixtureA->GetBody()->GetUserData()))
            ->onContact(static_cast<SensorID>(
                reinterpret_cast<uintptr_t>(fixtureA->GetUserData())));
    else if (sensorB)
        (static_cast<Player*>(fixtureB->GetBody()->GetUserData()))
            ->onContact(static_cast<SensorID>(
                reinterpret_cast<uintptr_t>(fixtureA->GetUserData())));
}

void MyContactListener::EndContact(b2Contact* contact) {}
