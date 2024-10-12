#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/tools/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

#include <cubos/engine/tools/play_pause/plugin.hpp>

#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>

#include "obstacle.hpp"
#include "powerup.hpp"
#include "player.hpp"
#include "spawner.hpp"
#include "jetpack.hpp"
#include "armor.hpp"

using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("ee5bb451-05b7-430f-a641-a746f7009eef");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("101da567-3d23-46ae-a391-c10ec00e8718");
static const Asset<InputBindings> InputBindingsAsset = AnyAsset("b20900a4-20ee-4caa-8830-14585050bead");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(toolsPlugin);

    cubos.plugin(playerPlugin);
    cubos.plugin(spawnerPlugin);
    cubos.plugin(obstaclePlugin);
    cubos.plugin(powerupPlugin);
    cubos.plugin(jetpackPlugin);
    cubos.plugin(armorPlugin);

    cubos.startupSystem("configure settings").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("set the palette, environment, input bindings and spawn the scene")
        .tagged(assetsTag)
        .call([](Commands commands, const Assets& assets, RenderPalette& palette, Input& input,
                 RenderEnvironment& environment) {
            palette.asset = PaletteAsset;
            environment.ambient = {0.1F, 0.1F, 0.1F};
            environment.skyGradient[0] = {0.2F, 0.4F, 0.8F};
            environment.skyGradient[1] = {0.6F, 0.6F, 0.8F};
            input.bind(*assets.read(InputBindingsAsset));
            commands.spawn(assets.read(SceneAsset)->blueprint);
        });

    cubos.system("restart the game on input")
        .call([](Commands cmds, const Assets& assets, const Input& input, Query<Entity> all) {
            if (input.justPressed("restart"))
            {
                for (auto [ent] : all)
                {
                    cmds.destroy(ent);
                }

                cmds.spawn(assets.read(SceneAsset)->blueprint);
            }
        });

    cubos.system("speedup").call([](DeltaTime& dt) {
        if (dt.scale > 0)
            dt.scale += 1 / dt.scale * dt.value() / 100;
    });

    cubos.system("detect unarmored player vs obstacles collisions")
        .with<Player>()
        .without<Armor>()
        .without<Jetpack>()
        .call([](Commands cmds, DeltaTime& dt, const Assets& assets, Query<const Player&, const CollidingWith&, const Obstacle&> collisions, Query<Entity> all) {
            for (auto [player, collidingWith, obstacle] : collisions)
            {
                CUBOS_INFO("Unarmored player collision!");
                for (auto [ent] : all)
                {
                    cmds.destroy(ent);
                }
                dt.scale = 1.0F;
                cmds.spawn(assets.read(SceneAsset)->blueprint);
                (void)player; // here to shut up 'unused variable warning', you can remove it
            }
        });

    cubos.observer("add jetpack asset")
        .onAdd<Jetpack>()
        .without<Armor>()
        .call([](Commands cmds, Query<Entity, const Player&, RenderVoxelGrid&> players) {
            for (auto [playerEntity, player, grid] : players)
            {
                CUBOS_INFO("Loading jetpack asset...");
                grid.asset = AnyAsset("c7263b46-be18-47c2-b3ef-05592b2e9dec");
                cmds.add(playerEntity, LoadRenderVoxels{});
            } 
        });

    cubos.observer("add jetpack armor asset")
        .onAdd<Jetpack>()
        .with<Armor>()
        .call([](Commands cmds, Query<Entity, const Player&, RenderVoxelGrid&> players) {
            for (auto [playerEntity, player, grid] : players)
            {
                CUBOS_INFO("Loading jetpack-armor asset...");
                grid.asset = AnyAsset("4892c2f3-10b3-4ca7-9de3-822b77a0ba7f");
                cmds.add(playerEntity, LoadRenderVoxels{});
            } 
        });


    cubos.observer("remove jetpack asset")
        .onRemove<Jetpack>()
        .without<Armor>()
        .call([](Commands cmds, Query<Entity, const Player&, RenderVoxelGrid&> players) {
            for (auto [playerEntity, player, grid] : players)
            {
                CUBOS_INFO("Removing jetpack asset...");
                grid.asset = AnyAsset("57d1b886-8543-4b8b-8f78-d911e9c4f896");
                cmds.add(playerEntity, LoadRenderVoxels{});
            } 
        });

    cubos.observer("remove jetpack armor asset")
        .onRemove<Jetpack>()
        .with<Armor>()
        .call([](Commands cmds, Query<Entity, const Player&, RenderVoxelGrid&> players) {
            for (auto [playerEntity, player, grid] : players)
            {
                CUBOS_INFO("Removing jetpack-armor asset...");
                grid.asset = AnyAsset("4892c2f3-10b3-4ca7-9de3-822b77a0ba7e");
                cmds.add(playerEntity, LoadRenderVoxels{});
            } 
        });

    cubos.observer("add armor asset")
        .onAdd<Armor>()
        .call([](Commands cmds, Query<Entity, const Player&, RenderVoxelGrid&> players) {
            for (auto [playerEntity, player, grid] : players)
            {
                CUBOS_INFO("Loading armor asset...");
                grid.asset = AnyAsset("4892c2f3-10b3-4ca7-9de3-822b77a0ba7e");
                cmds.add(playerEntity, LoadRenderVoxels{});
            } 
        });

    cubos.observer("remove armor asset")
        .onRemove<Armor>()
        .call([](Commands cmds, Query<Entity, const Player&, RenderVoxelGrid&> players) {
            for (auto [playerEntity, player, grid] : players)
            {
                CUBOS_INFO("Removing armor asset...");
                grid.asset = AnyAsset("4892c2f3-10b3-4ca7-9de3-822b77a0ba7e");
                // cmds.add(playerEntity, LoadRenderVoxels{});
                (void)player;
            } 
            (void )cmds;
        });

    cubos.run();
}
