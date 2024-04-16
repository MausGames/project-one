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

// TODO: 2 blinking arrow cursors (blinking, not moving, for contrast with rotating pin)


// ****************************************************************
// world map definitions
#define WORLDMAP_PINS      (9u)                         // 
#define WORLDMAP_LINES     (WORLDMAP_PINS - 1u)         // 
#define WORLDMAP_PIN_SIZE  (coreVector2(0.05f,0.05f))   // 
#define WORLDMAP_LINE_SIZE (0.005f)                     // 


// ****************************************************************
// world map class
class cWorldMap final : public coreObject2D
{
private:
    coreObject2D m_aPin   [WORLDMAP_PINS];    // 
    coreObject2D m_aPinDot[WORLDMAP_PINS];    // 
    coreObject2D m_aLine  [WORLDMAP_LINES];   // 
    coreObject2D m_Cursor;                    // 

    cTooltip m_Tooltip;                       // 

    coreFlow  m_fMove;                        // 
    coreUint8 m_iOldPin;                      // 
    coreUint8 m_iNewPin;                      // 

    coreUint16 m_iEnabled;                    // 

    coreFlow m_fPinAngle;                     // 
    coreFlow m_fCursorAngle;                  // 


public:
    cWorldMap()noexcept;

    DISABLE_COPY(cWorldMap)

    // render and move the world map
    void Render()final;
    void Move  ()final;

    // 
    void Arrange();

    // 
    void EnablePin(const coreUintW iIndex, const coreBool bShow, const coreBool bTease);
    void SelectPin(const coreUintW iIndex);

    // 
    inline coreBool         GetSelectionState()const {return m_aPin[m_iNewPin].IsClicked();}
    inline const coreUint8& GetSelectionIndex()const {return m_iNewPin;}
};


#endif // _P1_GUARD_WORLDMAP_H_