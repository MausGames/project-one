///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreFlow    cHelper::s_fAnimation = 0.0f;
coreVector2 cHelper::s_vDirection = coreVector2(0.0f,1.0f);


// ****************************************************************
// constructor
cHelper::cHelper()noexcept
: m_fLifeTime       (0.0f)
, m_fLifeTimeBefore (0.0f)
, m_bSmooth         (false)
, m_fAngleOverride  (FLT_MAX)
, m_iElement        (ELEMENT_NEUTRAL)
, m_pShield         (NULL)
{
    // load object resources
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_glow_program");

    // set object properties
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // set initial status
    m_iStatus = HELPER_STATUS_DEAD;

    // 
    this->SetMaxHealth(1);

    // 
    m_Wind.DefineModel  ("object_sphere.md3");
    m_Wind.DefineTexture(0u, "effect_energy.png");
    m_Wind.DefineProgram("effect_energy_flat_direct_program");
    m_Wind.SetTexSize   (coreVector2(1.0f,5.0f) * 0.7f);
}


// ****************************************************************
// destructor
cHelper::~cHelper()
{
    // 
    this->Kill(false);
}


// ****************************************************************
// configure the helper
void cHelper::Configure(const coreUint8 iElement)
{
    // 
    m_iElement = iElement;

    // select appearance type
    coreHashString sModel;
    coreVector3    vBase;
    coreVector3    vEnergy;
    switch(iElement)
    {
    default: ASSERT(false)
    case ELEMENT_WHITE:   sModel = "ship_helper_01.md3"; vBase = COLOR_ENERGY_WHITE   * 0.6f; vEnergy = COLOR_ENERGY_WHITE;   break;
    case ELEMENT_YELLOW:  sModel = "ship_helper_03.md3"; vBase = COLOR_ENERGY_YELLOW  * 0.7f; vEnergy = COLOR_ENERGY_YELLOW;  break;
    case ELEMENT_ORANGE:  sModel = "ship_helper_06.md3"; vBase = COLOR_ENERGY_ORANGE  * 0.9f; vEnergy = COLOR_ENERGY_ORANGE;  break;
    case ELEMENT_RED:     sModel = "ship_helper_02.md3"; vBase = COLOR_ENERGY_RED     * 0.8f; vEnergy = COLOR_ENERGY_RED;     break;
    case ELEMENT_MAGENTA: sModel = "ship_helper_07.md3"; vBase = COLOR_ENERGY_MAGENTA * 0.9f; vEnergy = COLOR_ENERGY_MAGENTA; break;
    case ELEMENT_PURPLE:  sModel = "ship_helper_05.md3"; vBase = COLOR_ENERGY_PURPLE  * 0.9f; vEnergy = COLOR_ENERGY_PURPLE;  break;
    case ELEMENT_BLUE:    sModel = "ship_helper_09.md3"; vBase = COLOR_ENERGY_BLUE    * 0.9f; vEnergy = COLOR_ENERGY_BLUE;    break;
    case ELEMENT_CYAN:    sModel = "ship_helper_08.md3"; vBase = COLOR_ENERGY_CYAN    * 0.9f; vEnergy = COLOR_ENERGY_CYAN;    break;
    case ELEMENT_GREEN:   sModel = "ship_helper_04.md3"; vBase = COLOR_ENERGY_GREEN   * 0.8f; vEnergy = COLOR_ENERGY_GREEN;   break;
    }

    // load models
    this->DefineModelHigh(sModel);
    this->DefineModelLow (sModel);

    // set color
    this->SetBaseColor(vBase);

    // 
    m_Wind.SetColor3(vEnergy * (1.6f/1.1f));
}


// ****************************************************************
// render the helper
void cHelper::Render()
{
    if(!HAS_FLAG(m_iStatus, HELPER_STATUS_DEAD) && !HAS_FLAG(m_iStatus, HELPER_STATUS_BOTTOM) && !HAS_FLAG(m_iStatus, HELPER_STATUS_TOP))
    {
        // 
        cLodObject::RenderHighObject(this);
    }
}

