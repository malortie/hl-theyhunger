# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [hunger/1.0.3] - 2021-10-26

### Changed

- Reworked houndeye attack code

### Fixed

- Issues with glock silencer state
- Incorrect grunt bodygroups
- Zoom HUD not updated on save/restore

## [hunger/1.0.2] - 2021-05-08

### Added

- Visual Studio 2019 project files

### Changed

- Custom cvars are now set by default to values from skill.cfg from Steampipe patch
- Made batteries pickable in multiplayer only
- Made hud battery drawable in multiplayer only
- Made health vertical bar drawable in multiplayer only
- Made AP9 spread cone wider in secondary attack
- Made AP9 spread cone wider in primary attack
- Made snipers firing cone wider when zoomed out
- Reworked AP9 code
- Reworked Chaingun code
- Reworked Glock silencer code
- Reworked Medkit code
- Reworked snipers code
- Reworked Taurus code

### Removed

- Hardcoded Python reload sound

### Fixed

- Chicken class not precaching headcrab bite sound
- Skeletons not throwing gibs when killed
- Taurus not playing shoot empty animation
- Chaingun now fires the same amount of bullet as it uses ammo
- Issue where Chaingun m_iClip could be negative
- Player speed not restored when switching from Chaingun
- Wrong max clip macro used in chaingun.cpp
- Baby skeleton not using the correct health skill variable
- Zombie bull not using the correct whip damage skill variable
- Taurus now uses correct 10 clip magazine
- Missing m_fSilencerOn in sync on client side
- vuser1 and vuser2 variables mismatch on client/server
- Missing fields to Chaingun save data table
- Missing fields to weapon_einar1 save data table
- Missing fields to Glock save data table
- Missing weapon icon on Medkit pickup
- Missing weapon icon on Chaingun pickup
- Missing weapon icon on Crowbar pickup
- Missing weapon icon on Glock pickup

## [hunger/1.0.1] - 2017-05-16

### Fixed

- Taurus now uses correct 20 clip magazine
- Chaingun fires 2 bullets each shot
- Helicopters no longer shoot missiles
- Flamethrower now plays idle animation

## [hunger/1.0.0] - 2016-01-20

### Added

- They Hunger reimplementation source code
