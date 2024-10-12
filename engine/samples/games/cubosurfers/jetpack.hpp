#pragma once

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <cubos/engine/prelude.hpp>

struct Jetpack {
    CUBOS_REFLECT;

    float period{20.0F};                               // Time before buff expires in seconds
    float accumulator{0.0F};                          // Time accumulator
};

void jetpackPlugin(cubos::engine::Cubos& cubos);
