#pragma once

#include "editor.hpp"
#include "app.hpp"
#include <vector>

#define MINIAUDIO_IMPLEMENTATION
#include "../third_party/miniaudio/miniaudio.h"

struct AudioDevice {
	ma_result result;
 	ma_engine engine;
 	ma_sound sound;
  	ma_device device;
  	ma_device_config deviceConfig;
  	ma_decoder decoder;

  	float volume = 1.0f;
  	float maxVolume = 1.0f;
  	float prevVolume = 1.0f;

  	bool muted = false;
} audioDevice;

struct AudioInfo {
	int songIndex = -1;
	float currentSeconds = 0.0f;
	float maxSeconds = 0.0f;
	int audioLength = 0;
	int sampleRate = 0;

	AudioFile currentSong;
	ImVector<int> selection;
} audioInfo;