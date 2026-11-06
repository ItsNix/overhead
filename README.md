# Overhead

Shows the aircraft flying over your home on an Elecrow CrowPanel 7" ESP32-S3
board (800×480). On the left is a north-up radar centred on home, with range
rings. Each aircraft is drawn as an icon rotated to its heading and colored by
altitude, with its callsign, altitude, a climb/descent arrow and a short trail.
Positions are extrapolated between updates so the icons move smoothly. On the
right are a clock, the nearest aircraft, the feed status and range buttons.

When no aircraft are airborne within range, the screen switches to the
[colorclock](https://github.com/ItsNix/colorclock) face: the background color is
the time (red = hour, green = minute, blue = second), with the hex code, time,
date and RGB values on top. It goes back to the radar as soon as an aircraft
comes into range, and returns to the clock after about 20 s of empty sky. Tap
the clock to look at the radar for 30 s. A small corner tag shows "No WiFi" or
"No aircraft data" when the sky is only empty because the feed is down.

Built on the display and board code from
[colorclock](https://github.com/ItsNix/colorclock).

## Data sources

- **Your own ADS-B receiver**: readsb, dump1090 or tar1090. The display reads
  its `aircraft.json` over the LAN.
- **Online**: [adsb.lol](https://adsb.lol) (default, free, no key) or
  airplanes.live, which now returns 403 until they approve your project by
  email.

**Auto** mode (the default) uses the receiver when one is configured and
answering, and otherwise falls back to the online feed. Receivers don't send
aircraft type or registration, so those appear only with the online feed.

## Supported boards

| Board | Flash | Build define |
|---|---|---|
| CrowPanel 7.0" HMI, SKU DIS08070H (TN), HW V2.x / V3.0 | 4 MB | `BOARD_CROWPANEL_7` (default) |
| CrowPanel Advance 7.0" (IPS), HW V1.3 / V1.4 / V1.5 | 16 MB | `BOARD_CROWPANEL_ADVANCE_7` |

The Advance also has a battery-backed RTC and a buzzer, which this project uses
for the overhead beep.

## Building

You need `arduino-cli` with the `esp32:esp32` core 3.x (tested with 3.3.8) and
three libraries:

```sh
arduino-cli core install esp32:esp32
arduino-cli lib install LovyanGFX WiFiManager ArduinoJson
```

```sh
# Basic CrowPanel 7.0 (DIS08070H)
FQBN=esp32:esp32:esp32s3:PSRAM=opi,FlashMode=dio,FlashSize=4M,PartitionScheme=huge_app,CDCOnBoot=default
arduino-cli compile --fqbn $FQBN --build-path build-basic .
arduino-cli upload  --fqbn $FQBN --build-path build-basic -p /dev/ttyUSB0 .

# CrowPanel Advance 7.0
FQBN=esp32:esp32:esp32s3:PSRAM=opi,FlashMode=dio,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,CDCOnBoot=default
arduino-cli compile --fqbn $FQBN --build-path build-adv \
  --build-property compiler.cpp.extra_flags=-DBOARD_CROWPANEL_ADVANCE_7 .
arduino-cli upload  --fqbn $FQBN --build-path build-adv -p /dev/ttyUSB0 .
```

Don't use the core's `elecrow_crowpanel_7` board definition: it builds for quad
PSRAM and panics at boot.

## First-time setup

1. On first boot the display opens a WiFi hotspot, **`Overhead-Setup`**
   (password `overhead`). Join it and pick your network.
2. Open **http://overhead.local** (or the IP shown on screen) and set your home:
   search for an address, click the map, or drag the pin. The default is
   Pocatello, Idaho. The map uses OpenStreetMap tiles, so no API key is needed.

### Changing the WiFi network

Any of these opens the **`Overhead-Setup`** hotspot (password `overhead`) for
up to 10 minutes. Join it from a phone and pick the new network. The display
keeps running meanwhile and stays on the old network until a new one is saved.

- **On screen:** open the settings (top-left then top-right corner) and tap
  **Change WiFi** at the bottom. This works when the display is offline too.
- **Web page:** **Change WiFi Network** at the bottom.
- **Automatically at boot** if the saved network doesn't connect. On the
  Advance the RTC already has the time, so it used to run offline silently;
  now it shows the setup screen, with a **Cancel** button to run offline.

To forget the saved network entirely, touch and hold the screen while the boot
splash is showing, or use **Reset WiFi Credentials** on the web page.

## Using it

- **Tap an aircraft** on the radar or in the list to see its details (type,
  registration, altitude, vertical rate, speed, track, distance, squawk). Tap
  the card to close it.
- **− / +** change the range (2–250 nm).
- **Tap the top-left and top-right corners** within 5 s to open the on-screen
  settings: brightness, range, labels, trails, color clock (on/off, 24 h,
  seconds, date), dim/off schedules, and the overhead radius, card and beep.

Web UI settings: home location (map picker), range, labels, trails, data source
(receiver URL, online feed, poll interval), overhead radius / card / beep, color
clock (on/off, seconds, date, gamma), time zone, 12/24 h, brightness, dim and
off schedules.

## Overhead card

When an airborne aircraft is about to pass within the overhead radius of home
(1 mile by default; 1, 2, 3 or 5 mi, at any altitude), the screen switches to a
full-page card about it. The card opens up to 60 s before the aircraft's
predicted closest approach (from its speed and track), shows "OVERHEAD IN n s",
then "OVERHEAD", then "PASSED". It stays up at least 45 s. The card shows:

- callsign, operator, and live distance and direction from home
- route, e.g. MSP → DEN (from adsb.lol's route database; hidden when the
  database's route doesn't match where the aircraft actually is)
- manufacturer and model (hexdb.io), registration, type and ICAO address
- a photo with photographer credit (planespotters.net, re-encoded through
  images.weserv.nl because the board can't decode progressive JPEGs), or a
  silhouette pointing along its track
- live altitude, vertical rate, speed, track and squawk (emergency codes in red)

After that, it closes once the aircraft has passed and is more than 25% outside
the radius, or has landed. It closes at once if the aircraft drops out of the
feed. You can also tap it to close it; that aircraft won't reopen it
until it has left. The lookups take about 3–6 s, and the card shows the live
data meanwhile.

On the Advance, it can also beep twice when an aircraft comes overhead (at most
once per aircraft every 10 minutes, and not while the screen is scheduled dim or off).

The radius is the dashed circle on the radar and the red circle on the web
page's map.

## Diagnostics

- `/status`: feed status as JSON
- `/aircraft.json`: the aircraft currently shown
- `/snap`: raw screen capture; `python3 tools/snap.py overhead.local snap.png` saves it as a PNG
- `/panel[?mhz=N]`: pixel clock and refresh rate

## Files

- `overhead.ino`: the app (feed task, radar, settings, web UI, WiFi, schedules)
- `crowpanel7.h/.cpp`: display, touch, backlight, RTC and buzzer driver
- `fonts.h`: **generated** by `python3 tools/make_fonts.py` (do not edit)
- `smooth_font.h`, `app_types.h`: helper headers

## License

MIT; see [LICENSE](LICENSE). The font data in `fonts.h` is rendered from
DejaVu Sans Bold, which is under the
[DejaVu Fonts license](https://dejavu-fonts.github.io/License.html).
