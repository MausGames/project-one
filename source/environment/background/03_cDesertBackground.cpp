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
cDesertBackground::cDesertBackground()noexcept
: m_vSandMove (coreVector2(0.0f,-1.0f))
, m_fSandWave (0.0f)
{
    coreBatchList* pList1;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("dust", "crack", OUTDOOR_ALGORITHM_DESERT, 4.0f, false);

    // allocate stone list
    pList1 = new coreBatchList(DESERT_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_stone_01.md3");
        oBase.DefineTexture(0u, "environment_stone_diff.png");
        oBase.DefineTexture(1u, "environment_stone_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < DESERT_STONE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, DESERT_STONE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -23.0f) && (fHeight < -18.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(5.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(2.0f, 2.6f));
                    pObject->SetDirection  (coreVector3::Rand());
                    pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.2f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, DESERT_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    m_Sand.DefineTexture(0u, "effect_sand.png");
    m_Sand.DefineProgram("effect_weather_sand_program");
    m_Sand.SetPosition  (coreVector2(0.0f,0.0f));
    m_Sand.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Sand.SetColor3    (coreVector3(200.0f/255.0f, 186.0f/255.0f, 156.0f/255.0f) * 1.02f);
}


// ****************************************************************
// destructor
cDesertBackground::~cDesertBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cDesertBackground::__InitOwn()
{
    // load wind sound-effect
    m_pWindSound = Core::Manager::Resource->Get<coreSound>("environment_wind.wav");
    m_pWindSound.OnUsableOnce([this, pResource = m_pWindSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cDesertBackground::__ExitOwn()
{
    // stop wind sound-effect
    m_pWindSound.OnUsableOnce([this, pResource = m_pWindSound]()
    {
        if(pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// 
void cDesertBackground::__RenderOwnAfter()
{
    // enable the shader-program
    if(!m_Sand.GetProgram().IsUsable()) return;
    if(!m_Sand.GetProgram()->Enable())  return;

    // 
    coreProgram* pLocal = m_Sand.GetProgram().GetResource();
    for(coreUintW i = 0u; i < DESERT_SAND_NUM; ++i)
    {
        const coreVector2 vNewTexOffset = m_Sand.GetTexOffset() + coreVector2(0.3f,0.3f) * I_TO_F(POW2(i)) + coreVector2(0.26f * SIN(m_fSandWave * (0.125f*PI) + I_TO_F(POW2(i))), 0.0f);
        const coreFloat   fNewScale     = 1.6f - 0.12f * I_TO_F(i);

        pLocal->SendUniform(s_asOverlayTransform[i], coreVector3(vNewTexOffset.Processed(FRACT), fNewScale));
    }

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Sand.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the desert background
void cDesertBackground::__MoveOwn()
{
    // 
    const coreVector2 vEnvMove   = coreVector2(0.0f,1.0f) * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexSize   = coreVector2(1.0f,1.0f) * 2.3f;
    const coreVector2 vTexOffset = m_Sand.GetTexOffset() + (m_vSandMove.InvertedX() + vEnvMove) * (0.4f * TIME);

    // 
    m_Sand.SetDirection(g_pEnvironment->GetDirection().InvertedX());
    m_Sand.SetTexSize  (vTexSize);
    m_Sand.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Sand.Move();

    // 
    m_fSandWave.UpdateMod(SQRT(MAX(ABS(g_pEnvironment->GetSpeed()), 1.0f)), 16.0f);

    // 
    if(m_pWindSound->EnableRef(this))
    {
        m_pWindSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
        m_pWindSound->SetPitch (1.0f + 0.1f * (m_vSandMove.Length() - 1.0f));
    }
}