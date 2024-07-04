#pragma once

#include "editor.hpp"
#include <vector>

#define MINIAUDIO_IMPLEMENTATION
#include "../third_party/miniaudio/miniaudio.h"

#define SAMPLE_RATE     44100
struct AudioFile
{
  int ID = 0;
  int index = 0;

  std::string title;
  std::string artist;
  std::string album;
  std::string comment;
  std::string genre;
  std::string path;

  unsigned int year = 0;
  unsigned int track = 0;

  int bitrate = 0;
  int sampleRate = SAMPLE_RATE;
  int channels = 2;
  int length = 0;

  GLuint texture = 0;
};

class AudioInfo {
public:
  std::vector<AudioFile> files;
  AudioFile currentSong;
  ImVector<int> selection;
  float currentSeconds = 0.0F;
  float maxSeconds = 0.0F;

  inline void set_index(int ind) { index = ind; }
  inline void inc_index() { index++; }
  inline void dec_index() { index--; }
  inline void set_length(int len) { length = len; }
  inline void set_sample_rate(int spr) { sampleRate = spr; }
  inline void set_table_selection(ImVector<int> &tsi) { selection = tsi; }

  [[nodiscard]] inline int get_index() const { return index; }
  [[nodiscard]] inline int get_length() const { return length; }
  [[nodiscard]] inline int get_sample_rate() const { return sampleRate; }
  [[nodiscard]] inline ImVector<int> get_table_selection() const { return selection; }

private:
  int index = -1;
  int length = 0;
  int sampleRate = 0;

};

class AudioDevice
{
  public:
  bool init();
  void play(std::string &path, bool paused, ma_uint64 pcmFrame);
  void poll(AudioInfo *audioInfo);
  void cleanup();
  inline void stop() { ma_sound_stop(&sound); }
  inline ma_bool32 is_playing() { return ma_sound_is_playing(&sound); }

  inline void set_ma_volume() { ma_engine_set_volume(&engine, volume); }
  inline void set_volume(float vol) { volume = vol; }
  inline void set_prev_volume() { prevVolume = volume; }
  [[nodiscard]] inline float get_prev_volume() const {return prevVolume; }
  [[nodiscard]] inline float get_max_volume() const {return maxVolume; }
  inline float &get_volume() {return volume; }
  inline void mute(bool value) {muted = value; }
  inline void toggle_mute() {muted = !muted; }
  [[nodiscard]] inline bool is_muted() const {return muted; }

  void seek(float seconds);
  ma_uint32 get_pcm_frame(ma_uint32 seconds) {return seconds * ma_engine_get_sample_rate(&engine);}

  private:
  ma_result result;
  ma_engine engine;
  ma_sound sound;
  ma_device device;
  ma_device_config deviceConfig;
  ma_decoder decoder;
  ma_resource_manager_config resourceManagerConfig;
  ma_resource_manager resourceManager;
  ma_resource_manager_data_source dataSource;


  float volume = 1.0F;
  float maxVolume = 1.0F;
  float prevVolume = 1.0F;
  bool muted = false;

  static void ma_data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);
};

std::vector<AudioFile> audioFiles;

