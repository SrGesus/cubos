#include "powerup.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Powerup)
{
    return cubos::core::ecs::TypeBuilder<Powerup>("Powerup")
        .withField("velocity", &Powerup::velocity)
        .withField("angularVelocity", &Powerup::angularVelocity)
        .withField("killZ", &Powerup::killZ)
        .build();
}

void powerupPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Powerup>();

    cubos.system("move powerups")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, const Powerup&, Position&, Rotation&> powerups) {
            for (auto [ent, powerup, position, rotation] : powerups)
            {
                position.vec += powerup.velocity * dt.value();
                rotation.quat = glm::angleAxis(powerup.angularVelocity * dt.unscaledValue, glm::vec3{0.0, 1.0, 0.0}) * rotation.quat;
                
                if (position.vec.z < powerup.killZ)
                {
                    cmds.destroy(ent);
                }
            }
        });
}
