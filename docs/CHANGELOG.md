# Changelog

## V1.4 — 22-11-2026

### Modernized
- Ported from SFML 2 to SFML 3 (SFML 2 is no longer distributed, so the project now builds with current toolchains)
- Split the single-file source into modules: `main.cpp`, `ui`, `morse`, `content`
- Consistent naming across the codebase; merged the two near-identical translation screens into one function

### Fixed
- The program now runs no matter where it is launched from — previously it only worked when started from the project root
- Side buttons (Back / Clear / Switch / Next Lesson) are anchored to the window edge, no longer off-screen on monitors narrower than 1920px
- Clicks on empty screen areas no longer insert stray Morse characters into the translator
- Lesson 19 appears in the lesson picker; lesson and practice counts are now read from the files on disk instead of hardcoded

### Added
- Unit test suite for the Morse translation and the lesson/practice file parsers
- Keyboard controls: **Esc** returns to the main menu (and exits from it), Morse can be typed directly in Morse-to-Text mode, and **Space** inserts the letter separator in Practice mode
- Project documentation: README, setup guide, `.gitignore`

### Changed
- Window title renamed to "Morse Code Learning Program"

## V1.3 — 07-09-2025

- Added and error-checked the lessons and practice files
- Visual improvements and adjustments
- Error-checking pass; comments added
- Presentation-ready build

## V1.2 — 01-09-2025

- Fixed ghost Switch Button error
- Visual improvements; comments added
- Small bug fixes
- Separate space sound
- Clearer Translation Mode and improved feedback
- Lessons and practice files still in progress

## V1.0 — XX-XX-2025

- Beta versions finished
- Lessons and practice files still lacking
