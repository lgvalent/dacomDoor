#include "SPIFFS.h"
#include <FS.h>
#include <SPI.h>
#include <Time.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT_SPIFFS_IF_FAILED true

typedef struct SQL_ROW {
  int n;
  char **data;
  char **cols;
  bool allocated;

  SQL_ROW() { allocated = false; }
} SQL_ROW;
String SQL_NULL = "@@NULL";

// sql result manipulation
void sql_row_free(SQL_ROW *row) {
  if (!row->allocated)
    return;

  for (int i = 0; i < row->n; i++) {
    free(row->data[i]);
    free(row->cols[i]);
  }
  free(row->data);
  free(row->cols);
  row->allocated = false;
}

void sql_row_init(SQL_ROW *row, int n, char **data, char **coll) {
  if (row->allocated)
    sql_row_free(row);
  row->n = n;
  row->data = (char **)calloc(n, sizeof(char **));
  row->cols = (char **)calloc(n, sizeof(char **));
  for (int i = 0; i < n; i++) {
    row->data[i] = strdup(data[i]);
    row->cols[i] = strdup(coll[i]);
  }
  row->allocated = true;
}

String sql_row_get_col(SQL_ROW *row, String colName) {
  if (!row->allocated) {
    return SQL_NULL;
  }
  int i;
  for (i = 0; i < row->n; i++) {
    if (colName.equals(row->cols[i])) {
      if (row->data[i]) {
        return String(row->data[i]);
      } else {
        return SQL_NULL;
      }
    }
  }
  return SQL_NULL;
}

void sql_row_print1(int n, char **data, char **cols) {
  int i;
  for (i = 0; i < n; i++) {
    Serial.print(cols[i]);
    Serial.print(": ");
    if (data[i]) {
      Serial.println(data[i]);
    } else {
      Serial.println(SQL_NULL);
    }
  }
}

void sql_row_print(SQL_ROW *row) {
  if (!row->allocated) {
    return;
  }
  sql_row_print1(row->n, row->data, row->cols);
}

// globals
sqlite3 *db;
long db_event_id = 1;
SQL_ROW db_last_result;
bool db_query_returned = false;
bool db_query_error = false;

#define SQL_LIMIT_LENGTH 256

#define DOOR_PIN 12
#define DOOR_OPEN_SECONDS 5

// utils
String pad_2_zero(int x) { return x > 9 ? String(x) : "0" + String(x); }

bool is_query_returned() {
  // Consume the information of db_query_returned
  // On call, reset the value of db_query_returned to false
  if (db_query_returned) {
    db_query_returned = false;
    return true;
  }
  return false;
}

bool is_query_error() {
  // Consume the information of db_query_error
  // On call, reset the value of db_query_error to false
  if (db_query_error) {
    db_query_error = false;
    return true;
  }
  return false;
}

String dia_da_semana(int day) {
  switch (day) {
  case 1:
    return "Domingo";
  case 2:
    return "Segunda";
  case 3:
    return "Terca";
  case 4:
    return "Quarta";
  case 5:
    return "Quinta";
  case 6:
    return "Sexta";
  case 7:
    return "Sabado";
  }
  return "Domingo";
}

// database utils
int db_open(const char *filename, sqlite3 **db) {
  int rc = sqlite3_open(filename, db);
  if (rc) {
    Serial.printf("[ERROR]: Can't open database: %s\n", sqlite3_errmsg(*db));
    return rc;
  }
  return rc;
}

int get_first_callback(void *id, int n, char **data, char **cols) {
  if (db_query_returned)
    return 0;
  sql_row_init(&db_last_result, n, data, cols);
  db_query_returned = true;
  return 0;
}

int print_callback(void *id, int n, char **data, char **cols) {
  sql_row_print1(n, data, cols);
  Serial.println("<==== END ROW ====>");
  return 0;
}

