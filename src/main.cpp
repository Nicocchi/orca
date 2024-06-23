#include <filesystem>
#include <iomanip>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <random>
#include <fstream>

#include "taglib/fileref.h"
#include "taglib/tag.h"

#include "editor.cpp"
#include "audio.cpp"
#include "database.cpp"
#include "dialogue.cpp"
#include "app.hpp"



struct App {
  GLFWwindow *window;
  int width = 1280;
  int height = 720;
  char title[5] = "Orca";
  const char *glsl_version = "#version 460";

  bool shuffled = false;

  ImGuiIO io;
  ImGuiStyle style;
  ImFont *fafont;

  int songIndex = -1;
  float currentSeconds = 0.f;
  float maxSeconds = 0.f;
  int sampleRate = 0;
  int audioLength = 0;
  AudioFile currentSong;
  ImVector<int> selection;
} app;

// TODO: Refactor these variables

char *template_items_names[] = {};

bool modifyTable = false;

static void glfwErrorCallback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void text_centered(char *text) {
  auto windowWidth = ImGui::GetWindowSize().x;
  auto textWidth = ImGui::CalcTextSize(text).x;

  ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
  ImGui::Text("%s", text);
}

void CircleImage(ImTextureID user_texture_id, float diameter, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1))
{
    ImVec2 p_min = ImGui::GetCursorScreenPos();
    ImVec2 p_max = ImVec2(p_min.x + diameter, p_min.y + diameter);
    ImGui::GetWindowDrawList()->AddImageRounded(user_texture_id, p_min, p_max, uv0, uv1, ImGui::GetColorU32(tint_col), diameter * 0.5f);
    ImGui::Dummy(ImVec2(diameter, diameter));
}

