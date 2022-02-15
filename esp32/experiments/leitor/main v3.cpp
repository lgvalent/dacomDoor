#include "SPIFFS.h"
#include <FS.h>
#include <SPI.h>
#include <Time.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT_SPIFFS_IF_FAILED true

// globals
sqlite3 *db;
time_t gtime = 1612265968533;

// utils
String dia_da_semana(int day) {
  switch (day) {
  case 1:
    return "Sabado";
  case 2:
    return "Domingo";
  case 3:
    return "Segunda";
  case 4:
    return "Terca";
  case 5:
    return "Quarta";
  case 6:
    return "Quinta";
  case 7:
    return "Sexta";
  }
  return "Domingo";
}

// database utils
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
    Serial.printf("[ERROR]: Can't open database: %s\n", sqlite3_errmsg(*db));
    return rc;
  } else {
    Serial.printf("[LOG]: Opened database successfully\n");
  }
  return rc;
}

char *zErrMsg = 0;
const char *data = "Callback function called";
int db_exec(sqlite3 *db, const char *sql) {
  Serial.println(F("[LOG GROUP START]: db_exec"));
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
  Serial.println(F("[LOG GROUP END]: db_exec"));
  return rc;
}

// manager
String query_rfid_if_is_allowed(char *rfid) {
  String query = "SELECT * \n"
                 "FROM rfids_permitidos\n"
                 "WHERE rfid = '{rfid}'\n"
                 "LIMIT 1;";
  query.replace("{rfid}", String(rfid));
  return query;
}

String query_is_allowed_hour(char *userType) {
  // https://playground.arduino.cc/Code/Time/
  String query = "SELECT *\n"
                 "FROM horarios_permitidos\n"
                 "WHERE (\n"
                 "\ttipo_usuario = '{tipo_usuario}' and\n"
                 "\tdia = '{dia}' and\n"
                 "\thora_inicio <= TIME('{hora}') and\n"
                 "\thora_fim >= TIME('{hora}')\n"
                 ")\n"
                 "LIMIT 1;";

  time_t t = 1611604681946L;
  int weekDay = weekday(t);
  int h = hour(t);
  int m = minute(t);
  int s = second(t);

  // remove timezone from hour
  h -= 3;
  h = h < 0 ? 24 + h : h;

  String hour = String(h) + ":" + String(m) + ":" + String(s);

  Serial.println(t);
  query.replace("{tipo_usuario}", String(userType));
  query.replace("{dia}", dia_da_semana(weekDay));
  query.replace("{hora}", hour);
  return query;
}

// setup
int initialize_database() {
  int rc;

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("[ERROR]: Failed to mount file system");
    return 1;
  }
  // Remove existing database
  SPIFFS.remove("/database.db");

  // Initialize database
  sqlite3_initialize();

  // Create database
  if (db_open("/spiffs/database.db", &db)) {
    return 1;
  }

  // Create tables
  rc = db_exec(db, "CREATE TABLE IF NOT EXISTS rfids_permitidos (\n"
                   "\tid INTEGER,\n"
                   "\trfid varchar(255),\n"
                   "\ttipo varchar(255),\n"
                   "\tPRIMARY KEY (id)\n"
                   ");");
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  rc = db_exec(db, "CREATE TABLE IF NOT EXISTS horarios_permitidos (\n"
                   "\tid INTEGER,\n"
                   "\tdia varchar(32),\n"
                   "\ttipo_usuario varchar(255),\n"
                   "\thora_inicio TIME,\n"
                   "\thora_fim TIME,\n"
                   "\tPRIMARY KEY (id)\n"
                   ");");
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  rc = db_exec(db, "CREATE TABLE IF NOT EXISTS eventos (\n"
                   "\tid INTEGER,\n"
                   "\trfid varchar(255),\n"
                   "\ttipo varchar(255),\n"
                   "\thorario DATETIME,\n"
                   "\tPRIMARY KEY (id)\n"
                   ");");
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  return 0;
}

void setup() {
  // wait until Arduino Serial Monitor opens
  Serial.begin(115200);
  delay(1000);

  // Initialize database and create tables
  // if (initialize_database()) {
  //   Serial.print("[ERROR]: Fail on initilizing database!");
  // }

  char *rfid = "1234";
  char *userType = "aluno";
  Serial.println(query_is_allowed_hour(userType));
  Serial.println(query_rfid_if_is_allowed(rfid));
}

// loop
void loop() {}