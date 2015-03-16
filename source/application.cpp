///////////////////////////////////////////////
//*-----------------------------------------*//
//| Auto-Generated by the Core Engine       |//
//| Direct modifications may be overwritten |//
//*-----------------------------------------*//
///////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// setup all defined resources
static void SetupResources()
{
    Core::Manager::Resource->Load<coreModel>  ("bullet_ray.md3",                  CORE_RESOURCE_UPDATE_AUTO,   "data/models/bullet_ray.md3");
    Core::Manager::Resource->Load<coreModel>  ("bullet_orb.md3",                  CORE_RESOURCE_UPDATE_AUTO,   "data/models/bullet_orb.md3");
    Core::Manager::Resource->Load<coreModel>  ("environment_reed_01.md3",         CORE_RESOURCE_UPDATE_AUTO,   "data/models/environment_reed_01.md3");
    Core::Manager::Resource->Load<coreModel>  ("environment_reed_02.md3",         CORE_RESOURCE_UPDATE_AUTO,   "data/models/environment_reed_02.md3");
    Core::Manager::Resource->Load<coreModel>  ("environment_stone_01.md3",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/environment_stone_01.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_boss_crossfield_high.md3",   CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_boss_crossfield_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_boss_crossfield_low.md3",    CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_boss_crossfield_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_arrow_high.md3",       CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_arrow_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_arrow_low.md3",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_arrow_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_cinder_high.md3",      CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_cinder_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_cinder_low.md3",       CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_cinder_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_freezer_high.md3",     CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_freezer_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_freezer_low.md3",      CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_freezer_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_miner_high.md3",       CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_miner_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_miner_low.md3",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_miner_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_scout_high.md3",       CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_scout_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_scout_low.md3",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_scout_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_star_high.md3",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_star_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_star_low.md3",         CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_star_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_warrior_high.md3",     CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_warrior_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_enemy_warrior_low.md3",      CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_enemy_warrior_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_player_def_high.md3",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_player_def_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_player_def_low.md3",         CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_player_def_low.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_player_atk_high.md3",        CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_player_atk_high.md3");
    Core::Manager::Resource->Load<coreModel>  ("ship_player_atk_low.md3",         CORE_RESOURCE_UPDATE_AUTO,   "data/models/ship_player_atk_low.md3");

    Core::Manager::Resource->Load<coreTexture>("effect_energy.png",               CORE_RESOURCE_UPDATE_AUTO,   "data/textures/effect_energy.png");
    Core::Manager::Resource->Load<coreTexture>("environment_clouds_blue.png",     CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_clouds_blue.png");
    Core::Manager::Resource->Load<coreTexture>("environment_clouds_low.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_clouds_low.png");
    Core::Manager::Resource->Load<coreTexture>("environment_clouds_mid.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_clouds_mid.png");
    Core::Manager::Resource->Load<coreTexture>("environment_clouds_high.png",     CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_clouds_high.png");
    Core::Manager::Resource->Load<coreTexture>("environment_flowers.png",         CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_flowers.png");
    Core::Manager::Resource->Load<coreTexture>("environment_dust_diff.png",       CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_dust_diff.png");
    Core::Manager::Resource->Load<coreTexture>("environment_dust_norm.png",       CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_dust_norm.png");
    Core::Manager::Resource->Load<coreTexture>("environment_grass_diff.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_grass_diff.png");
    Core::Manager::Resource->Load<coreTexture>("environment_grass_norm.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_grass_norm.png");
    Core::Manager::Resource->Load<coreTexture>("environment_reed.png",            CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_reed.png");
    Core::Manager::Resource->Load<coreTexture>("environment_stone_diff.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_stone_diff.png");
    Core::Manager::Resource->Load<coreTexture>("environment_stone_norm.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_stone_norm.png");
    Core::Manager::Resource->Load<coreTexture>("environment_water_norm.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_water_norm.png");
    Core::Manager::Resource->Load<coreTexture>("game_logo.png",                   CORE_RESOURCE_UPDATE_AUTO,   "data/textures/game_logo.png");
    Core::Manager::Resource->Load<coreTexture>("maus_logo.png",                   CORE_RESOURCE_UPDATE_AUTO,   "data/textures/maus_logo.png");
    Core::Manager::Resource->Load<coreTexture>("menu_background_black.png",       CORE_RESOURCE_UPDATE_AUTO,   "data/textures/menu_background_black.png");
    Core::Manager::Resource->Load<coreTexture>("menu_detail_01.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/menu_detail_01.png");
    Core::Manager::Resource->Load<coreTexture>("menu_detail_02.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/menu_detail_02.png");
    Core::Manager::Resource->Load<coreTexture>("menu_detail_03.png",              CORE_RESOURCE_UPDATE_AUTO,   "data/textures/menu_detail_03.png");
    Core::Manager::Resource->Load<coreTexture>("ship_enemy.png",                  CORE_RESOURCE_UPDATE_AUTO,   "data/textures/ship_enemy.png");
    Core::Manager::Resource->Load<coreTexture>("ship_player.png",                 CORE_RESOURCE_UPDATE_AUTO,   "data/textures/ship_player.png");

    Core::Manager::Resource->Load<coreShader> ("effect_decal.vert",               CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_decal.vert");
    Core::Manager::Resource->Load<coreShader> ("effect_decal_spheric.frag",       CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_decal.frag", SHADER_SPHERIC);
    Core::Manager::Resource->Load<coreShader> ("effect_decal_inst.vert",          CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_decal.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("effect_decal_spheric_inst.frag",  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_decal.frag", CORE_SHADER_OPTION_INSTANCING SHADER_SPHERIC);
    Core::Manager::Resource->Load<coreShader> ("effect_energy.vert",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_energy.vert");
    Core::Manager::Resource->Load<coreShader> ("effect_energy_spheric.vert",      CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_energy.vert", SHADER_SPHERIC);
    Core::Manager::Resource->Load<coreShader> ("effect_energy_direct.vert",       CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_energy.vert", SHADER_DIRECT);
    Core::Manager::Resource->Load<coreShader> ("effect_energy.frag",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_energy.frag");
    Core::Manager::Resource->Load<coreShader> ("effect_energy_inst.vert",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_energy.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("effect_energy_spheric_inst.vert", CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_energy.vert", CORE_SHADER_OPTION_INSTANCING SHADER_SPHERIC);
    Core::Manager::Resource->Load<coreShader> ("effect_energy_direct_inst.vert",  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_energy.vert", CORE_SHADER_OPTION_INSTANCING SHADER_DIRECT);
    Core::Manager::Resource->Load<coreShader> ("effect_energy_inst.frag",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_energy.frag", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("effect_outline.vert",             CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_outline.vert");
    Core::Manager::Resource->Load<coreShader> ("effect_outline.frag",             CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_outline.frag");
    Core::Manager::Resource->Load<coreShader> ("effect_outline_direct.frag",      CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_outline.frag", SHADER_DIRECT);
    Core::Manager::Resource->Load<coreShader> ("effect_outline_inst.vert",        CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_outline.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("effect_outline_inst.frag",        CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_outline.frag", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("effect_outline_direct_inst.frag", CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_outline.frag", CORE_SHADER_OPTION_INSTANCING SHADER_DIRECT);
    Core::Manager::Resource->Load<coreShader> ("effect_shadow.vert",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_shadow.vert");
    Core::Manager::Resource->Load<coreShader> ("effect_shadow.frag",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_shadow.frag");
    Core::Manager::Resource->Load<coreShader> ("effect_shadow_inst.vert",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_shadow.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("environment_clouds.vert",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_clouds.vert");
    Core::Manager::Resource->Load<coreShader> ("environment_clouds.frag",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_clouds.frag");
    Core::Manager::Resource->Load<coreShader> ("environment_clouds_inst.vert",    CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_clouds.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("environment_clouds_inst.frag",    CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_clouds.frag", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("environment_outdoor.vert",        CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_outdoor.vert", CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("environment_outdoor.frag",        CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_outdoor.frag");
    Core::Manager::Resource->Load<coreShader> ("environment_water.vert",          CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_water.vert", CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("environment_water.frag",          CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_water.frag");
    Core::Manager::Resource->Load<coreShader> ("full.vert",                       CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/full.vert");
    Core::Manager::Resource->Load<coreShader> ("full_blur_1.frag",                CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/full_blur_1.frag");
    Core::Manager::Resource->Load<coreShader> ("full_blur_2.frag",                CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/full_blur_2.frag");
    Core::Manager::Resource->Load<coreShader> ("full_post.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/full_post.frag", SHADER_GLOW);
    Core::Manager::Resource->Load<coreShader> ("full_transition.frag",            CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/full_transition.frag");
    Core::Manager::Resource->Load<coreShader> ("menu_animate.vert",               CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/menu_animate.vert", CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("menu_animate.frag",               CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/menu_animate.frag");
    Core::Manager::Resource->Load<coreShader> ("menu_border.vert",                CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/menu_border.vert", CORE_SHADER_OPTION_NO_ROTATION);
    Core::Manager::Resource->Load<coreShader> ("menu_border.frag",                CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/menu_border.frag");
    Core::Manager::Resource->Load<coreShader> ("menu_sharp.frag",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/menu_sharp.frag");
    Core::Manager::Resource->Load<coreShader> ("object.vert",                     CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object.vert");
    Core::Manager::Resource->Load<coreShader> ("object_ground.frag",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object_ground.frag", SHADER_SHADOW);
    Core::Manager::Resource->Load<coreShader> ("object_ship.frag",                CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object_ship.frag");
    Core::Manager::Resource->Load<coreShader> ("object_inst.vert",                CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("object_ground_inst.frag",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object_ground.frag", CORE_SHADER_OPTION_INSTANCING SHADER_SHADOW);

    Core::Manager::Resource->Load<coreSound>  ("bullet_ray.wav",                  CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/bullet_ray.wav");
    Core::Manager::Resource->Load<coreSound>  ("environment_nature.wav",          CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/environment_nature.wav");

    Core::Manager::Resource->Load<coreFont>   ("ethnocentric.ttf",                CORE_RESOURCE_UPDATE_AUTO,   "data/fonts/ethnocentric.ttf");

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_decal_spheric_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_decal.vert")
        ->AttachShader("effect_decal_spheric.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_decal_spheric_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_decal_inst.vert")
        ->AttachShader("effect_decal_spheric_inst.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_energy_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_energy.vert")
        ->AttachShader("effect_energy.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_energy_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_energy_inst.vert")
        ->AttachShader("effect_energy_inst.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_energy_spheric_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_energy_spheric.vert")
        ->AttachShader("effect_energy.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_energy_spheric_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_energy_spheric_inst.vert")
        ->AttachShader("effect_energy_inst.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_energy_direct_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_energy_direct.vert")
        ->AttachShader("effect_energy.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_energy_direct_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_energy_direct_inst.vert")
        ->AttachShader("effect_energy_inst.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_outline_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_outline.vert")
        ->AttachShader("effect_outline.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_outline_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_outline_inst.vert")
        ->AttachShader("effect_outline_inst.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_outline_direct_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_outline.vert")
        ->AttachShader("effect_outline_direct.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_outline_direct_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_outline_inst.vert")
        ->AttachShader("effect_outline_direct_inst.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_shadow_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_shadow.vert")
        ->AttachShader("effect_shadow.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("effect_shadow_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("effect_shadow_inst.vert")
        ->AttachShader("effect_shadow.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("environment_clouds_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("environment_clouds.vert")
        ->AttachShader("environment_clouds.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("environment_clouds_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("environment_clouds_inst.vert")
        ->AttachShader("environment_clouds_inst.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("environment_outdoor_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("environment_outdoor.vert")
        ->AttachShader("environment_outdoor.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("environment_water_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("environment_water.vert")
        ->AttachShader("environment_water.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("full_blur_1_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("full.vert")
        ->AttachShader("full_blur_1.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("full_blur_2_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("full.vert")
        ->AttachShader("full_blur_2.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("full_post_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("full.vert")
        ->AttachShader("full_post.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("full_transition_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("full.vert")
        ->AttachShader("full_transition.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("menu_animate_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("menu_animate.vert")
        ->AttachShader("menu_animate.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("menu_border_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("menu_border.vert")
        ->AttachShader("menu_border.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("menu_sharp_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d.vert")
        ->AttachShader("menu_sharp.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("object_ground_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("object.vert")
        ->AttachShader("object_ground.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("object_ship_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("object.vert")
        ->AttachShader("object_ship.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("object_ground_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("object_inst.vert")
        ->AttachShader("object_ground_inst.frag")
        ->Finish();
}


// ****************************************************************
// setup all defined designs
static void SetupDesigns()
{

}


// ****************************************************************
// setup the application
void CoreApp::Setup()
{
    // set window title and icon
    Core::System->SetWindowTitle("Project One");
    Core::System->SetWindowIcon("data/textures/game_icon.png");

    // set view frustum
    Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 50.0f, 500.0f);

    // set cursor
#if defined(_CORE_LINUX_)
    Core::Input->DefineCursor("data/textures/default_cursor_low.png");
#else
    Core::Input->DefineCursor("data/textures/default_cursor.png");
#endif

    // setup resources and designs
    SetupResources();
    SetupDesigns();
}