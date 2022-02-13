#ifndef APP_CONFIG
#define APP_CONFIG

// GLOBALS

// Specify board version, with pins definition "-v 2|3|4|.."
static int BOARD_VERSION = 6;
// Specify server URL with name and port like "-u http://localhost:5000"
// static char URL_SERVER[] = "http://dacom.cm.utfpr.edu.br:19500";
static char URL_SERVER[] = "http://localhost:3430";
// Specify room code (may be room short name) like "-r E003"
static char ROOM_NAME[] = "CAFE";
// ... ?
static char ROOM_LAST_UPDATE_FAKE = "2001-01-01 00:00:00";
// Specify delay time, in seconds, to update each buffer "-d 60"
static int UPDATE_DELAY = 60;
// Specify delay time, in seconds, to keep relay door active "-d .5" for half second
static double RELAY_DELAY = 0.1;
// Specify delay time, in seconds, to alert door opened
static int DOOR_OPENED_ALERT_DELAY = 30;

#endif