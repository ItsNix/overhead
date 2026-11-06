# Overhead

Shows the aircraft flying over your home on an Elecrow CrowPanel 7" ESP32-S3
board (800×480). On the left is a north-up radar centred on home, with range
rings. Each aircraft is drawn as an icon rotated to its heading and colored by
altitude, with its callsign, altitude, a climb/descent arrow and a short trail.
Positions are extrapolated between updates so the icons move smoothly. On the
right are a clock, the nearest aircraft, the feed status and range buttons.

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
for the overhead alert.

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
2. Open **http://overhead.local** (or the IP shown on screen) and set your home
   latitude and longitude. The default is Pocatello, Idaho.

To forget the saved WiFi network, touch and hold the screen while the boot
splash is showing.

## Using it

- **Tap an aircraft** on the radar or in the list to see its details (type,
  registration, altitude, vertical rate, speed, track, distance, squawk). Tap
  the card to close it.
- **− / +** change the range (2–250 nm).
- **Tap the top-left and top-right corners** within 5 s to open the on-screen
  settings: brightness, range, labels, trails, dim/off schedules, and the alert.

Web UI settings: home location, range, labels, trails, data source (receiver
URL, online feed, poll interval), overhead alert, time zone, 12/24 h,
brightness, dim and off schedules.

## Overhead alert (Advance only)

The board beeps twice when an airborne aircraft first comes within the alert
radius (1–10 nm), and then not again for that aircraft for 10 minutes. The
radius is drawn on the radar as a dashed circle. The alert stays quiet while
the screen is scheduled dim or off.

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
