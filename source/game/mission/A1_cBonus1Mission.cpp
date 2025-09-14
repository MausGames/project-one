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
cBonus1Mission::cBonus1Mission()noexcept
: m_vBeamPos   (coreVector2(0.0f,0.0f))
, m_vBeamDir   (coreVector2(0.0f,0.0f))
, m_fBeamWidth (0.0f)
, m_fBeamSpeed (1.0f)
{
    // 
    m_apBoss[0] = &m_Vaus;

    // 
    for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i)
    {
        // 
        m_aShelter[i].DefineModel  ("object_sphere_center.md3");
        m_aShelter[i].DefineTexture(0u, "effect_energy.png");
        m_aShelter[i].DefineProgram("effect_energy_flat_spheric_program");
        m_aShelter[i].SetSize      (coreVector3(1.0f,1.0f,1.0f) * 12.0f);
        m_aShelter[i].SetColor3    (COLOR_ENERGY_WHITE * 0.6f);
        m_aShelter[i].SetTexSize   (coreVector2(2.4f,0.4f) * 0.5f);
        m_aShelter[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

        // 
        m_aShelterWave[i].DefineModel  ("object_sphere_center.md3");
        m_aShelterWave[i].DefineTexture(0u, "effect_energy.png");
        m_aShelterWave[i].DefineProgram("effect_energy_flat_program");
        m_aShelterWave[i].SetSize      (coreVector3(1.0f,1.0f,1.0f) * 15.0f);
        m_aShelterWave[i].SetColor3    (COLOR_ENERGY_WHITE * 0.4f);
        m_aShelterWave[i].SetTexSize   (coreVector2(2.4f,0.4f) * 1.0f);
        m_aShelterWave[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

        // 
        m_aShelterBack1[i].DefineModel         ("object_cube_top.md3");
        m_aShelterBack1[i].DefineTexture       (0u, "default_black.webp");
        m_aShelterBack1[i].DefineProgram       ("object_ship_program");
        m_aShelterBack1[i].SetSize             (coreVector3(0.0f,50.0f,0.5f));
        m_aShelterBack1[i].SetCollisionModifier(coreVector3(1.1f, 1.1f,1.1f));
        m_aShelterBack1[i].SetColor3           (coreVector3(0.0f, 0.0f,0.0f));
        m_aShelterBack1[i].SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

        // 
        m_aShelterBack2[i].DefineModel  ("object_sphere.md3");
        m_aShelterBack2[i].DefineTexture(0u, "default_black.webp");
        m_aShelterBack2[i].DefineProgram("object_ship_program");
        m_aShelterBack2[i].SetSize      (m_aShelter[i].GetSize());
        m_aShelterBack2[i].SetColor3    (coreVector3(0.0f,0.0f,0.0f));
        m_aShelterBack2[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Beam.DefineModel         ("object_tube.md3");
    m_Beam.DefineTexture       (0u, "effect_energy.png");
    m_Beam.DefineProgram       ("effect_energy_flat_program");
    m_Beam.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    m_Beam.SetCollisionModifier(coreVector3(0.8f,1.0f,0.8f));
    m_Beam.SetColor3           (COLOR_ENERGY_MAGENTA * 0.8f);
    m_Beam.SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// destructor
cBonus1Mission::~cBonus1Mission()
{
    for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i) this->DisableShelter(i, false);
    this->DisableBeam(false);
}


// ****************************************************************
// 
void cBonus1Mission::EnableShelter(const coreUintW iIndex)
{
    ASSERT(iIndex < BONUS1_SHELTERS)
    coreObject3D& oShelter = m_aShelter     [iIndex];
    coreObject3D& oWave    = m_aShelterWave [iIndex];
    coreObject3D& oBack1   = m_aShelterBack1[iIndex];
    coreObject3D& oBack2   = m_aShelterBack2[iIndex];

    // 
    WARN_IF(oShelter.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableShelter(iIndex, false);

    // 
    ASSERT(m_Beam.IsEnabled(CORE_OBJECT_ENABLE_ALL))
    g_pGlow->BindObject(&oShelter);
    g_pGlow->BindObject(&oWave);
    g_pGlow->BindObject(&oBack1);
    g_pGlow->BindObject(&oBack2);

    // 
    oShelter.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    oWave   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    oBack1  .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    oBack2  .SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cBonus1Mission::DisableShelter(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < BONUS1_SHELTERS)
    coreObject3D& oShelter = m_aShelter     [iIndex];
    coreObject3D& oWave    = m_aShelterWave [iIndex];
    coreObject3D& oBack1   = m_aShelterBack1[iIndex];
    coreObject3D& oBack2   = m_aShelterBack2[iIndex];

    // 
    if(!oShelter.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    g_pGlow->UnbindObject(&oShelter);
    g_pGlow->UnbindObject(&oWave);
    g_pGlow->UnbindObject(&oBack1);
    g_pGlow->UnbindObject(&oBack2);

    // 
    //if(!bAnimated)
    {
        oShelter.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        oWave   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        oBack1  .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        oBack2  .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cBonus1Mission::EnableBeam()
{
    // 
    WARN_IF(m_Beam.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBeam(false);

    // 
    m_vBeamPos   = HIDDEN_POS;
    m_vBeamDir   = coreVector2(0.0f,1.0f);
    m_fBeamWidth = 10.0f;
    m_fBeamTime  = -2.0f;

    // 
    m_Beam.SetPosition(coreVector3(HIDDEN_POS, 0.0f));
    m_Beam.SetEnabled (CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Beam);
}


// ****************************************************************
// 
void cBonus1Mission::DisableBeam(const coreBool bAnimated)
{
    // 
    if(!m_Beam.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    //if(!bAnimated)
    {
        m_Beam.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_Beam);
    }
}


// ****************************************************************
// 
void cBonus1Mission::FadeBeam(const coreBool bEnable, const coreFloat fTime)
{
    if(bEnable)
    {
        m_fBeamTime = 1.0f;
    }
    else
    {
        m_fBeamTime = -1.0f;
    }
    m_fBeamSpeed = 1.0f / fTime;
}


// ****************************************************************
// 
void cBonus1Mission::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i) m_aShelterWave[i].Render();
    }
    glDepthMask(true);

    // 
    glColorMask(false, false, false, false);
    {
        for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i) m_aShelterBack1[i].Render();
        for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i) m_aShelterBack2[i].Render();
    }
    glColorMask(true, true, true, true);

    // 
    m_Beam.Render();

    DEPTH_PUSH

    // 
    for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i) m_aShelter[i].Render();
    for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aShelter[i]);
}


// ****************************************************************
// 
void cBonus1Mission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i)
    {
        coreObject3D& oShelter = m_aShelter     [i];
        coreObject3D& oWave    = m_aShelterWave [i];
        coreObject3D& oBack1   = m_aShelterBack1[i];
        coreObject3D& oBack2   = m_aShelterBack2[i];
        if(!oShelter.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreVector2 vPos  = oShelter.GetPosition().xy() + m_vBeamDir * oBack1.GetSize().y;
        const coreFloat   fSize = oShelter.GetVisualRadius() * (0.99f + 0.01f * SIN(10.0f*PI * m_fAnimation + I_TO_F(i)));

        // 
        oShelter.SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.5f));
        oShelter.Move();

        // 
        oWave.SetPosition (oShelter.GetPosition());
        oWave.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
        oWave.Move();

        // 
        oBack1.SetPosition (coreVector3(vPos,       0.0f));
        oBack1.SetSize     (coreVector3(fSize,      oBack1.GetSize().yz()));
        oBack1.SetDirection(coreVector3(m_vBeamDir, 0.0f));
        oBack1.Move();

        // 
        oBack2.SetPosition(oShelter.GetPosition());
        oBack2.SetSize    (oShelter.GetSize());
        oBack2.Move();
    }

    // 
    if(m_Beam.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        
        const coreFloat fOldTime = m_fBeamTime;

        if(m_fBeamTime > 0.0f) m_fBeamTime.UpdateMin( m_fBeamSpeed,  3.0f);
                          else m_fBeamTime.UpdateMax(-m_fBeamSpeed, -2.0f);
                          
        if((fOldTime < m_fBeamTime) && InBetween(2.0f, fOldTime, m_fBeamTime))
        {
            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 0.8f, 1.0f, SOUND_EFFECT_FIRE);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
        }

        coreFloat fAlpha = 0.0f;
        coreFloat fScale = 1.0f;
        coreFloat fScale2 = 1.0f;
        coreFloat fSign  = 1.0f;
        if(m_fBeamTime >= 2.0f)
        {
            fAlpha = 1.0f;
            fScale = 1.0f;//LERP(0.0f, 1.0f, MIN1((m_fBeamTime - 2.0f) * 5.0f));
            fScale2 = 1.0f;//LERP(1.0f, 1.0f, MIN1((m_fBeamTime - 2.0f) * 6.0f / m_fBeamSpeed));
            fSign  = 1.0f;
        }
        else if(m_fBeamTime >= 1.0f)
        {
            fAlpha =  LERPH3(0.5f, 0.0f, m_fBeamTime - 1.0f);
            fScale =  1.0f;
            fScale2 = LERP(1.0f, 0.5f, m_fBeamTime - 1.0f);
            fSign  = LERP(-0.6f, -0.4f, m_fBeamTime - 1.0f);
        }
        else if(m_fBeamTime <= -1.0f)
        {
            fAlpha =  1.0f;//LERP(1.0f, 0.5f, MIN1(-m_fBeamTime - 1.0f));
            fScale2 =  LERP(1.0f, 0.0f, MIN1(-m_fBeamTime - 1.0f));
            //fAlpha =  LERP(1.0f, 0.0f, MIN1((-m_fBeamTime - 1.0f) * 5.0f));
            //fScale2 =  1.0f;
            fSign  = 1.0f;
        }
        
        
        
        m_Beam.SetAlpha(fAlpha);
        
        
        const coreFloat fLen = 75.0f * fScale;
        
        const coreVector2 vPos = m_vBeamPos + m_vBeamDir * fLen;
        
        const coreFloat fTexScale = fLen / 65.0f + CORE_MATH_PRECISION;

        // 
        m_Beam.SetPosition (coreVector3(vPos,   0.0f));
        m_Beam.SetSize     (coreVector3(m_fBeamWidth * fScale2, fLen, 1.0f));
        m_Beam.SetDirection(coreVector3(m_vBeamDir, 0.0f));
        m_Beam.SetTexSize  (coreVector2(m_fBeamWidth * fScale2 * 0.2f, fTexScale * 2.0f));
        m_Beam.SetTexOffset(coreVector2(0.0f, FRACT(m_Beam.GetTexOffset().y - 2.0f * TIME * fSign / fTexScale)));
        m_Beam.Move();
        
        if(!InBetween(m_fBeamTime, 1.0f, 2.0f))
        {
            const coreUintW iNum  = MAX(F_TO_UI(m_Beam.GetSize().x * 0.5f), 2u);
            const coreFloat fHalf = I_TO_F(iNum - 1u) * 0.5f;
            
            for(coreUintW i = 0u; i < iNum; ++i)
            {
                const coreVector2 vTan = m_Beam.GetDirection().xy().Rotated90();
                
                const coreVector3 vRayDir = coreVector3(m_Beam.GetDirection().xy(), 0.0f);
                const coreVector3 vRayPos = coreVector3(m_Beam.GetPosition ().xy() + vTan * ((I_TO_F(i) / fHalf - 1.0f)) * m_Beam.GetSize().x * 0.5f, 0.0f) - vRayDir * 50.0f;
    
                coreFloat fHit = 1000.0f;
                for(coreUintW j = 0u; j < BONUS1_SHELTERS; ++j)
                {
                    coreFloat fHitDistance = 0.0f;
                    coreUint8 iHitCount    = 1u;
                    if(Core::Manager::Object->TestCollision(&m_aShelter[j], vRayPos, vRayDir, &fHitDistance, &iHitCount))
                    {
                        fHit = MIN(fHit, fHitDistance);
                    }
                }

                fHit = MIN(fHit, g_pForeground->RayIntersection(vRayPos.xy(), vRayDir.xy()));
    
                const coreVector3 vEffectPos = vRayPos + vRayDir * fHit;
                
    
                g_pSpecialEffects->CreateSplashFire(vEffectPos,      20.0f, 1u, COLOR_ENERGY_MAGENTA);
                g_pSpecialEffects->CreateBlowColor(vEffectPos, -vRayDir, 100.0f, 1u, COLOR_ENERGY_MAGENTA);
                
            }
        }
    }
}