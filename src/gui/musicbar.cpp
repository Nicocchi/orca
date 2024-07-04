#include "orca.hpp"
#include "editor.hpp"
#include <random>
#include "audio.hpp"

void CircleImage(ImTextureID user_texture_id, float diameter, const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1), const ImVec4 &tint_col = ImVec4(1, 1, 1, 1))
{
  ImVec2 p_min = ImGui::GetCursorScreenPos();
  ImVec2 p_max = ImVec2(p_min.x + diameter, p_min.y + diameter);
  ImGui::GetWindowDrawList()->AddImageRounded(user_texture_id, p_min, p_max, uv0, uv1, ImGui::GetColorU32(tint_col), diameter * 0.5f);
  ImGui::Dummy(ImVec2(diameter, diameter));
}

void shuffleSong()
{
  auto rd = std::random_device{};
  auto rng = std::default_random_engine{rd()};
  std::shuffle(std::begin(audioFiles), std::end(audioFiles), rng);
}

bool compareInterval(AudioFile a1, AudioFile a2)
{
  return (a1.ID < a2.ID);
}

void resetShuffle()
{
  std::sort(audioFiles.begin(), audioFiles.end(), compareInterval);
}

void drawMusicBar(AudioDevice *audioDevice, AudioInfo *audioInfo, App app)
{
  // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{1.0f, 0.30f, 0.20f, 1.0f});
  ImGui::SetNextWindowPos(ImVec2(0, app.height - 90));
  ImGui::BeginChild("musicBar", {static_cast<float>(app.width), 90});
  {
    ImVec2 lastWinPos = ImVec2(app.width - 220, app.height - 90);
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.10f, 0.50f, 0.30f, 1.0f});
    // ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::BeginChild("infoMainWindow", {200, 90});
    {
      // TODO: Implement music info -> title, artist, album, album art

      //   ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.16f, 1.89f, 0.20f, 1.0f});
      ImGui::SetCursorPos(ImVec2(15, 0));
      ImGui::BeginChild("artWindow", ImVec2(70, 90));
      {
        ImGui::SetCursorPos(ImVec2(15, 20));
        CircleImage((void *)(intptr_t)audioInfo->currentSong.texture, 50.0f, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1));
        ImGui::EndChild();
      }
      //   ImGui::PopStyleColor();
      ///
      // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{1.20f, 0.20f, 1.30f, 1.0f});
      ImGui::SameLine();
      ImGui::BeginChild("infoWindow", {180, 90});
      {
        ImGui::SetCursorPos(ImVec2(5, 22));
        ImGui::SetWindowFontScale(0.6f);
        ImGui::Text("%s", audioInfo->currentSong.artist.c_str());
        ImGui::SetCursorPos(ImVec2(5, 45));
        ImGui::SetWindowFontScale(0.5f);
        ImGui::Text("%s", audioInfo->currentSong.title.c_str());
        ImGui::EndChild();
      }
      // ImGui::PopStyleColor();
      ///
      ///
      ImGui::EndChild();
    }
    // ImGui::PopStyleColor();
    ///
    ///
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.10f, 0.50f, 0.30f, 1.0f});
    ImGui::SameLine();
    ImGui::BeginChild("controlWindow", {190, 90}, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
    {
      // TODO: Implement playback controls -> play/pause, next, previous
      // ImGui::PushFont(app.fafont);
      if (app.shuffled)
      {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.49f, 0.51f, 1.0f, 1.0f});
      }
      else
      {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
      }

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{1.0f, 1.0f, 1.0f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::SetWindowFontScale(0.6f);
      ImGui::SetCursorPos(ImVec2(20, 30));
      if (ImGui::Button(ICON_FA_SHUFFLE))
      {
        if (app.shuffled)
        {
          resetShuffle();
          app.shuffled = false;
        }
        else
        {
          shuffleSong();
          app.shuffled = true;
        }
      }
      ImGui::PopStyleColor(4);
      // ImGui::PopFont();
      ImGui::SameLine();
      // ImGui::PushFont(app.fafont);
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::SetWindowFontScale(0.7f);
      ImGui::SetCursorPosY(28);
      if (ImGui::Button(ICON_FA_CARET_LEFT))
      {
        if (audioInfo->get_index() > -1)
        {
          if (audioDevice->is_playing())
          {
            audioDevice->stop();
          }

          audioInfo->dec_index();
          if (audioInfo->get_index() < 0)
          {
            audioInfo->set_index(audioFiles.size());
          }
          audioDevice->play(audioFiles[audioInfo->get_index()].path, false, 0);
          update_song_info(audioFiles[audioInfo->get_index()], audioInfo);
          audioInfo->selection.clear();
          audioInfo->selection.push_back(audioInfo->get_index());
        }
      }
      ImGui::PopStyleColor(3);
      // ImGui::PopFont();
      ImGui::SameLine();
      // ImGui::PushFont(app.fafont);
      if (audioDevice->is_playing())
      {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosY(22);
        if (ImGui::Button(ICON_FA_PAUSE))
        {
          if (audioInfo->get_index() > -1)
          {
            if (audioDevice->is_playing())
            {
              audioDevice->stop();
            }
          }
        }

        ImGui::PopStyleColor(3);
      }
      else
      {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosY(22);
        if (ImGui::Button(ICON_FA_PLAY))
        {
          if (audioInfo->get_index() > -1)
          {
            int pcmFrame = audioDevice->get_pcm_frame(audioInfo->currentSeconds);
            audioDevice->play(audioFiles[audioInfo->get_index()].path, true, pcmFrame);
            update_song_info(audioFiles[audioInfo->get_index()], audioInfo);
          }
        }
        ImGui::PopStyleColor(3);
      }

      // ImGui::PopFont();
      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::SetWindowFontScale(0.7f);
      ImGui::SetCursorPosY(28);
      if (ImGui::Button(ICON_FA_CARET_RIGHT))
      {
        if (audioInfo->get_index() > -1)
        {
          if (audioDevice->is_playing())
          {
            audioDevice->stop();
          }
          audioInfo->inc_index();
          if (audioInfo->get_index() > (int)audioFiles.size())
          {
            audioInfo->set_index(0);
          }

          audioDevice->play(audioFiles[audioInfo->get_index()].path, false, 0);
          // play(audioFiles[audioInfo->get_index()].path, false, 0);
          update_song_info(audioFiles[audioInfo->get_index()], audioInfo);
          audioInfo->selection.clear();
          audioInfo->selection.push_back(audioInfo->get_index());
        }
      }
      ImGui::PopStyleColor(3);
      // ImGui::PopFont();
      ///
      ///
      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::SetWindowFontScale(0.5f);
      ImGui::SetCursorPosY(30);
      if (ImGui::Button(ICON_FA_REPEAT))
      {
        // TODO: Repeat song code
      }
      ImGui::PopStyleColor(3);
      // ImGui::PopFont();
      ImGui::EndChild();
    }
    // ImGui::PopStyleColor();
    ///
    ///
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.10f, 0.50f, 0.30f, 1.0f});
    ImGui::SameLine();
    ImGui::BeginChild("spectrumWindow", {app.width - 630.0f, 90}, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
    {
      // TODO: Implement spectrum analyzer
      ImGui::SetCursorPos(ImVec2(10, 34));
      std::string cs = get_formatted_time(audioInfo->currentSeconds);
      std::string ms = get_formatted_time(audioInfo->get_length());
      ImGui::SetWindowFontScale(0.5f);
      ImGui::Text("%s / %s", cs.c_str(), ms.c_str());
      ImGui::SameLine();
      {
        static bool animate = true;
        static float values[90] = {};
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (!animate || refresh_time == 0.0)
          refresh_time = ImGui::GetTime();
        while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
        {
          static float phase = 0.0f;
          values[values_offset] = cosf(phase);
          values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
          phase += 0.10f * values_offset;
          refresh_time += 1.0f / 60.0f;
        }

        ImGui::SetCursorPos(ImVec2(110, 28));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 0.f));
        ImGui::PushItemWidth(app.width - 750.0f);
        ImGui::PlotLines("##Lines", values, IM_ARRAYSIZE(values), values_offset, NULL, -1.0f, 1.0f, ImVec2(0, 30.0f));
        ImGui::PopStyleColor();
      }
      ImGui::EndChild();
    }
    // ImGui::PopStyleColor();
    ///
    ///
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.10f, 0.50f, 0.30f, 1.0f});
    ImGui::SetNextWindowPos(lastWinPos);
    ImGui::BeginChild("miscWindow", {220, 90}, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
    {
      // TODO: Implement misc controls -> volume, playlist, like
      ImGui::SetCursorPos(ImVec2(5, 28));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      // if (currentSong.liked)
      // {
      //   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.49f, 0.51f, 1.0f, 1.0f});
      // }
      // else
      // {
      //   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
      // }
      ImGui::SetWindowFontScale(0.6f);
      if (ImGui::Button(ICON_FA_HEART))
      {
        // TODO: Implement favouriting song
      }
      ImGui::SameLine();
      ImGui::SetWindowFontScale(0.6f);
      if (ImGui::Button(ICON_FA_ALIGN_LEFT))
      {
        // TODO: Implement add to playlist
      }
      ImGui::PopStyleColor(3);
      ImGui::SameLine();
      ImGui::SetWindowFontScale(0.6f);
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      if (ImGui::Button(audioDevice->is_muted() ? ICON_FA_VOLUME_OFF : ICON_FA_VOLUME_HIGH))
      {
        if (audioDevice->is_muted())
        {
          audioDevice->set_volume(audioDevice->get_prev_volume());
          audioDevice->mute(false);
        }
        else
        {
          audioDevice->set_prev_volume();
          audioDevice->set_volume(0);
          audioDevice->mute(true);
        }

        audioDevice->set_ma_volume();
      }
      ImGui::PopStyleColor(3);
      ImGui::SameLine();
      ImGui::PushItemWidth(100);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.5f, 0.5f));
      // ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.5f));
      // ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{0.49f, 0.51f, 1.0f, 1.0f});
      ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.7f, 0.5f));
      ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, (ImVec4)ImColor::HSV(172.f, 245.f, 237.f, 200.f));
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.9f, 0.9f));
      ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 0.f);
      ImGui::PushStyleVar(ImGuiStyleVar_SliderThickness, 0.10f);
      ImGui::PushStyleVar(ImGuiStyleVar_SliderContrast, 0.7f);
      ImGui::SetCursorPos(ImVec2(100, 28));
      if (ImGui::SliderFloat("##volumeSlider", &audioDevice->get_volume(), 0.0f, audioDevice->get_max_volume(), "", ImGuiSliderFlags_None))
      {
        audioDevice->set_ma_volume();
      }
      ImGui::PopStyleVar(3);
      ImGui::PopStyleColor(5);
      ImGui::EndChild();
    }
    // ImGui::PopStyleColor();
    ///
    ImGui::EndChild();
  }
  // ImGui::PopStyleColor();dioDevice.engine
}
