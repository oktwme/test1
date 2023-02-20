![Example](https://i.imgur.com/ACMxyEC.png)

[More screenshots](https://imgur.com/a/OFgtemU)


## Download
- [x64](https://github.com/BennyExtreme/KBotExt/releases/latest/download/KBotExt.exe)
- [x86](https://github.com/BennyExtreme/KBotExt/releases/latest/download/KBotExt_x86.exe)

## Technologies used
* No external libraries, only win32 functions for HTTP requests
* JsonCpp
* DirectX 11 with ImGui

## Features
* Works every patch
* Launch multiple clients
* Language changer
* Start any lobby/game
* Custom bots difficulty
* The fastest instalock, instapick, auto accept, instant message and auto ban
* Automatically pick a secondary, or dodge if your champion is banned
* Instantly mute everyone in champion select
* Dodge lobbies without closing the client
* Mass invite all friends to lobby
* Multi-search op.gg/u.gg/poro.gg/porofessor.gg etc. with all players in a champion select (works in ranked - lobby reveal)
* Set the best runes for your selected champion from op.gg (works even when runes aren't unlocked)
* Play any champion/skin for free (Refund exploit)
* Shows which map side you are on, in all gamemodes
* Set custom icon/background/status/rank/mastery visible for everyone
* Set glitched or empty challenge badges (tokens)
* Set invisible profile/lobby banner
* Info of any player using his nickname or id
* List of champions/skins with all info and ability to sort them
* Force close the client instantly
* Mass delete all friends sorted by folders
* Accept or delete all friend requests
* Set custom in-game minimap scale
* Disenchant any loot you want with 1 click (mass disenchant)
* Champion name to id lookup
* Send any custom request to LCU, Riot Client, RTMP, Store and Ledge
* Stream proof
* IFEO debugger for using HTTP debuggers (Fiddler, Charles, etc.)
* Log cleaner
* Automatically saves your preferences
* Unicode support and customizable window size
* 1 click login with automated client open
* Force client to run without admin
* Lobby invites flooder
* (Partially patched) Check the email linked to account you're logged on
* (Patched) Free skin and free champion (Riot Girl Tristana)
* (Patched) Free ARAM boost

## Development/Building

- **Install [GIT for windows](https://git-scm.com/download/win)**
- **Installing [vcpkg](https://github.com/microsoft/vcpkg#quick-start-windows) and [freetype](https://github.com/ocornut/imgui/tree/master/misc/freetype)**
  1. Open Windows Command Prompt (**CMD**)
  2. Run `git clone https://github.com/microsoft/vcpkg.git`
  3. `cd vcpkg`
  4. `bootstrap-vcpkg.bat`
  5. `vcpkg integrate install`
  6. `vcpkg install freetype:x86-windows-static` or `vcpkg install freetype:x64-windows-static`
- **Building the project**
  1. Open Windows Command Prompt (**CMD**)
  2. Clone the repository with `git clone https://github.com/BennyExtreme/KBotExt.git`
  3. Open **KBotExt.sln** in Visual Studio (Recommended Visual Studio 2022)
  4. Set the solution platform to the freetype version you have installed
  5. Build the project
  6. Feel free to make a pull request with your changes :-)