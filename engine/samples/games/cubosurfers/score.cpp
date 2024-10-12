#include "score.hpp"

#include <imgui.h>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::engine;

#include "player.hpp"
#include "obstacle.hpp"

CUBOS_REFLECT_IMPL(Score)
{
    return cubos::core::ecs::TypeBuilder<Score>("Score")
        .withField("accumulator", &Score::accumulator)
        .build();
}
void scorePlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(settingsPlugin);

    cubos.component<Score>();

    cubos.observer("read highscore from settings")
        .onAdd<Score>()
        .call([](Settings& settings, Query<Score&> scores) {
            for (auto [score] : scores) {
                score.highscore = settings.getInteger("highscore", 0);
            }
        });

    cubos.system("show scores")
        .tagged(imguiTag)
        .call([](Settings& settings, DeltaTime& dt, Query<Score&> scores) {
            for (auto [score] : scores) {
                score.accumulator += dt.value();

                score.score = (int)(score.accumulator / 7.0F);

                ImGui::Begin("Score", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Score: %d", score.score);
                ImGui::Text("High Score: %d", settings.getInteger("highscore", 0));
                ImGui::End();
            }
        });
}