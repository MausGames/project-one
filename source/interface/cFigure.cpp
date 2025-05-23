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
: m_iBase    (SDL_GAMEPAD_TYPE_UNKNOWN)
, m_iBaseRef (UINT8_MAX)
, m_iKey     (SDL_GAMEPAD_BUTTON_INVALID)
, m_iKeyRef  (INT16_MAX)
{
    // 
    this->DefineProgram("default_2d_program");

    // 
    m_Fallback.Construct(MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
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
    m_Fallback.SetAlpha(this->GetAlpha());
    m_Fallback.Render();
}


// ****************************************************************
// 
void cFigure::Move()
{
    if(!this->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) return;

    if((m_iBase != m_iBaseRef) || (m_iKey != m_iKeyRef))
    {
        // 
        const coreHashString pcTexture  = cFigure::DetermineTexture  (m_iBase);
        const coreVector2    vTexOffset = cFigure::DetermineTexOffset(m_iBase, m_iKey);

        // 
        this->DefineTexture(0u, pcTexture);
        this->SetTexSize   ((m_iBase == FIGURE_BASE_KEYBOARD) ? FIGURE_SCALE_KEYBOARD : FIGURE_SCALE_GAMEPAD);

        if(vTexOffset != FIGURE_INVALID)
        {
                // 
            this->SetTexOffset(vTexOffset);

            // 
            m_Fallback.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
        else
        {
            const coreChar* pcText;
            if(m_iKey == INPUT_KEY_INVALID)
            {
                // 
                pcText = "-";

                // 
                this->SetTexOffset(coreVector2(1.0f,1.0f) - this->GetTexSize());
            }
            else if(m_iBase == FIGURE_BASE_KEYBOARD)
            {
                if(m_iKey <= 0)
                {
                    // 
                    pcText = coreData::ToChars(-m_iKey);

                    // 
                    this->SetTexOffset(coreVector2(3.0f,9.0f) * FIGURE_SCALE_KEYBOARD);
                }
                else
                {
                    // 
                    pcText = SDL_GetKeyName(SDL_GetKeyFromScancode(SDL_Scancode(m_iKey), SDL_KMOD_NONE, true));

                    // 
                    this->SetTexOffset(((std::strlen(pcText) > 3u) ? coreVector2(9.0f,4.0f) : coreVector2(8.0f,4.0f)) * FIGURE_SCALE_KEYBOARD);
                }
            }
            else
            {
                // 
                pcText = SDL_GetGamepadStringForButton(SDL_GamepadButton(m_iKey));
                pcText = pcText ? coreData::StrToUpper(pcText) : coreData::ToChars(m_iKey);

                // 
                this->SetTexOffset(coreVector2(4.0f,0.0f) * FIGURE_SCALE_GAMEPAD);
            }

            // 
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
#if defined(_CORE_SWITCH_)

    // 
    switch(iBase)
    {
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:          return "input_gamepad_switch_full.png";
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:  return "input_gamepad_switch_left.png";
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: return "input_gamepad_switch_right.png";
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:  return "input_gamepad_switch_full.png";
    default:                                            return "input_gamepad_switch_full.png";
    }

#else

    // 
    switch(iBase)
    {
    case FIGURE_BASE_KEYBOARD:                          return "input_keyboard.png";
    case SDL_GAMEPAD_TYPE_XBOX360:                      return "input_gamepad_xbox360.png";
    case SDL_GAMEPAD_TYPE_XBOXONE:                      return "input_gamepad_xboxone.png";
    case SDL_GAMEPAD_TYPE_PS3:                          return "input_gamepad_ps3.png";
    case SDL_GAMEPAD_TYPE_PS4:                          return "input_gamepad_ps4.png";
    case SDL_GAMEPAD_TYPE_PS5:                          return "input_gamepad_ps5.png";
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:          return "input_gamepad_switch.png";
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:  return "input_gamepad_switch.png";
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: return "input_gamepad_switch.png";
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:  return "input_gamepad_switch.png";
    case CORE_INPUT_TYPE_STEAM:                         return "input_gamepad_steamdeck.png";
    case CORE_INPUT_TYPE_LUNA:                          return "input_gamepad_luna.png";
    case CORE_INPUT_TYPE_STADIA:                        return "input_gamepad_stadia.png";
    default:                                            return "input_gamepad_xboxone.png";
    }

#endif
}


// ****************************************************************
// 
coreVector2 cFigure::DetermineTexOffset(const coreUint8 iBase, const coreInt16 iKey)
{
    // 
    if(iKey == INPUT_KEY_INVALID)
    {
        return FIGURE_INVALID;
    }

    // 
    if(iBase == FIGURE_BASE_KEYBOARD)
    {
        if(iKey <= 0)
        {
            // 
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
            // 
            switch(SDL_GetKeyFromScancode(SDL_Scancode(iKey), SDL_KMOD_NONE, true))
            {
            case CORE_INPUT_CHAR(0):             return coreVector2(0.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(1):             return coreVector2(1.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(2):             return coreVector2(2.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(3):             return coreVector2(3.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(4):             return coreVector2(4.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(5):             return coreVector2(5.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(6):             return coreVector2(6.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(7):             return coreVector2(7.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(8):             return coreVector2(8.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(9):             return coreVector2(9.0f,0.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(A):             return coreVector2(0.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(B):             return coreVector2(1.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(C):             return coreVector2(2.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(D):             return coreVector2(3.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(E):             return coreVector2(4.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F):             return coreVector2(5.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(G):             return coreVector2(6.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(H):             return coreVector2(7.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(I):             return coreVector2(8.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(J):             return coreVector2(9.0f,1.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(K):             return coreVector2(0.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(L):             return coreVector2(1.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(M):             return coreVector2(2.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(N):             return coreVector2(3.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(O):             return coreVector2(4.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(P):             return coreVector2(5.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(Q):             return coreVector2(6.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(R):             return coreVector2(7.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(S):             return coreVector2(8.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(T):             return coreVector2(9.0f,2.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(U):             return coreVector2(0.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(V):             return coreVector2(1.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(W):             return coreVector2(2.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(X):             return coreVector2(3.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(Y):             return coreVector2(4.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(Z):             return coreVector2(5.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F1):            return coreVector2(6.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F2):            return coreVector2(7.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F3):            return coreVector2(8.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F4):            return coreVector2(9.0f,3.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F5):            return coreVector2(0.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F6):            return coreVector2(1.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F7):            return coreVector2(2.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F8):            return coreVector2(3.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F9):            return coreVector2(4.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F10):           return coreVector2(5.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F11):           return coreVector2(6.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(F12):           return coreVector2(7.0f,4.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(UP):            return coreVector2(0.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(LEFT):          return coreVector2(1.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(DOWN):          return coreVector2(2.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(RIGHT):         return coreVector2(3.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(PLUS):          return coreVector2(4.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(MINUS):         return coreVector2(5.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(ASTERISK):      return coreVector2(6.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(BACKSLASH):     return coreVector2(7.0f,5.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(LESS):          return coreVector2(0.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(GREATER):       return coreVector2(1.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(LEFTBRACKET):   return coreVector2(2.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(RIGHTBRACKET):  return coreVector2(3.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(QUESTION):      return coreVector2(4.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(DBLAPOSTROPHE): return coreVector2(5.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(SEMICOLON):     return coreVector2(6.0f,6.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(ESCAPE):        return coreVector2(0.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(LGUI):          return (DEFINED(_CORE_MACOS_) ? coreVector2(2.0f,7.0f) : coreVector2(1.0f,7.0f)) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(INSERT):        return coreVector2(3.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(DELETE):        return coreVector2(4.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(HOME):          return coreVector2(5.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(END):           return coreVector2(6.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(PAGEUP):        return coreVector2(7.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(PAGEDOWN):      return coreVector2(8.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(PRINTSCREEN):   return coreVector2(9.0f,7.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(TAB):           return coreVector2(0.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(LSHIFT):        return coreVector2(1.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(LCTRL):         return coreVector2(2.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(LALT):          return coreVector2(3.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(BACKSPACE):     return coreVector2(4.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(RETURN):        return coreVector2(5.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            case CORE_INPUT_CHAR(SPACE):         return coreVector2(6.0f,8.0f) * FIGURE_SCALE_KEYBOARD;
            default:                             return FIGURE_INVALID;
            }
        }
    }
    else
    {
        // 
        const SDL_GamepadButtonLabel eLabel = SDL_GetGamepadButtonLabelForType(SDL_GamepadType(iBase), SDL_GamepadButton(iKey));
        if((eLabel == SDL_GAMEPAD_BUTTON_LABEL_A) || (eLabel == SDL_GAMEPAD_BUTTON_LABEL_CROSS))    return coreVector2(0.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        if((eLabel == SDL_GAMEPAD_BUTTON_LABEL_B) || (eLabel == SDL_GAMEPAD_BUTTON_LABEL_CIRCLE))   return coreVector2(1.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        if((eLabel == SDL_GAMEPAD_BUTTON_LABEL_X) || (eLabel == SDL_GAMEPAD_BUTTON_LABEL_SQUARE))   return coreVector2(2.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        if((eLabel == SDL_GAMEPAD_BUTTON_LABEL_Y) || (eLabel == SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE)) return coreVector2(3.0f,0.0f) * FIGURE_SCALE_GAMEPAD;

        // 
        switch(iKey)
        {
        case SDL_GAMEPAD_BUTTON_SOUTH:          return coreVector2(0.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_EAST:           return coreVector2(1.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_WEST:           return coreVector2(2.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_NORTH:          return coreVector2(3.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_BACK:           return coreVector2(4.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_GUIDE:          return FIGURE_INVALID;
        case SDL_GAMEPAD_BUTTON_START:          return coreVector2(5.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_LEFT_STICK:     return coreVector2(2.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_RIGHT_STICK:    return coreVector2(3.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:  return coreVector2(0.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: return coreVector2(1.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_DPAD_UP:        return coreVector2(0.0f,1.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:      return coreVector2(2.0f,1.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:      return coreVector2(1.0f,1.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:     return coreVector2(3.0f,1.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_MISC1:          return coreVector2(4.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1:  return FIGURE_INVALID;
        case SDL_GAMEPAD_BUTTON_LEFT_PADDLE1:   return FIGURE_INVALID;
        case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2:  return FIGURE_INVALID;
        case SDL_GAMEPAD_BUTTON_LEFT_PADDLE2:   return FIGURE_INVALID;
        case SDL_GAMEPAD_BUTTON_TOUCHPAD:       return coreVector2(5.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case CORE_INPUT_BUTTON_LEFT_TRIGGER:    return coreVector2(2.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case CORE_INPUT_BUTTON_RIGHT_TRIGGER:   return coreVector2(3.0f,3.0f) * FIGURE_SCALE_GAMEPAD;
        case CORE_INPUT_BUTTON_A:               return coreVector2(0.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case CORE_INPUT_BUTTON_B:               return coreVector2(1.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case CORE_INPUT_BUTTON_X:               return coreVector2(2.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case CORE_INPUT_BUTTON_Y:               return coreVector2(3.0f,0.0f) * FIGURE_SCALE_GAMEPAD;
        case FIGURE_KEY_LEFT_STICK:             return coreVector2(0.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        case FIGURE_KEY_RIGHT_STICK:            return coreVector2(1.0f,2.0f) * FIGURE_SCALE_GAMEPAD;
        default:                                return FIGURE_INVALID;
        }
    }
}