void cHelper::RenderBefore()
{
    if(!HAS_FLAG(m_iStatus, HELPER_STATUS_DEAD))
    {
        if(m_pShield)
        {
            // 
            m_pShield->Render();
            g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(m_pShield);
        }

        glDepthMask(false);
        {
            // 
            m_Wind.Render();
        }
        glDepthMask(true);

        if(HAS_FLAG(m_iStatus, HELPER_STATUS_BOTTOM))
        {
            // 
            cLodObject::RenderHighObject(this);
            g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(this);
        }
    }
}

void cHelper::RenderAfter()
{
    if(!HAS_FLAG(m_iStatus, HELPER_STATUS_DEAD))
    {
        if(HAS_FLAG(m_iStatus, HELPER_STATUS_TOP))
        {
            // 
            g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(this);   // # depth-test disabled
            cLodObject::RenderHighObject(this);
        }
    }
}


// ****************************************************************
// move the helper
void cHelper::Move()
{
    // 
    this->_UpdateAlwaysBefore();

    if(!HAS_FLAG(m_iStatus, HELPER_STATUS_DEAD))
    {
        // 
        m_fLifeTimeBefore = m_fLifeTime;
        m_fLifeTime.Update(1.0f);

        // 
        const coreFloat   fFade   = m_bSmooth ? BLENDH3(MIN1(m_fLifeTime * 2.0f)) : 1.0f;
        const coreFloat   fOffset = I_TO_F(m_iElement) * (1.0f/8.0f);
        const coreVector2 vDir    = (m_fAngleOverride != FLT_MAX) ? coreVector2::Direction(m_fAngleOverride) : s_vDirection;

        // 
        this->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.75f * fFade);
        this->SetDirection(coreVector3(vDir, 0.0f));

        // move the 3d-object
        this->coreObject3D::Move();

        // 
        this->SetEnabled(HAS_FLAG(m_iStatus, HELPER_STATUS_HIDDEN) ? CORE_OBJECT_ENABLE_MOVE : CORE_OBJECT_ENABLE_ALL);

        // 
        m_Wind.SetPosition (this->GetPosition());
        m_Wind.SetSize     (coreVector3(1.0f,1.08f,1.0f) * 3.3f * fFade);
        m_Wind.SetAlpha    (this->GetAlpha() * fFade);
        m_Wind.SetTexOffset(coreVector2(0.0f, m_fLifeTime * -0.2f + fOffset));
        m_Wind.SetEnabled  (this->GetEnabled());
        m_Wind.Move();

        if(m_pShield)
        {
            // 
            m_pShield->SetPosition (this->GetPosition());
            m_pShield->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 5.0f * fFade);
            m_pShield->SetAlpha    (this->GetAlpha() * fFade);
            m_pShield->SetTexOffset(coreVector2(0.0f, m_fLifeTime * 0.1f + fOffset));
            m_pShield->SetEnabled  (this->GetEnabled());
            m_pShield->Move();
        }
    }

    // 
    this->_UpdateAlwaysAfter();
}