std::map<ma_result, std::string> const maMap = {
    {MA_SUCCESS, "MA_SUCCESS"},
    {MA_ERROR, "MA_ERROR"},
    {MA_INVALID_ARGS, "MA_INVALID_ARGS"},
    {MA_INVALID_OPERATION, "MA_INVALID_OPERATION"},
    {MA_OUT_OF_MEMORY, "MA_OUT_OF_MEMORY"},
    {MA_OUT_OF_RANGE, "MA_OUT_OF_RANGE"},
    {MA_ACCESS_DENIED, "MA_ACCESS_DENIED"},
    {MA_DOES_NOT_EXIST, "MA_DOES_NOT_EXIST"},
    {MA_ALREADY_EXISTS, "MA_ALREADY_EXISTS"},
    {MA_TOO_MANY_OPEN_FILES, "MA_TOO_MANY_OPEN_FILES"},
    {MA_INVALID_FILE, "MA_INVALID_FILE"},
    {MA_TOO_BIG, "MA_TOO_BIG"},
    {MA_PATH_TOO_LONG, "MA_PATH_TOO_LONG"},
    {MA_NAME_TOO_LONG, "MA_NAME_TOO_LONG"},
    {MA_NOT_DIRECTORY, "MA_NOT_DIRECTORY"},
    {MA_IS_DIRECTORY, "MA_IS_DIRECTORY"},
    {MA_DIRECTORY_NOT_EMPTY, "MA_DIRECTORY_NOT_EMPTY"},
    {MA_AT_END, "MA_AT_END"},
    {MA_NO_SPACE, "MA_NO_SPACE"},
    {MA_BUSY, "MA_BUSY"},
    {MA_IO_ERROR, "MA_IO_ERROR"},
    {MA_INTERRUPT, "MA_INTERRUPT"},
    {MA_UNAVAILABLE, "MA_UNAVAILABLE"},
    {MA_ALREADY_IN_USE, "MA_ALREADY_IN_USE"},
    {MA_BAD_ADDRESS, "MA_BAD_ADDRESS"},
    {MA_BAD_SEEK, "MA_BAD_SEEK"},
    {MA_BAD_PIPE, "MA_BAD_PIPE"},
    {MA_DEADLOCK, "MA_DEADLOCK"},
    {MA_TOO_MANY_LINKS, "MA_TOO_MANY_LINKS"},
    {MA_NOT_IMPLEMENTED, "MA_NOT_IMPLEMENTED"},
    {MA_NO_MESSAGE, "MA_NO_MESSAGE"},
    {MA_BAD_MESSAGE, "MA_BAD_MESSAGE"},
    {MA_NO_DATA_AVAILABLE, "MA_NO_DATA_AVAILABLE"},
    {MA_INVALID_DATA, "MA_INVALID_DATA"},
    {MA_TIMEOUT, "MA_TIMEOUT"},
    {MA_NO_NETWORK, "MA_NO_NETWORK"},
    {MA_NOT_UNIQUE, "MA_NOT_UNIQUE"},
    {MA_NOT_SOCKET, "MA_NOT_SOCKET"},
    {MA_NO_ADDRESS, "MA_NO_ADDRESS"},
    {MA_PROTOCOL_UNAVAILABLE, "MA_PROTOCOL_UNAVAILABLE"},
    {MA_PROTOCOL_NOT_SUPPORTED, "MA_PROTOCOL_NOT_SUPPORTED"},
    {MA_PROTOCOL_FAMILY_NOT_SUPPORTED, "MA_PROTOCOL_FAMILY_NOT_SUPPORTED"},
    {MA_ADDRESS_FAMILY_NOT_SUPPORTED, "MA_ADDRESS_FAMILY_NOT_SUPPORTED"},
    {MA_SOCKET_NOT_SUPPORTED, "MA_SOCKET_NOT_SUPPORTED"},
    {MA_CONNECTION_RESET, "MA_CONNECTION_RESET"},
    {MA_ALREADY_CONNECTED, "MA_ALREADY_CONNECTED"},
    {MA_NOT_CONNECTED, "MA_NOT_CONNECTED"},
    {MA_CONNECTION_REFUSED, "MA_CONNECTION_REFUSED"},
    {MA_NO_HOST, "MA_NO_HOST"},
    {MA_IN_PROGRESS, "MA_IN_PROGRESS"},
    {MA_CANCELLED, "MA_CANCELLED"},
    {MA_MEMORY_ALREADY_MAPPED, "MA_MEMORY_ALREADY_MAPPED"},

    /* General miniaudio-specific errors. */
    {MA_FORMAT_NOT_SUPPORTED, "MA_FORMAT_NOT_SUPPORTED"},
    {MA_DEVICE_TYPE_NOT_SUPPORTED, "MA_DEVICE_TYPE_NOT_SUPPORTED"},
    {MA_SHARE_MODE_NOT_SUPPORTED, "MA_SHARE_MODE_NOT_SUPPORTED"},
    {MA_NO_BACKEND, "MA_NO_BACKEND"},
    {MA_NO_DEVICE, "MA_NO_DEVICE"},
    {MA_API_NOT_FOUND, "MA_API_NOT_FOUND"},
    {MA_INVALID_DEVICE_CONFIG, "MA_INVALID_DEVICE_CONFIG"},
    {MA_LOOP, "MA_LOOP"},

    /* State errors. */
    {MA_DEVICE_NOT_INITIALIZED, "MA_DEVICE_NOT_INITIALIZED"},
    {MA_DEVICE_ALREADY_INITIALIZED, "MA_DEVICE_ALREADY_INITIALIZED"},
    {MA_DEVICE_NOT_STARTED, "MA_DEVICE_NOT_STARTED"},
    {MA_DEVICE_NOT_STOPPED, "MA_DEVICE_NOT_STOPPED"},

    /* Operation errors. */
    {MA_FAILED_TO_INIT_BACKEND, "MA_FAILED_TO_INIT_BACKEND"},
    {MA_FAILED_TO_OPEN_BACKEND_DEVICE, "MA_FAILED_TO_OPEN_BACKEND_DEVICE"},
    {MA_FAILED_TO_START_BACKEND_DEVICE, "MA_FAILED_TO_START_BACKEND_DEVICE"},
    {MA_FAILED_TO_STOP_BACKEND_DEVICE, "MA_FAILED_TO_STOP_BACKEND_DEVICE"},
};

static const ImGuiTableSortSpecs *s_current_sort_specs = nullptr;

// Compare function to be used by qsort()
static int IMGUI_CDECL CompareWithSortSpecs(const void *lhs, const void *rhs)
{
  const AudioFile *a = (const AudioFile *)lhs;
  const AudioFile *b = (const AudioFile *)rhs;
  for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
  {
    // Here we identify columns using the ColumnUserID value that we ourselves
    // passed to TableSetupColumn() We could also choose to identify columns
    // based on their index (sort_spec->ColumnIndex), which is simpler!
    const ImGuiTableColumnSortSpecs *sort_spec = &s_current_sort_specs->Specs[n];
    int delta = 0;
    switch (sort_spec->ColumnUserID)
    {
    case TableColumnID_ID:
      delta = (a->ID - b->ID);
      break;
    case TableColumnID_Title:
      delta = (strcmp(a->title.c_str(), b->title.c_str()));
      break;
    case TableColumnID_Length:
      delta = (a->length - b->length);
      break;
    case TableColumnID_Artist:
      delta = (strcmp(a->artist.c_str(), b->artist.c_str()));
      break;
    default:
      IM_ASSERT(0);
      break;
    }
    if (delta > 0)
    {
      return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
    }
    if (delta < 0)
    {
      return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
    }
  }

  // qsort() is instable so always return a way to differenciate items.
  // Your own compare function may want to avoid fallback on implicit sort
  // specs e.g. a Name compare if it wasn't already part of the sort specs.
  return (a->ID - b->ID);
}
