///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cTracker::cTracker()noexcept
: m_fThrust    (0.0f)
, m_fAnimation (0.0f)
{
    // load object resources
    this->DefineModelHigh("ship_projectone_high.md3");
    this->DefineModelLow ("ship_projectone_low.md3");
    this->DefineTexture  (0u, "ship_player.png");
    this->DefineProgram  ("object_ship_program");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // set initial status
    m_iStatus = TRACKER_STATUS_DEAD;

    // 
    this->SetMaxHealth(1);
    this->SetBaseColor(COLOR_SHIP_GREY);

    // 
    m_Range.DefineModel  ("object_penta_top.md3");
    m_Range.DefineTexture(0u, "effect_energy.png");
    m_Range.DefineProgram("effect_energy_flat_invert_program");
    m_Range.SetAlpha     (0.0f);
    m_Range.SetTexSize   (coreVector2(0.1f,0.1f));
    m_Range.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Wind.DefineModel  ("object_sphere.md3");
    m_Wind.DefineTexture(0u, "effect_energy.png");
    m_Wind.DefineProgram("effect_energy_flat_direct_program");
    m_Wind.SetDirection (coreVector3(0.0f,-1.0f,0.0f));
    m_Wind.SetAlpha     (0.0f);
    m_Wind.SetTexSize   (coreVector2(1.0f,5.0f));
    m_Wind.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Exhaust.DefineModel  ("object_tube.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_direct_program");
    m_Exhaust.SetDirection (this->GetDirection());
    m_Exhaust.SetAlpha     (0.7f);
    m_Exhaust.SetTexSize   (coreVector2(0.5f,0.25f));
    m_Exhaust.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    
    
    m_Range  .SetColor3(COLOR_PLAYER_GREEN);
    m_Wind   .SetColor3(COLOR_PLAYER_GREEN * (1.6f/1.1f));
    m_Exhaust.SetColor3(COLOR_PLAYER_GREEN);
}


// ****************************************************************
// destructor
cTracker::~cTracker()
{
    // 
    this->Kill(false);
}


// ****************************************************************
// render the tracker
void cTracker::Render()
{
    if(!HAS_FLAG(m_iStatus, TRACKER_STATUS_DEAD))
    {
        // 
        cLodObject::RenderHighObject(this);
    }
}

void cTracker::RenderBefore()
{
    if(!HAS_FLAG(m_iStatus, TRACKER_STATUS_DEAD))
    {
        // 
        m_Wind   .Render();
        m_Exhaust.Render();
    }
}

void cTracker::RenderMiddle()
{
    if(!HAS_FLAG(m_iStatus, TRACKER_STATUS_DEAD))
    {

    }
}

void cTracker::RenderAfter()
{
    if(!HAS_FLAG(m_iStatus, TRACKER_STATUS_DEAD))
    {
        // 
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Range);

        // 
        m_Range.Render();
    }
}


// ****************************************************************
// move the tracker
void cTracker::Move()
{
    // 
    this->_UpdateAlwaysBefore();

    if(!HAS_FLAG(m_iStatus, TRACKER_STATUS_DEAD))
    {
        // 
        m_fAnimation.UpdateMod(1.0f, 20.0f);

        // move the 3d-object
        this->coreObject3D::Move();

        // 
             if( m_fThrust && !m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->EnableExhaust();
        else if(!m_fThrust &&  m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableExhaust();

        if(m_Range.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            const coreVector2 vDir = coreVector2::Direction(m_fAnimation * (-1.6f*PI));

            // 
            m_Range.SetPosition (this->GetPosition());
            m_Range.SetSize     (coreVector3(1.0f,1.0f,1.0f) * PLAYER_RANGE_SIZE * this->GetSize().x);
            m_Range.SetDirection(coreVector3(vDir, 0.0f));
            m_Range.SetAlpha    (this->GetAlpha());
            m_Range.SetTexOffset(m_Range.GetTexOffset() - m_Range.GetDirection().xy() * (0.1f * TIME));
            m_Range.Move();
        }

        if(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_Wind.SetPosition (this->GetPosition());
            m_Wind.SetSize     (coreVector3(1.0f,1.08f,1.0f) * PLAYER_WIND_SIZE * this->GetSize().x);
            m_Wind.SetAlpha    (this->GetAlpha());
            m_Wind.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.3f));
            m_Wind.Move();
        }

        if(m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            const coreFloat   fLen   = m_fThrust * 40.0f;
            const coreFloat   fWidth = 1.0f - m_fThrust * 0.25f;
            const coreVector3 vSize  = coreVector3(fWidth, fLen, fWidth) * (0.6f * this->GetSize().x);

            // 
            m_Exhaust.SetPosition (this->GetPosition() - this->GetDirection() * (vSize.y + 4.0f * this->GetSize().x));
            m_Exhaust.SetSize     (vSize);
            m_Exhaust.SetDirection(this->GetDirection());
            m_Exhaust.SetAlpha    (this->GetAlpha());
            m_Exhaust.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.75f));
            m_Exhaust.Move();
        }
    }

    // 
    this->_UpdateAlwaysAfter();
}


// ****************************************************************
// add tracker to the game
void cTracker::Resurrect()
{
    // resurrect tracker
    if(!HAS_FLAG(m_iStatus, TRACKER_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, TRACKER_STATUS_DEAD)

    // add ship to global shadow and outline
    cShadow::GetGlobalContainer()->BindObject(this);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindObject(this);

    // add ship to the game
    this->_Resurrect();
}


// ****************************************************************
// remove tracker from the game
void cTracker::Kill(const coreBool bAnimated)
{
    // kill tracker
    if(HAS_FLAG(m_iStatus, TRACKER_STATUS_DEAD)) return;
    ADD_FLAG(m_iStatus, TRACKER_STATUS_DEAD)

    // 
    this->DisableRange();
    this->DisableWind();
    this->DisableExhaust();

    // 
    m_fThrust = 0.0f;

    // 
    if(bAnimated && this->IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), this->GetColor3());
    }

    // remove ship from global shadow and outline
    cShadow::GetGlobalContainer()->UnbindObject(this);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->UnbindObject(this);

    // remove ship from the game
    this->_Kill(bAnimated);
}


// ****************************************************************
// 
void cTracker::EnableRange()
{
    WARN_IF(m_Range.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableRange();

    // 
    m_Range.SetAlpha(0.0f);

    // 
    m_Range.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Range);
}


// ****************************************************************
// 
void cTracker::DisableRange()
{
    if(!m_Range.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Range.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Range);
}


// ****************************************************************
// 
void cTracker::EnableWind()
{
    WARN_IF(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableWind();

    // 
    m_Wind.SetAlpha(0.0f);

    // 
    m_Wind.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Wind);
}


// ****************************************************************
// 
void cTracker::DisableWind()
{
    if(!m_Wind.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Wind.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Wind);
}


// ****************************************************************
// 
void cTracker::EnableExhaust()
{
    WARN_IF(m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableExhaust();

    // 
    m_Exhaust.SetAlpha(0.0f);

    // 
    m_Exhaust.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cTracker::DisableExhaust()
{
    if(!m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Exhaust.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Exhaust);
}