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
: cBackground       (false)
, m_Block           (DARK_BLOCKS)
, m_afStartHeight   {}
, m_fFlyOffset      (0.0f)
, m_iIndexOffset    (0u)
, m_fAppear         (0.0f)
, m_fDissolve       (-1.0f)
, m_afFade          {}
, m_vColor          (cDarkBackground::Color)
, m_vColor2         (cDarkBackground::Color2)
, m_iToken          (0u)
, m_fLightningFlash (0.0f)
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

            // 
            m_afStartHeight[i] = Core::Rand->Float(-0.5f, 0.5f) * DARK_DETAIL;
            
            const coreVector3 vColor = DARK_COLOR_DEFAULT * LERP(0.1f, 1.0f, STEP(-DARK_DETAIL * 1.5f, DARK_DETAIL * 1.5f, m_afStartHeight[i]));

            // set object properties
            pBlock->SetPosition(coreVector3(0.0f, 0.0f, DARK_HEIGHT + m_afStartHeight[i]));
            pBlock->SetSize    (coreVector3(0.5f,0.5f,0.5f) * DARK_DETAIL);
            pBlock->SetColor4  (coreVector4(vColor, 1.0f) * Core::Rand->Float(0.65f, 1.0f));
            
            const coreUintW iType = ((i + (i / DARK_BLOCKS_X)) * 3u) % 8u;
            pBlock->SetTexSize  (coreVector2(0.25f,0.5f));
            pBlock->SetTexOffset(coreVector2(I_TO_F(iType % 4u) * 0.25f, I_TO_F((iType / 4u) % 2u) * 0.5f));

            // add object to the list
            m_Block.BindObject(pBlock);
        }
    }

    // 
    m_pOutdoor->GetShadowMap()->BindList(&m_Block);

    // 
    m_Lightning.DefineProgram("menu_color_program");
    m_Lightning.SetPosition  (coreVector2(0.0f,0.0f));
    m_Lightning.SetSize      (coreVector2(1.0f,1.0f));
    m_Lightning.SetAlpha     (0.0f);
    m_Lightning.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Headlight.SetAlpha(0.0f);
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
void cDarkBackground::Appear()
{
    m_fAppear = 1.3f + 1.2f;
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
    m_iToken = m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cDarkBackground::__ExitOwn()
{
    // stop base sound-effect
    if(!Core::Manager::Resource->DetachFunction(m_iToken))
    {
        m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
        {
            if(pResource->EnableRef(this))
                pResource->Stop();
        });
    }
}


// ****************************************************************
// 
void cDarkBackground::__RenderOwnBefore()
{
    if(!this->IsDissolved())
    {
        glDisable(GL_BLEND);
        {
            // 
            m_Block.Render();
        }
        glEnable(GL_BLEND);
    }
}


// ****************************************************************
// 
void cDarkBackground::__RenderOwnAfter()
{
    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Lightning.Render();

        // 
        if(m_Headlight.GetAlpha()) m_Headlight.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cDarkBackground::__MoveOwn()
{
    // 
    m_fFlyOffset.Update(g_pEnvironment->GetSpeed() * DARK_SPEED);

    if(!this->IsDissolved())
    {
        // 
        const coreVector2 vCamPos = g_pEnvironment->GetCameraPos().xy() - g_pEnvironment->GetSideOffset();
        const coreFloat   fRange  = I_TO_F(DARK_BLOCKS_Y) * DARK_DETAIL * 0.5f;
        
        if(m_fAppear)
        {
            m_fAppear.UpdateMax(-0.3f, 0.0f);
            const coreFloat fAppear2 = BLENDBR(m_fAppear / (1.3f + 1.2f)) * (1.3f + 1.2f);
            
            for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
            {
                const coreFloat fRand = 1.0f - STEP(0.65f, 1.0f, m_aBlockRaw[i].GetAlpha());// I_TO_F((i * 41u) % DARK_BLOCKS) / I_TO_F(DARK_BLOCKS);
                const coreFloat fTime = (LERP(1.0f, 0.0f, CLAMP01((fAppear2 * 3.0f / 1.3f - fRand * 2.0f))));
                
                // 
                const coreFloat fHeight = LERPB(-10.0f, m_afStartHeight[i], fTime);
                const coreVector3 vColor2 = DARK_COLOR_DEFAULT * LERP(0.1f, 1.0f, STEP(-DARK_DETAIL * 1.5f, DARK_DETAIL * 1.5f, m_afStartHeight[i]));
                const coreVector3 vColor = LERPB(coreVector3(0.0f,0.0f,0.0f), vColor2, fTime);
                
                
                
                this->SetBlockHeight(i, fHeight);
                this->SetBlockColor (i, vColor);
            }
        }

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

        // 
        m_iIndexOffset = DARK_BLOCKS - (F_TO_UI(m_fFlyOffset / DARK_DETAIL) * DARK_BLOCKS_X) % DARK_BLOCKS;

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
                m_afFade[i].Update(MAX0(m_fDissolve - (0.7f * fDist) - (0.8f * fShift)) * 0.2f);

                // 
                const coreFloat   fLerp = BLENDH3(MIN1(m_afFade[i]));
                const coreVector2 vNorm = vRelPos.Normalized();
                const coreVector3 vAxis = coreVector3::Cross(coreVector3(vNorm, 0.0f), coreVector3(0.0f,0.0f,1.0f));
                const coreMatrix3 mRota = coreMatrix4::RotationAxis(fLerp * -3.0f, vAxis).m123();

                // 
                m_aBlockRaw[i].SetPosition   (coreVector3(vAbsPos + vNorm * (fLerp * 50.0f), DARK_HEIGHT + (fLerp * 100.0f)));
                m_aBlockRaw[i].SetDirection  (coreVector3(0.0f,1.0f,0.0f) * mRota);
                m_aBlockRaw[i].SetOrientation(coreVector3(0.0f,0.0f,1.0f) * mRota);
                m_aBlockRaw[i].SetEnabled    ((m_afFade[i] >= 1.0f) ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
            }
        }

        // 
        m_Block.MoveNormal();
    }

    // adjust volume of the base sound-effect
    if(m_pBaseSound.IsUsable() && m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this) * (1.0f - STEP(0.0f, 10.0f, m_fDissolve)));
    }

    // 
    m_fLightningFlash.UpdateMax(-1.0f, 0.0f);

    // 
    this->__UpdateLightning();

    // 
    if(m_Headlight.GetAlpha()) m_Headlight.Move();
}


// ****************************************************************
// 
void cDarkBackground::__UpdateOwn()
{
    // 
    if(m_Headlight.GetAlpha()) m_Headlight.UpdateDefault(1u);
}


// ****************************************************************
// 
void cDarkBackground::__UpdateLightning()
{
    // 
    m_Lightning.SetColor3 (g_CurConfig.Graphics.iFlash ? coreVector3(1.0f,1.0f,1.0f) : COLOR_MENU_BLACK);
    m_Lightning.SetAlpha  (BLENDH3(MIN1(m_fLightningFlash)) * 0.7f);
    m_Lightning.SetEnabled(m_fLightningFlash ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Lightning.Move();
}


// ****************************************************************
// 
coreVector2 cDarkBackground::__GetCameraPos()
{
    // 
    return g_pEnvironment->GetCameraPos().xy();
}