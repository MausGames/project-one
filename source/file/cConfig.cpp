///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

sConfig    g_CurConfig               = {};
sConfig    g_OldConfig               = {};
sGameInput g_aGameInput[INPUT_TYPES] = {};
sGameInput g_TotalInput              = {};
coreUint8  g_iTotalType              = INPUT_SETS_KEYBOARD;   // # always start with joystick
sMenuInput g_MenuInput               = {};

static coreBool  s_abFireToggle[INPUT_TYPES + 1u] = {};
static coreBool  s_abFireSpeed [INPUT_TYPES + 1u] = {};
static coreUint8 s_aiTwinState [INPUT_TYPES + 1u] = {};

static coreList<coreVector2> s_avOldStick = {};


// ****************************************************************
// 
static void UpgradeConfig()
{
    coreInt32 iVersion = Core::Config->GetInt(CORE_CONFIG_BASE_VERSION);

    #define __UPGRADE(x) ((iVersion == (x)) && [&]() {iVersion = (x) + 1; return true;}())
    {
        if(__UPGRADE(0))
        {
            Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME, CONFIG_DEFAULT_VOLUME);

            for(coreUintW i = 0u; i < INPUT_SETS; ++i)
            {
                for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
                {
                    Core::Config->SetInt(CONFIG_INPUT_ACTION(i, j), DEFAULT_ACTION(i, j));
                }
            }
        }

        if(__UPGRADE(1))
        {
            Core::Config->SetInt(CONFIG_GAME_GAME_DIRECTION, Core::Config->GetInt("Game", "GameRotation", 0));
            Core::Config->SetInt(CONFIG_GAME_HUD_DIRECTION,  Core::Config->GetInt("Game", "HudRotation",  0));
        }
    }
    #undef __UPGRADE

    Core::Config->SetInt(CORE_CONFIG_BASE_VERSION, iVersion);
}


// ****************************************************************
// check configuration for valid values
static void CheckConfig(sConfig* OUTPUT pConfig)
{
#if defined(CONFIG_FORCE)

    // force specific settings
    g_OldConfig.Graphics.iReflection = 1u;
    g_OldConfig.Graphics.iGlow       = 1u;
    //g_OldConfig.Graphics.iDistortion = 1u;

#endif

    // clamp input set selections
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
    {
        pConfig->Input.aiType[i] = CLAMP(pConfig->Input.aiType[i], 0u, INPUT_SETS-1u);

        //           without available device               
        if((pConfig->Input.aiType[i] >= INPUT_SETS_KEYBOARD) && (pConfig->Input.aiType[i] - INPUT_SETS_KEYBOARD >= Core::Input->GetJoystickNum()))
        {
            pConfig->Input.aiType[i] = i;
        }
        // TODO 1: vielleicht zum ersten ungenutzten keyboard set wechseln
    }

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
        {
            oSet.aiAction[j] = CLAMP(oSet.aiAction[j], iFrom, iTo);
        }
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
                {
                    pConfig->Input.aiType[k] = k;
                }
        // TODO 1: vielleicht erst zu joystick zurücksetzen wenn möglich

                // leave all loops
                i = j = INPUT_TYPES;
            }
        }
    }

#if defined(_CORE_SWITCH_)

    // 
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
    {
        pConfig->Input.aiType[i] = INPUT_SETS_KEYBOARD + i;
    }

