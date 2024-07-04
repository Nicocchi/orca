#include "editor.hpp"

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char *filename, GLuint *out_texture, int *out_width, int *out_height)
{
  // Load from file
  int image_width = 0;
  int image_height = 0;
  unsigned char *image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
  if (image_data == NULL)
    return false;

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  *out_texture = image_texture;
  *out_width = image_width;
  *out_height = image_height;

  return true;
}

bool init_editor(GLFWwindow *window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  editor.io = ImGui::GetIO();
  (void)editor.io;

  editor.style = ImGui::GetStyle();
  editor.style.Colors[ImGuiCol_WindowBg] = ImVec4(255, 255, 255, 255);

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(editor.glsl_version);

  std::string dir = std::filesystem::current_path();
  editor.io.Fonts->AddFontDefault();
  float baseFontSize = 28.0f;
  float iconFontSize = baseFontSize;

  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.GlyphMinAdvanceX = iconFontSize;

  ImFontConfig config;
  config.MergeMode = true;
  config.PixelSnapH = true;
  config.GlyphMinAdvanceX = iconFontSize;
  static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
  editor.iconFont = editor.io.Fonts->AddFontFromFileTTF(dir.append("/assets/fonts/fa-solid-900.ttf").c_str(), iconFontSize, &config, icons_ranges);
  editor.io.Fonts->Build();

  return true;
}

void editor_cleanup()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