// ****************************************************************
// add helper to the game
void cHelper::Resurrect(const coreBool bSmooth)
{
    // resurrect helper
    if(!HAS_FLAG(m_iStatus, HELPER_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, HELPER_STATUS_DEAD)

    const coreBool bBottom = HAS_FLAG(m_iStatus, HELPER_STATUS_BOTTOM);
    const coreBool bTop    = HAS_FLAG(m_iStatus, HELPER_STATUS_TOP);

    // 
    m_fLifeTime       = 0.0f;
    m_fLifeTimeBefore = 0.0f;
    m_bSmooth         = bSmooth;
    m_fAngleOverride  = FLT_MAX;

    // 
    if(bSmooth)
    {
        this ->SetSize(coreVector3(0.0f,0.0f,0.0f));
        m_Wind.SetSize(coreVector3(0.0f,0.0f,0.0f));
    }

    // 
    g_pGlow->BindObject(&m_Wind);

    // add ship to global shadow and outline
    cShadow::GetGlobalContainer()->BindObject(this);
    if(!bBottom && !bTop) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindObject(this);

    // add ship to the game
    this->_Resurrect();
}


// ****************************************************************
// remove helper from the game
void cHelper::Kill(const coreBool bAnimated)
{
    // kill helper
    if(HAS_FLAG(m_iStatus, HELPER_STATUS_DEAD)) return;
    ADD_FLAG(m_iStatus, HELPER_STATUS_DEAD)

    const coreBool bBottom = HAS_FLAG(m_iStatus, HELPER_STATUS_BOTTOM);
    const coreBool bTop    = HAS_FLAG(m_iStatus, HELPER_STATUS_TOP);

    // 
    this->DisableShield(bAnimated);
    
    REMOVE_FLAG(m_iStatus, HELPER_STATUS_HIDDEN)
    this->SetAlpha(1.0f);
    
    // 
    g_pGlow->UnbindObject(&m_Wind);

    // 
    if(bAnimated && this->IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        g_pSpecialEffects->MacroExplosionColorSmall(this->GetPosition(), this->GetColor3());
        g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_02);
    }

    // remove ship from global shadow and outline
    cShadow::GetGlobalContainer()->UnbindObject(this);
    if(!bBottom && !bTop) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->UnbindObject(this);

    // remove ship from the game
    this->_Kill(bAnimated);
}


// ****************************************************************
// 
void cHelper::EnableShield()
{
    WARN_IF(m_pShield) return;

    // 
    coreVector3 vColor;
    switch(m_iElement)
    {
    default: ASSERT(false)
    case ELEMENT_WHITE:   vColor = COLOR_ENERGY_WHITE;   break;
    case ELEMENT_YELLOW:  vColor = COLOR_ENERGY_YELLOW;  break;
    case ELEMENT_ORANGE:  vColor = COLOR_ENERGY_ORANGE;  break;
    case ELEMENT_RED:     vColor = COLOR_ENERGY_RED;     break;
    case ELEMENT_MAGENTA: vColor = COLOR_ENERGY_MAGENTA; break;
    case ELEMENT_PURPLE:  vColor = COLOR_ENERGY_PURPLE;  break;
    case ELEMENT_BLUE:    vColor = COLOR_ENERGY_BLUE;    break;
    case ELEMENT_CYAN:    vColor = COLOR_ENERGY_CYAN;    break;
    case ELEMENT_GREEN:   vColor = COLOR_ENERGY_GREEN;   break;
    }

    // 
    m_pShield = new coreObject3D();

    // 
    m_pShield->DefineModel  ("object_sphere.md3");
    m_pShield->DefineTexture(0u, "effect_energy.png");
    m_pShield->DefineProgram("effect_energy_flat_spheric_program");
    m_pShield->SetSize      (coreVector3(0.0f,0.0f,0.0f));
    m_pShield->SetColor3    (vColor);
    m_pShield->SetTexSize   (coreVector2(1.0f,1.0f) * 4.0f);

    // 
    g_pGlow->BindObject(m_pShield);
}


// ****************************************************************
// 
void cHelper::DisableShield(const coreBool bAnimated)
{
    if(!m_pShield) return;

    // 
    g_pGlow->UnbindObject(m_pShield);

    // 
    SAFE_DELETE(m_pShield)
}


// ****************************************************************
// 
void cHelper::GlobalUpdate()
{
    // 
    s_fAnimation.UpdateMod(1.0f, 10.0f);
    s_vDirection = coreVector2::Direction(s_fAnimation * (0.6f*PI));
}


// ****************************************************************
// 
void cHelper::GlobalReset()
{
    // 
    s_fAnimation = 0.0f;
    s_vDirection = coreVector2(0.0f,1.0f);
}