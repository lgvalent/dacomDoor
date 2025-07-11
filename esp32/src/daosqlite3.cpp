#ifndef DAO_SQLITE3
#define DAO_SQLITE3

#include <Arduino.h>
#include <sqlite3.h>
#include <vector>
#include <functional>

#include "models.cpp"

#define FILE_NAME "/spiffs/dacomDoor.db"

/**
 * sqlite3_bind_int() uses a 32bit integer and models.cpp::Uid is a u_int32_t compatible
 */

class DBManager {
private:
    sqlite3* db;

    DBManager() {
        sqlite3_initialize();
        if(sqlite3_open(FILE_NAME, &db)){
          Serial.printf("[ERROR]: Can't open database: %s\n", sqlite3_errmsg(db));
        }
      
    }
    ~DBManager() {
        sqlite3_close(db);
    }
public:
    // Prevent copying
    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

    // Singleton accessor
    static DBManager& instance() {
        static DBManager instance;
        return instance;
    }

    sqlite3* getDb() { return db; }
  };

template <typename T>
class DaoGeneric {
  sqlite3 *db;
  std::string tableName;
public:
  using Binder = std::function<void(sqlite3_stmt*, const T&)>;
  using Loader = std::function<T(sqlite3_stmt*)>;
  using Processor = std::function<bool(const T&)>;

  DaoGeneric(const std::string& table): tableName(table) {

    this->db = DBManager::instance().getDb();
  }

  bool executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
      sqlite3_free(errMsg);
      return false;
    }
    return true;
  }

  bool insert(const std::string& sql, const T& model, Binder binder) {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    binder(stmt, model);
    bool result = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return result;
  }

  bool update(const std::string& sql, const T& model, Binder binder) {
    return insert(sql, model, binder); // Same pattern
  }

  bool remove(const std::string& sql, Uid id) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    bool result = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return result;
  }

  std::vector<T> queryAll(const std::string& sql, Loader loader) {
    std::vector<T> results;
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      results.push_back(loader(stmt));
    }
    sqlite3_finalize(stmt);
    return results;
  }

  void processAll(const std::string& sql, Loader loader, Processor processor) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      processor(loader(stmt));
    }
    sqlite3_finalize(stmt);
  }

  std::vector<T> queryAll(const std::string& sql, char filterValue, Loader loader) {
    std::vector<T> results;
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, filterValue);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      results.push_back(loader(stmt));
    }
    sqlite3_finalize(stmt);
    return results;
  }

  T queryOne(const std::string& sql, Uid id, Loader loader) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    T result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
      result = loader(stmt);
    }
    sqlite3_finalize(stmt);
    return result;
  }

};

class KeyringDao {
  DaoGeneric<Keyring> dao;

public:
  KeyringDao() : dao("keyrings") {
    dao.executeSQL("CREATE TABLE IF NOT EXISTS keyrings ("
                   "userId INTEGER PRIMARY KEY, "
                   "uid INTEGER, "
                   "userType INTEGER, "
                   "lastUpdate INTEGER);");
  }

  bool save(const Keyring& k) {
    return dao.insert(
      "INSERT INTO keyrings (userId, uid, userType, lastUpdate) VALUES (?, ?, ?, ?);",
      k,
      [](sqlite3_stmt* stmt, const Keyring& m) {
        sqlite3_bind_int(stmt, 1, m.getUserId());
        sqlite3_bind_int(stmt, 2, m.getUid());
        sqlite3_bind_int(stmt, 3, static_cast<char>(m.getUserType()));
        sqlite3_bind_int64(stmt, 4, m.getLastUpdate());
      }
    );
  }

  bool update(const Keyring& k) {
    return dao.update(
      "UPDATE keyrings SET uid=?, userType=?, lastUpdate=? WHERE userId=?;",
      k,
      [](sqlite3_stmt* stmt, const Keyring& m) {
        sqlite3_bind_int(stmt, 1, m.getUid());
        sqlite3_bind_int(stmt, 2, static_cast<char>(m.getUserType()));
        sqlite3_bind_int64(stmt, 3, m.getLastUpdate());
        sqlite3_bind_int(stmt, 4, m.getUserId());
      }
    );
  }

