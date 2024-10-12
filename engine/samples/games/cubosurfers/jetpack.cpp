#include "jetpack.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>

using namespace cubos::engine;

#include "player.hpp"
#include "obstacle.hpp"

CUBOS_REFLECT_IMPL(Jetpack)
{
    return cubos::core::ecs::TypeBuilder<Jetpack>("Jetpack")
        .withField("period", &Jetpack::period)
        .withField("accumulator", &Jetpack::accumulator)
        .build();
}

void jetpackPlugin(Cubos& cubos)
{
    cubos.depends(playerPlugin);
    cubos.depends(obstaclePlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(cubos::engine::renderVoxelsPlugin);

    cubos.component<Jetpack>();

    cubos.system("detect player picking up jetpack")
        .call([](Commands cmds, DeltaTime& dt, Query<Entity, Player&, RenderVoxelGrid&, Rotation&, const CollidingWith&, Entity , const Jetpack&> collisions) {
            for (auto [playerEntity, player, playerVoxelGrid, rotation, collidingWith, jetpackEntity, jetpack] : collisions)
            {
                CUBOS_INFO("Player collided with an jetpack!");
                cmds.destroy(jetpackEntity);
                cmds.add(playerEntity, Jetpack{});
                dt.scale += 1.0F;
            }
        });

    cubos.system("jetpack fuel")
        .call([](Commands cmds, DeltaTime& dt, Query<Entity, Player&, RenderVoxelGrid&, Rotation&, Jetpack&> players) {
            for (auto [playerEntity, player, playerVoxelGrid, rotation, jetpack] : players)
            {
                jetpack.accumulator += dt.value();

                if (jetpack.accumulator < 1.0F) {
                    player.y = glm::sin(jetpack.accumulator * glm::pi<float>() / 2.0F) * 12.0F;
                    rotation.quat = glm::angleAxis(jetpack.accumulator * glm::pi<float>() / 3.0F, glm::vec3{1.0, 0.0, 0.0});
                } else if (jetpack.accumulator < jetpack.period - 1.0F) {
                    player.y = 12.0F + glm::sin((jetpack.accumulator - 1.0F) * glm::pi<float>()) / 10.0F;
                } else {
                    player.y = 12.0F - glm::sin((jetpack.accumulator - jetpack.period + 1.0F) * glm::pi<float>() / 2.0F) * 12.0F;
                }
                
                if (jetpack.accumulator > jetpack.period) {

                    player.y = 0.0;
                    dt.scale -= 1.2F;
                    cmds.remove<Jetpack>(playerEntity);

                    rotation.quat = glm::angleAxis(0.0F, glm::vec3{1.0, 0.0, 0.0});
                }
            }
        });
}