void shuffleSong()
{
  auto rd = std::random_device {};
  auto rng = std::default_random_engine { rd() };
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

void drawAudioFeedTable() {
  //const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
  static ImGuiTableFlags flags =
      ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti |
      ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX |
      ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_NoBordersInBodyUntilResize;

  static ImGuiTableFlags modifyFlags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable |
      ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg |
      ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
      ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody;
  enum ContentsType {
    CT_Text,
    CT_Button,
    CT_SmallButton,
    CT_FillButton,
    CT_Selectable,
    CT_SelectableSpanRow
  };

  static int freeze_cols = 1;
  static int freeze_rows = 1;
  static ImVec2 outer_size_value = ImVec2(0.0f, TEXT_BASE_HEIGHT * 202);
  static float row_min_height = 40.0f;         // Auto
  static bool show_headers = true;

  static ImVector<AudioFile> items;
  static bool items_need_sort = false;

  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));
  if (ImGui::BeginTable("audioFeedTable", 13, modifyTable ? modifyFlags : flags)) {
    ImGui::TableSetupColumn(
        " ",
        ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoHide |
            ImGuiTableColumnFlags_WidthFixed |
            ImGuiTableColumnFlags_NoHeaderLabel |
            ImGuiTableColumnFlags_NoHeaderWidth |
            ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize,
        0.0);
    ImGui::TableSetupColumn("ID",
                            ImGuiTableColumnFlags_DefaultHide |
                                ImGuiTableColumnFlags_WidthFixed,
                            0.0f, TableColumnID_ID);
    // ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort |
    // ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
    ImGui::TableSetupColumn("Title",
                            ImGuiTableColumnFlags_DefaultSort |
                                ImGuiTableColumnFlags_WidthFixed,
                            0.0f, TableColumnID_Title);
    ImGui::TableSetupColumn("Artist",
                            (flags & ImGuiTableFlags_NoHostExtendX)
                                ? 0
                                : ImGuiTableColumnFlags_WidthStretch,
                            0.0f, TableColumnID_Artist);
    ImGui::TableSetupColumn("Album",
                            (flags & ImGuiTableFlags_NoHostExtendX)
                                ? 0
                                : ImGuiTableColumnFlags_WidthStretch,
                            0.0f, TableColumnID_Album);
    ImGui::TableSetupColumn("Year",
                            (flags & ImGuiTableFlags_NoHostExtendX)
                                ? 0
                                : ImGuiTableColumnFlags_WidthStretch,
                            0.0f, TableColumnID_Year);
    ImGui::TableSetupColumn("Track", ImGuiTableColumnFlags_WidthStretch, 0.0f,
                            TableColumnID_Track);
    ImGui::TableSetupColumn("Genre",
                            (flags & ImGuiTableFlags_NoHostExtendX)
                                ? 0
                                : ImGuiTableColumnFlags_WidthStretch,
                            0.0f, TableColumnID_Genre);
    ImGui::TableSetupColumn("Duration",
                            ImGuiTableColumnFlags_PreferSortDescending, 0.0f,
                            TableColumnID_Length);
    ImGui::TableSetupColumn("Comment",
                            ImGuiTableColumnFlags_DefaultHide |
                                ImGuiTableColumnFlags_NoSort,
                            0.0f, TableColumnID_Comment);
    ImGui::TableSetupColumn("Bitrate",
                            ImGuiTableColumnFlags_DefaultHide |
                                ImGuiTableColumnFlags_NoSort,
                            0.0f, TableColumnID_Bitrate);
    ImGui::TableSetupColumn("Sample Rate",
                            ImGuiTableColumnFlags_DefaultHide |
                                ImGuiTableColumnFlags_NoSort,
                            0.0f, TableColumnID_SampleRate);
    ImGui::TableSetupColumn("Channels",
                            ImGuiTableColumnFlags_DefaultHide |
                                ImGuiTableColumnFlags_NoSort,
                            0.0f, TableColumnID_Channels);
    ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);

    // Sort our data if sort specs have been changed!
    ImGuiTableSortSpecs *sorts_specs = ImGui::TableGetSortSpecs();
    if (sorts_specs && sorts_specs->SpecsDirty)
      items_need_sort = true;
    if (sorts_specs && items_need_sort && items.Size > 1) {
      AudioFile::s_current_sort_specs =
          sorts_specs; // Store in variable accessible by the sort function.
      qsort(&items[0], (size_t)items.Size, sizeof(items[0]),
            AudioFile::CompareWithSortSpecs);
      AudioFile::s_current_sort_specs = NULL;
      sorts_specs->SpecsDirty = false;
    }
    items_need_sort = false;

    //const bool sorts_specs_using_quantity =
    //    (ImGui::TableGetColumnFlags(3) & ImGuiTableColumnFlags_IsSorted) != 0;

    if (show_headers) {
      ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.09f, 0.09f, 0.19f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
      ImGui::TableHeadersRow();
      ImGui::PopStyleColor(2);
    }
    ImGui::PushButtonRepeat(true);

    for (auto it = begin(audioFiles); it != end(audioFiles); ++it) {
      const bool item_is_selected = app.selection.contains(it->ID);
      ImGui::PushID(it->ID);
      ImGui::TableNextRow(ImGuiTableRowFlags_None, row_min_height);

      // if (ImGui::BeginPopupContextItem())
      // {
      // 	ImGui::Text("This is custom");
      // 	ImGui::EndPopup();
      // }

      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});

      // Empty
      if (ImGui::TableSetColumnIndex(0)) {
        // ImGui::Text(u8"%04d", it->ID);
        ImGuiSelectableFlags selectable_flags =
            ImGuiSelectableFlags_SpanAllColumns |
            ImGuiSelectableFlags_AllowItemOverlap |
            ImGuiSelectableFlags_AllowDoubleClick;
        if (ImGui::Selectable("", item_is_selected, selectable_flags,
                              ImVec2(0, row_min_height))) {
          audioInfo.selection.clear();
          audioInfo.selection.push_back(it->ID);

          if (ImGui::IsMouseDoubleClicked(0)) {
            update_song_info(*it);
            audioInfo.songIndex = it->ID - 1;
            play(it->path);
          }
        }
      }

      // ID
      if (ImGui::TableSetColumnIndex(1)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%04d", it->ID);
      }

      // Title
      if (ImGui::TableSetColumnIndex(2)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->title.c_str());
      }

      // Artist
      if (ImGui::TableSetColumnIndex(3)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->artist.c_str());
      }

      // Album
      if (ImGui::TableSetColumnIndex(4)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->album.c_str());
      }

      // Year
      if (ImGui::TableSetColumnIndex(5)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->year);
      }

      // Track
      if (ImGui::TableSetColumnIndex(6)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->track);
      }

      // Genre
      if (ImGui::TableSetColumnIndex(7)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->genre.c_str());
      }

      // Duration
      if (ImGui::TableSetColumnIndex(8)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
	  std::string ms = get_formatted_time(it->length);
	  ImGui::Text("%s", ms.c_str());
        // ImGui::Text(u8"%d", it->length);
      }

      // Comment
      if (ImGui::TableSetColumnIndex(9)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->comment.c_str());
      }

      // Bitrate
      if (ImGui::TableSetColumnIndex(10)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->bitrate);
      }

      // Sample Rate
      if (ImGui::TableSetColumnIndex(11)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->sampleRate);
      }

      // Channels
      if (ImGui::TableSetColumnIndex(12)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 -
                             ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->channels);
      }

      ImGui::PopStyleColor();
      ImGui::PopID();
    }
    ImGui::PopButtonRepeat();
    ImGui::EndTable();
  }
  ImGui::PopStyleVar();
}

