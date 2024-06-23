#pragma once

#include "editor.hpp"

struct AudioFile {
  int ID = 0;
  int index = 0;

  std::string title = "";
  std::string artist = "";
  std::string album = "";
  std::string comment = "";
  std::string genre = "";
  std::string path;

  unsigned int year = 0;
  unsigned int track = 0;

  int bitrate = 0;
  int sampleRate = 44100;
  int channels = 2;
  int length = 0;
  
  GLuint texture = 0;

  static const ImGuiTableSortSpecs *s_current_sort_specs;

  // Compare function to be used by qsort()
  static int IMGUI_CDECL CompareWithSortSpecs(const void *lhs,
                                              const void *rhs) {
    const AudioFile *a = (const AudioFile *)lhs;
    const AudioFile *b = (const AudioFile *)rhs;
    for (int n = 0; n < s_current_sort_specs->SpecsCount; n++) {
      // Here we identify columns using the ColumnUserID value that we ourselves
      // passed to TableSetupColumn() We could also choose to identify columns
      // based on their index (sort_spec->ColumnIndex), which is simpler!
      const ImGuiTableColumnSortSpecs *sort_spec =
          &s_current_sort_specs->Specs[n];
      int delta = 0;
      switch (sort_spec->ColumnUserID) {
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
        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1
                                                                          : -1;
      if (delta < 0)
        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1
                                                                          : +1;
    }

    // qsort() is instable so always return a way to differenciate items.
    // Your own compare function may want to avoid fallback on implicit sort
    // specs e.g. a Name compare if it wasn't already part of the sort specs.
    return (a->ID - b->ID);
  }
};

std::vector<AudioFile> audioFiles;
const ImGuiTableSortSpecs *AudioFile::s_current_sort_specs = NULL;

int defaultImageWidth = 0;
int defaultImageHeight = 0;
GLuint defaultTexture = 0;