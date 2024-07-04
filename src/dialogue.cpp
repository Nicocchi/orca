#include "audio.hpp"
#include "database.hpp"

std::string lastOpenedDirectory = "";

bool open_file_directory() {
  // Check that a backend is available
  if (!pfd::settings::available()) {
    printf("PFD is not available on this platform.\n");
    return false;
  }

  // Set verbosity to true
  // TODO: disable in release
  pfd::settings::verbose(true);

  std::string directoryPath = pfd::path::home();

  if (lastOpenedDirectory != "") {
    directoryPath = lastOpenedDirectory;
  }
  std::string directory =
      pfd::select_folder("Select any directory", directoryPath).result();
  if (directory.empty()) {
    return false;
  }

  lastOpenedDirectory = directory;

  // File info
  for (const auto &directoryEntry :
       std::filesystem::recursive_directory_iterator(directory)) {
    if (std::filesystem::path(directoryEntry).extension() == ".wav" ||
        std::filesystem::path(directoryEntry).extension() == ".flac" ||
        std::filesystem::path(directoryEntry).extension() == ".ogg") {
      TagLib::FileRef fref(directoryEntry.path().c_str());

      AudioFile file;
      file.ID = audioFiles.size();
      file.path = directoryEntry.path().c_str();

      // tag attributes
      if (!fref.isNull() && fref.tag()) {
        TagLib::Tag *tag = fref.tag();

        if (tag->title() != "") {
          file.title = tag->title().toCString(true);
        } else {
          file.title = directoryEntry.path().filename().c_str();
        }

        file.artist = tag->artist().toCString(true);
        file.album = tag->album().toCString(true);
        file.comment = tag->comment().toCString(true);
        file.genre = tag->genre().toCString(true);
        file.year = tag->year();
        file.track = tag->track();
      }

      // properties
      if (!fref.isNull() && fref.audioProperties()) {
        TagLib::AudioProperties *properties = fref.audioProperties();

        file.bitrate = properties->bitrate();
        file.sampleRate = properties->sampleRate();
        file.channels = properties->channels();
        file.length = properties->lengthInSeconds();
      }

      sql_insert(&file);
      audioFiles.push_back(file);
    }
  }

  return true;
}
