#include "armor.hpp"

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
#include "jetpack.hpp"

CUBOS_REFLECT_IMPL(Armor)
{
    return cubos::core::ecs::TypeBuilder<Armor>("Armor")
        .build();
}

void armorPlugin(Cubos& cubos)
{
    cubos.depends(playerPlugin);
    cubos.depends(obstaclePlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(jetpackPlugin);
    cubos.depends(cubos::engine::renderVoxelsPlugin);

    cubos.component<Armor>();

    cubos.system("detect player picking up armor")
        .call([](Commands cmds, Query<Entity, const Player&, RenderVoxelGrid&, const CollidingWith&, Entity , const Armor&> collisions) {
            for (auto [playerEntity, player, playerVoxelGrid, collidingWith, armorEntity, armor] : collisions)
            {
                CUBOS_INFO("Player collided with an armor!");
                cmds.destroy(armorEntity);
                cmds.add(playerEntity, Armor{});
            }
        });

    cubos.system("detect armored player vs obstacle collisions")
        .without<Jetpack>()
        .call([](Commands cmds, Query<Entity, const Player&,  RenderVoxelGrid&, const Armor&, const CollidingWith&, Entity, const Obstacle&> collisions) {
            for (auto [playerEntity, player, playerVoxelGrid, armor, collidingWith, obstacleEntity, obstacle] : collisions)
            {
                CUBOS_INFO("Armored player collision!");
                cmds.destroy(obstacleEntity);
                cmds.remove<Armor>(playerEntity);
                CUBOS_INFO("Armored player collision 2!");
            }
        });
}
