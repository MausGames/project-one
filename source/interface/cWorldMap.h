///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WORLDMAP_H_
#define _P1_GUARD_WORLDMAP_H_

// TODO 1: design gamepad navigation (free movement?, fixed direction? (arbritrary, left-right), improve pin-order)
// TODO 1: 2 blinking arrow cursors, to highlight navigation (blinking, not moving, for contrast with rotating pin)
// TODO 1: add mission names directly on worldmap (permanent color and greyed if not focused, or color only on focus)
// TODO 1: jedes gebiet hat eigenes symbol, welches auch im intro erscheint, leaderboard, replay, fragment, etc. (auch erwähnt in design document)


// ****************************************************************
// world map definitions
#define WORLDMAP_PINS      (MISSIONS - 1u)              // 
#define WORLDMAP_LINES     (WORLDMAP_PINS - 1u)         // 
#define WORLDMAP_PIN_SIZE  (coreVector2(0.05f,0.05f))   // 
#define WORLDMAP_LINE_SIZE (0.005f)                     // 

static constexpr coreVector2 g_avWorldMapPosition[] =
{
    coreVector2(-0.36f,-0.36f),
    coreVector2(-0.09f,-0.27f),
    coreVector2( 0.18f,-0.33f),
    coreVector2( 0.33f,-0.03f),
    coreVector2( 0.12f, 0.12f),
    coreVector2( 0.30f, 0.27f),
    coreVector2( 0.03f, 0.36f),
    coreVector2(-0.15f, 0.18f),
    coreVector2(-0.33f, 0.30f),
    coreVector2(-0.33f, 0.21f),
    coreVector2(-0.33f, 0.12f)
};

STATIC_ASSERT(ARRAY_SIZE(g_avWorldMapPosition) == WORLDMAP_PINS)


// ****************************************************************
// world map class
class cWorldMap final : public cGuiObject
{
private:
    cGuiObject m_aPin   [WORLDMAP_PINS];    // 
    cGuiObject m_aPinDot[WORLDMAP_PINS];    // 
    cGuiObject m_aLine  [WORLDMAP_LINES];   // 
    cGuiObject m_Cursor;                    // 

    cTooltip m_Tooltip;                     // 

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
    inline cGuiObject* GetPin(const coreUintW iIndex) {ASSERT(iIndex < WORLDMAP_PINS) return &m_aPin[iIndex];}

    // 
    inline coreBool         GetSelectionState()const {return m_aPin[m_iNewPin].IsClicked();}
    inline const coreUint8& GetSelectionIndex()const {return m_iNewPin;}
};


#endif // _P1_GUARD_WORLDMAP_H_