#include "ui.hpp"
#include "renderer.hpp"
#include <imgui.h>
namespace {
constexpr ImVec4 accent{.38f, .78f, .94f, 1}, muted{.61f, .69f, .78f, 1};
void focus(Scene &s, Camera &c, int i) {
  s.selected = i;
  c.free = false;
  c.distance = s.focusDistance(i);
}
} // namespace
void UI::style() {
  ImGui::StyleColorsDark();
  auto &st = ImGui::GetStyle();
  st.WindowRounding = 12;
  st.FrameRounding = 5;
  st.GrabRounding = 5;
  st.WindowPadding = {18, 16};
  st.FramePadding = {10, 7};
  st.ItemSpacing = {9, 9};
  st.WindowBorderSize = 1;
  st.Colors[ImGuiCol_WindowBg] = {.025f, .041f, .065f, .96f};
  st.Colors[ImGuiCol_Border] = {.13f, .20f, .27f, 1};
  st.Colors[ImGuiCol_Button] = {.07f, .13f, .19f, 1};
  st.Colors[ImGuiCol_ButtonHovered] = {.12f, .30f, .39f, 1};
  st.Colors[ImGuiCol_ButtonActive] = {.13f, .38f, .49f, 1};
  st.Colors[ImGuiCol_CheckMark] = accent;
  st.Colors[ImGuiCol_SliderGrab] = accent;
  st.Colors[ImGuiCol_Text] = {.90f, .94f, .98f, 1};
  st.Colors[ImGuiCol_TextDisabled] = muted;
}
void UI::draw(Scene &s, Camera &c, int w, int h, float fps) {
  auto *fg = ImGui::GetForegroundDrawList();
  char perf[80];
  snprintf(perf, sizeof(perf), "%.0f FPS   /   %s", fps,
           c.free ? "FREE FLIGHT" : "ORBIT CAMERA");
  fg->AddText({float(w) - 270, 22}, IM_COL32(145, 220, 244, 255), perf);
  if (!visible)
    return;
  float panelWidth = w < 900 ? 250.f : 292.f;
  ImGui::SetNextWindowPos({18, 18}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({panelWidth, float(h) - 36}, ImGuiCond_Always);
  ImGui::Begin("Mission control", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove);
  ImGui::TextColored(accent, "SOLAR SYSTEM");
  ImGui::SetWindowFontScale(1.6f);
  ImGui::TextUnformatted("Explorer");
  ImGui::SetWindowFontScale(1);
  ImGui::TextColored(muted, "Interactive orbital observatory");
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();
  if (ImGui::Button("Overview  [0]", {-1, 32}))
    focus(s, c, -1);
  ImGui::TextColored(muted, "DESTINATIONS");
  for (int i = 0; i < 9; ++i) {
    char label[64];
    snprintf(label, sizeof(label), "%s  [%s]", Scene::bodies[i].name,
             i == 8 ? "9 / M" : std::to_string(i + 1).c_str());
    if (ImGui::Selectable(label, s.selected == i, 0, {0, 20}))
      focus(s, c, i);
  }
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::TextColored(muted, "SIMULATION");
  if (ImGui::Button(s.paused ? "Resume [Space]" : "Pause [Space]"))
    s.paused = !s.paused;
  ImGui::SameLine();
  if (ImGui::Button("Reset [R]")) {
    s.reset();
    c = Camera();
  }
  ImGui::SetNextItemWidth(panelWidth - 102);
  ImGui::SliderFloat("days/s", &s.speed, 0.f, 120.f, "%.1f");
  ImGui::Text("Elapsed  %.1f days", s.days);
  if (ImGui::CollapsingHeader("Rendering controls",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Checkbox("Earth day / night [N]", &s.night);
    ImGui::Checkbox("Saturn rings [G]", &s.rings);
    ImGui::Checkbox("Asteroid belt [B]", &s.asteroids);
    ImGui::Checkbox("Camera light [L]", &s.headlight);
    if (s.headlight)
      ImGui::SliderFloat("Intensity", &s.lightPower, .1f, 8.f);
    ImGui::Checkbox("Orbit paths [O]", &s.orbits);
    ImGui::Checkbox("Planet labels", &s.labels);
  }
  if (ImGui::Button(c.free ? "Orbit camera [F]" : "Free flight [F]",
                    {-1, 30})) {
    c.free = !c.free;
  }
  if (ImGui::CollapsingHeader("Controls and scale")) {
    ImGui::TextWrapped(
        "Right-drag to look. Scroll to zoom. Arrow keys orbit. WASD + Q/E fly "
        "in free mode. +/- time speed. H hides panel. Esc exits.");
    ImGui::TextWrapped(
        "Stylized sizes and distances. Circular coplanar orbits. Orbital "
        "periods are approximate. This is not an ephemeris.");
  }
  ImGui::End();
  if (s.selected >= 0) {
    auto &b = Scene::bodies[s.selected];
    ImGui::SetNextWindowPos({panelWidth + 40, float(h) - 116});
    ImGui::SetNextWindowSize(
        {std::max(250.f, float(w) - panelWidth - 330), 94});
    ImGui::SetNextWindowBgAlpha(.82f);
    ImGui::Begin("Destination", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
    ImGui::TextColored(accent, "%s", b.name);
    ImGui::TextWrapped("%s", b.description);
    ImGui::TextColored(muted, "Orbital period: ~%.2f days", b.period);
    ImGui::End();
  }
  if (s.labels) {
    auto vp = Renderer::projection(w, h) * c.view();
    for (int i = 0; i < 9; ++i) {
      auto q =
          vp * glm::vec4(s.position(i) +
                             glm::vec3(0, Scene::bodies[i].radius + .25f, 0),
                         1);
      if (q.w <= 0)
        continue;
      auto ndc = glm::vec3(q) / q.w;
      float x = (ndc.x * .5f + .5f) * w, y = (.5f - ndc.y * .5f) * h;
      if (ndc.z < 1 && x > panelWidth + 30 && x < w - 90 && y > 60 &&
          y < h - 130)
        fg->AddText({x, y}, IM_COL32(180, 205, 224, 220),
                    Scene::bodies[i].name);
    }
  }
}
