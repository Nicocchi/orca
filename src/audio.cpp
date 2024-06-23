#include "audio.hpp"

void ma_data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                    ma_uint32 frameCount) {
  ma_decoder *pDecoder = (ma_decoder *)pDevice->pUserData;
  if (pDecoder == NULL) {
    return;
  }

  ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

  (void)pInput;
}

bool init_ma_audio()
{
	audioDevice.result = ma_engine_init(NULL, &audioDevice.engine);
    if (audioDevice.result != MA_SUCCESS) {
      std::cerr << "Error: Failed to initialize audio engine" << std::endl;
      return false;
    }

    audioDevice.deviceConfig = ma_device_config_init(ma_device_type_playback);
    audioDevice.deviceConfig.playback.format = audioDevice.decoder.outputFormat;
    audioDevice.deviceConfig.playback.channels = audioDevice.decoder.outputChannels;
    audioDevice.deviceConfig.sampleRate = audioDevice.decoder.outputSampleRate;
    audioDevice.deviceConfig.dataCallback = ma_data_callback;
    audioDevice.deviceConfig.pUserData = &audioDevice.decoder;

    if (ma_device_init(NULL, &audioDevice.deviceConfig, &audioDevice.device) != MA_SUCCESS) {
      std::cerr << "Error: Failed to open playback device" << std::endl;
      ma_decoder_uninit(&audioDevice.decoder);
      return false;
    }

    ma_engine_set_volume(&audioDevice.engine, audioDevice.volume);

    return true;
}

void play(std::string &path, bool paused = false, ma_uint64 pcmFrame = 0) {
  audioDevice.result = ma_sound_stop(&audioDevice.sound);
  if (audioDevice.result != MA_SUCCESS)
  {
    printf("Could not stop sound\n");
  }
  else
  {
    ma_sound_uninit(&audioDevice.sound);
  }
  
  audioDevice.result = ma_sound_init_from_file(&audioDevice.engine, path.c_str(), 0, NULL, NULL, &audioDevice.sound);
  if (audioDevice.result != MA_SUCCESS)
  {
    printf("Could not play file: %s\n", path.c_str());
  }

  if (!paused)
  {
    ma_sound_set_start_time_in_pcm_frames(&audioDevice.sound, ma_engine_get_time(&audioDevice.engine) + (ma_engine_get_sample_rate(&audioDevice.engine) * 2));
  }
  else
  {
    ma_sound_seek_to_pcm_frame(&audioDevice.sound, pcmFrame);
  }

  ma_sound_start(&audioDevice.sound);
}

std::string get_formatted_time(int length) {
  int seconds = length % 60;
  int minutes = (length - seconds) / 60;
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << int(minutes) << ":"
      << std::setfill('0') << std::setw(2) << int(seconds);
  return oss.str();
}

void update_song_info(AudioFile song) {
  audioInfo.currentSong.album = song.album;
  audioInfo.currentSong.artist = song.artist;
  audioInfo.currentSong.bitrate = song.bitrate;
  audioInfo.currentSong.channels = song.channels;
  audioInfo.currentSong.comment = song.comment;
  audioInfo.currentSong.genre = song.genre;
  audioInfo.currentSong.ID = song.ID;
  audioInfo.currentSong.length = song.length;
  audioInfo.currentSong.path = song.path;
  audioInfo.currentSong.sampleRate = song.sampleRate;
  audioInfo.currentSong.title = song.title;
  audioInfo.currentSong.track = song.track;
  audioInfo.currentSong.year = song.year;
  audioInfo.audioLength = song.length;
  audioInfo.sampleRate = song.sampleRate;

  // extract cover and save to file and load
  TagLib::FileRef fref(audioInfo.currentSong.path.c_str());

  if (!fref.isNull() && fref.tag())
  {
    TagLib::StringList names = fref.complexPropertyKeys();
    for (const auto &name : names)
    {
      const auto &properties = fref.complexProperties(name);
      for (const auto &property : properties)
      {
        for (const auto &[key, value] : property)
        {
          if (value.type() == TagLib::Variant::ByteVector)
          {
            // extract
            std::ofstream picture;
            //TagLib::String fn();
            picture.open("cover.jpg", std::ios_base::out | std::ios_base::binary);
            picture << value.value<TagLib::ByteVector>();
            picture.close();
          }
        }
      }
    }
  }

  // load
  bool ret = LoadTextureFromFile("cover.jpg", &audioInfo.currentSong.texture, &defaultImageWidth, &defaultImageHeight);
  IM_ASSERT(ret);
}

void autoplay_next_song()
{
  if (audioInfo.maxSeconds != 0 && audioInfo.currentSeconds >= audioInfo.maxSeconds) {
    audioInfo.songIndex++;
    if (audioInfo.songIndex > (int)audioFiles.size()) {
      audioInfo.songIndex = 0;
    }

    play(audioFiles[audioInfo.songIndex].path, false, 0);
    // updateSongInfo(audioFiles[audioInfo.songIndex]);
    audioInfo.selection.clear();
    audioInfo.selection.push_back(audioInfo.songIndex);
  }
}

void poll_audio()
{
  ma_sound_get_cursor_in_seconds(&audioDevice.sound, &audioInfo.currentSeconds);
  ma_sound_get_length_in_seconds(&audioDevice.sound, &audioInfo.maxSeconds);
}

void audio_cleanup()
{
  ma_device_uninit(&audioDevice.device);
}