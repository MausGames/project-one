///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

sConfig    g_CurConfig               = {};
sConfig    g_OldConfig               = {};
sGameInput g_aGameInput[INPUT_TYPES] = {{}};
sGameInput g_TotalInput              = {};
sMenuInput g_MenuInput               = {};


// ****************************************************************
// check configuration for valid values
static void CheckConfig(sConfig* pConfig)
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
    for(coreUintW i = 0u; i < INPUT_SETS; ++i)
    {
        const coreInt16 iFrom = (i < INPUT_SETS_KEYBOARD) ? -coreInt16(CORE_INPUT_BUTTONS_MOUSE   -1) : 0;
        const coreInt16 iTo   = (i < INPUT_SETS_KEYBOARD) ?  coreInt16(CORE_INPUT_BUTTONS_KEYBOARD-1) : coreInt16(CORE_INPUT_BUTTONS_JOYSTICK-1);
        auto&           oSet  = pConfig->Input.aSet[i];

        // clamp movement keys
        oSet.iMoveUp    = CLAMP(oSet.iMoveUp,    iFrom, iTo);
        oSet.iMoveLeft  = CLAMP(oSet.iMoveLeft,  iFrom, iTo);
        oSet.iMoveDown  = CLAMP(oSet.iMoveDown,  iFrom, iTo);
        oSet.iMoveRight = CLAMP(oSet.iMoveRight, iFrom, iTo);

        // clamp action keys
        for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
            oSet.aiAction[j] = CLAMP(oSet.aiAction[j], iFrom, iTo);
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
    std::memset(&g_CurConfig,  0, sizeof(g_CurConfig));
    std::memset(&g_OldConfig,  0, sizeof(g_OldConfig));
    std::memset(&g_aGameInput, 0, sizeof(g_aGameInput));
    std::memset(&g_TotalInput, 0, sizeof(g_TotalInput));
    std::memset(&g_MenuInput,  0, sizeof(g_MenuInput));

    // load configuration file
    Core::Config->Load(true);

    // read game values
    g_OldConfig.Game.iTextSize       = Core::Config->GetInt(CONFIG_GAME_TEXT_SIZE);
    g_OldConfig.Game.iGameRotation   = Core::Config->GetInt(CONFIG_GAME_GAME_ROTATION);
    g_OldConfig.Game.iGameScale      = Core::Config->GetInt(CONFIG_GAME_GAME_SCALE);
    g_OldConfig.Game.iHudRotation    = Core::Config->GetInt(CONFIG_GAME_HUD_ROTATION);
    g_OldConfig.Game.iHudScale       = Core::Config->GetInt(CONFIG_GAME_HUD_SCALE);
    g_OldConfig.Game.iHudType        = Core::Config->GetInt(CONFIG_GAME_HUD_TYPE);

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
        g_OldConfig.Input.aiType  [i] = Core::Config->GetInt(CONFIG_INPUT_TYPE  (i));
        g_OldConfig.Input.aiRumble[i] = Core::Config->GetInt(CONFIG_INPUT_RUMBLE(i));
    }
    for(coreUintW i = 0u; i < INPUT_SETS;  ++i)
    {
        g_OldConfig.Input.aSet[i].iMoveUp    = Core::Config->GetInt(CONFIG_INPUT_MOVE_UP   (i));
        g_OldConfig.Input.aSet[i].iMoveLeft  = Core::Config->GetInt(CONFIG_INPUT_MOVE_LEFT (i));
        g_OldConfig.Input.aSet[i].iMoveDown  = Core::Config->GetInt(CONFIG_INPUT_MOVE_DOWN (i));
        g_OldConfig.Input.aSet[i].iMoveRight = Core::Config->GetInt(CONFIG_INPUT_MOVE_RIGHT(i));
        for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
        {
            g_OldConfig.Input.aSet[i].aiAction[j] = Core::Config->GetInt(CONFIG_INPUT_ACTION(i, j));
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
    Core::Config->SetInt(CONFIG_GAME_TEXT_SIZE,      g_OldConfig.Game.iTextSize);
    Core::Config->SetInt(CONFIG_GAME_GAME_ROTATION,  g_OldConfig.Game.iGameRotation);
    Core::Config->SetInt(CONFIG_GAME_GAME_SCALE,     g_OldConfig.Game.iGameScale);
    Core::Config->SetInt(CONFIG_GAME_HUD_ROTATION,   g_OldConfig.Game.iHudRotation);
    Core::Config->SetInt(CONFIG_GAME_HUD_SCALE,      g_OldConfig.Game.iHudScale);
    Core::Config->SetInt(CONFIG_GAME_HUD_TYPE,       g_OldConfig.Game.iHudType);

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
        Core::Config->SetInt(CONFIG_INPUT_TYPE  (i), g_OldConfig.Input.aiType  [i]);
        Core::Config->SetInt(CONFIG_INPUT_RUMBLE(i), g_OldConfig.Input.aiRumble[i]);
    }
    for(coreUintW i = 0u; i < INPUT_SETS;  ++i)
    {
        Core::Config->SetInt(CONFIG_INPUT_MOVE_UP   (i), g_OldConfig.Input.aSet[i].iMoveUp);
        Core::Config->SetInt(CONFIG_INPUT_MOVE_LEFT (i), g_OldConfig.Input.aSet[i].iMoveLeft);
        Core::Config->SetInt(CONFIG_INPUT_MOVE_DOWN (i), g_OldConfig.Input.aSet[i].iMoveDown);
        Core::Config->SetInt(CONFIG_INPUT_MOVE_RIGHT(i), g_OldConfig.Input.aSet[i].iMoveRight);
        for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
        {
            Core::Config->SetInt(CONFIG_INPUT_ACTION(i, j), g_OldConfig.Input.aSet[i].aiAction[j]);
        }
    }

    // save configuration file
    Core::Config->Save();
}


