#pragma once

#include <glm/vec3.hpp>

#include <cubos/engine/prelude.hpp>

struct Powerup
{
    CUBOS_REFLECT;

    glm::vec3 velocity{0.0F, 0.0F, -1.0F};
    float angularVelocity{3.14F};
    float killZ{0.0F};
};

void powerupPlugin(cubos::engine::Cubos& cubos);