int db_exec_with(const char *sql, int (*cb)(void *, int, char **, char **)) {
  char *zErrMsg = NULL;
  Serial.println("[LOG]: Executing SQL: '''");
  Serial.println(sql);
  Serial.println("'''");
  long start = micros();
  int rc = sqlite3_exec(db, sql, cb, NULL, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.print(F("[ERROR]: SQL error: "));
    Serial.println(zErrMsg);
    sqlite3_free(zErrMsg);
    db_query_error = true;
  }
  Serial.print(F("[LOG]: Time taken: "));
  Serial.println(micros() - start);
  return rc;
}

int db_exec(const char *sql) { return db_exec_with(sql, get_first_callback); }

int db_exec_print(const char *sql) { return db_exec_with(sql, print_callback); }

int db_exec_str(String sql) {
  int n = SQL_LIMIT_LENGTH - 1;
  if (sql.length() > n) {
    return 1;
  }

  char buffer[SQL_LIMIT_LENGTH];
  sql.toCharArray(buffer, n);
  return db_exec(buffer);
}

int db_init_session() { return db_open("/spiffs/database.db", &db); }

void db_close_session() { sqlite3_close(db); }

// queries
String sql_query_rfid(String rfid) {
  String query = "SELECT tipo \n"
                 "FROM rfids_permitidos\n"
                 "WHERE rfid = '{rfid}'\n"
                 "LIMIT 1;";
  query.replace("{rfid}", rfid);
  return query;
}

String sql_query_is_allowed_hour(String user_type) {
  // https://playground.arduino.cc/Code/Time/
  String query = "SELECT id\n"
                 "FROM horarios_permitidos\n"
                 "WHERE (\n"
                 "\ttipo_usuario = '{tipo_usuario}' and\n"
                 "\tdia = '{dia}' and\n"
                 "\thora_inicio <= TIME('{hora}') and\n"
                 "\thora_fim >= TIME('{hora}')\n"
                 ")\n"
                 "LIMIT 1;";

  time_t t = now();
  int weekDay = weekday(t);
  int h = hour(t);
  int m = minute(t);
  int s = second(t);

  String hStr = pad_2_zero(h);
  String mStr = pad_2_zero(m);
  String sStr = pad_2_zero(s);

  String hour = hStr + ":" + mStr + ":" + sStr;

  query.replace("{tipo_usuario}", user_type);
  query.replace("{dia}", dia_da_semana(weekDay));
  query.replace("{hora}", hour);
  return query;
}

String sql_insert_event(long id, String rfid, time_t t, String type) {
  String query = "INSERT INTO eventos VALUES \n"
                 "({id}, '{rfid}', '{tipo}', '{horario}')";

  int h = hour(t);
  int m = minute(t);
  int s = second(t);
  int d = day(t);
  int mo = month(t);
  int y = year(t);

  String dStr = pad_2_zero(d);
  String moStr = pad_2_zero(mo);
  String hStr = pad_2_zero(h);
  String mStr = pad_2_zero(m);
  String sStr = pad_2_zero(s);

  String date_hour = String(y) + "-" + moStr + "-" + dStr + "T" + hStr + ":" +
                     mStr + ":" + sStr;
  query.replace("{id}", String(id));
  query.replace("{rfid}", rfid);
  query.replace("{tipo}", type);
  query.replace("{horario}", date_hour);
  return query;
}

// reader
void reader_open() {
  pinMode(DOOR_PIN, OUTPUT);
  digitalWrite(DOOR_PIN, HIGH);
  sleep(DOOR_OPEN_SECONDS);
  digitalWrite(DOOR_PIN, LOW);
}

bool reader_manage_door(String rfid) {
  if (db_init_session()) {
    return false;
  }

  db_exec_str(sql_query_rfid(rfid));

  bool result = false;
  String user_type = sql_row_get_col(&db_last_result, "tipo");
  Serial.print("[LOG]: User type: ");
  Serial.println(user_type);

  if (is_query_returned() && !user_type.equals(SQL_NULL)) {
    if (!user_type.equalsIgnoreCase("aluno")) {
      result = true;
    } else {
      db_exec_str(sql_query_is_allowed_hour(user_type));
      result = is_query_returned();
    }
  }

  sql_row_free(&db_last_result);
  db_close_session();
  return result;
}

