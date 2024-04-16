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
cDarkBackground::cDarkBackground()noexcept
: m_Block      (DARK_BLOCKS)
, m_fFlyOffset (0.0f)
, m_fDissolve  (-1.0f)
, m_afFade     {}
{
    // 
    this->__InitOwn();

    // 
    m_pOutdoor = new cOutdoor();
    m_pOutdoor->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Block.DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("object_cube_ice.md3");
        oBase.DefineTexture(0u, "environment_block_diff.png");
        oBase.DefineTexture(1u, "environment_block_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
        {
            // 
            coreObject3D* pBlock = &m_aBlockRaw[i];
            (*pBlock) = oBase;

            // set object properties
            pBlock->SetPosition(coreVector3(0.0f, 0.0f, DARK_HEIGHT + Core::Rand->Float(-0.5f, 0.5f) * DARK_DETAIL));
            pBlock->SetSize    (coreVector3(0.5f,0.5f,0.5f) * DARK_DETAIL);
            pBlock->SetColor4  (coreVector4(1.0f,1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

            // add object to the list
            m_Block.BindObject(pBlock);
        }
    }

    // 
    m_pOutdoor->GetShadowMap()->BindList(&m_Block);
}


// ****************************************************************
// destructor
cDarkBackground::~cDarkBackground()
{
    // 
    m_pOutdoor->GetShadowMap()->UnbindList(&m_Block);

    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cDarkBackground::Dissolve()
{
    ASSERT(coreMath::IsNear(g_pEnvironment->GetSpeed(), 0.0f))
    m_fDissolve = 2.0f;
}


// ****************************************************************
// 
void cDarkBackground::__InitOwn()
{
    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_dark.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cDarkBackground::__ExitOwn()
{
    // stop base sound-effect
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        if(pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// 
void cDarkBackground::__RenderOwnBefore()
{
    if(this->IsDissolved()) return;

    glDisable(GL_BLEND);
    {
        // 
        m_Block.Render();
    }
    glEnable(GL_BLEND);
}


// ****************************************************************
// 
void cDarkBackground::__MoveOwn()
{
    if(this->IsDissolved()) return;

    // 
    m_fFlyOffset.Update(g_pEnvironment->GetSpeed() * DARK_SPEED);

    // 
    const coreVector2 vCamPos = g_pEnvironment->GetCameraPos().xy();
    const coreFloat   fRange  = I_TO_F(DARK_BLOCKS_Y) * DARK_DETAIL * 0.5f;

    // 
    for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
    {
        // 
        const coreFloat   fX   = (I_TO_F(i % DARK_BLOCKS_X) - I_TO_F(DARK_BLOCKS_X - 1u) * 0.5f) * DARK_DETAIL;
        const coreFloat   fY   = (I_TO_F(i / DARK_BLOCKS_X) - I_TO_F(DARK_BLOCKS_Y - 1u) * 0.5f) * DARK_DETAIL;
        const coreVector2 vPos = coreVector2(fX, FmodRange(fY - m_fFlyOffset, -fRange, fRange));

        // 
        m_aBlockRaw[i].SetPosition(coreVector3(vCamPos + vPos, m_aBlockRaw[i].GetPosition().z));
    }

    if(m_fDissolve >= 0.0f)
    {
        // 
        m_fDissolve.Update(2.0f);

        // 
        for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
        {
            // 
            const coreVector2 vAbsPos = m_aBlockRaw[i].GetPosition().xy();
            const coreVector2 vRelPos = (vAbsPos - vCamPos) * 0.1f;

            // 
            const coreFloat fDist  = MAX(ABS(vRelPos.x), ABS(vRelPos.y));
            const coreFloat fShift = 0.5f + 0.5f * COS(18.0f * (vRelPos.x + 2.0f * vRelPos.y));
            m_afFade[i].Update(MAX(m_fDissolve - (0.7f * fDist) - (0.8f * fShift), 0.0f) * 0.2f);

            // 
            const coreFloat   fLerp = LERPH3(0.0f, 1.0f, MIN(m_afFade[i], 1.0f));
            const coreVector2 vNorm = vRelPos.Normalized();
            const coreVector3 vAxis = coreVector3::Cross(coreVector3(vNorm, 0.0f), coreVector3(0.0f,0.0f,1.0f));
            const coreMatrix3 mRota = coreMatrix4::RotationAxis(fLerp * -3.0f, vAxis).m123();

            // 
            m_aBlockRaw[i].SetPosition   (coreVector3(vAbsPos + vNorm * (fLerp * 50.0f), DARK_HEIGHT + (fLerp * 100.0f)));
            m_aBlockRaw[i].SetDirection  (coreVector3(0.0f,1.0f,0.0f) * mRota);
            m_aBlockRaw[i].SetOrientation(coreVector3(0.0f,0.0f,1.0f) * mRota);
        }
    }

    // 
    m_Block.MoveNormal();

    // adjust volume of the base sound-effect
    if(m_pBaseSound->EnableRef(this))
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}


// ****************************************************************
// 
coreVector2 cDarkBackground::__GetCameraPos()
{
    // 
    return g_pEnvironment->GetCameraPos().xy();
}