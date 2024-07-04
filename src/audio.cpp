#include "audio.hpp"
#include "app.hpp"
#include <iomanip>
//#include <fstream>

void AudioDevice::ma_data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                    ma_uint32 frameCount) {
  ma_decoder *pDecoder = (ma_decoder *)pDevice->pUserData;
  if (pDecoder == nullptr) {
    return;
  }

  ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

  (void)pInput;
}

std::string get_formatted_time(int length) {
  int seconds = length % 60;
  int minutes = (length - seconds) / 60;
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << int(minutes) << ":"
      << std::setfill('0') << std::setw(2) << int(seconds);
  return oss.str();
}

void update_song_info(AudioFile song, AudioInfo *audioInfo) {
  audioInfo->currentSong.album = song.album;
  audioInfo->currentSong.artist = song.artist;
  audioInfo->currentSong.bitrate = song.bitrate;
  audioInfo->currentSong.channels = song.channels;
  audioInfo->currentSong.comment = song.comment;
  audioInfo->currentSong.genre = song.genre;
  audioInfo->currentSong.ID = song.ID;
  audioInfo->currentSong.length = song.length;
  audioInfo->currentSong.path = song.path;
  audioInfo->currentSong.sampleRate = song.sampleRate;
  audioInfo->currentSong.title = song.title;
  audioInfo->currentSong.track = song.track;
  audioInfo->currentSong.year = song.year;
  audioInfo->set_length(song.length);
  audioInfo->set_sample_rate(song.sampleRate);

  // extract cover and save to file and load
  TagLib::FileRef fref(audioInfo->currentSong.path.c_str());

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
  bool ret = LoadTextureFromFile("cover.jpg", &audioInfo->currentSong.texture, &defaultImageWidth, &defaultImageHeight);
  IM_ASSERT(ret);
}

void autoplay_next_song(AudioDevice *audioDevice, AudioInfo *audioInfo)
{
  if (audioInfo->maxSeconds != 0 && audioInfo->currentSeconds >= audioInfo->maxSeconds) {
    audioInfo->inc_index();
    if (audioInfo->get_index() > (int)audioFiles.size()) {
      audioInfo->set_index(0);
    }

    audioDevice->play(audioFiles[audioInfo->get_index()].path, false, 0);
    update_song_info(audioFiles[audioInfo->get_index()], audioInfo);
    audioInfo->selection.clear();
    audioInfo->selection.push_back(audioInfo->get_index());
  }
}

bool AudioDevice::init()
{
  result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
      std::cerr << "Error: Failed to initialize audio engine" << std::endl;
      return false;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = ma_data_callback;
    deviceConfig.pUserData = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
      std::cerr << "Error: Failed to open playback device" << std::endl;
      ma_decoder_uninit(&decoder);
      return false;
    }

    ma_engine_set_volume(&engine, volume);

    return true;
}

void AudioDevice::play(std::string &path, bool paused = false, ma_uint64 pcmFrame = 0) {
  result = ma_sound_stop(&sound);
  if (result != MA_SUCCESS)
  {
    printf("Could not stop sound\n");
  }
  else
  {
    ma_sound_uninit(&sound);
  }

  result = ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, &sound);
  if (result != MA_SUCCESS)
  {
    printf("Could not play file: %s\n", path.c_str());
  }

  if (!paused)
  {
    ma_sound_set_start_time_in_pcm_frames(&sound, ma_engine_get_time(&engine) + (ma_engine_get_sample_rate(&engine) * 2));
  }
  else
  {
    ma_sound_seek_to_pcm_frame(&sound, pcmFrame);
  }

  ma_sound_start(&sound);
}

void AudioDevice::seek(float seconds)
{
  int pcmFrame = get_pcm_frame(seconds);
  ma_sound_seek_to_pcm_frame(&sound, pcmFrame);
}

void AudioDevice::poll(AudioInfo *audioInfo)
{
  ma_sound_get_cursor_in_seconds(&sound, &audioInfo->currentSeconds);
  ma_sound_get_length_in_seconds(&sound, &audioInfo->maxSeconds);
}

void AudioDevice::cleanup()
{
  ma_sound_uninit(&sound);
  ma_device_uninit(&device);
}