  bool remove(Uid userId) {
    return dao.remove("DELETE FROM keyrings WHERE userId=?;", userId);
  }

  std::vector<Keyring> findAll() {
    return dao.queryAll(
      "SELECT userId, uid, userType, lastUpdate FROM keyrings;",
      [](sqlite3_stmt* stmt) {
        Uid id = sqlite3_column_int(stmt, 0);
        Uid uid = sqlite3_column_int(stmt, 1);
        UserType type = static_cast<UserType>(sqlite3_column_int(stmt, 2));
        time_t last = sqlite3_column_int64(stmt, 3);
        Keyring m;
        m.build(uid, id, type, last);
        return m;
      }
    );
  }

  Keyring findByUserId(Uid userId) {
    return dao.queryOne(
      "SELECT uid, userType, lastUpdate FROM keyrings WHERE userId=?;",
      userId,
      [userId](sqlite3_stmt* stmt) {
        Uid uid = sqlite3_column_int(stmt, 0);
        UserType type = static_cast<UserType>(sqlite3_column_int(stmt, 1));
        time_t last = sqlite3_column_int64(stmt, 2);
        Keyring model;
        model.build(uid, userId, type, last);
        return model;
      }
    );
  }

  Keyring findByUid(Uid uid) {
    return dao.queryOne(
      "SELECT userId, userType, lastUpdate FROM keyrings WHERE uid=?;",
      uid,
      [uid](sqlite3_stmt* stmt) {
        Uid userId = sqlite3_column_int(stmt, 0);
        UserType type = static_cast<UserType>(sqlite3_column_int(stmt, 1));
        time_t last = sqlite3_column_int64(stmt, 2);
        Keyring model;
        model.build(uid, userId, type, last);
        return model;
      }
    );
  }

  time_t getLastUpdate(){
    Keyring keyring = dao.queryOne(
      "SELECT lastUpdate FROM keyrings WHERE 0=? ORDER BY lastUpdate DESC LIMIT 1;",
      0,
      [](sqlite3_stmt* stmt) {
        time_t last = sqlite3_column_int64(stmt, 0);
        Keyring model;
        model.build(0, 0, UserType::PROFESSOR, last);
        return model;
      }
    );

    return keyring.getLastUpdate();
  }
};

class EventDao {
  DaoGeneric<Event> dao;

public:
  EventDao() : dao("events") {
    dao.executeSQL("CREATE TABLE IF NOT EXISTS events ("
                   "uid INTEGER, "
                   "time INTEGER, "
                   "eventType INTEGER;");
  }

  bool save(const Event& event) {
    return dao.insert(
      "INSERT INTO events (uid, time, eventType) VALUES (?, ?, ?);",
      event,
      [](sqlite3_stmt* stmt, const Event& m) {
        sqlite3_bind_int(stmt, 1, m.getUid());
        sqlite3_bind_int(stmt, 2, m.getTime());
        sqlite3_bind_int(stmt, 3, static_cast<char>(m.getEventType()));
      }
    );
  }

  std::vector<Event> findAll() {
    return dao.queryAll(
      "SELECT uid, time, eventType FROM events;",
      [](sqlite3_stmt* stmt) {
        Uid uid = static_cast<Uid>(sqlite3_column_int(stmt, 0));
        time_t time = sqlite3_column_int64(stmt, 1);
        EventType type = static_cast<EventType>(sqlite3_column_int(stmt, 2));
        Event model;
        model.build(uid, time, type);
        return model;
      }
    );
  }

  void removeAll(){
    dao.remove("DELETE FROM events WHERE 0=?", 0);
  }

  void process(DaoGeneric<Event>::Processor processor) {
    dao.processAll("SELECT uid, time, eventType FROM events;", 
      [](sqlite3_stmt* stmt) {
        Uid uid = static_cast<Uid>(sqlite3_column_int(stmt, 0));
        time_t time = sqlite3_column_int64(stmt, 1);
        EventType type = static_cast<EventType>(sqlite3_column_int(stmt, 2));
        Event model;
        model.build(uid, time, type);
        return model;
      }
        ,processor);
  }
};

