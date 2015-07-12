//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

cConfig g_CurConfig;          // = 0x00;
cConfig g_OldConfig;          // = 0x00;
cInput  g_aInput[INPUT_SETS]; // = 0x00;


// ****************************************************************
// check configuration for valid values
static void CheckConfig(cConfig* pConfig)
{
#if defined(CONFIG_FORCE)

    // force specific settings
    g_OldConfig.Graphics.iReflection = 1;
    g_OldConfig.Graphics.iGlow       = 1;
    g_OldConfig.Graphics.iDistortion = 1;

#endif

    // clamp input set selections
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
        pConfig->Input.aiType[i] = CLAMP(pConfig->Input.aiType[i], 0u, INPUT_SETS-1u);

    // loop trough input sets
    for(coreUintW i = 0u; i < INPUT_SETS;  ++i)
    {
        const coreInt16 iFrom = (i < INPUT_SETS_KEYBOARD) ? -coreInt16(CORE_INPUT_BUTTONS_MOUSE   -1) : 0;
        const coreInt16 iTo   = (i < INPUT_SETS_KEYBOARD) ?  coreInt16(CORE_INPUT_BUTTONS_KEYBOARD-1) : coreInt16(CORE_INPUT_BUTTONS_JOYSTICK-1);
        auto&           oSet  = pConfig->Input.aSet[i];

        // clamp movement keys
        oSet.iMoveLeft  = CLAMP(oSet.iMoveLeft,  iFrom, iTo);
        oSet.iMoveRight = CLAMP(oSet.iMoveRight, iFrom, iTo);
        oSet.iMoveDown  = CLAMP(oSet.iMoveDown,  iFrom, iTo);
        oSet.iMoveUp    = CLAMP(oSet.iMoveUp,    iFrom, iTo);

        // clamp input buttons
        for(coreUintW j = 0u; j < INPUT_BUTTONS; ++j)
            oSet.aiButton[j] = CLAMP(oSet.aiButton[j], iFrom, iTo);
    }

    // check for input sets with more than one selection
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
    {
        for(coreUintW j = i+1u; j < INPUT_TYPES; ++j)
        {
            WARN_IF(pConfig->Input.aiType[i] == pConfig->Input.aiType[j])
            {
                // reset all selections to default values
                for(coreUintW k = 0u; k < INPUT_TYPES; ++k)
                    pConfig->Input.aiType[k] = k;

                // leave all loops
                i = j = INPUT_TYPES;
            }
        }
    }
}


// ****************************************************************
// load configuration
void LoadConfig()
{
    // reset memory
    std::memset(&g_CurConfig, 0, sizeof(g_CurConfig));
    std::memset(&g_OldConfig, 0, sizeof(g_OldConfig));
    std::memset(&g_aInput,    0, sizeof(g_aInput));

    // load configuration file
    Core::Config->Load();

    // read game values
    g_OldConfig.Game.iTransparent    = Core::Config->GetInt(CONFIG_GAME_TRANSPARENT);
    g_OldConfig.Game.Combat.iDamage  = Core::Config->GetInt(CONFIG_GAME_COMBAT_DAMAGE);
    g_OldConfig.Game.Combat.iChain   = Core::Config->GetInt(CONFIG_GAME_COMBAT_CHAIN);
    g_OldConfig.Game.Combat.iCombo   = Core::Config->GetInt(CONFIG_GAME_COMBAT_COMBO);

    // read graphics values
    g_OldConfig.Graphics.iShadow     = Core::Config->GetInt(CONFIG_GRAPHICS_SHADOW);
    g_OldConfig.Graphics.iReflection = Core::Config->GetInt(CONFIG_GRAPHICS_REFLECTION);
    g_OldConfig.Graphics.iGlow       = Core::Config->GetInt(CONFIG_GRAPHICS_GLOW);
    g_OldConfig.Graphics.iDistortion = Core::Config->GetInt(CONFIG_GRAPHICS_DISTORTION);

    // read audio values
    g_OldConfig.Audio.iAmbient       = Core::Config->GetInt(CONFIG_AUDIO_AMBIENT);

    // read input values
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
    {
        g_OldConfig.Input.aiType[i] = Core::Config->GetInt(CONFIG_INPUT_TYPE(i));
    }
    for(coreUintW i = 0u; i < INPUT_SETS;  ++i)
    {
        g_OldConfig.Input.aSet[i].iMoveLeft  = Core::Config->GetInt(CONFIG_INPUT_MOVE_LEFT (i));
        g_OldConfig.Input.aSet[i].iMoveRight = Core::Config->GetInt(CONFIG_INPUT_MOVE_RIGHT(i));
        g_OldConfig.Input.aSet[i].iMoveDown  = Core::Config->GetInt(CONFIG_INPUT_MOVE_DOWN (i));
        g_OldConfig.Input.aSet[i].iMoveUp    = Core::Config->GetInt(CONFIG_INPUT_MOVE_UP   (i));
        for(coreUintW j = 0u; j < INPUT_BUTTONS; ++j)
        {
            g_OldConfig.Input.aSet[i].aiButton[j] = Core::Config->GetInt(CONFIG_INPUT_BUTTON(i, j));
        }
    }

    // check configuration for valid values
    CheckConfig(&g_OldConfig);

    // forward values to the current structure
    g_CurConfig = g_OldConfig;
}


