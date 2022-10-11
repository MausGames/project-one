cmake_minimum_required(VERSION 3.22)
set(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..)


project(ProjectOne VERSION 0.1.0)
add_subdirectory(${BASE_DIRECTORY}/../CoreEngine/projects/cmake CoreEngine)
add_executable(ProjectOne)


set_target_properties(
    ProjectOne PROPERTIES
    C_STANDARD 17
    CXX_STANDARD 20
    DEBUG_POSTFIX _debug
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH $ORIGIN
)


target_sources(
    ProjectOne PRIVATE
    ${BASE_DIRECTORY}/source/environment/background/cBackground.cpp
    ${BASE_DIRECTORY}/source/environment/cEnvironment.cpp
    ${BASE_DIRECTORY}/source/environment/cOutdoor.cpp
    ${BASE_DIRECTORY}/source/environment/cWater.cpp
    ${BASE_DIRECTORY}/source/file/cConfig.cpp
    ${BASE_DIRECTORY}/source/file/cReplay.cpp
    ${BASE_DIRECTORY}/source/file/cSave.cpp
    ${BASE_DIRECTORY}/source/game/boss/cBoss.cpp
    ${BASE_DIRECTORY}/source/game/mission/01_cViridoMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/02_cNevoMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/03_cHarenaMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/04_cRutilusMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/05_cGeluMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/06_cCalorMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/07_cMuscusMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/08_cAterMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/99_cIntroMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/A1_cBonus1Mission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/A2_cBonus2Mission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/B1_cErrorMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/B2_cDemoMission_setup.cpp
    ${BASE_DIRECTORY}/source/game/mission/cMission.cpp
    ${BASE_DIRECTORY}/source/game/cBullet.cpp
    ${BASE_DIRECTORY}/source/game/cCrash.cpp
    ${BASE_DIRECTORY}/source/game/cEnemy.cpp
    ${BASE_DIRECTORY}/source/game/cGame.cpp
    ${BASE_DIRECTORY}/source/game/cHelper.cpp
    ${BASE_DIRECTORY}/source/game/cItem.cpp
    ${BASE_DIRECTORY}/source/game/cPlayer.cpp
    ${BASE_DIRECTORY}/source/game/cShield.cpp
    ${BASE_DIRECTORY}/source/game/cShip.cpp
    ${BASE_DIRECTORY}/source/game/cTable.cpp
    ${BASE_DIRECTORY}/source/game/cWeapon.cpp
    ${BASE_DIRECTORY}/source/interface/menu/cMenu.cpp
    ${BASE_DIRECTORY}/source/interface/cArcadeInput.cpp
    ${BASE_DIRECTORY}/source/interface/cCombatText.cpp
    ${BASE_DIRECTORY}/source/interface/cCreditRoll.cpp
    ${BASE_DIRECTORY}/source/interface/cInterface.cpp
    ${BASE_DIRECTORY}/source/interface/cMsgBox.cpp
    ${BASE_DIRECTORY}/source/interface/cScrollBox.cpp
    ${BASE_DIRECTORY}/source/interface/cTooltip.cpp
    ${BASE_DIRECTORY}/source/interface/cWorldMap.cpp
    ${BASE_DIRECTORY}/source/visual/cBlur.cpp
    ${BASE_DIRECTORY}/source/visual/cDistortion.cpp
    ${BASE_DIRECTORY}/source/visual/cForeground.cpp
    ${BASE_DIRECTORY}/source/visual/cGlow.cpp
    ${BASE_DIRECTORY}/source/visual/cHeadlight.cpp
    ${BASE_DIRECTORY}/source/visual/cInk.cpp
    ${BASE_DIRECTORY}/source/visual/cOutline.cpp
    ${BASE_DIRECTORY}/source/visual/cPostProcessing.cpp
    ${BASE_DIRECTORY}/source/visual/cShadow.cpp
    ${BASE_DIRECTORY}/source/visual/cSnow.cpp
    ${BASE_DIRECTORY}/source/visual/cSpecialEffects.cpp
    ${BASE_DIRECTORY}/source/visual/cWindscreen.cpp
    ${BASE_DIRECTORY}/source/application.cpp
    ${BASE_DIRECTORY}/source/main.cpp
)


target_include_directories(
    ProjectOne PRIVATE
    ${BASE_DIRECTORY}/source
)


target_precompile_headers(
   ProjectOne PRIVATE
   $<$<COMPILE_LANGUAGE:CXX>: ${BASE_DIRECTORY}/source/main.h>
)


target_link_libraries(
    ProjectOne PRIVATE
    CoreEngine
)


if(EMSCRIPTEN)


    set_target_properties(
        ProjectOne PROPERTIES
        SUFFIX .html
        RUNTIME_OUTPUT_DIRECTORY ${BASE_DIRECTORY}/executable/bin/emscripten/
    )


else()


    set_target_properties(
        ProjectOne PROPERTIES
        SUFFIX .elf
        RUNTIME_OUTPUT_DIRECTORY ${BASE_DIRECTORY}/executable/bin/linux_x86_64/
    )


endif()