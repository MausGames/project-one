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
cSeaBackground::cSeaBackground()noexcept
: m_fWaveTime       (0.0f)
, m_fAlgaeTime      (0.0f)
, m_fOverdriveTime  (0.0f)
, m_vOverdriveCount (0u)
, m_bOverdrive      (false)
, m_Loaded          ()
{
    coreBatchList* pList1;
    coreBatchList* pList2;
    coreBatchList* pList3;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("dust", "earth", OUTDOOR_ALGORITHM_SEA, 4.0f, false);

    // 
    pList1 = new coreBatchList(SEA_CORAL_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SEA_CORAL_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");

    pList3 = new coreBatchList(SEA_CORAL_3_RESERVE);
    pList3->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_sea.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SEA_CORAL_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_CORAL_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -22.0f) && (fHeight < -16.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                // determine object type
                const coreUint8 iType = Core::Rand->Bool() ? 0u : (Core::Rand->Bool(0.75f) ? 1u : 2u);

                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2((iType == 2u) ? 6.0f : 4.0f)) &&
                   !cBackground::_CheckIntersectionQuick(pList2, vPosition, POW2((iType == 2u) ? 6.0f : 4.0f)) &&
                   !cBackground::_CheckIntersectionQuick(pList3, vPosition, POW2(6.0f)))
                {
                    // 
                    const coreVector3 vDirection   = m_pOutdoor->RetrieveBackNormal(vPosition);
                    const coreVector3 vTangent     = coreVector3::Cross(vDirection, -CAMERA_DIRECTION).Normalized();
                    const coreVector3 vOrientation = coreVector3::Cross(vDirection, vTangent);

                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                    pObject->DefineModel((iType == 0u) ? "environment_sea_02.md3" : ((iType == 1u) ? "environment_sea_03.md3" : "environment_sea_04.md3"));

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(2.2f, 2.6f) * 1.65f);
                    pObject->SetDirection  (vOrientation);
                    pObject->SetOrientation(vDirection);
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.7f, 0.85f));

                    // add object to the list
                         if(iType == 0u) pList1->BindObject(pObject);
                    else if(iType == 1u) pList2->BindObject(pObject);
                    else                 pList3->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);
        this->_StoreHeight(pList2, 0.0f);
        this->_StoreHeight(pList3, 1.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, SEA_CORAL_1_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, SEA_CORAL_2_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        cBackground::_FillInfinite(pList3, SEA_CORAL_3_RESERVE);
        m_apGroundObjectList.push_back(pList3);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
        m_pOutdoor->GetShadowMap()->BindList(pList3);
    }

    // allocate weed lists
    pList1 = new coreBatchList(SEA_WEED_RESERVE);
    pList1->DefineProgram("object_wave_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_sea_01.md3");
        oBase.DefineTexture(0u, "environment_sea.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_wave_program");

        for(coreUintW i = 0u; i < SEA_WEED_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_WEED_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -23.0f) && (fHeight < -18.0f))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(2.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(4.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(4.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[2], vPosition, POW2(4.0f)))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition(coreVector3(vPosition, 0.0f));
                        pObject->SetSize    (coreVector3::Rand(0.7f,1.6f, 0.7f,1.6f, 0.7f,1.6f) * 5.2f);
                        pObject->SetColor3  (coreVector3(1.0f, Core::Rand->Float(0.9f, 1.0f), 0.9f) * 0.4f);

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -1.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, SEA_WEED_RESERVE);
        m_apGroundObjectList.push_front(pList1);   // # front

        // bind list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    pList1 = new coreBatchList(SEA_ANIMAL_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SEA_ANIMAL_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_animal_diff.png");
        oBase.DefineTexture(1u, "environment_animal_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SEA_ANIMAL_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_ANIMAL_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight < -22.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(6.0f)) &&
                   !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2(6.0f)) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(4.0f)) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(4.0f)) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[2], vPosition, POW2(4.0f)) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[3], vPosition, POW2(4.0f)))
                {

                    // 
                    const coreVector3 vDirection = m_pOutdoor->RetrieveBackNormal(vPosition);

                    // determine object type
                    const coreUint8 bType = Core::Rand->Bool(0.75f);

                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                    pObject->DefineModel(bType ? "environment_animal_01.md3" : "environment_animal_02.md3");

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(2.2f, 2.6f) * 1.35f);
                    pObject->SetDirection  (vDirection);
                    pObject->SetOrientation(coreVector3(Core::Rand->Float(-1.0f, 1.0f), -2.0f, 0.0f).Normalized());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.7f, 0.85f));

                    // add object to the list
                    if(bType) pList1->BindObject(pObject);
                         else pList2->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.9f);
        this->_StoreHeight(pList2, 0.9f);
        this->_StoreNormalList(pList1);
        this->_StoreNormalList(pList2);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, SEA_ANIMAL_1_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, SEA_ANIMAL_2_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // allocate algae list
    pList1 = new coreBatchList(SEA_ALGAE_RESERVE);
    pList1->DefineProgram("effect_decal_single_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_particle_02.png");
        oBase.DefineProgram("effect_decal_single_program");

        for(coreUintW i = 0u; i < SEA_ALGAE_NUM; ++i)
        {
            for(coreUintW j = 20u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_ALGAE_NUM);
                const coreFloat   fHeight   = Core::Rand->Float(10.0f, 50.0f);

                // test for valid values
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2((i % 2u) ? 2.0f : 15.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition(coreVector3(vPosition, fHeight));
                    pObject->SetSize    (coreVector3(1.0f,1.0f,1.0f) * 1.25f * 1.2f);
                    pObject->SetAlpha   (Core::Rand->Float(0.3f, 0.8f) * 0.7f);

                    // add object to the list
                    pList1->BindObject(pObject);
                    break;
                }
            }
        }

        // 
        coreSet<coreObject3D*>* papContent = pList1->List();
        if(papContent->size() % 2u)
        {
            POOLED_DELETE(s_MemoryPool, papContent->back())
            papContent->pop_back();
        }

        // 
        m_iAlgaeNum = papContent->size();
        ASSERT(!(m_iAlgaeNum % 2u))

        // post-process list and add to the ground (# for distortion)
        cBackground::_FillInfinite(pList1, SEA_ALGAE_RESERVE);
        m_apDecalObjectList.push_back(pList1);
    }

    // 
    this->SetGroundDensity(4u, 0.0f);
    this->SetGroundDensity(5u, 0.0f);
}


