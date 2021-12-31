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
: m_iElement (ELEMENT_NEUTRAL)
{
    // load object resources
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_glow_program");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f) * 1.7f);
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // set initial status
    m_iStatus = HELPER_STATUS_DEAD;

    // 
    this->SetMaxHealth(1);
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
    coreVector3    vColor;
    switch(iElement)
    {
    default: ASSERT(false)
    case ELEMENT_WHITE:   sModel = "ship_helper_01.md3"; vColor = COLOR_ENERGY_WHITE   * 0.6f; break;
    case ELEMENT_YELLOW:  sModel = "ship_helper_03.md3"; vColor = COLOR_ENERGY_YELLOW  * 0.7f; break;
    case ELEMENT_ORANGE:  sModel = "ship_helper_06.md3"; vColor = COLOR_ENERGY_ORANGE  * 0.9f; break;
    case ELEMENT_RED:     sModel = "ship_helper_02.md3"; vColor = COLOR_ENERGY_RED     * 0.8f; break;
    case ELEMENT_MAGENTA: sModel = "ship_helper_07.md3"; vColor = COLOR_ENERGY_MAGENTA * 0.9f; break;
    case ELEMENT_PURPLE:  sModel = "ship_helper_05.md3"; vColor = COLOR_ENERGY_PURPLE  * 0.9f; break;
    case ELEMENT_BLUE:    sModel = "ship_helper_09.md3"; vColor = COLOR_ENERGY_BLUE    * 0.9f; break;
    case ELEMENT_CYAN:    sModel = "ship_helper_08.md3"; vColor = COLOR_ENERGY_CYAN    * 0.9f; break;
    case ELEMENT_GREEN:   sModel = "ship_helper_04.md3"; vColor = COLOR_ENERGY_GREEN   * 0.8f; break;
    }

    // load models
    this->DefineModelHigh(sModel);
    this->DefineModelLow (sModel);

    // set color
    this->SetBaseColor(vColor);
}


// ****************************************************************
// render the helper
void cHelper::Render()
{
    if(!HAS_FLAG(m_iStatus, HELPER_STATUS_DEAD))
    {
        // 
        cLodObject::RenderHighObject(this);
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
        this->SetDirection(coreVector3(s_vDirection, 0.0f));

        // move the 3d-object
        this->coreObject3D::Move();
    }

    // 
    this->_UpdateAlwaysAfter();
}


// ****************************************************************
// add helper to the game
void cHelper::Resurrect()
{
    // resurrect helper
    if(!HAS_FLAG(m_iStatus, HELPER_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, HELPER_STATUS_DEAD)

    // add ship to global shadow and outline
    cShadow::GetGlobalContainer()->BindObject(this);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindObject(this);

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

    // 
    if(bAnimated && this->IsEnabled(CORE_OBJECT_ENABLE_RENDER))
        g_pSpecialEffects->MacroExplosionPhysicalColorBig(this->GetPosition(), this->GetColor3());

    // remove ship from global shadow and outline
    cShadow::GetGlobalContainer()->UnbindObject(this);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->UnbindObject(this);

    // remove ship from the game
    this->_Kill(bAnimated);
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