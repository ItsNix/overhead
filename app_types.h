// Types used in overhead.ino function signatures. They live in a header so
// Arduino's auto-generated prototypes (inserted above the sketch's own code)
// can see them.
#pragma once
#include <stdint.h>

// Which backlight schedule applies right now (off wins over dim).
enum Schedule { SCHED_NORMAL, SCHED_DIM, SCHED_OFF };

// One row of the on-screen settings.
enum RowKind { ROW_TOGGLE, ROW_PERCENT, ROW_TIME, ROW_CHOICE, ROW_ACTION };
struct ConfigRow {
  const char*     label;
  RowKind         kind;
  void*           value;          // bool* / uint8_t* / uint16_t*; none for ROW_ACTION
  uint8_t         min, max;       // ROW_PERCENT range
  const uint16_t* choices;        // ROW_CHOICE: ascending values for a uint16_t
  uint8_t         nChoices;
  const char*     unit;           // ROW_CHOICE: shown after the value
};

// Where aircraft positions come from.
enum Source   { SRC_AUTO, SRC_LOCAL, SRC_ONLINE };   // auto = local receiver, else online
enum Provider { PROV_AIRPLANES_LIVE, PROV_ADSB_LOL };

#define ALT_GROUND   (-100000)
#define ALT_UNKNOWN  (-200000)

// One aircraft from the feed, as of the fetch that produced it.
struct Aircraft {
  char     hex[7];        // ICAO 24-bit address, lowercase
  char     flight[9];     // callsign, trailing spaces trimmed; may be empty
  char     type[5];       // ICAO type designator (online feeds), e.g. B738
  char     reg[11];       // registration (online feeds)
  char     squawk[5];
  char     cat[3];        // emitter category, e.g. A3
  double   lat, lon;
  int32_t  alt;           // feet; ALT_GROUND / ALT_UNKNOWN
  int16_t  gs;            // knots; -1 = unknown
  int16_t  track;         // degrees true; -1 = unknown
  int16_t  rate;          // vertical rate, ft/min
  float    seen_pos;      // seconds since the position was received, at fetch time
  float    dist_nm;       // from home, at fetch time
  float    bearing;       // from home, degrees true
  uint32_t fetched;       // millis() of the fetch
};

// What the fetch task needs from the settings (copied under the data mutex).
struct FetchConfig {
  double   lat, lon;
  bool     home_set;
  uint16_t range_nm;
  uint8_t  source;        // Source
  uint8_t  provider;      // Provider
  uint16_t poll_s;
  char     local_url[128];
};

// Recent positions of one aircraft, a ring buffer of the last n points.
#define TRAIL_POINTS 40
struct Trail {
  char     hex[7];        // "" = free slot
  uint8_t  n, head;
  float    lat[TRAIL_POINTS], lon[TRAIL_POINTS];
  uint32_t lastSeen;      // millis()
};
