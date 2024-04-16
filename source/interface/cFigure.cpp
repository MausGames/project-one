///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreList<coreTexturePtr> cFigure::s_apTexCache = {};
coreList<coreUint8>      cFigure::s_apTexBase  = {};


// ****************************************************************
// constructor
cFigure::cFigure()noexcept
: m_iBase    (SDL_CONTROLLER_TYPE_UNKNOWN)
, m_iBaseRef (UINT8_MAX)
, m_iKey     (SDL_CONTROLLER_BUTTON_INVALID)
, m_iKeyRef  (INT16_MAX)
{
    // 
    this->DefineProgram("default_2d_program");

    // 
    m_Fallback.Construct(MENU_FONT_STANDARD_1, TOOLTIP_OUTLINE_SIZE);
    m_Fallback.SetColor3(COLOR_MENU_WHITE);
}


// ****************************************************************
// 
void cFigure::Render()
{
    if(!this->IsEnabled(CORE_OBJECT_ENABLE_RENDER)) return;

    // 
    this->cGuiObject::Render();

    // 
    m_Fallback.Render();
}


// ****************************************************************
// 
void cFigure::Move()
{
    if(!this->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) return;

    if((m_iBase != m_iBaseRef) || (m_iKey != m_iKeyRef))
    {
        const coreHashString pcTexture  = cFigure::DetermineTexture  (m_iBase);
        const coreVector2    vTexOffset = cFigure::DetermineTexOffset(m_iBase, m_iKey);

        this->DefineTexture(0u, pcTexture);
        this->SetTexSize   ((m_iBase == FIGURE_BASE_KEYBOARD) ? FIGURE_SCALE_KEYBOARD : FIGURE_SCALE_GAMEPAD);

        if(vTexOffset != FIGURE_INVALID)
        {
            this->SetTexOffset(vTexOffset);

            m_Fallback.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
        else
        {
            const coreChar* pcText;
            if(m_iBase == FIGURE_BASE_KEYBOARD)
            {
                pcText = (m_iKey <= 0) ? coreData::ToChars(-m_iKey) : SDL_GetKeyName(SDL_GetKeyFromScancode(SDL_Scancode(m_iKey)));

                this->SetTexOffset(((m_iKey <= 0) ? coreVector2(3.0f,9.0f) : ((std::strlen(pcText) > 3u) ? coreVector2(9.0f,4.0f) : coreVector2(8.0f,4.0f))) * FIGURE_SCALE_KEYBOARD);
            }
            else
            {
                pcText = SDL_GameControllerGetStringForButton(SDL_GameControllerButton(m_iKey));
                pcText = pcText ? coreData::StrToUpper(pcText) : coreData::ToChars(m_iKey);

                this->SetTexOffset(coreVector2(4.0f,0.0f) * FIGURE_SCALE_GAMEPAD);
            }

            m_Fallback.SetText   (pcText);
            m_Fallback.SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }

        // 
        m_iBaseRef = m_iBase;
        m_iKeyRef  = m_iKey;
    }

    // 
    m_Fallback.SetPosition (this->GetPosition ());
    m_Fallback.SetCenter   (this->GetCenter   ());
    m_Fallback.SetAlignment(this->GetAlignment());
    m_Fallback.SetColor3   (this->GetColor3   () * COLOR_MENU_WHITE);
    m_Fallback.SetAlpha    (this->GetAlpha    ());
    m_Fallback.SetScale    (this->GetSize     ().Processed(SIGN));
    m_Fallback.Move();

    // 
    this->cGuiObject::Move();
}


// ****************************************************************
// 
void cFigure::SetBaseAsType(const coreUint8 iType)
{
    // 
    this->SetBase((iType < INPUT_SETS_KEYBOARD) ? FIGURE_BASE_KEYBOARD : Core::Input->GetJoystickGamepadType(iType - INPUT_SETS_KEYBOARD));
}


// ****************************************************************
// 
void cFigure::GlobalInit()
{
    // 
    s_apTexCache.push_back(Core::Manager::Resource->Get<coreTexture>(cFigure::DetermineTexture(FIGURE_BASE_KEYBOARD)));
}


// ****************************************************************
// 
void cFigure::GlobalExit()
{
    // 
    s_apTexCache.clear();
    s_apTexBase .clear();
}


// ****************************************************************
// 
void cFigure::GlobalUpdate()
{
    // 
    const coreUintW iNum = Core::Input->GetJoystickNum() + 1u;

    // 
    s_apTexCache.resize(iNum, NULL);
    s_apTexBase .resize(iNum, 0xFFu);

    // 
    for(coreUintW i = 1u; i < iNum; ++i)
    {
        const coreUint8 iBase = Core::Input->GetJoystickGamepadType(i - 1u);
        if(iBase != s_apTexBase[i])
        {
            s_apTexCache[i] = Core::Manager::Resource->Get<coreTexture>(cFigure::DetermineTexture(iBase));
            s_apTexBase [i] = iBase;
        }
    }
}


// ****************************************************************
// 
coreHashString cFigure::DetermineTexture(const coreUint8 iBase)
{
    switch(iBase)
    {
    case FIGURE_BASE_KEYBOARD:                    return "input_keyboard.png";
    case SDL_CONTROLLER_TYPE_XBOX360:             return "input_gamepad_xbox360.png";
    case SDL_CONTROLLER_TYPE_XBOXONE:             return "input_gamepad_xboxone.png";
    case SDL_CONTROLLER_TYPE_PS3:                 return "input_gamepad_ps3.png";
    case SDL_CONTROLLER_TYPE_PS4:                 return "input_gamepad_ps4.png";
    case SDL_CONTROLLER_TYPE_PS5:                 return "input_gamepad_ps5.png";
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO: return "input_gamepad_switch.png";
    case SDL_CONTROLLER_TYPE_AMAZON_LUNA:         return "input_gamepad_luna.png";
    case SDL_CONTROLLER_TYPE_GOOGLE_STADIA:       return "input_gamepad_stadia.png";
    case CORE_INPUT_TYPE_STEAM:                   return "input_gamepad_steamdeck.png";
    default:                                      return "input_gamepad_xboxone.png";
    }
}


// ****************************************************************
// 
coreVector2 cFigure::DetermineTexOffset(const coreUint8 iBase, const coreInt16 iKey)
{
    if(iBase == FIGURE_BASE_KEYBOARD)
    {
        if(iKey <= 0)
        {
            switch(-iKey)
            {
            case CORE_INPUT_LEFT:   return coreVector2(0.0f,9.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_MIDDLE: return coreVector2(1.0f,9.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_RIGHT:  return coreVector2(2.0f,9.0f) * FIGURE_SCALE_KEYBOARD;
            default:                return FIGURE_INVALID;
            }
        }
        else
        {
            switch(SDL_GetKeyFromScancode(SDL_Scancode(iKey)))
            {
            case SDLK_0:            return coreVector2(0.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_1:            return coreVector2(1.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_2:            return coreVector2(2.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_3:            return coreVector2(3.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_4:            return coreVector2(4.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_5:            return coreVector2(5.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_6:            return coreVector2(6.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_7:            return coreVector2(7.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_8:            return coreVector2(8.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_9:            return coreVector2(9.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_a:            return coreVector2(0.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_b:            return coreVector2(1.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_c:            return coreVector2(2.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_d:            return coreVector2(3.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_e:            return coreVector2(4.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_f:            return coreVector2(5.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_g:            return coreVector2(6.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_h:            return coreVector2(7.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_i:            return coreVector2(8.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_j:            return coreVector2(9.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_k:            return coreVector2(0.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_l:            return coreVector2(1.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_m:            return coreVector2(2.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_n:            return coreVector2(3.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_o:            return coreVector2(4.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_p:            return coreVector2(5.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_q:            return coreVector2(6.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_r:            return coreVector2(7.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_s:            return coreVector2(8.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_t:            return coreVector2(9.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_u:            return coreVector2(0.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_v:            return coreVector2(1.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_w:            return coreVector2(2.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_x:            return coreVector2(3.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_y:            return coreVector2(4.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_z:            return coreVector2(5.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F1:           return coreVector2(6.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F2:           return coreVector2(7.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F3:           return coreVector2(8.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F4:           return coreVector2(9.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F5:           return coreVector2(0.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F6:           return coreVector2(1.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F7:           return coreVector2(2.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F8:           return coreVector2(3.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F9:           return coreVector2(4.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F10:          return coreVector2(5.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F11:          return coreVector2(6.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_F12:          return coreVector2(7.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_UP:           return coreVector2(0.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_LEFT:         return coreVector2(1.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_DOWN:         return coreVector2(2.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_RIGHT:        return coreVector2(3.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_PLUS:         return coreVector2(4.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_MINUS:        return coreVector2(5.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_ASTERISK:     return coreVector2(6.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_BACKSLASH:    return coreVector2(7.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_LESS:         return coreVector2(0.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_GREATER:      return coreVector2(1.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_LEFTBRACKET:  return coreVector2(2.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_RIGHTBRACKET: return coreVector2(3.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_QUESTION:     return coreVector2(4.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_QUOTEDBL:     return coreVector2(5.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_SEMICOLON:    return coreVector2(6.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_ESCAPE:       return coreVector2(0.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_LGUI:         return (DEFINED(_CORE_MACOS_) ? coreVector2(2.0f,7.0f) : coreVector2(1.0f,7.0f)) * FIGURE_SCALE_KEYBOARD;
            case SDLK_INSERT:       return coreVector2(3.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_DELETE:       return coreVector2(4.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_HOME:         return coreVector2(5.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_END:          return coreVector2(6.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_PAGEUP:       return coreVector2(7.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_PAGEDOWN:     return coreVector2(8.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_PRINTSCREEN:  return coreVector2(9.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_TAB:          return coreVector2(0.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_LSHIFT:       return coreVector2(1.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_LCTRL:        return coreVector2(2.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_LALT:         return coreVector2(3.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_BACKSPACE:    return coreVector2(4.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_RETURN:       return coreVector2(5.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case SDLK_SPACE:        return coreVector2(6.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            default:                return FIGURE_INVALID;
            }
        }
    }
    else
    {
        switch(iKey)
        {
        case SDL_CONTROLLER_BUTTON_A:             return coreVector2(0.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_B:             return coreVector2(1.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_X:             return coreVector2(2.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_Y:             return coreVector2(3.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_BACK:          return coreVector2(4.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_GUIDE:         return FIGURE_INVALID;
        case SDL_CONTROLLER_BUTTON_START:         return coreVector2(5.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK:     return coreVector2(2.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK:    return coreVector2(3.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return coreVector2(0.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return coreVector2(1.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:       return coreVector2(0.0f,1.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:     return coreVector2(2.0f,1.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:     return coreVector2(1.0f,1.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:    return coreVector2(3.0f,1.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_MISC1:         return coreVector2(4.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_CONTROLLER_BUTTON_PADDLE1:       return FIGURE_INVALID;
        case SDL_CONTROLLER_BUTTON_PADDLE2:       return FIGURE_INVALID;
        case SDL_CONTROLLER_BUTTON_PADDLE3:       return FIGURE_INVALID;
        case SDL_CONTROLLER_BUTTON_PADDLE4:       return FIGURE_INVALID;
        case SDL_CONTROLLER_BUTTON_TOUCHPAD:      return coreVector2(5.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case CORE_INPUT_BUTTON_LEFTTRIGGER:       return coreVector2(2.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case CORE_INPUT_BUTTON_RIGHTTRIGGER:      return coreVector2(3.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        default:                                  return FIGURE_INVALID;
        }
    }
}