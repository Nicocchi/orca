#include "orca.hpp"
#include "editor.hpp"

void drawSeekBar(AudioDevice *audioDevice, AudioInfo *audioInfo, App app)
{
  ImGui::SetNextWindowPos(ImVec2(0, app.height - 115));
  ImGui::BeginChild("seekBar", {static_cast<float>(app.width), 25});
  {
    ImGui::PushItemWidth(app.width);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.5f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.7f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, (ImVec4)ImColor::HSV(172.f, 245.f, 237.f, 200.f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.9f, 0.9f));
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_SliderThickness, 0.20f);
    ImGui::PushStyleVar(ImGuiStyleVar_SliderContrast, 0.7f);
    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::SliderFloat("##seekSlider", &audioInfo->currentSeconds, 0, audioInfo->maxSeconds, "", ImGuiSliderFlags_NoInput))
    {
      audioDevice->seek(audioInfo->currentSeconds);
    }
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(5);
    ImGui::EndChild();
  }
}
