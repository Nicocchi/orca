#include "orca.hpp"
#include "../dialogue.cpp"
#include "database.hpp"

void drawSidebar(App app)
{
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::BeginChild("leftSidebar", {200, static_cast<float>(app.height - 106)});
  {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::BeginChild("titleWindow", {200, 60});
    {
      ImGui::SetWindowFontScale(1.0f);
      ImGui::SetCursorPos(ImVec2(35, 20));
      ImGui::Text("%s", app.title);
      ImGui::EndChild();
    }
    ///
    ///
    ImGui::SetNextWindowPos(ImVec2(0, 100));
    ImGui::BeginChild("menuWindow", ImVec2(200, 130));
    {
      ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.2f, 0.5f));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{1.0f, 1.0f, 1.0f, 0.0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::SetWindowFontScale(0.5f);

      if (ImGui::Button(ICON_FA_HOUSE "   Open", ImVec2(200, 40)))
      {
        // TODO: Feed button layout switch
        if (!open_file_directory())
        {
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
      ImGui::EndChild();
    }
    ImGui::EndChild();
    ///
    ///
    ImGui::SetNextWindowPos(ImVec2(0, 270));
    ImGui::BeginChild("listWindow", ImVec2(200, 300));
    {
      ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.2f, 0.5f));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{1.0f, 1.0f, 1.0f, 0.5f});
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.3f, 0.4f, .0f});
      ImGui::SetWindowFontScale(0.5f);
        bool playlistOpen = true;
        static char playlistTitle[128] = "Playlist Name";
      /*if (ImGui::Button("New Playlist", ImVec2(200, 40)))
      {
        // TODO: Feed button layout switch
        ImGui::OpenPopup("Add New Playlist");
      }
        if (ImGui::BeginPopupModal("Add New Playlist", &playlistOpen))
            {
                ImGui::Text("Add a new playlist");
                ImGui::InputText("Name", playlistTitle, IM_ARRAYSIZE(playlistTitle));
                if (ImGui::Button("Add"))
                {
                    sql_insert_new_playlist(playlistTitle);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
      if (ImGui::Button("Favourites", ImVec2(200, 40)))
      {
        // TODO: Search button layout switch
      }*/
      ImGui::PopStyleColor(3);
      ImGui::PopStyleVar();
      ImGui::EndChild();
    }
    ///
    ///
    ImGui::SetNextWindowPos(ImVec2(0, 370));
    ImGui::BeginChild("playlistWindow", ImVec2(200, static_cast<float>(app.height - 476)));
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
  }
}
