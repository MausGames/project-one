///////////////////////////////////////////////
//*-----------------------------------------*//
//| Auto-Generated by the Core Engine       |//
//| Direct modifications may be overwritten |//
//*-----------------------------------------*//
///////////////////////////////////////////////
#include "Core.h"


// ****************************************************************
// setup all defined resources
static void SetupResources()
{
    Core::Manager::Resource->Load<coreModel>  ("default_square.md5mesh",          CORE_RESOURCE_UPDATE_AUTO,   "data/models/default_square.md5mesh");
    Core::Manager::Resource->Load<coreModel>  ("rock.md5mesh",                    CORE_RESOURCE_UPDATE_AUTO,   "data/models/rock.md5mesh");
                                                                                  
    Core::Manager::Resource->Load<coreTexture>("default_white.png",               CORE_RESOURCE_UPDATE_AUTO,   "data/textures/default_white.png");
    Core::Manager::Resource->Load<coreTexture>("environment_clouds_blue.png",     CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_clouds_blue.png");
    Core::Manager::Resource->Load<coreTexture>("environment_clouds_low.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_clouds_low.png");
    Core::Manager::Resource->Load<coreTexture>("environment_clouds_mid.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_clouds_mid.png");
    Core::Manager::Resource->Load<coreTexture>("environment_clouds_high.png",     CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_clouds_high.png");
    Core::Manager::Resource->Load<coreTexture>("environment_dust_diff.png",       CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_dust_diff.png");
    Core::Manager::Resource->Load<coreTexture>("environment_dust_norm.png",       CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_dust_norm.png");
    Core::Manager::Resource->Load<coreTexture>("environment_grass_diff.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_grass_diff.png");
    Core::Manager::Resource->Load<coreTexture>("environment_grass_norm.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_grass_norm.png");
    Core::Manager::Resource->Load<coreTexture>("environment_stone_diff.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_stone_diff.png");
    Core::Manager::Resource->Load<coreTexture>("environment_stone_norm.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_stone_norm.png");
    Core::Manager::Resource->Load<coreTexture>("environment_water_norm.png",      CORE_RESOURCE_UPDATE_AUTO,   "data/textures/environment_water_norm.png");
    Core::Manager::Resource->Load<coreTexture>("menu_background_black.png",       CORE_RESOURCE_UPDATE_AUTO,   "data/textures/menu_background_black.png");
                                                                                  
    Core::Manager::Resource->Load<coreShader> ("default_2d.vert",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_2d.vert");
    Core::Manager::Resource->Load<coreShader> ("default_2d.frag",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_2d.frag");
    Core::Manager::Resource->Load<coreShader> ("default_3d.vert",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_3d.vert");
    Core::Manager::Resource->Load<coreShader> ("default_3d.frag",                 CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_3d.frag");
    Core::Manager::Resource->Load<coreShader> ("default_3d_inst.vert",            CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_3d.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("default_3d_inst.frag",            CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/default_3d.frag", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("effect_shadow.vert",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_shadow.vert");
    Core::Manager::Resource->Load<coreShader> ("effect_shadow.frag",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_shadow.frag");
    Core::Manager::Resource->Load<coreShader> ("effect_shadow_inst.vert",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/effect_shadow.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("environment_clouds.vert",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_clouds.vert");
    Core::Manager::Resource->Load<coreShader> ("environment_clouds.frag",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_clouds.frag");
    Core::Manager::Resource->Load<coreShader> ("environment_clouds_inst.vert",    CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_clouds.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("environment_clouds_inst.frag",    CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_clouds.frag", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("environment_outdoor.vert",        CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_outdoor.vert");
    Core::Manager::Resource->Load<coreShader> ("environment_outdoor.frag",        CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_outdoor.frag");
    Core::Manager::Resource->Load<coreShader> ("environment_water.vert",          CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_water.vert");
    Core::Manager::Resource->Load<coreShader> ("environment_water.frag",          CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/environment_water.frag");
    Core::Manager::Resource->Load<coreShader> ("full_post.frag",                  CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/full_post.frag");
    Core::Manager::Resource->Load<coreShader> ("full_transition.frag",            CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/full_transition.frag");
    Core::Manager::Resource->Load<coreShader> ("object.vert",                     CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object.vert");
    Core::Manager::Resource->Load<coreShader> ("object.frag",                     CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object.frag");
    Core::Manager::Resource->Load<coreShader> ("object_shadow.frag",              CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object.frag");
    Core::Manager::Resource->Load<coreShader> ("object_inst.vert",                CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object.vert", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("object_inst.frag",                CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object.frag", CORE_SHADER_OPTION_INSTANCING);
    Core::Manager::Resource->Load<coreShader> ("object_inst_shadow.frag",         CORE_RESOURCE_UPDATE_MANUAL, "data/shaders/object.frag", CORE_SHADER_OPTION_INSTANCING);
     
    Core::Manager::Resource->Load<coreSound>  ("environment_nature.wav",          CORE_RESOURCE_UPDATE_AUTO,   "data/sounds/environment_nature.wav");
                                                                                  
    Core::Manager::Resource->Load<coreFont>   ("ethnocentric.ttf",                CORE_RESOURCE_UPDATE_AUTO,   "data/fonts/ethnocentric.ttf");

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("default_2d_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d.vert")
        ->AttachShader("default_2d.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("default_3d_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_3d.vert")
        ->AttachShader("default_3d.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("default_3d_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_3d_inst.vert")
        ->AttachShader("default_3d_inst.frag")
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

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("full_post_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d.vert")
        ->AttachShader("full_post.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("full_transition_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("default_2d.vert")
        ->AttachShader("full_transition.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("object_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("object.vert")
        ->AttachShader("object.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("object_shadow_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("object.vert")
        ->AttachShader("object_shadow.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("object_inst_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("object_inst.vert")
        ->AttachShader("object_inst.frag")
        ->Finish();

    ((coreProgram*)Core::Manager::Resource->Load<coreProgram>("object_inst_shadow_program", CORE_RESOURCE_UPDATE_AUTO, NULL)->GetResource())
        ->AttachShader("object_inst.vert")
        ->AttachShader("object_inst_shadow.frag")
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