class ScheduleDao {
  DaoGeneric<Schedule> dao;

public:
  ScheduleDao() : dao("schedules") {
    dao.executeSQL("CREATE TABLE IF NOT EXISTS schedules ("
                   "id INTEGER PRIMARY KEY, "
                   "dayOfWeek INTEGER, "
                   "beginTime INTEGER, "
                   "endTime INTEGER, "
                   "userType INTEGER, "
                   "lastUpdate INTEGER);");
  }

  bool save(const Schedule& schedule) {
    return dao.insert(
      "INSERT INTO schedules (id, dayOfWeek, beginTime, endTime, userType, lastUpdate) VALUES (?, ?, ?, ?, ?, ?);",
      schedule,
      [](sqlite3_stmt* stmt, const Schedule& m) {
        sqlite3_bind_int(stmt, 1, m.getId());
        sqlite3_bind_int(stmt, 2, static_cast<char>(m.getDayOfWeek()));
        sqlite3_bind_int(stmt, 3, m.getBeginTime());
        sqlite3_bind_int(stmt, 4, m.getEndTime());
        sqlite3_bind_int(stmt, 5, static_cast<char>(m.getUserType()));
        sqlite3_bind_int(stmt, 6, m.getLastUpdate());
      }
    );
  }

  bool update(const Schedule& schedule) {
    return dao.update(
      "UPDATE schedules SET dayOfWeek=?, beginTime=?, endTime=?, userType=?, lastUpdate=? WHERE id=?;",
      schedule,
      [](sqlite3_stmt* stmt, const Schedule& m) {
        sqlite3_bind_int(stmt, 1, static_cast<int>(m.getDayOfWeek()));
        sqlite3_bind_int(stmt, 2, m.getBeginTime());
        sqlite3_bind_int(stmt, 3, m.getEndTime());
        sqlite3_bind_int(stmt, 4, static_cast<char>(m.getUserType()));
        sqlite3_bind_int(stmt, 5, m.getLastUpdate());
        sqlite3_bind_int(stmt, 6, m.getId());
      }
    );
  }

  bool remove(Uid id) {
    return dao.remove("DELETE FROM schedules WHERE id=?;", id);
  }

  std::vector<Schedule> findAll() {
    return dao.queryAll(
      "SELECT id, dayOfWeek, beginTime, endTime, userType, lastUpdate FROM schedules;",
      [](sqlite3_stmt* stmt) {
        Uid id = sqlite3_column_int64(stmt, 0);
        DayOfWeek day = static_cast<DayOfWeek>(sqlite3_column_int(stmt, 1));
        time_t begin = sqlite3_column_int64(stmt, 2);
        time_t end = sqlite3_column_int64(stmt, 3);
        UserType type = static_cast<UserType>(sqlite3_column_int(stmt, 4));
        time_t last = sqlite3_column_int64(stmt, 5);
        Schedule model;
        model.build(id, day, begin, end, type, last);
        return model;
      }
    );
  }

  Schedule findById(Uid id) {
    return dao.queryOne(
      "SELECT dayOfWeek, beginTime, endTime, userType, lastUpdate FROM schedules WHERE id=?;",
      id,
      [id](sqlite3_stmt* stmt) {
        DayOfWeek day = static_cast<DayOfWeek>(sqlite3_column_int(stmt, 0));
        time_t begin = sqlite3_column_int64(stmt, 1);
        time_t end = sqlite3_column_int64(stmt, 2);
        UserType type = static_cast<UserType>(sqlite3_column_int(stmt, 3));
        time_t last = sqlite3_column_int64(stmt, 4);
        Schedule model;
        model.build(id, day, begin, end, type, last);
        return model;
      }
    );
  }