#endif

    // 
    pConfig->Audio.fEffectVolume  = MAX(pConfig->Audio.fEffectVolume,  0.0f);
    pConfig->Audio.fAmbientVolume = MAX(pConfig->Audio.fAmbientVolume, 0.0f);
    pConfig->Audio.fMenuVolume    = MAX(pConfig->Audio.fMenuVolume,    0.0f);
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
    Core::Config->Save();
    Core::Config->Load();

    // 
    UpgradeConfig();

    // read graphics values
    g_OldConfig.Graphics.iRender     = Core::Config->GetInt(CONFIG_GRAPHICS_RENDER);
    g_OldConfig.Graphics.iShadow     = Core::Config->GetInt(CONFIG_GRAPHICS_SHADOW);
    g_OldConfig.Graphics.iReflection = Core::Config->GetInt(CONFIG_GRAPHICS_REFLECTION);
    g_OldConfig.Graphics.iGlow       = Core::Config->GetInt(CONFIG_GRAPHICS_GLOW);
    g_OldConfig.Graphics.iDistortion = Core::Config->GetInt(CONFIG_GRAPHICS_DISTORTION);
    g_OldConfig.Graphics.iShake      = Core::Config->GetInt(CONFIG_GRAPHICS_SHAKE);
    g_OldConfig.Graphics.iFlash      = Core::Config->GetInt(CONFIG_GRAPHICS_FLASH);
    g_OldConfig.Graphics.iHitStop    = Core::Config->GetInt(CONFIG_GRAPHICS_HIT_STOP);
    g_OldConfig.Graphics.iChroma     = Core::Config->GetInt(CONFIG_GRAPHICS_CHROMA);

    // read audio values
    g_OldConfig.Audio.fEffectVolume  = Core::Config->GetFloat(CONFIG_AUDIO_EFFECT_VOLUME);
    g_OldConfig.Audio.fAmbientVolume = Core::Config->GetFloat(CONFIG_AUDIO_AMBIENT_VOLUME);
    g_OldConfig.Audio.fMenuVolume    = Core::Config->GetFloat(CONFIG_AUDIO_MENU_VOLUME);
    g_OldConfig.Audio.iQuality       = Core::Config->GetInt  (CONFIG_AUDIO_QUALITY);
    g_OldConfig.Audio.i3DSound       = Core::Config->GetInt  (CONFIG_AUDIO_3D_SOUND);

    // read input values
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
    {
        g_OldConfig.Input.aiType       [i] = Core::Config->GetInt(CONFIG_INPUT_TYPE        (i));
        g_OldConfig.Input.aiRumble     [i] = Core::Config->GetInt(CONFIG_INPUT_RUMBLE      (i));
        g_OldConfig.Input.aiFireMode   [i] = Core::Config->GetInt(CONFIG_INPUT_FIRE_MODE   (i));
        g_OldConfig.Input.aiControlMode[i] = Core::Config->GetInt(CONFIG_INPUT_CONTROL_MODE(i));
    }
    for(coreUintW i = 0u; i < INPUT_SETS;  ++i)
    {
        g_OldConfig.Input.aSet[i].iMoveUp    = Core::Config->GetInt(CONFIG_INPUT_MOVE_UP   (i), DEFAULT_MOVE_UP   (i));
        g_OldConfig.Input.aSet[i].iMoveLeft  = Core::Config->GetInt(CONFIG_INPUT_MOVE_LEFT (i), DEFAULT_MOVE_LEFT (i));
        g_OldConfig.Input.aSet[i].iMoveDown  = Core::Config->GetInt(CONFIG_INPUT_MOVE_DOWN (i), DEFAULT_MOVE_DOWN (i));
        g_OldConfig.Input.aSet[i].iMoveRight = Core::Config->GetInt(CONFIG_INPUT_MOVE_RIGHT(i), DEFAULT_MOVE_RIGHT(i));
        for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
        {
            g_OldConfig.Input.aSet[i].aiAction[j] = Core::Config->GetInt(CONFIG_INPUT_ACTION(i, j), DEFAULT_ACTION(i, j));
        }
    }

    // read game values
    g_OldConfig.Game.iGameDirection = Core::Config->GetInt(CONFIG_GAME_GAME_DIRECTION);
    g_OldConfig.Game.iGameSpeed     = Core::Config->GetInt(CONFIG_GAME_GAME_SPEED);
    g_OldConfig.Game.iMirrorMode    = Core::Config->GetInt(CONFIG_GAME_MIRROR_MODE);
    g_OldConfig.Game.iHudDirection  = Core::Config->GetInt(CONFIG_GAME_HUD_DIRECTION);
    g_OldConfig.Game.iHudType       = Core::Config->GetInt(CONFIG_GAME_HUD_TYPE);
    g_OldConfig.Game.iCombatText    = Core::Config->GetInt(CONFIG_GAME_COMBAT_TEXT);
    g_OldConfig.Game.iBackRotation  = Core::Config->GetInt(CONFIG_GAME_BACK_ROTATION);
    g_OldConfig.Game.iBackSpeed     = Core::Config->GetInt(CONFIG_GAME_BACK_SPEED);
    g_OldConfig.Game.iUpdateFreq    = Core::Config->GetInt(CONFIG_GAME_UPDATE_FREQ);
    g_OldConfig.Game.iPureMode      = Core::Config->GetInt(CONFIG_GAME_PURE_MODE);
    g_OldConfig.Game.iVersion       = Core::Config->GetInt(CONFIG_GAME_VERSION);

    // 
    g_OldConfig.Legacy.iRotationTurn = Core::Config->GetInt(CONFIG_LEGACY_ROTATION_TURN);

    // check configuration for valid values
    CheckConfig(&g_OldConfig);

    // 
    Core::Audio->SetSoundVolume(1.0f);
    Core::Audio->SetTypeVolume(g_OldConfig.Audio.fEffectVolume,  SOUND_EFFECT);
    Core::Audio->SetTypeVolume(g_OldConfig.Audio.fAmbientVolume, SOUND_AMBIENT);
    Core::Audio->SetTypeVolume(g_OldConfig.Audio.fMenuVolume,    SOUND_MENU);

    // forward values to the current structure
    g_CurConfig = g_OldConfig;
}