void drawSidebar() {
//   ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::BeginChild("leftSidebar", {200, static_cast<float>(app.height - 106)});
  {
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.10f, 0.50f, 0.30f, 1.0f});
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::BeginChild("titleWindow", {200, 60});
    {
      //   ImGui::PushFont(app.robotom);
      // ImGui::PushFont(app.titleFont);
      ImGui::SetWindowFontScale(1.0f);
      ImGui::SetCursorPos(ImVec2(35, 20));
      // text_centered(app.title);
      ImGui::Text("%s", app.title);
      // ImGui::PopFont();
      ImGui::EndChild();
    }
    // ImGui::PopStyleColor();
    ///
    ///
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.30f, 1.00f, 1.00f, 1.0f});
    ImGui::SetNextWindowPos(ImVec2(0, 100));
    ImGui::BeginChild("menuWindow", ImVec2(200, 130));
    {
      ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.2f, 0.5f));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{1.0f, 1.0f, 1.0f, 0.0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      // ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.49f, 0.51f, 1.0f, 1.0f});
      ImGui::SetWindowFontScale(0.5f);
      
      if (ImGui::Button(ICON_FA_HOUSE "   Feed", ImVec2(200, 40))) {
        // TODO: Feed button layout switch
        if (!open_file_directory()) {
          printf("Error opening file directory\n");
        }
      }
      ImGui::PopStyleColor(3);
      //   if (ImGui::Button("Library", ImVec2(200, 40))) {
      //     // TODO: Search button layout switch
      //   }
      //   if (ImGui::Button("Statistics", ImVec2(200, 40))) {
      //     // TODO: Library button layout switch
      //   }
      ImGui::PopStyleVar();
      //   ImGui::PopFont();
      ImGui::EndChild();
    }
    // ImGui::PopStyleColor();
    ImGui::EndChild();
    ///
    ///
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.30f, 1.00f, 1.00f, 1.0f});
    ImGui::SetNextWindowPos(ImVec2(0, 270));
    ImGui::BeginChild("listWindow", ImVec2(200, 300));
    {
      //   ImGui::PushFont(app.roboto);
      ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
	  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
	  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
    ImGui::SetWindowFontScale(0.5f);
      if (ImGui::Button("New Playlists", ImVec2(200, 40))) {
        // TODO: Feed button layout switch
      }
      if (ImGui::Button("Favourites", ImVec2(200, 40))) {
        // TODO: Search button layout switch
      }
	  ImGui::PopStyleColor(3);
      ImGui::PopStyleVar();
      //   ImGui::PopFont();
      ImGui::EndChild();
    }
    // ImGui::PopStyleColor();
    ///
    ///
    // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.30f, 1.00f, 1.00f, 1.0f});
    ImGui::SetNextWindowPos(ImVec2(0, 370));
    ImGui::BeginChild("playlistWindow",
                      ImVec2(200, static_cast<float>(app.height - 476)));
    {
      //   ImGui::PushFont(app.roboto);
    //   ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.2f, 0.5f));
    //   if (ImGui::Button("Metalcore", ImVec2(200, 40))) {
    //     // TODO: Feed button layout switch
    //   }
    //   if (ImGui::Button("Electro", ImVec2(200, 40))) {
    //     // TODO: Search button layout switch
    //   }
    //   if (ImGui::Button("Funk", ImVec2(200, 40))) {
    //     // TODO: Library button layout switch
    //   }
    //   if (ImGui::Button("Denpa", ImVec2(200, 40))) {
    //     // TODO: Library button layout switch
    //   }
    //   if (ImGui::Button("Pop", ImVec2(200, 40))) {
    //     // TODO: Library button layout switch
    //   }
    //   ImGui::PopStyleVar();
      //   ImGui::PopFont();
      ImGui::EndChild();
    }
    // ImGui::PopStyleColor();
  }
