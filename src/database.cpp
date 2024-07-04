#include "database.hpp"

static int sql_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  (void)NotUsed;
  for (int i = 0; i < argc; i++)
  {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

static int sql_db_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  (void)NotUsed;
  AudioFile file;
  for (int i = 0; i < argc; i++)
  {
    if (strcmp(azColName[i], "ID") == 0)
    {
      file.ID = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "TITLE") == 0)
    {
      file.title = argv[i];
    }
    else if (strcmp(azColName[i], "ARTIST") == 0)
    {
      file.artist = argv[i];
    }
    else if (strcmp(azColName[i], "ALBUM") == 0)
    {
      file.album = argv[i];
    }
    else if (strcmp(azColName[i], "YEAR") == 0)
    {
      file.year = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "TRACK") == 0)
    {
      file.track = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "GENRE") == 0)
    {
      file.genre = argv[i];
    }
    else if (strcmp(azColName[i], "CHANNELS") == 0)
    {
      file.channels = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "COMMENT") == 0)
    {
      file.comment = argv[i];
    }
    else if (strcmp(azColName[i], "BITRATE") == 0)
    {
      file.bitrate = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "LENGTH") == 0)
    {
      file.length = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "SAMPLE_RATE") == 0)
    {
      file.sampleRate = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "LOCATION") == 0)
    {
      file.path = argv[i];
    }

  }
  audioFiles.push_back(file);

  return 0;
}

bool sql_create_table()
{
  databaseContext.rc = sqlite3_open("orca.db", &databaseContext.db);
  if (databaseContext.rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(databaseContext.db));
    return -1;
  } else {
    fprintf(stderr, "Opened database successfully\n");
  }

  databaseContext.sql = "CREATE TABLE MUSIC(" \
  "ID INTEGER PRIMARY KEY   NOT NULL," \
  "TITLE          TEXT  NOT NULL," \
  "ARTIST         TEXT  ," \
  "ALBUM          TEXT  ," \
  "YEAR           INT   ," \
  "TRACK          INT   ," \
  "GENRE          TEXT  ," \
  "CHANNELS       INT   ," \
  "COMMENT        TEXT  ," \
  "BITRATE        INT   ," \
  "LENGTH         INT   ," \
  "SAMPLE_RATE    INT   ," \
  "LOCATION       TEXT);";

  databaseContext.rc = sqlite3_exec(databaseContext.db, databaseContext.sql, sql_callback, 0, &databaseContext.zErrMsg);

  if (databaseContext.rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL ERROR: %s\n", databaseContext.zErrMsg);
    sqlite3_free(databaseContext.zErrMsg);
    return 1;
  } else {
    fprintf(stdout, "Table created successfully\n");
  }
  sqlite3_close(databaseContext.db);
  return 0;
}

bool sql_create_playlists_table()
{

  databaseContext.rc = sqlite3_open("orca.db", &databaseContext.db);
  if (databaseContext.rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(databaseContext.db));
    return -1;
  } else {
    fprintf(stderr, "Opened database successfully\n");
  }

  databaseContext.sql = "CREATE TABLE playlist("
                        "id INTEGER PRIMARY KEY   NOT NULL,"
                        "title          TEXT  NOT NULL,"
                        "song_id         INT);";

  databaseContext.rc = sqlite3_exec(databaseContext.db, databaseContext.sql, sql_callback, 0, &databaseContext.zErrMsg);

  if (databaseContext.rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL ERROR: %s\n", databaseContext.zErrMsg);
    sqlite3_free(databaseContext.zErrMsg);
    return 1;
  } else {
    fprintf(stdout, "Table created successfully\n");
  }
  sqlite3_close(databaseContext.db);
  return 0;
}

bool sql_open()
{
  databaseContext.rc = sqlite3_open("orca.db", &databaseContext.db);
  databaseContext.sql = "SELECT * from MUSIC";
  databaseContext.rc = sqlite3_exec(databaseContext.db, databaseContext.sql, sql_db_callback, 0, &databaseContext.zErrMsg);

  if (databaseContext.rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL ERROR: %s\n", databaseContext.zErrMsg);
    sqlite3_free(databaseContext.zErrMsg);
    return 1;
  } else {
    fprintf(stdout, "Operation done successfully\n");
  }

  sqlite3_close(databaseContext.db);
  return 0;
}


/*bool sql_open_playlist(std::string title)
{
  databaseContext.rc = sqlite3_open("orca.db", &databaseContext.db);
  databaseContext.sql = "SELECT * from playlist WHERE title='" + title + "'";
  databaseContext.rc = sqlite3_exec(databaseContext.db, databaseContext.sql, sql_db_callback, 0, &databaseContext.zErrMsg);

  if (databaseContext.rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL ERROR: %s\n", databaseContext.zErrMsg);
    sqlite3_free(databaseContext.zErrMsg);
    return 1;
  } else {
    fprintf(stdout, "Operation done successfully\n");
  }

  sqlite3_close(databaseContext.db);
  return 0;
}*/

bool sql_insert(AudioFile *file)
{
  databaseContext.rc = sqlite3_open("orca.db", &databaseContext.db);
  std::string ss = "INSERT INTO MUSIC (TITLE, ARTIST, ALBUM, YEAR, TRACK, GENRE, CHANNELS, COMMENT, BITRATE, LENGTH, SAMPLE_RATE, LOCATION)" \
  "VALUES('" + file->title + "','" + file->artist + "','" + file->album + "','" + std::to_string(file->year) + "','" + std::to_string(file->track) + "','" + file->genre + "','" + std::to_string(file->channels) + "','" + file->comment + "','" + std::to_string(file->bitrate) + "','" + std::to_string(file->length) + "','" + std::to_string(file->sampleRate) + "','" + file->path + "');";

  databaseContext.rc = sqlite3_exec(databaseContext.db, ss.c_str(), sql_callback, 0, &databaseContext.zErrMsg);

  if (databaseContext.rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL ERROR: %s\n", databaseContext.zErrMsg);
    sqlite3_free(databaseContext.zErrMsg);
    return 1;
  } else {
    fprintf(stdout, "Records created successfully\n");
  }
  sqlite3_close(databaseContext.db);
  return 0;
}

bool sql_insert_new_playlist(char *title)
{
    databaseContext.rc = sqlite3_open("orca.db", &databaseContext.db);
    std::string ss = "INSERT INTO playlist (title) VALUES('" + std::string(title) + "');";

    databaseContext.rc = sqlite3_exec(databaseContext.db, ss.c_str(), sql_callback, 0, &databaseContext.zErrMsg);

    if (databaseContext.rc != SQLITE_OK)
    {
      fprintf(stderr, "SQL ERROR: %s\n", databaseContext.zErrMsg);
      sqlite3_free(databaseContext.zErrMsg);
      return 1;
    }
    else
    {
      fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(databaseContext.db);
    return 0;
}

/*bool sql_insert_playlist(std::string title, int id)
{
    databaseContext.rc = sqlite3_open("orca.db", &databaseContext.db);
    std::string ss = "INSERT INTO playlist (title, song_id) VALUES('" + std::to_string(title) + "','" + std::to_string(id) + "');";

    databaseContext.rc = sqlite3_exec(databaseContext.db, ss.c_str(), sql_callback, 0, &databaseContext.zErrMsg);

    if (databaseContext.rc != SQLITE_OK)
    {
      fprintf(stderr, "SQL ERROR: %s\n", databaseContext.zErrMsg);
      sqlite3_free(databaseContext.zErrMsg);
      return 1;
    }
    else
    {
      fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(databaseContext.db);
    return 0;
}*/
