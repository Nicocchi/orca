#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/imgui/stb_image.h"
#define IMGUI_IMPLEMENTATION
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/imgui_impl_glfw.h"
#include "../third_party/imgui/imgui_impl_opengl3.h"
#include "../third_party/imgui/imgui_internal.h"

#include "GLFW/glfw3.h"

#include "../third_party/fonts/IconsFontAwesome6.h"

#include "../third_party/portable_file_dialogs/portable_file_dialogs.h"

// Enforce cdecl calling convention for functions called by the standard
// library, in case compilation settings changed the default to e.g.
// __vectorcall
#ifndef IMGUI_CDECL
#ifdef _MSC_VER
#define IMGUI_CDECL __cdecl
#else
#define IMGUI_CDECL
#endif
#endif

IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);

enum TableColumnID {
  TableColumnID_ID,
  TableColumnID_Action,
  TableColumnID_Title,
  TableColumnID_Artist,
  TableColumnID_Album,
  TableColumnID_Year,
  TableColumnID_Comment,
  TableColumnID_Track,
  TableColumnID_Genre,
  TableColumnID_Length,
  TableColumnID_Bitrate,
  TableColumnID_SampleRate,
  TableColumnID_Channels,
};

struct Editor {
	ImGuiIO io;
  ImGuiStyle style;
  ImFont *iconFont;

  const char *glsl_version = "#version 460";
} editor;
