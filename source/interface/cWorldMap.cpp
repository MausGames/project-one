///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cWorldMap::cWorldMap()noexcept
: m_iOldPin      (0u)
, m_iNewPin      (0u)
, m_fMove        (0.0f)
, m_fPinAngle    (0.0f)
, m_fCursorAngle (0.0f)
{
    // 
    // TODO: change   
    this->DefineTexture(0u, Core::Manager::Resource->Load<coreTexture>("menu_background_white.png", CORE_RESOURCE_UPDATE_AUTO, "data/textures/menu_background_white.png"));
    this->DefineProgram("default_2d_program");
    this->SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.3f);
    this->SetTexSize   (coreVector2(0.767f,0.517f)  * 3.0f);

    // 
    for(coreUintW i = 0u; i < WORLDMAP_PINS; ++i)
    {
        m_aPin[i].DefineTexture   (0u, "menu_mission.png");
        m_aPin[i].DefineProgram   ("default_2d_program");
        m_aPin[i].SetSize         (WORLDMAP_PIN_SIZE);
        m_aPin[i].SetTexSize      (coreVector2(0.5f,0.5f) / 1.55f);
        m_aPin[i].SetTexOffset    (coreVector2(0.5f,0.0f) + 0.5f * (coreVector2(0.5f,0.5f) - m_aPin[i].GetTexSize()));
        m_aPin[i].SetFocusModifier(coreVector2(1.5f,1.5f));

        m_aPinDot[i].DefineTexture(0u, "menu_mission.png");
        m_aPinDot[i].DefineProgram("default_2d_program");
        m_aPinDot[i].SetSize      (WORLDMAP_PIN_SIZE * 1.25f);
        m_aPinDot[i].SetTexSize   (coreVector2(0.25f, 0.25f));
        m_aPinDot[i].SetTexOffset (coreVector2(0.125f,0.625f));
    }

    // 
    for(coreUintW i = 0u; i < WORLDMAP_LINES; ++i)
    {
        m_aLine[i].DefineTexture(0u, "default_white.png");
        m_aLine[i].DefineProgram("default_2d_program");
    }

    // 
    m_Cursor.DefineTexture(0u, "menu_mission.png");
    m_Cursor.DefineProgram("default_2d_program");
    m_Cursor.SetSize      (WORLDMAP_PIN_SIZE * 1.55f);
    m_Cursor.SetTexSize   (coreVector2(0.5f,0.5f));
}


// ****************************************************************
// render the world map
void cWorldMap::Render()
{
    // 
    this->coreObject2D::Render();

    // 
    for(coreUintW i = 0u; i < WORLDMAP_LINES; ++i) m_aLine  [i].Render();
    for(coreUintW i = 0u; i < WORLDMAP_PINS;  ++i) m_aPinDot[i].Render();
    for(coreUintW i = 0u; i < WORLDMAP_PINS;  ++i) m_aPin   [i].Render();

    // 
    m_Cursor.Render();
}


// ****************************************************************
// move the world map
void cWorldMap::Move()
{
    // 
    this->coreObject2D::Move();

    // 
    for(coreUintW i = 0u; i < WORLDMAP_PINS; ++i)
    {
        m_aPin[i].Interact();
        if(m_aPin[i].IsFocused() && (m_iNewPin != i))
        {
            m_iOldPin   = m_iNewPin;
            m_iNewPin   = i;
            m_fMove     = 0.0f;
            m_fPinAngle = m_aPin[i].GetDirection().Angle();

            break;
        }
    }

    // 
    if(m_fMove < 1.0f)
    {
        m_fMove.UpdateMin(6.0f, 1.0f);
        m_Cursor.SetPosition(LERPB(m_aPin[m_iOldPin].GetPosition(), m_aPin[m_iNewPin].GetPosition(), m_fMove));
    }

    // 
    m_fPinAngle.UpdateMod(-1.0f, 2.0f*PI);
    m_aPin[m_iNewPin].SetDirection(coreVector2::Direction(m_fPinAngle));

    // 
    m_fCursorAngle.UpdateMod(2.0f, 2.0f*PI);
    m_Cursor.SetDirection(coreVector2::Direction(m_fCursorAngle));

    // 
    for(coreUintW i = 0u; i < WORLDMAP_PINS; ++i)
    {
        m_aPin[i].SetAlpha(this->GetAlpha() * 0.8f);
        m_aPin[i].Move();

        m_aPinDot[i].SetAlpha(this->GetAlpha() * 1.2f);
        m_aPinDot[i].Move();
    }

    // 
    for(coreUintW i = 0u; i < WORLDMAP_LINES; ++i)
    {
        m_aLine[i].SetAlpha(this->GetAlpha() * 0.8f);
        m_aLine[i].Move();
    }

    // 
    m_Cursor.SetAlpha(this->GetAlpha() * 0.8f);
    m_Cursor.Move();
}