// ****************************************************************
// destructor
cSeaBackground::~cSeaBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cSeaBackground::__InitOwn()
{
    m_Loaded.Unlock();
    
    // create underwater-surface object
    m_pWater = new cUnderWater();

    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_sea.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
        m_Loaded.Lock();
    });
}


// ****************************************************************
// 
void cSeaBackground::__ExitOwn()
{
    // 
    SAFE_DELETE(m_pWater)

    // stop base sound-effect
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        if(m_Loaded.IsLocked() && pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// move the sea background
void cSeaBackground::__MoveOwn()
{
    // 
    m_fAlgaeTime.Update(0.25f * MAX(ABS(g_pEnvironment->GetSpeed()), 2.0f));

    // 
    coreBatchList* pList = m_apDecalObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pAlgae = (*pList->List())[i];

        // 
        const coreFloat   fOffset = I_TO_F(POW2(i) % m_iAlgaeNum);
        const coreFloat   fTime   = m_fAlgaeTime * ((i % 2u) ? 1.0f : -1.0f) + fOffset;
        const coreFloat   fPos    = SIN(fTime * 0.1f + fOffset) * (I_TO_F(OUTDOOR_WIDTH) * OUTDOOR_DETAIL * 0.2f);
        const coreVector2 vDir    = coreVector2::Direction(fTime);

        // 
        pAlgae->SetPosition (coreVector3(fPos, pAlgae->GetPosition().yz()));   // # always, to determine visibility
        pAlgae->SetDirection(coreVector3(vDir, 0.0f));
    }
    pList->MoveNormal();

    // 
    const coreVector3 vWeedDir = MapToAxisInv(coreVector3(-1.0f,1.0f,0.0f).Normalized(), g_pEnvironment->GetDirection());

    //
    pList = m_apGroundObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pWeed = (*pList->List())[i];
        if(!pWeed->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        // 
        pWeed->SetDirection(vWeedDir);
    }
    pList->MoveNormal();

    // 
    m_fWaveTime.Update(1.4f);

    // adjust volume of the base sound-effect
    if(m_Loaded.IsLocked() && m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
    }
    
    
    
    if(m_bOverdrive)
    {
        // 
        const coreVector2 vEnvDirection = g_pEnvironment->GetDirection();
        const coreFloat   fEnvSpeed     = g_pEnvironment->GetSpeed();
        const coreFloat   fEnvFlyOffset = g_pEnvironment->GetFlyOffset();
    
        if(TIME && (fEnvSpeed > 0.0f))
        {
            m_fOverdriveTime.Update(fEnvSpeed * 0.1f);
            if(m_fOverdriveTime >= 1.0f)
            {
                m_fOverdriveTime = FRACT(m_fOverdriveTime);
                

                const coreFloat fOffest = 10.0f * OUTDOOR_DETAIL;
    
                const coreVector2 vGroundPos = coreVector2(SIN((fEnvFlyOffset + fOffest / OUTDOOR_DETAIL) * (0.075f*PI)) * -12.5f, fEnvFlyOffset * OUTDOOR_DETAIL);
    
                const coreVector2 vBodyPos = MapToAxis(coreVector2(vGroundPos.x, fOffest), vEnvDirection);

                this->__CreateOverdrive(coreVector3(vBodyPos, g_pEnvironment->RetrieveSafeHeight(vBodyPos) - 0.0f));
            }
        }
    }
}


// ****************************************************************
// 
void cSeaBackground::__UpdateOwn()
{
    const coreBatchList*  pGround  = m_apGroundObjectList[0];
    const coreProgramPtr& pProgram = pGround->IsInstanced() ? pGround->GetProgram() : pGround->List()->front()->GetProgram();

    // enable the shader-program
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;

    // 
    pProgram->SendUniform("u_v1Time", m_fWaveTime);

    // 
    cShadow::EnableShadowRead(pGround->IsInstanced() ? SHADOW_HANDLE_OBJECT_WAVE_INST : SHADOW_HANDLE_OBJECT_WAVE);
}


// ****************************************************************
// 
void cSeaBackground::__CreateOverdrive(const coreVector3 vIntersect)
{
    const coreUintW i = (m_vOverdriveCount++);
    
    // 
    const coreBool    bRotated   = i % 2u;
    const coreBool    bFlipped   = (i / 2u) % 2u;
    const coreVector3 vDecalPos  = vIntersect;
    const coreVector2 vDecalSize = coreVector2(1.0f,1.0f) * 15.0f;
    const coreVector2 vDecalDir  = coreVector2(0.0f,1.0f);

    // load object resources
    coreObject3D* pObject = MANAGED_NEW(coreObject3D);
    pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
    pObject->DefineTexture(0u, "effect_soot.png");
    pObject->DefineProgram("effect_decal_single_program");

    // set object properties
    pObject->SetSize     (coreVector3((bRotated ? vDecalSize.yx()       : vDecalSize),                            1.0f));
    pObject->SetDirection(coreVector3((bRotated ? vDecalDir.Rotated90() : vDecalDir) * (bFlipped ? -1.0f : 1.0f), 0.0f));
    pObject->SetColor3   (coreVector3(0.0f, 0.0f, 0.0f));

    // add object to background
    this->AddDecal(pObject, vDecalPos, 256u, "effect_decal_single_inst_program", LIST_KEY);
}