// ****************************************************************
// update input interface
void UpdateInput()
{
    // forward hat input to stick input
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
        Core::Input->ForwardHatToStick(i);

    // reset mapped input values
    std::memset(&g_aGameInput, 0, sizeof(g_aGameInput));
    std::memset(&g_TotalInput, 0, sizeof(g_TotalInput));
    std::memset(&g_MenuInput,  0, sizeof(g_MenuInput));

    // loop trough input sets
    for(coreUintW i = 0u; i < INPUT_SETS; ++i)
    {
        const auto& oSet = g_CurConfig.Input.aSet[i];
        sGameInput  oMap = {};

        if(i < INPUT_SETS_KEYBOARD)   // # keyboard and mouse
        {
            // check key depending on its value
            const auto nCheckKeyFunc = [](const coreInt16 iKey, const coreInputType eType)
            {
                if(iKey <= 0) return Core::Input->GetMouseButton   (coreUint8   (-iKey), eType);
                         else return Core::Input->GetKeyboardButton(coreInputKey( iKey), eType);
            };

            // map movement input
                 if(nCheckKeyFunc(oSet.iMoveLeft,  CORE_INPUT_HOLD)) oMap.vMove.x = -1.0f;
            else if(nCheckKeyFunc(oSet.iMoveRight, CORE_INPUT_HOLD)) oMap.vMove.x =  1.0f;
                 if(nCheckKeyFunc(oSet.iMoveDown,  CORE_INPUT_HOLD)) oMap.vMove.y = -1.0f;
            else if(nCheckKeyFunc(oSet.iMoveUp,    CORE_INPUT_HOLD)) oMap.vMove.y =  1.0f;

            // map action input
            for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
            {
                if(nCheckKeyFunc(oSet.aiAction[j], CORE_INPUT_PRESS))   ADD_BIT(oMap.iActionPress,   j);
                if(nCheckKeyFunc(oSet.aiAction[j], CORE_INPUT_RELEASE)) ADD_BIT(oMap.iActionRelease, j);
                if(nCheckKeyFunc(oSet.aiAction[j], CORE_INPUT_HOLD))    ADD_BIT(oMap.iActionHold,    j);
            }
        }
        else   // # joystick/gamepad
        {
            const coreUintW iJoystickID = i - INPUT_SETS_KEYBOARD;

            // map movement input
            oMap.vMove = Core::Input->GetJoystickRelativeL(iJoystickID);

            // restrict movement input to the 8 base directions
            if(!oMap.vMove.IsNull()) oMap.vMove = AlongStarNormal(oMap.vMove.Normalized());

            // map action input
            for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
            {
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiAction[j]), CORE_INPUT_PRESS))   ADD_BIT(oMap.iActionPress,   j);
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiAction[j]), CORE_INPUT_RELEASE)) ADD_BIT(oMap.iActionRelease, j);
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiAction[j]), CORE_INPUT_HOLD))    ADD_BIT(oMap.iActionHold,    j);
            }
        }

        if(!oMap.vMove.IsNull())
        {
            // 
            const coreVector2& vGame  = g_pPostProcessing->GetDirection();
            const coreVector2  vHud   = coreVector2(0.0f,1.0f).InvertedX(); 
            const coreVector2  vFinal = MapToAxis(vGame, vHud);
            ASSERT(vFinal.IsNormalized())

            // 
            oMap.vMove = MapToAxis(oMap.vMove, vFinal);
            oMap.vMove = oMap.vMove.Normalized();
        }

        // 
        for(coreUintW j = 0u; j < INPUT_TYPES; ++j)
        {
            if(g_CurConfig.Input.aiType[j] == i)
            {
                g_aGameInput[j] = oMap;
                break;
            }
        }

        // 
        if(g_TotalInput.vMove.IsNull()) g_TotalInput.vMove = oMap.vMove;   // use first valid movement input
        g_TotalInput.iActionPress   |= oMap.iActionPress;
        g_TotalInput.iActionRelease |= oMap.iActionRelease;
        g_TotalInput.iActionHold    |= oMap.iActionHold;

        // 
        if(i >= INPUT_SETS_KEYBOARD)
        {
            if(CONTAINS_BIT(oMap.iActionPress, 0u))                     g_MenuInput.bAccept = true;
            if(CONTAINS_BIT(oMap.iActionPress, 1u))                     g_MenuInput.bCancel = true;
            if(CONTAINS_BIT(oMap.iActionPress, INPUT_KEYS_ACTION - 1u)) g_MenuInput.bPause  = true;
        }
    }

    // 
         if(!coreMath::IsNear(g_TotalInput.vMove.x, 0.0f)) g_MenuInput.iMove = (g_TotalInput.vMove.x > 0.0f) ? 4u : 2u;
    else if(!coreMath::IsNear(g_TotalInput.vMove.y, 0.0f)) g_MenuInput.iMove = (g_TotalInput.vMove.y > 0.0f) ? 1u : 3u;

    // 
    else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(UP),    CORE_INPUT_HOLD)) g_MenuInput.iMove = 1u;
    else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LEFT),  CORE_INPUT_HOLD)) g_MenuInput.iMove = 2u;
    else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(DOWN),  CORE_INPUT_HOLD)) g_MenuInput.iMove = 3u;
    else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RIGHT), CORE_INPUT_HOLD)) g_MenuInput.iMove = 4u;

    // 
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RETURN),      CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_ENTER),    CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(CORE_INPUT_KEY(SPACE),       CORE_INPUT_PRESS)) g_MenuInput.bAccept     = true;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE),      CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(CORE_INPUT_KEY(BACKSPACE),   CORE_INPUT_PRESS) ||
       Core::Input->GetMouseButton   (CORE_INPUT_RIGHT,            CORE_INPUT_PRESS)) g_MenuInput.bCancel     = true;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE),      CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PAUSE),       CORE_INPUT_PRESS)) g_MenuInput.bPause      = true;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PRINTSCREEN), CORE_INPUT_PRESS)) g_MenuInput.bScreenshot = true;
}