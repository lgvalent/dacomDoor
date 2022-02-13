#include "SPIFFS.h"
#include <FS.h>
#include <MFRC522.h>
#include <sqlite3.h>

// user type: 'STUDENT','PROFESSOR','EMPLOYEE'
// day of week: 'SUNDAY','MONDAY','THUESDAY','WEDNESDAY','THURSDAY','FRIDAY','SATURDAY'

#define FORMAT_SPIFFS_IF_FAILED true

String SQL_NULL = "@@NULL";

void sql_row_print1(int n, char **data, char **cols)
{
  int i;
  for (i = 0; i < n; i++)
  {
    Serial.print(cols[i]);
    Serial.print(": ");
    if (data[i])
      Serial.println(data[i]);
    else
      Serial.println(SQL_NULL);
  }
}

int noop_callback(void *id, int n, char **data, char **cols) { return 0; }
int print_callback(void *id, int n, char **data, char **cols)
{
  sql_row_print1(n, data, cols);
  Serial.println("<==== END ROW ====>");
  return 0;
}

class SqliteDB
{
private:
  const char *filename;
  sqlite3 *db;

protected:
  int initSession()
  {
    int rc = sqlite3_open(this->filename, &this->db);
    if (rc)
    {
      Serial.printf("[ERROR]: Can't open database: %s\n", sqlite3_errmsg(this->db));
      return rc;
    }
    return rc;
  }

  void closeSession() { sqlite3_close(this->db); }

  int execWith(const char *sql, int (*cb)(void *, int, char **, char **))
  {
    char *zErrMsg = NULL;
    Serial.println("[LOG]: Executing SQL: '''");
    Serial.println(sql);
    Serial.println("'''");
    long start = micros();
    int rc = sqlite3_exec(this->db, sql, cb, NULL, &zErrMsg);
    if (rc != SQLITE_OK)
    {
      Serial.print(F("[ERROR]: SQL error: "));
      Serial.println(zErrMsg);
      sqlite3_free(zErrMsg);
    }
    Serial.print(F("[LOG]: Time taken: "));
    Serial.println(micros() - start);
    return rc;
  }

public:
  SqliteDB(const char *filename)
  {
    this->filename = filename;
  }

  void init(const char *startup_sql_filepath)
  {
    // Initialize sqlite3 library
    sqlite3_initialize();

    int rc = this->initSession();
    if (rc != SQLITE_OK)
      return;

    File file = SPIFFS.open(startup_sql_filepath, FILE_READ);

    while (file.available())
    {
      String cmd = file.readStringUntil(';');
      if (cmd.length() == 0)
      {
        break;
      }
      else
      {
        rc = this->exec(cmd.c_str());
      }
    }

    file.close();
  }

  int exec(const char *sql) { return this->execWith(sql, noop_callback); }
  int exec1(const char *sql)
  {
    int rc = this->initSession();
    if (rc != SQLITE_OK)
      return rc;

    rc = this->exec(sql);
    this->closeSession();
    return rc;
  }
  int execPrint(const char *sql) { return this->execWith(sql, print_callback); }
  int execPrint1(const char *sql)
  {
    int rc = this->initSession();
    if (rc != SQLITE_OK)
      return rc;

    rc = this->execPrint(sql);
    this->closeSession();
    return rc;
  }
};

char STARTUP_SQL_FILEPATH[] = "/startup.sql";
SqliteDB db("/spiffs/database.db");

void setup()
{
  // Wait a time to Arduino Serial Monitor opens
  Serial.begin(115200);
  delay(1000);

  // Initialize file system handle
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("[ERROR]: Failed to mount file system");
  }

  db.init(STARTUP_SQL_FILEPATH);
}

void loop()
{
}