  std::vector<Schedule> findByUserType(UserType userType){
    return dao.queryAll(
      "SELECT id, dayOfWeek, beginTime, endTime, lastUpdate FROM schedules WHERE userType=?;",
      static_cast<char>(userType),
      [userType](sqlite3_stmt* stmt) {
        Uid id = sqlite3_column_int(stmt, 0);
        DayOfWeek day = static_cast<DayOfWeek>(sqlite3_column_int(stmt, 1));
        time_t begin = sqlite3_column_int64(stmt, 2);
        time_t end = sqlite3_column_int64(stmt, 3);
        time_t last = sqlite3_column_int64(stmt, 4);
        Schedule model;
        model.build(id, day, begin, end, userType, last);
        return model;
      }
    );
  }

  bool hasSchedule(UserType userType, time_t time){
    /** TODO filter by time and dayOfWeek */
    Schedule schedule = dao.queryOne(
      "SELECT COUNT(*) FROM schedules WHERE userType=?;",
      static_cast<char>(userType),
      [userType](sqlite3_stmt* stmt) {
        int count = sqlite3_column_int(stmt, 0);
        Schedule model;
        model.build(count, DayOfWeek::SUNDAY, 0, 0, userType, 0);
        return model;
      }
    );
    return schedule.getId() > 0;
  }

  time_t getLastUpdate(){
    Schedule schedule = dao.queryOne(
      "SELECT lastUpdate FROM events WHERE 0=? ORDER BY lastUpdate DESC LIMIT 1;",
      0,
      [](sqlite3_stmt* stmt) {
        time_t last = sqlite3_column_int64(stmt, 0);
        Schedule model;
        model.build(0, DayOfWeek::SUNDAY, 0, 0, UserType::PROFESSOR, last);
        return model;
      }
    );

    return schedule.getLastUpdate();
  }
};


class ConfigDao {
  DaoGeneric<Config> dao;

public:
  ConfigDao() : dao("config") {
    dao.executeSQL("CREATE TABLE IF NOT EXISTS config ("
        "id INTEGER PRIMARY KEY CHECK (id = 1),"
        "boardVersion INTEGER,"
        "serverURL TEXT,"
        "roomName TEXT,"
        "fakeLastUpdate INTEGER,"
        "updateDelay INTEGER,"
        "relayDelay REAL,"
        "doorOpenedAlertDelay INTEGER,"
        "wifiSSID TEXT,"
        "wifiPassword TEXT);");
  }

  bool save(const Config& config) {
    return dao.insert(
      "INSERT OR REPLACE INTO board_config ("
      "id, boardVersion, serverURL, roomName, fakeLastUpdate,"
      "updateDelay, relayDelay, doorOpenedAlertDelay, wifiSSID, wifiPassword"
      ") VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?);",
      config,
      [config](sqlite3_stmt* stmt, const Config& m) {
    sqlite3_bind_int(stmt, 1, config.boardVersion);
    sqlite3_bind_text(stmt, 2, config.serverURL.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, config.roomName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, config.fakeLastUpdate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, config.updateDelay);
    sqlite3_bind_double(stmt, 6, config.relayDelay);
    sqlite3_bind_int(stmt, 7, config.doorOpenedAlertDelay);
    sqlite3_bind_text(stmt, 8, config.wifiSSID.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, config.wifiPassword.c_str(), -1, SQLITE_TRANSIENT);
      }
    );
  }

  Config retrieve() {
    return dao.queryOne(
      "SELECT boardVersion, serverURL, roomName, fakeLastUpdate, updateDelay, relayDelay, doorOpenedAlertDelay, wifiSSID, wifiPassword FROM board_config WHERE id = ?",
      1,
      [](sqlite3_stmt* stmt) {
        Config config;
        config.boardVersion = sqlite3_column_int(stmt, 0);
        config.serverURL = (const char *)sqlite3_column_text(stmt, 1);
        config.roomName =(const char *)sqlite3_column_text(stmt, 2);
        config.fakeLastUpdate = (const char *)sqlite3_column_text(stmt, 3);
        config.updateDelay = sqlite3_column_int(stmt, 4);
        config.relayDelay = sqlite3_column_double(stmt, 5);
        config.doorOpenedAlertDelay = sqlite3_column_int(stmt, 6);
        config.wifiSSID = (const char *)sqlite3_column_text(stmt, 7);
        config.wifiPassword = (const char *)sqlite3_column_text(stmt, 8);
        return config;
      }
    );
  }
};

#endif
