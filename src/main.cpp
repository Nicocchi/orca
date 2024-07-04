#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

#include "taglib/fileref.h"
#include "taglib/tag.h"

#include "orca.hpp"
#include "app.hpp"
#include "editor.cpp"
#include "audio.cpp"
#include "database.cpp"
#include "gui/table.cpp"
#include "gui/sidebar.cpp"
#include "gui/seekbar.cpp"
#include "gui/musicbar.cpp"


static void glfwErrorCallback(int error, const char *description)
{
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void text_centered(char *text)
{
  auto windowWidth = ImGui::GetWindowSize().x;
  auto textWidth = ImGui::CalcTextSize(text).x;

  ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
  ImGui::Text("%s", text);
}

int main()
{
  {
    sql_create_table();
    sql_create_playlists_table();
    sql_open();
  }

  AudioDevice audioDevice;
  audioDevice.init();
  AudioInfo audioInfo = {};
  App app;

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
    glfwSetWindowSizeLimits(app.window, 1280, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);
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
    audioDevice.poll(&audioInfo);
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::SetNextWindowSize(ImVec2(app.width, app.height));
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.19f, 1.00f));
      ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);
      {
        drawSidebar(app);
        ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
        // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.12f, 1.0f, 0.64f, 1.0f});
        ImGui::SetNextWindowPos(ImVec2(200, 0));
        ImGui::BeginChild("mainLayout", {static_cast<float>(app.width - 200), static_cast<float>(app.height - 106)});
        {
          // TODO: Implement layouts for Home, Search, Library
          ImGui::SetWindowFontScale(0.5f);
          drawAudioFeedTable(&audioDevice, &audioInfo);
          ImGui::EndChild();
        }
        // ImGui::PopStyleColor();
        ///
        drawSeekBar(&audioDevice, &audioInfo, app);
        drawMusicBar(&audioDevice, &audioInfo, app);
      }
      ImGui::PopStyleColor();
      ImGui::End();
    }

    autoplay_next_song(&audioDevice, &audioInfo);

    // Rendering
    ImGui::Render();
    glfwGetFramebufferSize(app.window, &app.width, &app.height);
    glViewport(0, 0, app.width, app.height);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(app.window);
  }


  audioDevice.cleanup();
  editor_cleanup();

  glfwDestroyWindow(app.window);
  glfwTerminate();

  printf("I really love Yazawa Nico\n");
}
