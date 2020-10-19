///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WORLDMAP_H_
#define _P1_GUARD_WORLDMAP_H_

// TODO: design gamepad navigation (free movement?, fixed direction? (arbritrary, left-right), improve pin-order)
// TODO: 2 blinking arrow cursors, to highlight navigation (blinking, not moving, for contrast with rotating pin)


// ****************************************************************
// world map definitions
#define WORLDMAP_PINS      (9u)                         // 
#define WORLDMAP_LINES     (WORLDMAP_PINS - 1u)         // 
#define WORLDMAP_PIN_SIZE  (coreVector2(0.05f,0.05f))   // 
#define WORLDMAP_LINE_SIZE (0.005f)                     // 


// ****************************************************************
// world map class
class cWorldMap final : public cGuiObject
{
private:
    // 
    struct sData final
    {
        coreInt32       iID;         // 
        const coreChar* pcName;      // 
        coreVector3     vColor;      // 
        coreVector2     vPosition;   // 
    };


private:
    cGuiObject m_aPin   [WORLDMAP_PINS];    // 
    cGuiObject m_aPinDot[WORLDMAP_PINS];    // 
    cGuiObject m_aLine  [WORLDMAP_LINES];   // 
    cGuiObject m_Cursor;                    // 

    cTooltip m_Tooltip;                     // 

    sData m_aData[WORLDMAP_PINS];           // 

    coreFlow  m_fMove;                      // 
    coreUint8 m_iOldPin;                    // 
    coreUint8 m_iNewPin;                    // 

    coreFlow m_fPinAngle;                   // 
    coreFlow m_fCursorAngle;                // 

    coreUint16 m_iEnabled;                  // 


public:
    cWorldMap()noexcept;

    DISABLE_COPY(cWorldMap)

    // render and move the world map
    void Render()final;
    void Move  ()final;

    // 
    void Arrange();

    // 
    void EnablePin(const coreUintW iIndex, const coreBool bEnable, const coreBool bShow);
    void SelectPin(const coreUintW iIndex);

    // 
    inline coreBool         GetSelectionState()const {return m_aPin[m_iNewPin].IsClicked();}
    inline const coreUint8& GetSelectionIndex()const {return m_iNewPin;}
    inline const coreInt32& GetSelectionID   ()const {return m_aData[m_iNewPin].iID;}
};


#endif // _P1_GUARD_WORLDMAP_H_