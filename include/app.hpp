#pragma once

#include "editor.hpp"

int defaultImageWidth = 0;
int defaultImageHeight = 0;
GLuint defaultTexture = 0;

struct App
{
  GLFWwindow *window;
  int width = 1280;
  int height = 720;
  char title[5] = "Orca";
  const char *glsl_version = "#version 460";

  bool shuffled = false;

  ImGuiIO io;
  ImGuiStyle style;
  ImFont *fafont;
};
