/*
    This creates two empty databases, populates values, and retrieves them back
    from the SPIFFS file
*/
#include "SPIFFS.h"
#include <FS.h>
#include <SPI.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

const char *data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName) {
  int i;
  Serial.printf("%s: ", (const char *)data);
  for (i = 0; i < argc; i++) {
    Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  Serial.printf("\n");
  return 0;
}

int db_open(const char *filename, sqlite3 **db) {
  int rc = sqlite3_open(filename, db);
  if (rc) {
    Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
    return rc;
  } else {
    Serial.printf("Opened database successfully\n");
  }
  return rc;
}

char *zErrMsg = 0;
int db_exec(sqlite3 *db, const char *sql) {
  Serial.println(sql);
  long start = micros();
  int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    Serial.printf("Operation done successfully\n");
  }
  Serial.print(F("Time taken:"));
  Serial.println(micros() - start);
  return rc;
}

int create_tables() {
  
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  sqlite3 *db;
  int rc;

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("Failed to mount file system");
    return;
  }

  // remove existing file
  SPIFFS.remove("/database.db");

  sqlite3_initialize();

  if (db_open("/spiffs/database.db", &db)) {
    return;
  }

  rc = db_exec(db, "CREATE TABLE rfids_permitidos (\n"
                   "\tid INTEGER,\n"
                   "\trfid varchar(255),\n"
                   "\ttipo varchar(255),\n"
                   "\tPRIMARY KEY (id)\n"
                   ");");
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }

  rc = db_exec(db, "CREATE TABLE horarios_permitidos (\n"
                   "\tid INTEGER,\n"
                   "\tdia varchar(32),\n"
                   "\ttipo_usuario varchar(255),\n"
                   "\thora_inicio TIME,\n"
                   "\thora_fim TIME,\n"
                   "\tPRIMARY KEY (id)\n"
                   ");");
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }

  rc = db_exec(db, "CREATE TABLE eventos (\n"
                   "\tid INTEGER,\n"
                   "\trfid varchar(255),\n"
                   "\ttipo varchar(255),\n"
                   "\thorario DATETIME,\n"
                   "\tPRIMARY KEY (id)\n"
                   ");");
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }
}

void loop() {}