// ****************************************************************
// 
void cWorldMap::Arrange()
{
    // 
    m_aPin[0].SetPosition(coreVector2(-0.12f,-0.12f) * 3.0f);
    m_aPin[1].SetPosition(coreVector2(-0.03f,-0.09f) * 3.0f);
    m_aPin[2].SetPosition(coreVector2( 0.06f,-0.11f) * 3.0f);
    m_aPin[3].SetPosition(coreVector2( 0.11f,-0.01f) * 3.0f);
    m_aPin[4].SetPosition(coreVector2( 0.04f, 0.04f) * 3.0f);
    m_aPin[5].SetPosition(coreVector2( 0.10f, 0.09f) * 3.0f);
    m_aPin[6].SetPosition(coreVector2( 0.01f, 0.12f) * 3.0f);
    m_aPin[7].SetPosition(coreVector2(-0.05f, 0.06f) * 3.0f);
    m_aPin[8].SetPosition(coreVector2(-0.11f, 0.10f) * 3.0f);
    STATIC_ASSERT(WORLDMAP_PINS == 9u)

    // 
    for(coreUintW i = 0u; i < WORLDMAP_PINS; ++i)
    {
        const coreVector2 vRealPos = m_aPin[i].GetPosition() * this->GetSize() + this->GetPosition();

        m_aPin   [i].SetPosition(vRealPos);
        m_aPinDot[i].SetPosition(vRealPos);
    }

    // 
    coreVector2 vOldPos = m_aPin[0].GetPosition();
    for(coreUintW i = 0u; i < WORLDMAP_LINES; ++i)
    {
        const coreVector2 vNewPos = m_aPin[i + 1u].GetPosition();
        const coreVector2 vDiff   = vNewPos - vOldPos;
        ASSERT(!vDiff.IsNull())

        m_aLine[i].SetPosition (vOldPos + vDiff * 0.5f);
        m_aLine[i].SetSize     (coreVector2(WORLDMAP_LINE_SIZE, vDiff.Length()));
        m_aLine[i].SetDirection(vDiff.Normalized().InvertedX());

        vOldPos = vNewPos;
    }
    STATIC_ASSERT(WORLDMAP_PINS > WORLDMAP_LINES)
}


// ****************************************************************
// 
void cWorldMap::EnablePin(const coreUintW iIndex, const coreBool bEnable)
{
    ASSERT(iIndex < WORLDMAP_PINS)

    const coreObjectEnable eEnabled = bEnable ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

    // 
    m_aPin   [iIndex].SetEnabled(eEnabled);
    m_aPinDot[iIndex].SetEnabled(eEnabled);

    // 
    if(iIndex > 0u)             m_aLine[iIndex - 1u].SetEnabled((m_aPin[iIndex - 1u].IsEnabled(CORE_OBJECT_ENABLE_ALL) && bEnable) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    if(iIndex < WORLDMAP_LINES) m_aLine[iIndex]     .SetEnabled((m_aPin[iIndex + 1u].IsEnabled(CORE_OBJECT_ENABLE_ALL) && bEnable) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    STATIC_ASSERT(WORLDMAP_PINS > WORLDMAP_LINES)
}