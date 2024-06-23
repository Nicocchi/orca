#pragma once

#include <stdio.h>
#include <sqlite3.h>

struct DatabaseContext {
	sqlite3 *db;
	char const *sql;
	char *zErrMsg = 0;
	int rc;
} databaseContext;