// ****************************************************************
// save configuration
void SaveConfig()
{
    // equalize both structures
    g_OldConfig = g_CurConfig;

    // write graphics values
    Core::Config->SetInt(CONFIG_GRAPHICS_RENDER,     g_OldConfig.Graphics.iRender);
    Core::Config->SetInt(CONFIG_GRAPHICS_SHADOW,     g_OldConfig.Graphics.iShadow);
    Core::Config->SetInt(CONFIG_GRAPHICS_REFLECTION, g_OldConfig.Graphics.iReflection);
    Core::Config->SetInt(CONFIG_GRAPHICS_GLOW,       g_OldConfig.Graphics.iGlow);
    Core::Config->SetInt(CONFIG_GRAPHICS_DISTORTION, g_OldConfig.Graphics.iDistortion);
    Core::Config->SetInt(CONFIG_GRAPHICS_SHAKE,      g_OldConfig.Graphics.iShake);
    Core::Config->SetInt(CONFIG_GRAPHICS_FLASH,      g_OldConfig.Graphics.iFlash);
    Core::Config->SetInt(CONFIG_GRAPHICS_HIT_STOP,   g_OldConfig.Graphics.iHitStop);
    Core::Config->SetInt(CONFIG_GRAPHICS_CHROMA,     g_OldConfig.Graphics.iChroma);

    // write audio values
    Core::Config->SetFloat(CONFIG_AUDIO_EFFECT_VOLUME,  g_OldConfig.Audio.fEffectVolume);
    Core::Config->SetFloat(CONFIG_AUDIO_AMBIENT_VOLUME, g_OldConfig.Audio.fAmbientVolume);
    Core::Config->SetFloat(CONFIG_AUDIO_MENU_VOLUME,    g_OldConfig.Audio.fMenuVolume);
    Core::Config->SetInt  (CONFIG_AUDIO_QUALITY,        g_OldConfig.Audio.iQuality);
    Core::Config->SetInt  (CONFIG_AUDIO_3D_SOUND,       g_OldConfig.Audio.i3DSound);

    // write input values
    for(coreUintW i = 0u; i < INPUT_TYPES; ++i)
    {
        Core::Config->SetInt(CONFIG_INPUT_TYPE        (i), g_OldConfig.Input.aiType       [i]);
        Core::Config->SetInt(CONFIG_INPUT_RUMBLE      (i), g_OldConfig.Input.aiRumble     [i]);
        Core::Config->SetInt(CONFIG_INPUT_FIRE_MODE   (i), g_OldConfig.Input.aiFireMode   [i]);
        Core::Config->SetInt(CONFIG_INPUT_CONTROL_MODE(i), g_OldConfig.Input.aiControlMode[i]);
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

    // write game values
    Core::Config->SetInt(CONFIG_GAME_GAME_DIRECTION, g_OldConfig.Game.iGameDirection);
    Core::Config->SetInt(CONFIG_GAME_GAME_SPEED,     g_OldConfig.Game.iGameSpeed);
    Core::Config->SetInt(CONFIG_GAME_MIRROR_MODE,    g_OldConfig.Game.iMirrorMode);
    Core::Config->SetInt(CONFIG_GAME_HUD_DIRECTION,  g_OldConfig.Game.iHudDirection);
    Core::Config->SetInt(CONFIG_GAME_HUD_TYPE,       g_OldConfig.Game.iHudType);
    Core::Config->SetInt(CONFIG_GAME_COMBAT_TEXT,    g_OldConfig.Game.iCombatText);
    Core::Config->SetInt(CONFIG_GAME_BACK_ROTATION,  g_OldConfig.Game.iBackRotation);
    Core::Config->SetInt(CONFIG_GAME_BACK_SPEED,     g_OldConfig.Game.iBackSpeed);
    Core::Config->SetInt(CONFIG_GAME_UPDATE_FREQ,    g_OldConfig.Game.iUpdateFreq);
    Core::Config->SetInt(CONFIG_GAME_PURE_MODE,      g_OldConfig.Game.iPureMode);
    Core::Config->SetInt(CONFIG_GAME_VERSION,        g_OldConfig.Game.iVersion);

    // 
    Core::Config->SetInt(CONFIG_LEGACY_ROTATION_TURN, g_OldConfig.Legacy.iRotationTurn);

    // 
    Core::Config->SetInt (CORE_CONFIG_GRAPHICS_QUALITY,          g_OldConfig.Graphics.iRender);
    Core::Config->SetBool(CORE_CONFIG_GRAPHICS_TEXTURETRILINEAR, g_OldConfig.Graphics.iRender ? true : false);
    Core::Config->SetInt (CORE_CONFIG_AUDIO_RESAMPLERINDEX,      g_OldConfig.Audio.iQuality ? 4 : 2);

    // save configuration file
    Core::Config->Save();
}


// ****************************************************************
// update input interface
void UpdateInput()
{
    // 
    s_avOldStick.resize(Core::Input->GetJoystickNum(), coreVector2(0.0f,0.0f));

    // forward hat input to stick input
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        Core::Input->ForwardHatToStick(i);
    }

    // reset mapped input values
    std::memset(&g_aGameInput, 0, sizeof(g_aGameInput));
    std::memset(&g_TotalInput, 0, sizeof(g_TotalInput));
    std::memset(&g_MenuInput,  0, sizeof(g_MenuInput));

    // loop trough input sets
    for(coreUintW i = DEFINED(_CORE_SWITCH_) ? INPUT_SETS_KEYBOARD : 0u; i < INPUT_SETS; ++i)
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
                if(nCheckKeyFunc(oSet.aiAction[j], CORE_INPUT_PRESS))   ADD_BIT(oMap.iActionPress,   j)
                if(nCheckKeyFunc(oSet.aiAction[j], CORE_INPUT_RELEASE)) ADD_BIT(oMap.iActionRelease, j)
                if(nCheckKeyFunc(oSet.aiAction[j], CORE_INPUT_HOLD))    ADD_BIT(oMap.iActionHold,    j)
            }
        }
        else   // # joystick/gamepad
        {
            const coreUintW iJoystickID = i - INPUT_SETS_KEYBOARD;

            // map movement input
            oMap.vMove = Core::Input->GetJoystickRelativeL(iJoystickID);

            // restrict movement input to the 8 base directions
            if(!oMap.vMove.IsNull()) oMap.vMove = AlongStarNormal(oMap.vMove);

            // map action input
            for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
            {
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiAction[j]), CORE_INPUT_PRESS))   ADD_BIT(oMap.iActionPress,   j)
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiAction[j]), CORE_INPUT_RELEASE)) ADD_BIT(oMap.iActionRelease, j)
                if(Core::Input->GetJoystickButton(iJoystickID, coreUint8(oSet.aiAction[j]), CORE_INPUT_HOLD))    ADD_BIT(oMap.iActionHold,    j)
            }
            
            if(Core::Input->GetJoystickButton(iJoystickID, SDL_CONTROLLER_BUTTON_A, CORE_INPUT_PRESS)) g_MenuInput.bAccept = true;
            if(Core::Input->GetJoystickButton(iJoystickID, SDL_CONTROLLER_BUTTON_B, CORE_INPUT_PRESS)) g_MenuInput.bCancel = true;

            if((iJoystickID < s_avOldStick.size()))
            {
                if(STATIC_ISVALID(g_pGame) && !g_pMenu->IsPaused())
                {
                    //const sGameInput* pCheck = NULL;
                    //for(coreUintW j = 0u; j < INPUT_TYPES; ++j)
                    //{
                    //    if(g_CurConfig.Input.aiType[j] == i)
                    //    {
                    //        pCheck = &g_aGameInput[j];
                    //        break;
                    //    }
                    //}
                    
                    //coreVector2 vPlayerDir;
                    //g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
                    //{
                    //    const sGameInput* pInput = pPlayer->GetInput();
                    //    if((pInput == &g_TotalInput) || (pInput == pCheck)) vPlayerDir = pPlayer->GetDirection().xy();
                    //});
                    
                    coreVector2 vNewStick = Core::Input->GetJoystickRelativeR(iJoystickID);
                    //if(vNewStick.IsNull()) s_avOldStick[iJoystickID] = vPlayerDir;
                    
                    const coreVector2 vOldStick = s_avOldStick[iJoystickID];
                    
                    const coreBool bValid    = (vNewStick.LengthSq() >= POW2(0.7f));
                    const coreBool bValidOld = (vOldStick.LengthSq() >= POW2(0.7f));
                    if(!vNewStick.IsNull()) vNewStick = vNewStick.Normalized();
                    
                    
                    for(coreUintW j = 0u; j < 4u; ++j)
                    {
                        const coreVector2 vBase = StepRotated90(j);
                        const coreBool bOldState = (!vOldStick.IsNull() && (coreVector2::Dot(vOldStick, vBase) >= (1.0f / SQRT2))) && bValidOld;
                        const coreBool bNewState = (!vNewStick.IsNull() && (coreVector2::Dot(vNewStick, vBase) >= (bOldState ? 0.01f : (vOldStick.IsNull() ? (1.0f / SQRT2) : 0.8f))) && (bOldState || bValid));
                        
                        if(!bOldState &&  bNewState) 
                            ADD_BIT(oMap.iActionPress,   3u + j)
                        if( bOldState && !bNewState) 
                            ADD_BIT(oMap.iActionRelease, 3u + j)
                        if( bNewState)               
                            ADD_BIT(oMap.iActionHold,    3u + j)
                        
                        
                        if(!bOldState && bNewState) s_avOldStick[iJoystickID] = AlongCrossNormal(vNewStick);
                    }
                       if(vNewStick.IsNull())     s_avOldStick[iJoystickID] = coreVector2(0.0f,0.0f);
                }
                else
                {
                    s_avOldStick[iJoystickID] = coreVector2(0.0f,0.0f);
                }
            }
            
            
            // TODO 1: es gibt keinen cancel-button mehr
            //if(Core::Input->GetJoystickButton(iJoystickID, SDL_CONTROLLER_BUTTON_B, CORE_INPUT_PRESS)) g_MenuInput.bCancel = true;
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
        if(!oMap.vMove.IsNull() || oMap.iActionPress) g_iTotalType = i;

        // 
        if(i >= INPUT_SETS_KEYBOARD)
        {
            //if(HAS_BIT(oMap.iActionPress, 0u))                     g_MenuInput.bAccept = true;
            //if(HAS_BIT(oMap.iActionPress, 1u))                     g_MenuInput.bCancel = true;
            if(HAS_BIT(oMap.iActionPress, 7u)) g_MenuInput.bPause  = true;
        }
    }

    // 
    const auto nDirectionFunc = [](sGameInput* OUTPUT pInput)
    {
        if(!pInput->vMove.IsNull())
        {
            // 
            const coreVector2 vFinal = CalcFinalDirection();

            // 
            pInput->vMove = MapToAxis(pInput->vMove, vFinal);
            pInput->vMove = pInput->vMove.NormalizedUnsafe();
        }

        if(g_CurConfig.Game.iMirrorMode == 1u)
        {
            // 
            pInput->vMove = pInput->vMove.InvertedX();

            // 
            const auto nFlipTurnFunc = [](coreUint16* OUTPUT piAction)
            {
                const coreBool bBit1 = HAS_BIT(*piAction, 1u);
                const coreBool bBit2 = HAS_BIT(*piAction, 2u);

                SET_BIT(*piAction, 1u, bBit2)
                SET_BIT(*piAction, 2u, bBit1)
            };
            nFlipTurnFunc(&pInput->iActionPress);
            nFlipTurnFunc(&pInput->iActionRelease);
            nFlipTurnFunc(&pInput->iActionHold);
        }
    };
    nDirectionFunc(&g_aGameInput[0]);
    nDirectionFunc(&g_aGameInput[1]);
    nDirectionFunc(&g_TotalInput);

    // 
    const auto nControlModeFunc = [](sGameInput* OUTPUT pInput, const coreUintW iModeIndex, const coreUintW iStateIndex)
    {
        constexpr coreUint16 iTurnBits   = BIT(1u) | BIT(2u);
        constexpr coreUint16 iShootBits1 = BIT(3u) | BIT(4u) | BIT(5u) | BIT(6u);
        constexpr coreUint16 iShootBits2 = iShootBits1 | BIT(0u) | BIT(8u);

        const coreUint8 iControlMode = g_CurConfig.Input.aiControlMode[iModeIndex];
        if((iControlMode == 1u) || (iControlMode == 2u))
        {
            
            coreBool bAllow = true;
            const coreUint8 iFireMode = g_CurConfig.Input.aiFireMode[iModeIndex];
            if((iFireMode == 2u) && s_abFireToggle[iStateIndex] && STATIC_ISVALID(g_pGame))
            {
                const coreFloat   fSide  = (g_CurConfig.Game.iMirrorMode == 1u) ? -1.0f : 1.0f;
                const coreVector2 vFinal = CalcFinalDirection();// * coreVector2(fSide, 1.0f);

                const coreVector2 vFlip = coreVector2(fSide, 1.0f);

                const cPlayer*    pPlayer = g_pGame->GetPlayer(iModeIndex);
                const coreVector2 vDir    = MapToAxis(pPlayer->GetDirection().xy(), vFinal);
                
                if((SameDirection90(vDir, coreVector2( 0.0f, 1.0f) * vFlip) && !HAS_BIT(pInput->iActionPress, 3u)) ||
                   (SameDirection90(vDir, coreVector2(-1.0f, 0.0f) * vFlip) && !HAS_BIT(pInput->iActionPress, 4u)) ||
                   (SameDirection90(vDir, coreVector2( 0.0f,-1.0f) * vFlip) && !HAS_BIT(pInput->iActionPress, 5u)) ||
                   (SameDirection90(vDir, coreVector2( 1.0f, 0.0f) * vFlip) && !HAS_BIT(pInput->iActionPress, 6u)))
                {
                    bAllow = false;
                }
            }
            
            
            // 
            SET_BIT(pInput->iActionPress,   0u, (pInput->iActionPress   & ((iControlMode == 1u) ? iShootBits1 : iShootBits2)) && !HAS_BIT(s_aiTwinState[iStateIndex], 1u) && bAllow)
            SET_BIT(pInput->iActionHold,    0u, (pInput->iActionHold    & ((iControlMode == 1u) ? iShootBits1 : iShootBits2)))
            SET_BIT(pInput->iActionRelease, 0u, (pInput->iActionRelease & ((iControlMode == 1u) ? iShootBits1 : iShootBits2)) && !HAS_BIT(pInput->iActionHold, 0u))

            // 
            if(iControlMode == 1u)
            {
                REMOVE_FLAG(pInput->iActionPress,   iTurnBits | BIT(8u))
                REMOVE_FLAG(pInput->iActionHold,    iTurnBits | BIT(8u))
                REMOVE_FLAG(pInput->iActionRelease, iTurnBits | BIT(8u))
            }

            // 
            SET_BIT(s_aiTwinState[iStateIndex], 1u, HAS_BIT(pInput->iActionHold, 0u))
            
            
            //if(HAS_BIT(pInput->iActionPress, 9u) && !g_pMenu->IsPaused())
            //{
            //    SET_BIT(s_aiTwinState[iStateIndex], 2u, !HAS_BIT(s_aiTwinState[iStateIndex], 2u))
            //    s_abFireSpeed[iStateIndex] = !s_abFireSpeed[iStateIndex];
            //}
            if(HAS_BIT(s_aiTwinState[iStateIndex], 2u))
            {
                SET_BIT(pInput->iActionPress,   8u, HAS_BIT(pInput->iActionPress,   0u))
                SET_BIT(pInput->iActionRelease, 8u, HAS_BIT(pInput->iActionRelease, 0u))
                SET_BIT(pInput->iActionHold,    8u, HAS_BIT(pInput->iActionHold,    0u))
                SET_BIT(pInput->iActionPress,   0u, false)
                SET_BIT(pInput->iActionRelease, 0u, false)
                SET_BIT(pInput->iActionHold,    0u, false)
            }
        }
        else if(iControlMode == 3u)
        {
            // 
            REMOVE_FLAG(pInput->iActionPress,   iShootBits1 | iTurnBits)
            REMOVE_FLAG(pInput->iActionHold,    iShootBits1 | iTurnBits)
            REMOVE_FLAG(pInput->iActionRelease, iShootBits1 | iTurnBits)
        }
        else
        {
            // 
            REMOVE_FLAG(pInput->iActionPress,   iShootBits1)
            REMOVE_FLAG(pInput->iActionHold,    iShootBits1)
            REMOVE_FLAG(pInput->iActionRelease, iShootBits1)
        }

        // 
        if(((iControlMode == 0u) || (iControlMode == 3u)) || !STATIC_ISVALID(g_pGame))
            s_aiTwinState[iStateIndex] = 0u;

        // 
        SET_BIT(pInput->iStatus, 2u, HAS_BIT(s_aiTwinState[iStateIndex], 2u))
    };
    nControlModeFunc(&g_aGameInput[0], 0u, 0u);
    nControlModeFunc(&g_aGameInput[1], 1u, 1u);
    nControlModeFunc(&g_TotalInput,    0u, 2u);

    // 
    const auto nFireModeFunc = [](sGameInput* OUTPUT pInput, const coreUintW iModeIndex, const coreUintW iToggleIndex)
    {
        const coreBool bFireA = HAS_BIT(pInput->iActionPress, 0u);
        const coreBool bFireB = HAS_BIT(pInput->iActionPress, 8u);

        const coreUint8 iFireMode = g_CurConfig.Input.aiFireMode[iModeIndex];
        if(iFireMode == 1u)
        {
            // 
            const coreBool bPress   = HAS_BIT(pInput->iActionPress,   0u) || HAS_BIT(pInput->iActionPress,   8u);
            const coreBool bRelease = HAS_BIT(pInput->iActionRelease, 0u) || HAS_BIT(pInput->iActionRelease, 8u);
            const coreBool bHold    = HAS_BIT(pInput->iActionHold,    0u) || HAS_BIT(pInput->iActionHold,    8u);

            SET_BIT(pInput->iActionPress,   0u,  bRelease)
            SET_BIT(pInput->iActionRelease, 0u,  bPress)
            SET_BIT(pInput->iActionHold,    0u, !bHold)
        }
        else if(iFireMode == 2u)
        {
            // 
            const coreBool bPress = s_abFireToggle[iToggleIndex] ? (s_abFireSpeed[iToggleIndex] ? bFireB : bFireA) : (bFireA || bFireB);
            if(bPress) s_abFireToggle[iToggleIndex] = !s_abFireToggle[iToggleIndex];

            SET_BIT(pInput->iStatus, 1u, HAS_BIT(pInput->iActionHold, 0u))

            SET_BIT(pInput->iActionPress,   0u,  s_abFireToggle[iToggleIndex] && bPress)
            SET_BIT(pInput->iActionRelease, 0u, !s_abFireToggle[iToggleIndex] && bPress)
            SET_BIT(pInput->iActionHold,    0u,  s_abFireToggle[iToggleIndex])
        }

        if((iFireMode == 1u) || (iFireMode == 2u))
        {
                 if(bFireA) s_abFireSpeed[iToggleIndex] = false;
            else if(bFireB) s_abFireSpeed[iToggleIndex] = true;

            SET_BIT(pInput->iActionPress,   8u,  s_abFireSpeed[iToggleIndex] && HAS_BIT(pInput->iActionPress,   0u)) if(s_abFireSpeed[iToggleIndex]) REMOVE_BIT(pInput->iActionPress,   0u)
            SET_BIT(pInput->iActionRelease, 8u,  s_abFireSpeed[iToggleIndex] && HAS_BIT(pInput->iActionRelease, 0u)) if(s_abFireSpeed[iToggleIndex]) REMOVE_BIT(pInput->iActionRelease, 0u)
            SET_BIT(pInput->iActionHold,    8u,  s_abFireSpeed[iToggleIndex] && HAS_BIT(pInput->iActionHold,    0u)) if(s_abFireSpeed[iToggleIndex]) REMOVE_BIT(pInput->iActionHold,    0u)
        }

        // 
        if((iFireMode != 2u) || !STATIC_ISVALID(g_pGame))
            s_abFireToggle[iToggleIndex] = false;

        // 
        if(((iFireMode != 1u) && (iFireMode != 2u)) || !STATIC_ISVALID(g_pGame))
            s_abFireSpeed[iToggleIndex] = false;

        // 
        SET_BIT(pInput->iStatus, 0u, s_abFireToggle[iToggleIndex])
        SET_BIT(pInput->iStatus, 1u, s_abFireSpeed [iToggleIndex])
    };
    nFireModeFunc(&g_aGameInput[0], 0u, 0u);
    nFireModeFunc(&g_aGameInput[1], 1u, 1u);
    nFireModeFunc(&g_TotalInput,    0u, 2u);

    // 
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        coreVector3 vColor = COLOR_LED_WHITE;

        // 
        for(coreUintW j = 0u; j < INPUT_TYPES; ++j)
        {
            const coreUint8 iType       = (!STATIC_ISVALID(g_pGame) || g_pGame->IsMulti()) ? g_CurConfig.Input.aiType[j] : g_iTotalType;
            const coreUintW iJoystickID = iType - INPUT_SETS_KEYBOARD;

            if(i == iJoystickID)
            {
                vColor = (STATIC_ISVALID(g_pGame) && (j < g_pGame->GetNumPlayers())) ? g_pGame->GetPlayer(j)->GetLedColor() : (j ? COLOR_LED_YELLOW : COLOR_LED_BLUE);
                break;
            }
        }

        // 
        Core::Input->JoystickChangeLED(i, vColor);
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
    //if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RETURN),      CORE_INPUT_PRESS) ||
    //   Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_ENTER),    CORE_INPUT_PRESS) ||
    //   Core::Input->GetKeyboardButton(CORE_INPUT_KEY(SPACE),       CORE_INPUT_PRESS)) g_MenuInput.bAccept     = true;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE),      CORE_INPUT_PRESS) ||
       Core::Input->GetKeyboardButton(CORE_INPUT_KEY(BACKSPACE),   CORE_INPUT_PRESS) ||
       Core::Input->GetMouseButton   (CORE_INPUT_RIGHT,            CORE_INPUT_PRESS)) g_MenuInput.bCancel     = true;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE),      CORE_INPUT_PRESS)) g_MenuInput.bPause      = true;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PRINTSCREEN), CORE_INPUT_PRESS)) g_MenuInput.bScreenshot = true;
    
    
    
    g_MenuInput.bAny = g_MenuInput.bAccept || g_MenuInput.bCancel || g_MenuInput.bPause || Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_PRESS);
}