//   ImGui::PopStyleColor();
}

void drawSeekBar() {
	// ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{1.0f, 1.30f, 1.20f, 1.0f});
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
	  if (ImGui::SliderFloat("##seekSlider", &audioInfo.currentSeconds, 0, audioInfo.maxSeconds, "", ImGuiSliderFlags_NoInput))
	{
		int pcmFrame = audioInfo.currentSeconds * ma_engine_get_sample_rate(&audioDevice.engine);
		ma_sound_seek_to_pcm_frame(&audioDevice.sound, pcmFrame);
	}
	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor(5);
	ImGui::EndChild();
  }
//   ImGui::PopStyleColor();dioDevice.engine
}

void drawMusicBar() {
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
        CircleImage((void*)(intptr_t)audioInfo.currentSong.texture, 50.0f, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1));
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
        ImGui::Text("%s", audioInfo.currentSong.artist.c_str());
        ImGui::SetCursorPos(ImVec2(5, 45));
        ImGui::SetWindowFontScale(0.5f);
        ImGui::Text("%s", audioInfo.currentSong.title.c_str());
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
			if (audioInfo.songIndex > -1)
			{
				if (ma_sound_is_playing(&audioDevice.sound))
				{
					ma_sound_stop(&audioDevice.sound);
				}

        audioInfo.songIndex--;
        if (audioInfo.songIndex < 0) {
          audioInfo.songIndex = audioFiles.size();
        }
        play(audioFiles[audioInfo.songIndex].path, false, 0);
        update_song_info(audioFiles[audioInfo.songIndex]);
        audioInfo.selection.clear();
        audioInfo.selection.push_back(audioInfo.songIndex);
			}
			
		}
		ImGui::PopStyleColor(3);
		// ImGui::PopFont();
		ImGui::SameLine();
		// ImGui::PushFont(app.fafont);
		if (ma_sound_is_playing(&audioDevice.sound))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::SetWindowFontScale(1.0f);
      ImGui::SetCursorPosY(22);
				if (ImGui::Button(ICON_FA_PAUSE))
				{
					if (audioInfo.songIndex > -1)
					{
						if (ma_sound_is_playing(&audioDevice.sound))
						{
							ma_sound_stop(&audioDevice.sound);
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
				if (audioInfo.songIndex > -1)
				{
					int pcmFrame = audioInfo.currentSeconds * ma_engine_get_sample_rate(&audioDevice.engine);
          play(audioFiles[audioInfo.songIndex].path, true, pcmFrame);
					update_song_info(audioFiles[audioInfo.songIndex]);
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
			if (audioInfo.songIndex > -1)
			{
				if (ma_sound_is_playing(&audioDevice.sound))
				{
					ma_sound_stop(&audioDevice.sound);
				}
				audioInfo.songIndex++;
				if (audioInfo.songIndex > (int)audioFiles.size()) {
					audioInfo.songIndex = 0;
				}

				play(audioFiles[audioInfo.songIndex].path, false, 0);
				update_song_info(audioFiles[audioInfo.songIndex]);
				audioInfo.selection.clear();
				audioInfo.selection.push_back(audioInfo.songIndex);
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
	  std::string cs = get_formatted_time(audioInfo.currentSeconds);
	  std::string ms = get_formatted_time(audioInfo.audioLength);
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
      if (ImGui::Button(audioDevice.muted ? ICON_FA_VOLUME_OFF : ICON_FA_VOLUME_HIGH))
      {
        if (audioDevice.muted)
        {
          audioDevice.volume = audioDevice.prevVolume;
          audioDevice.muted = false;
        }
        else
        {
          audioDevice.prevVolume = audioDevice.volume;
          audioDevice.volume = 0;
          audioDevice.muted = true;
        }

        ma_engine_set_volume(&audioDevice.engine, audioDevice.volume);
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
      if (ImGui::SliderFloat("##volumeSlider", &audioDevice.volume, 0.0f, audioDevice.maxVolume, "", ImGuiSliderFlags_None))
      {
        ma_engine_set_volume(&audioDevice.engine, audioDevice.volume);
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

void drawFeedLayout() {}

int main() {
  {
    sql_create_table();
    sql_open();
  }

  if (!init_ma_audio())  
  {
    fprintf(stderr, "Error initializing audio devices\n");
    return -1;
  }

  {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
      return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    app.window = glfwCreateWindow(app.width, app.height, app.title, NULL, NULL);
    if (app.window == NULL)
      return 1;
    glfwSetWindowSizeLimits(app.window, 1280, 720, GLFW_DONT_CARE,
                            GLFW_DONT_CARE);
    glfwMakeContextCurrent(app.window);
    glfwSwapInterval(1); // Enable vsync

    glfwGetFramebufferSize(app.window, &app.width, &app.height);
    glViewport(0, 0, app.width, app.height);
  }

  {
    init_editor(app.window);
  }

  ImVec4 clear_color = ImVec4(0.6f, 0.24f, 0.33f, 1.00f); // 6, 24, 33 , 140, 211, 33, 061821

  // bool ret = LoadTextureFromFile("/home/nicocchi/Pictures/ss.png", &defaultTexture, &defaultImageWidth, &defaultImageHeight);
  // IM_ASSERT(ret);

  while (!glfwWindowShouldClose(app.window))
  {	
    poll_audio();
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::SetNextWindowSize(ImVec2(app.width, app.height));
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.19f, 1.00f));
      ImGui::Begin("Main", NULL,
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_NoTitleBar);
      {
        drawSidebar();
        ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
        // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.12f, 1.0f, 0.64f, 1.0f});
        ImGui::SetNextWindowPos(ImVec2(200, 0));
        ImGui::BeginChild("mainLayout", {static_cast<float>(app.width - 200),
                                         static_cast<float>(app.height - 106)});
        {
          // TODO: Implement layouts for Home, Search, Library
          ImGui::SetWindowFontScale(0.5f);
          drawAudioFeedTable();
          ImGui::EndChild();
        }
        // ImGui::PopStyleColor();
        ///
		    drawSeekBar();
        drawMusicBar();
      }
      ImGui::PopStyleColor();
      ImGui::End();
    }

    autoplay_next_song();

    // Rendering
    ImGui::Render();
    glfwGetFramebufferSize(app.window, &app.width, &app.height);
    glViewport(0, 0, app.width, app.height);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(app.window);
  }

  audio_cleanup();
  editor_cleanup();

  glfwDestroyWindow(app.window);
  glfwTerminate();

  printf("I really love Yazawa Nico\n");
}
