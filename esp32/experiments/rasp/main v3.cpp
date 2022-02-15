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

    this->exec("DROP TABLE IF EXISTS alembic_version;");
    this->exec("CREATE TABLE alembic_version (\n"
               "  version_num varchar(32) NOT NULL,\n"
               "  PRIMARY KEY (version_num)\n"
               ");");
    this->exec("INSERT INTO alembic_version VALUES ('9267fd87f3bd');");

    this->exec("DROP TABLE IF EXISTS events;");
    this->exec("CREATE TABLE events (\n"
               "  id int(11) NOT NULL,\n"
               "  uid varchar(16) NOT NULL,\n"
               "  time datetime NOT NULL,\n"
               "  eventType varchar(4) NOT NULL,\n"
               "  PRIMARY KEY (id)\n"
               ");");

    this->exec("DROP TABLE IF EXISTS keyrings;");
    this->exec("CREATE TABLE keyrings (\n"
               "  id int(11) NOT NULL,\n"
               "  uid varchar(16) NOT NULL,\n"
               "  userId int(11) NOT NULL,\n"
               "  userType varchar(10) NOT NULL,\n"
               "  lastUpdate datetime NOT NULL,\n"
               "  PRIMARY KEY(id)\n"
               ");");
    this->exec("INSERT INTO keyrings VALUES (1,'28:D0:9F:59:3E',1,'PROFESSOR','0000-00-00 00:00:00'),(2,'F6:B0:91:89:5E',1,'STUDENT','0000-00-00 00:00:00'),(3,'1A008D81DDCB',1,'PROFESSOR','0000-00-00 00:00:00');");

    this->exec("DROP TABLE IF EXISTS schedules;");
    this->exec("CREATE TABLE schedules (\n"
               "  id int(11) NOT NULL,\n"
               "  dayOfWeek varchar(10) NOT NULL,\n"
               "  beginTime time NOT NULL,\n"
               "  endTime time NOT NULL,\n"
               "  userType varchar(10) NOT NULL,\n"
               "  lastUpdate datetime NOT NULL,\n"
               "  PRIMARY KEY (id)\n"
               ");");
    this->exec("INSERT INTO schedules VALUES (1,'SUNDAY','00:00:00','23:00:00','STUDENT','0000-00-00 00:00:00');");

    this->closeSession();
  }

  int exec(const char *sql)
  {
    delay(250);
    return this->execWith(sql, noop_callback);
  }
  int exec1(const char *sql)
  {
    int rc = this->initSession();
    if (rc != SQLITE_OK)
      return rc;

    rc = this->exec(sql);
    this->closeSession();
    return rc;
  }
  int execPrint(const char *sql)
  {
    delay(250);
    return this->execWith(sql, print_callback);
  }
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

  SPIFFS.remove("/database.db");
  delay(1000);
  db.init();
}

void loop()
{
}
