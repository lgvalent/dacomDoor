#include "SPIFFS.h"
#include <FS.h>
#include <MFRC522.h>
#include <sqlite3.h>
#include <Time.h>

// user type: 'STUDENT','PROFESSOR','EMPLOYEE'
// day of week: 'SUNDAY','MONDAY','THUESDAY','WEDNESDAY','THURSDAY','FRIDAY','SATURDAY'

#define FORMAT_SPIFFS_IF_FAILED true

class SqlRow
{
  int n;
  char **data;
  char **cols;
  bool allocated;

public:
  const static String SQL_NULL;

  SqlRow() { this->allocated = false; }

  void setup(int n, char **data, char **coll)
  {
    this->clear();

    this->n = n;
    this->data = (char **)calloc(n, sizeof(char **));
    this->cols = (char **)calloc(n, sizeof(char **));
    for (int i = 0; i < n; i++)
    {
      this->data[i] = strdup(data[i]);
      this->cols[i] = strdup(coll[i]);
    }
    this->allocated = true;
  }

  void clear()
  {
    if (!this->allocated)
      return;

    for (int i = 0; i < this->n; i++)
    {
      free(this->data[i]);
      free(this->cols[i]);
    }
    free(this->data);
    free(this->cols);
    this->allocated = false;
  }

  String getCol(String colName)
  {
    if (!this->allocated)
      return SQL_NULL;

    for (int i = 0; i < this->n; i++)
    {
      if (colName.equals(this->cols[i]))
      {
        if (this->data[i])
          return String(this->data[i]);
        else
          return SQL_NULL;
      }
    }

    return SQL_NULL;
  }

  static void print(int n, char **data, char **cols)
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

  void print()
  {
    if (this->allocated)
      SqlRow::print(this->n, this->data, this->cols);
  }
};

const String SqlRow::SQL_NULL = "@@NULL";

bool db_query_returned;
SqlRow first_row;

int get_first_callback(void *id, int n, char **data, char **cols)
{
  if (db_query_returned)
    return 0;
  first_row.setup(n, data, cols);
  db_query_returned = true;
  return 0;
}
int noop_callback(void *id, int n, char **data, char **cols) { return 0; }
int print_callback(void *id, int n, char **data, char **cols)
{
  SqlRow::print(n, data, cols);
  Serial.println("<==== END ROW ====>");
  return 0;
}

class SqliteDB
{
private:
  const char *filename;
  sqlite3 *db = NULL;

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

  void init()
  {
    // Initialize sqlite3 library
    sqlite3_initialize();

    int rc = this->initSession();
    if (rc != SQLITE_OK)
      return;

    this->exec("DROP TABLE IF EXISTS events;");
    this->exec("CREATE TABLE IF NOT EXISTS events (\n"
               "  id INTEGER NOT NULL,\n"
               "  uid varchar(16) NOT NULL,\n"
               "  time datetime NOT NULL,\n"
               "  eventType varchar(4) NOT NULL,\n"
               "  PRIMARY KEY (id)\n"
               ");\n");
    this->exec("DROP TABLE IF EXISTS keyrings;");
    this->exec("CREATE TABLE IF NOT EXISTS keyrings (\n"
               "  id INTEGER NOT NULL,\n"
               "  uid varchar(16) NOT NULL,\n"
               "  userId INTEGER NOT NULL,\n"
               "  userType varchar(10) NOT NULL,\n"
               "  lastUpdate datetime NOT NULL,\n"
               "  PRIMARY KEY (id)\n"
               ");");
    this->exec("DROP TABLE IF EXISTS schedules;");
    this->exec("CREATE TABLE IF NOT EXISTS schedules (\n"
               "  id INTEGER NOT NULL,\n"
               "  dayOfWeek varchar(10) NOT NULL,\n"
               "  beginTime time NOT NULL,\n"
               "  endTime time NOT NULL,\n"
               "  userType varchar(10) NOT NULL,\n"
               "  lastUpdate datetime NOT NULL,\n"
               "  PRIMARY KEY (id)\n"
               ");");

    this->exec("INSERT INTO keyrings VALUES "
               "(1,'28:D0:9F:59:3E',1,'PROFESSOR','0000-00-00 00:00:00'),"
               "(2,'F6:B0:91:89:5E',1,'STUDENT','0000-00-00 00:00:00'),"
               "(3,'1A008D81DDCB',1,'PROFESSOR','0000-00-00 00:00:00');");
    this->exec("INSERT INTO schedules VALUES "
               "(1,'SUNDAY','00:00:00','23:00:00','STUDENT','0000-00-00 00:00:00');");

    this->closeSession();
  }

  int exec(const char *sql) { return this->execWith(sql, noop_callback); }
  int exec1(const char *sql) { return this->execWith(sql, get_first_callback); }
  int execPrint(const char *sql) { return this->execWith(sql, print_callback); }
};

SqliteDB db("/spiffs/database.db");

class Utils
{
  static String pad2Zero(int x)
  {
    if (x > 9)
      return String(x);
    else
      return "0" + String(x);
  }
};

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

  SPIFFS.remove("/database.db");
  delay(1000);
  db.init();
}

void loop()
{
}
