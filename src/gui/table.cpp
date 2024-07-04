#include "orca.hpp"
#include "editor.hpp"

bool modifyTable = false;
void drawAudioFeedTable(AudioDevice *audioDevice, AudioInfo *audioInfo)
{
  //const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
  static ImGuiTableFlags flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize;

  static ImGuiTableFlags modifyFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody;
  enum ContentsType
  {
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
  static float row_min_height = 40.0f; // Auto
  static bool show_headers = true;

  static ImVector<AudioFile> items;
  static bool items_need_sort = false;

  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));
  if (ImGui::BeginTable("audioFeedTable", 13, modifyTable ? modifyFlags : flags))
  {
    ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoHeaderWidth | ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize, 0.0);
    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_WidthFixed, 0.0f, TableColumnID_ID);
    // ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort |
    // ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
    ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, TableColumnID_Title);
    ImGui::TableSetupColumn("Artist", (flags & ImGuiTableFlags_NoHostExtendX) ? 0 : ImGuiTableColumnFlags_WidthStretch, 0.0f, TableColumnID_Artist);
    ImGui::TableSetupColumn("Album", (flags & ImGuiTableFlags_NoHostExtendX) ? 0 : ImGuiTableColumnFlags_WidthStretch, 0.0f, TableColumnID_Album);
    ImGui::TableSetupColumn("Year", (flags & ImGuiTableFlags_NoHostExtendX) ? 0 : ImGuiTableColumnFlags_WidthStretch, 0.0f, TableColumnID_Year);
    ImGui::TableSetupColumn("Track", ImGuiTableColumnFlags_WidthStretch, 0.0f, TableColumnID_Track);
    ImGui::TableSetupColumn("Genre", (flags & ImGuiTableFlags_NoHostExtendX) ? 0 : ImGuiTableColumnFlags_WidthStretch, 0.0f, TableColumnID_Genre);
    ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_PreferSortDescending, 0.0f, TableColumnID_Length);
    ImGui::TableSetupColumn("Comment", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_NoSort, 0.0f, TableColumnID_Comment);
    ImGui::TableSetupColumn("Bitrate", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_NoSort, 0.0f, TableColumnID_Bitrate);
    ImGui::TableSetupColumn("Sample Rate", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_NoSort, 0.0f, TableColumnID_SampleRate);
    ImGui::TableSetupColumn("Channels", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_NoSort, 0.0f, TableColumnID_Channels);
    ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);

    // Sort our data if sort specs have been changed!
    ImGuiTableSortSpecs *sorts_specs = ImGui::TableGetSortSpecs();
    if (sorts_specs && sorts_specs->SpecsDirty)
      items_need_sort = true;
    if (sorts_specs && items_need_sort && items.Size > 1)
    {
      s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
      qsort(&items[0], (size_t)items.Size, sizeof(items[0]), CompareWithSortSpecs);
      s_current_sort_specs = NULL;
      sorts_specs->SpecsDirty = false;
    }
    items_need_sort = false;

    //const bool sorts_specs_using_quantity =
    //    (ImGui::TableGetColumnFlags(3) & ImGuiTableColumnFlags_IsSorted) != 0;

    if (show_headers)
    {
      ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.09f, 0.09f, 0.19f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
      ImGui::TableHeadersRow();
      ImGui::PopStyleColor(2);
    }
    ImGui::PushButtonRepeat(true);

    for (auto it = begin(audioFiles); it != end(audioFiles); ++it)
    {
      const bool item_is_selected = audioInfo->selection.contains(it->ID);
      ImGui::PushID(it->ID);
      ImGui::TableNextRow(ImGuiTableRowFlags_None, row_min_height);

      // if (ImGui::BeginPopupContextItem())
      // {
      // 	ImGui::Text("This is custom");
      // 	ImGui::EndPopup();
      // }

      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});

      // Empty
      if (ImGui::TableSetColumnIndex(0))
      {
        // ImGui::Text(u8"%04d", it->ID);
        ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick;
        if (ImGui::Selectable("", item_is_selected, selectable_flags, ImVec2(0, row_min_height)))
        {
          audioInfo->selection.clear();
          audioInfo->selection.push_back(it->ID);

          if (ImGui::IsMouseDoubleClicked(0))
          {
            update_song_info(*it, audioInfo);
            audioInfo->set_index(it->ID - 1);
            audioDevice->play(it->path);
          }
        }
      }

      // ID
      if (ImGui::TableSetColumnIndex(1))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%04d", it->ID);
      }

      // Title
      if (ImGui::TableSetColumnIndex(2))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->title.c_str());
      }

      // Artist
      if (ImGui::TableSetColumnIndex(3))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->artist.c_str());
      }

      // Album
      if (ImGui::TableSetColumnIndex(4))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->album.c_str());
      }

      // Year
      if (ImGui::TableSetColumnIndex(5))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->year);
      }

      // Track
      if (ImGui::TableSetColumnIndex(6))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->track);
      }

      // Genre
      if (ImGui::TableSetColumnIndex(7))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->genre.c_str());
      }

      // Duration
      if (ImGui::TableSetColumnIndex(8))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        std::string ms = get_formatted_time(it->length);
        ImGui::Text("%s", ms.c_str());
        // ImGui::Text(u8"%d", it->length);
      }

      // Comment
      if (ImGui::TableSetColumnIndex(9))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%s", it->comment.c_str());
      }

      // Bitrate
      if (ImGui::TableSetColumnIndex(10))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->bitrate);
      }

      // Sample Rate
      if (ImGui::TableSetColumnIndex(11))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
        ImGui::Text(u8"%d", it->sampleRate);
      }

      // Channels
      if (ImGui::TableSetColumnIndex(12))
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 - ImGui::CalcTextSize("A").x);
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
