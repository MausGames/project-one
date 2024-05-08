# Eigengrau - Project One

[![software license: zlib](material/readme/badge_license_software.svg)](LICENSE.txt)
[![content license: CC BY](material/readme/badge_license_content.svg)](https://creativecommons.org/licenses/by/4.0/)
[![credits.txt](material/readme/badge_credits.svg)](executable/data/credits.txt)

**A twist at every turn!**

Explore a colorful shoot 'em up with varied and diverse gameplay beyond the familiar shooting and dodging. Every situation, every enemy, and every boss offers a different handcrafted experience.

Journey across burning mountains, frozen plains, below the ocean, and outer space, driven towards an unknown goal. Collect ancient fragments of mysterious origin, and try to find yourself, while the world around you falls into darkness.

Are you really doing the right thing?

**Have Fun (Gameplay)**
- Experience more than 50 different handcrafted arcade stages and 10 challenging bosses across 8.5 areas.
- Swing around enemies, hide in the dark, jump through space and time, rotate with the environment, and avoid tricky obstacles.
- Each stage is carefully introduced, gradually advanced, and twisted into countless exciting variations.

**Be in Control (Accessibility)**
- You can change difficulty and game speed, or equip a shield to fit your playing style.
- Practice individual stages and find the best strategy, or play everything at once in Classic Mode.
- Disable unwanted effects, adjust the interface, rotate the screen (Tate), and remap controls to your liking.

**Reach the Top (Scoring)**
- Race against time, keep up your multiplier, and break the highscore.
- Solve additional tasks to receive bonus points and try to get the highest medal in every stage.
- Unlock trophies/achievements by turning the rules upside down and thinking outside the box.

**Work Together (Multiplayer)**
- A completely new experience: Local co-op changes how some of the stages work and feel.
- Play with a friend on the same computer and support one another. Or don't. 🙂

Made with the Core Engine ([GitHub](https://github.com/MausGames/core-engine), [GitLab](https://gitlab.com/MausGames/core-engine))

#

**Windows, Linux and macOS Version:**  
https://store.steampowered.com/app/1624320/Eigengrau/  
https://mausgames.itch.io/eigengrau (also with browser demo)  
https://store.epicgames.com/p/eigengrau-62aef0  

**Nintendo Switch™ Version:**  
https://www.nintendo.com/us/store/products/eigengrau-switch/ (Americas)  
https://www.nintendo.co.uk/Games/Eigengrau-2463187.html (Europe)  
https://store-jp.nintendo.com/list/software/70010000071402.html (Japan)

#

<table>
    <tr>
        <td><a href="material/screenshots/p1_screen_021.jpg?raw=true"><img src="material/screenshots/p1_screen_021t.jpg"></a></td>
        <td><a href="material/screenshots/p1_screen_022.jpg?raw=true"><img src="material/screenshots/p1_screen_022t.jpg"></a></td>
        <td><a href="material/screenshots/p1_screen_023.jpg?raw=true"><img src="material/screenshots/p1_screen_023t.jpg"></a></td>
    </tr>
    <tr>
        <td><a href="material/screenshots/p1_screen_024.jpg?raw=true"><img src="material/screenshots/p1_screen_024t.jpg"></a></td>
        <td><a href="material/screenshots/p1_screen_025.jpg?raw=true"><img src="material/screenshots/p1_screen_025t.jpg"></a></td>
        <td><a href="material/screenshots/p1_screen_026.jpg?raw=true"><img src="material/screenshots/p1_screen_026t.jpg"></a></td>
    </tr>
    <tr>
        <td><a href="material/screenshots/p1_screen_027.jpg?raw=true"><img src="material/screenshots/p1_screen_027t.jpg"></a></td>
        <td><a href="material/screenshots/p1_screen_028.jpg?raw=true"><img src="material/screenshots/p1_screen_028t.jpg"></a></td>
        <td><a href="material/screenshots/p1_screen_030.jpg?raw=true"><img src="material/screenshots/p1_screen_030t.jpg"></a></td>
    </tr>
</table>

## Highlights

**Areas:**
- Intro: [Stages](source/game/mission/99_cIntroMission_setup.cpp), [Base](source/game/mission/99_cIntroMission.cpp)
- Virido: [Stages](source/game/mission/01_cViridoMission_setup.cpp), [Base](source/game/mission/01_cViridoMission.cpp)
- Nevo: [Stages](source/game/mission/02_cNevoMission_setup.cpp), [Base](source/game/mission/02_cNevoMission.cpp)
- Harena: [Stages](source/game/mission/03_cHarenaMission_setup.cpp), [Base](source/game/mission/03_cHarenaMission.cpp)
- Rutilus: [Stages](source/game/mission/04_cRutilusMission_setup.cpp), [Base](source/game/mission/04_cRutilusMission.cpp)
- Gelu: [Stages](source/game/mission/05_cGeluMission_setup.cpp), [Base](source/game/mission/05_cGeluMission.cpp)
- Calor: [Stages](source/game/mission/06_cCalorMission_setup.cpp), [Base](source/game/mission/06_cCalorMission.cpp) (Snow: [Source](source/visual/cSnow.cpp), [Header](source/visual/cSnow.h))
- Muscus: [Stages](source/game/mission/07_cMuscusMission_setup.cpp), [Base](source/game/mission/07_cMuscusMission.cpp) (Darkness: [Source](source/visual/cHeadlight.cpp), [Header](source/visual/cHeadlight.h))
- Ater: [Stages](source/game/mission/08_cAterMission_setup.cpp), [Base](source/game/mission/08_cAterMission.cpp)

**Bosses:**
- [Shinai](source/game/boss/9901_cIntroBoss.cpp)
- [Torus](source/game/boss/0102_cTorusBoss.cpp)
- [Leviathan](source/game/boss/0203_cLeviathanBoss.cpp)
- [Tiger MK-III](source/game/boss/0302_cTigerBoss.cpp)
- [Messier 87](source/game/boss/0403_cMessierBoss.cpp)
- [Chol](source/game/boss/0503_cCholBoss.cpp)
- [Zeroth](source/game/boss/0603_cZerothBoss.cpp)
- [Nagual](source/game/boss/0702_cGemingaBoss.cpp)
- [Dharuk](source/game/boss/0101_cDharukBoss.cpp)
- [P1](source/game/boss/0801_cProjectOneBoss.cpp)
- [???](source/game/boss/0802_cEigengrauBoss.cpp)

**Other:**
- Game: [Source](source/game/cGame.cpp), [Header](source/game/cGame.h)
  - Player: [Source](source/game/cPlayer.cpp), [Header](source/game/cPlayer.h)
  - Enemies: [Source](source/game/cEnemy.cpp), [Header](source/game/cEnemy.h)
  - Bullets: [Source](source/game/cBullet.cpp), [Header](source/game/cBullet.h)
  - Weapons: [Source](source/game/cWeapon.cpp), [Header](source/game/cWeapon.h)
  - Areas (general): [Source](source/game/mission/cMission.cpp), [Header](source/game/mission/cMission.h)
  - Bosses (general): [Source](source/game/boss/cBoss.cpp), [Header](source/game/boss/cBoss.h)
- Environment: [Source](source/environment/cEnvironment.cpp), [Header](source/environment/cEnvironment.h)
  - [Backgrounds](source/environment/background)
  - Outdoor: [Source](source/environment/cOutdoor.cpp), [Header](source/environment/cOutdoor.h)
  - Water, Ice, Lava: [Source](source/environment/cWater.cpp), [Header](source/environment/cWater.h)
- User Interface
  - [Menus](source/interface/menu)
  - HUD: [Source](source/interface/cInterface.cpp), [Header](source/interface/cInterface.h)
  - Floating Text: [Source](source/interface/cCombatText.cpp), [Header](source/interface/cCombatText.h)
  - Button Prompts: [Source](source/interface/cFigure.cpp), [Header](source/interface/cFigure.h)
  - Gamepad Navigation: [Source](source/additional/cMenuNavigator.cpp), [Header](source/additional/cMenuNavigator.h)
  - Arcade Input: [Source](source/interface/cArcadeInput.cpp), [Header](source/interface/cArcadeInput.h)
  - Credit Roll: [Source](source/interface/cCreditRoll.cpp), [Header](source/interface/cCreditRoll.h)
- Visuals
  - [Shaders](executable/data/shaders)
  - Special Effects: [Source](source/visual/cSpecialEffects.cpp), [Header](source/visual/cSpecialEffects.h)
  - Post Processing, Viewport: [Source](source/visual/cPostProcessing.cpp), [Header](source/visual/cPostProcessing.h)
  - Shadow: [Source](source/visual/cShadow.cpp), [Header](source/visual/cShadow.h)
- Main: [Source](source/main.cpp), [Header](source/main.h)
- Save System: [Source](source/file/cSave.cpp), [Header](source/file/cSave.h)
- Replay System: [Source](source/file/cReplay.cpp), [Header](source/file/cReplay.h)
- Input Handling, Configuration: [Source](source/file/cConfig.cpp), [Header](source/file/cConfig.h)
- [Passwords](source/additional/cPasswords.cpp)

## Documentation

- [Design Doodles](https://drive.google.com/drive/folders/19IDtXZdvwtx6ng-0dzGaivwl6W_nE1Nq) (high-resolution scans of drawings and written records)
- [Design Notes](resources/notes) (various scratch files with tasks, ideas, concepts and more)
- [Screenshots](https://drive.google.com/drive/folders/1wUOl3Ni1_uNE3cLdaLv8gQ8veeAd05Rv) (a decade of screenshots, from the beginning to the end)

## Additional Notes

- The source code contains many empty comments for structural and historical reasons. I don't want to remove them anymore.
- This repository should not include any proprietary assets, confidential APIs, private keys or anything similar. Therefore, it does not contain the full playable game.
- If you have any questions, please feel free to open an issue or [contact me](https://www.maus-games.at/) directly.