// ****************************************************************
// save configuration
void SaveConfig()
{
    // equalize both structures
    g_OldConfig = g_CurConfig;

    // write game values
    Core::Config->SetInt(CONFIG_GAME_TRANSPARENT,    g_OldConfig.Game.iTransparent);
    Core::Config->SetInt(CONFIG_GAME_COMBAT_DAMAGE,  g_OldConfig.Game.Combat.iDamage);
    Core::Config->SetInt(CONFIG_GAME_COMBAT_CHAIN,   g_OldConfig.Game.Combat.iChain);
    Core::Config->SetInt(CONFIG_GAME_COMBAT_COMBO,   g_OldConfig.Game.Combat.iCombo);

    // write graphics values
    Core::Config->SetInt(CONFIG_GRAPHICS_SHADOW,     g_OldConfig.Graphics.iShadow);
    Core::Config->SetInt(CONFIG_GRAPHICS_REFLECTION, g_OldConfig.Graphics.iReflection);
    Core::Config->SetInt(CONFIG_GRAPHICS_GLOW,       g_OldConfig.Graphics.iGlow);
    Core::Config->SetInt(CONFIG_GRAPHICS_DISTORTION, g_OldConfig.Graphics.iDistortion);

    // write audio values
    Core::Config->SetInt(CONFIG_AUDIO_AMBIENT,       g_OldConfig.Audio.iAmbient);

    // write input values
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
    {
        Core::Config->SetInt(CONFIG_INPUT_TYPE(i), g_OldConfig.Input.aiType[i]);
    }
    for(coreUintW i = 0u; i < INPUT_SETS;  ++i)
    {
        Core::Config->SetInt(CONFIG_INPUT_MOVE_LEFT (i), g_OldConfig.Input.aSet[i].iMoveLeft);
        Core::Config->SetInt(CONFIG_INPUT_MOVE_RIGHT(i), g_OldConfig.Input.aSet[i].iMoveRight);
        Core::Config->SetInt(CONFIG_INPUT_MOVE_DOWN (i), g_OldConfig.Input.aSet[i].iMoveDown);
        Core::Config->SetInt(CONFIG_INPUT_MOVE_UP   (i), g_OldConfig.Input.aSet[i].iMoveUp);
        for(coreUintW j = 0u; j < INPUT_BUTTONS; ++j)
        {
            Core::Config->SetInt(CONFIG_INPUT_BUTTON(i, j), g_OldConfig.Input.aSet[i].aiButton[j]);
        }
    }

    // save configuration file
    Core::Config->Save();
}


// ****************************************************************
// update input interface
void UpdateInput()
{
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
    {
        const coreUint8& iType = g_CurConfig.Input.aiType[i];
        const auto&      oSet  = g_CurConfig.Input.aSet[iType];
        cInput&          oMap  = g_aInput[iType];

        // reset mapped input values
        std::memset(&oMap, 0, sizeof(oMap));

        // check for input source type
        if(iType < INPUT_SETS_KEYBOARD)   // # keyboard and mouse
        {
            // map movement input
                 if(Core::Input->GetKeyboardButton(coreInputKey(oSet.iMoveLeft),  CORE_INPUT_HOLD)) oMap.vMove.x = -1.0f;
            else if(Core::Input->GetKeyboardButton(coreInputKey(oSet.iMoveRight), CORE_INPUT_HOLD)) oMap.vMove.x =  1.0f;
                 if(Core::Input->GetKeyboardButton(coreInputKey(oSet.iMoveDown),  CORE_INPUT_HOLD)) oMap.vMove.y = -1.0f;
            else if(Core::Input->GetKeyboardButton(coreInputKey(oSet.iMoveUp),    CORE_INPUT_HOLD)) oMap.vMove.y =  1.0f;

            // map button input
            for(coreUintW j = 0u; j < INPUT_BUTTONS; ++j)
            {
                if(oSet.aiButton[j] <= 0)
                {
                    // check for mouse buttons
                    if(Core::Input->GetMouseButton(coreUint8(-oSet.aiButton[j]), CORE_INPUT_PRESS))   ADD_BIT(oMap.iButtonPress,   j);
                    if(Core::Input->GetMouseButton(coreUint8(-oSet.aiButton[j]), CORE_INPUT_RELEASE)) ADD_BIT(oMap.iButtonRelease, j);
                    if(Core::Input->GetMouseButton(coreUint8(-oSet.aiButton[j]), CORE_INPUT_HOLD))    ADD_BIT(oMap.iButtonHold,    j);
                }
                else
                {
                    // check for keyboard buttons
                    if(Core::Input->GetKeyboardButton(coreInputKey(oSet.aiButton[j]), CORE_INPUT_PRESS))   ADD_BIT(oMap.iButtonPress,   j);
                    if(Core::Input->GetKeyboardButton(coreInputKey(oSet.aiButton[j]), CORE_INPUT_RELEASE)) ADD_BIT(oMap.iButtonRelease, j);
                    if(Core::Input->GetKeyboardButton(coreInputKey(oSet.aiButton[j]), CORE_INPUT_HOLD))    ADD_BIT(oMap.iButtonHold,    j);
                }
            }
        }
        else   // # joystick/gamepad
        {
            const coreUintW iJoystickID = iType - INPUT_SETS_KEYBOARD;

            // 
            Core::Input->ForwardDpadToStick(iJoystickID);

            // map movement input
            oMap.vMove = Core::Input->GetJoystickRelative(iJoystickID);

            // map button input
            for(coreUintW j = 0u; j < INPUT_BUTTONS; ++j)
            {
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiButton[j]), CORE_INPUT_PRESS))   ADD_BIT(oMap.iButtonPress,   j);
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiButton[j]), CORE_INPUT_RELEASE)) ADD_BIT(oMap.iButtonRelease, j);
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiButton[j]), CORE_INPUT_HOLD))    ADD_BIT(oMap.iButtonHold,    j);
            }
        }

        // normalize movement input
        if(!oMap.vMove.IsNull()) oMap.vMove.Normalize();
    }
}