void reader_save_event(String rfid, time_t current_time, String event_type) {
  if (db_init_session()) {
    return;
  }

  db_exec_str(sql_insert_event(db_event_id, rfid, current_time, event_type));

  if (!is_query_error()) {
    db_event_id++;
  }

  db_close_session();
}

void reader_check_credentials(String rfid, String event_type) {
  bool is_allowed = reader_manage_door(rfid);
  if (is_allowed) {
    Serial.print("[LOG]: rfid allowed: [");
    Serial.print(rfid);
    Serial.println("]");

    reader_open();
    reader_save_event(rfid, now(), event_type);
  } else {
    Serial.print("[LOG]: rfid NOT allowed: [");
    Serial.print(rfid);
    Serial.println("]");
  }
}

// setup
void initialize_auto_increment_eventos() {
  String id;
  db_exec("SELECT id from eventos ORDER BY id DESC LIMIT 1");
  if (is_query_returned()) {
    id = sql_row_get_col(&db_last_result, "id");
    if (!id.equals(SQL_NULL)) {
      db_event_id = id.toInt() + 1L;
    }
  }
}

void initialize_database() {
  int rc;
  String id;

  if (db_init_session())
    goto finish;

  // Create tables
  rc = db_exec("CREATE TABLE IF NOT EXISTS rfids_permitidos (\n"
               "\tid INTEGER,\n"
               "\trfid varchar(255),\n"
               "\ttipo varchar(255),\n"
               "\tPRIMARY KEY (id)\n"
               ");");
  if (rc != SQLITE_OK)
    goto finish;

  rc = db_exec("CREATE TABLE IF NOT EXISTS horarios_permitidos (\n"
               "\tid INTEGER,\n"
               "\tdia varchar(32),\n"
               "\ttipo_usuario varchar(255),\n"
               "\thora_inicio TIME,\n"
               "\thora_fim TIME,\n"
               "\tPRIMARY KEY (id)\n"
               ");");
  if (rc != SQLITE_OK)
    goto finish;

  rc = db_exec("CREATE TABLE IF NOT EXISTS eventos (\n"
               "\tid INTEGER,\n"
               "\trfid varchar(255),\n"
               "\ttipo varchar(255),\n"
               "\thorario DATETIME,\n"
               "\tPRIMARY KEY (id)\n"
               ");");
  if (rc != SQLITE_OK)
    goto finish;

  initialize_auto_increment_eventos();

  db_exec("INSERT INTO horarios_permitidos VALUES\n"
          "(1, 'Segunda', 'aluno',     TIME('08:00:00'), TIME('12:00:00')),\n"
          "(3, 'Terca',   'aluno',     TIME('14:00:00'), TIME('23:20:00')),\n"
          "(4, 'Terca',   'aluno',     TIME('08:00:00'), TIME('12:00:00'));");

  db_exec("INSERT INTO rfids_permitidos VALUES\n"
          "(1, '0', 'aluno'),\n"
          "(2, '1', 'professor');");

  {
  finish:
    db_close_session();
  }
}

void setup() {
  // fake date
  setTime(17, 20, 0, 2, 2, 2021);

  // wait a time to Arduino Serial Monitor opens
  Serial.begin(115200);
  delay(1000);

  /*
  // Initialize file system handle
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("[ERROR]: Failed to mount file system");
  }

  // Drop database
  // SPIFFS.remove("/database.db");
  // delay(1000);

  // Initialize sqlite3 library
  sqlite3_initialize();

  // Initialize database and create tables
  initialize_database();

  if (false) {
    db_init_session();
    Serial.println("==================================================");
    db_exec_print("SELECT * from rfids_permitidos;");
    Serial.println("==================================================");
    db_exec_print("SELECT * from horarios_permitidos;");
    Serial.println("==================================================");
    db_exec_print("SELECT * from eventos;");
    Serial.println("==================================================");
    db_close_session();
  }
  */
}

// loop
void loop() {}
