#include "spawner.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Spawner)
{
    return cubos::core::ecs::TypeBuilder<Spawner>("Spawner")
        .withField("scene1", &Spawner::scene1)
        .withField("scene2", &Spawner::scene2)
        .withField("scene3", &Spawner::scene3)
        .withField("sceneRoot", &Spawner::sceneRoot)
        .withField("period", &Spawner::period)
        .withField("laneWidth", &Spawner::laneWidth)
        .withField("accumulator", &Spawner::accumulator)
        .build();
}

void spawnerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Spawner>();

    cubos.system("spawn stuff")
        .call([](Commands commands, const DeltaTime& dt, Assets& assets, Query<Spawner&, const Position&> spawners) {
            for (auto [spawner, position] : spawners)
            {
                spawner.accumulator += dt.value();
                if (spawner.accumulator >= spawner.period)
                {
                    spawner.accumulator -= spawner.period;

                    Position spawnPosition = position;
                    int offset = (rand() % 3) - 1;
                    spawnPosition.vec.x += static_cast<float>(offset) * spawner.laneWidth;

                    int scene = (rand() % 100);
                    if (scene < 85) {
                        commands.spawn(assets.read(spawner.scene1)->blueprint).add(spawner.sceneRoot, spawnPosition);
                    } else if (scene < 95) {
                        commands.spawn(assets.read(spawner.scene2)->blueprint).add(spawner.sceneRoot, spawnPosition);
                    } else {
                        commands.spawn(assets.read(spawner.scene3)->blueprint).add(spawner.sceneRoot, spawnPosition);
                    }
                }
            }
        });
}