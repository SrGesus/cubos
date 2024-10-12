#pragma once

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/prelude.hpp>

struct Score {
    CUBOS_REFLECT;

    float accumulator{0.0F}; // Time accumulator
    int score{0};
    int highscore{0};
};

void scorePlugin(cubos::engine::Cubos& cubos);
