///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

static coreUint32 m_iSparkNum = 0u;
// ****************************************************************
// constructor
cSeaBackground::cSeaBackground()noexcept
: m_fWaveTime (0.0f)
{
    // TODO 1: bubbles ?    

    coreBatchList* pList1;
    coreBatchList* pList2;
    coreBatchList* pList3;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("dust", "earth", 6u, 4.0f);

    // allocate animal lists
    pList1 = new coreBatchList(SEA_ANIMAL_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SEA_ANIMAL_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");

    pList3 = new coreBatchList(SEA_ANIMAL_2_RESERVE);
    pList3->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_sea.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SEA_ANIMAL_NUM/4; ++i)
        {
            for(coreUintW j = 8u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition   = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_ANIMAL_NUM/4);
                const coreFloat   fHeight     = m_pOutdoor->RetrieveBackHeight(vPosition);
                //const coreFloat   fHeightUp   = m_pOutdoor->RetrieveBackHeight(coreVector2(vPosition.x,     vPosition.y + OUTDOOR_DETAIL));
                //const coreFloat   fHeightDown = m_pOutdoor->RetrieveBackHeight(coreVector2(vPosition.x, MAX(vPosition.y - OUTDOOR_DETAIL, I_TO_F(OUTDOOR_VIEW / 2u))));

                // test for valid values
                if(//(fHeight > fHeightDown) && (fHeight < fHeightUp) &&
                   (fHeight > -22.0f)      && (fHeight < -16.0f)    && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
                {
                    
                        // determine object type
                        const coreUint8 iType = Core::Rand->Bool() ? 0u : (Core::Rand->Bool(0.75f) ? 1u : 2u);
                        
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2((iType == 2u) ? 6.0f : 4.0f)) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2((iType == 2u) ? 6.0f : 4.0f)) &&
                       !cBackground::_CheckIntersectionQuick(pList3,                  vPosition, POW2(6.0f)))
                    {
                        // 
                        const coreVector3 vDirection   = m_pOutdoor->RetrieveBackNormal(vPosition);
                        const coreVector3 vTangent     = coreVector3::Cross(vDirection, -CAMERA_DIRECTION).Normalized();
                        const coreVector3 vOrientation = coreVector3::Cross(vDirection, vTangent);


                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                        pObject->DefineModel((iType == 0u) ? "environment_sea_03.md3" : ((iType == 1u) ? "environment_sea_04.md3" : "environment_sea_05.md3"));

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                        pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(2.2f, 2.6f) * 1.6f);
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
        }

        // 
        this->_StoreHeight(pList1, 0.0f);
        this->_StoreHeight(pList2, 0.0f);
        this->_StoreHeight(pList3, 1.0f);
        this->_StoreNormalList(pList1);
        this->_StoreNormalList(pList2);
        this->_StoreNormalList(pList3);

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1, SEA_ANIMAL_1_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, SEA_ANIMAL_2_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        cBackground::_FillInfinite(pList3, SEA_ANIMAL_2_RESERVE);
        m_apGroundObjectList.push_back(pList3);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
        m_pOutdoor->GetShadowMap()->BindList(pList3);
    }

    // allocate weed lists
    pList1 = new coreBatchList(SEA_WEED_RESERVE);
    pList1->DefineProgram("object_wave_inst_program");

    pList2 = new coreBatchList(SEA_WEED_RESERVE);   // TODO 1: NOT YET, setting the shader-uniform is not submittable
    pList2->DefineProgram("object_wave_inst_program");
    {
        // load object resources
        coreObject3D oBase;
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
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2(2.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[2], vPosition, POW2(4.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(4.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(4.0f)))
                    {
                        // determine object type
                        const coreBool bType = Core::Rand->Bool(0.75f);

                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                        pObject->DefineModel(bType ? "environment_sea_01.md3" : "environment_sea_01.md3");

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                        pObject->SetSize       (coreVector3::Rand(0.7f,1.6f, 0.7f,1.6f, 0.7f,1.6f) * 4.0f * 1.3f);
                        pObject->SetDirection  (coreVector3((coreVector2::Rand().Processed(ABS) * coreVector2(-1.0f,1.0f) + coreVector2(-10.0f,10.0f)).Normalized(), 0.0f));
                        //pObject->SetOrientation(coreVector3(coreVector2::Rand(-0.1f,0.1f, -0.1f,0.1f), 1.0f).Normalized());
                        pObject->SetColor3     (coreVector3(0.5f, Core::Rand->Float(0.55f, 0.65f) * (bType ? 1.0f : 0.85f), 1.0f) * 0.85f);
                        pObject->SetColor3     (coreVector3(0.5f, Core::Rand->Float(0.55f, 0.65f), 1.0f) * 0.85f);
                        //pObject->SetColor3     (coreVector3(1.0f, 1.0f, 1.0f) * 0.6f);

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -1.0f);
        this->_StoreHeight(pList2, -1.0f);

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1, SEA_WEED_RESERVE);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList1);   // # cover more pixels

        cBackground::_FillInfinite(pList2, SEA_WEED_RESERVE);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList2);   // # cover more pixels

        // bind weed lists to shadow map
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
        oBase.DefineTexture(0u, "environment_algae.png");
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
                    pObject->SetSize    (coreVector3(1.0f,1.0f,1.0f));
                    //pObject->SetColor3  (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.7f, 1.0f));
                    pObject->SetAlpha   (Core::Rand->Float(0.0f, 1.0f));
                    //pObject->SetTexSize (coreVector2(0.5f,1.0f));

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
        m_iSparkNum = papContent->size();
        ASSERT(!(m_iSparkNum % 2u))

        // post-process list and add it to the ground (# for distortion)
        cBackground::_FillInfinite(pList1, SEA_ALGAE_RESERVE);
        m_apDecalObjectList.push_back(pList1);
    }
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
    // create underwater-surface object
    m_pWater = new cUnderWater();

    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_sea.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
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
        if(pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// move the sea background
void cSeaBackground::__MoveOwn()
{

    static coreFlow m_fSparkTime = 0.0f;
    // 
    m_fSparkTime.Update(0.25f * MAX(ABS(g_pEnvironment->GetSpeed()), 2.0f));

    // 
  //  coreBatchList* 
       coreBatchList*  pList = m_apDecalObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pSpark = (*pList->List())[i];
        if(!pSpark->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        // 
        const coreFloat fOffset = I_TO_F(POW2(i) % m_iSparkNum);
        const coreFloat fTime   = m_fSparkTime * ((i % 2u) ? 1.0f : -1.0f) + fOffset;
        const coreFloat fPos    = SIN(fTime * 0.1f + fOffset) * (I_TO_F(OUTDOOR_WIDTH) * OUTDOOR_DETAIL * 0.2f);

        // 
        pSpark->SetPosition(coreVector3(fPos, pSpark->GetPosition().yz()));
    }
    pList->MoveNormal();
    
    
    
    // TODO 1: weed all looks into light direction, always
    
    // 
    m_fWaveTime.Update(1.4f);

    // adjust volume of the base sound-effect
    if(m_pBaseSound->EnableRef(this))
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}


// ****************************************************************
// 
void cSeaBackground::__UpdateOwn()
{
    const coreBatchList*  pGround    = m_apGroundObjectList[0];
    const coreBool        bInstanced = pGround->IsInstanced();
    const coreProgramPtr& pProgram   = bInstanced ? pGround->GetProgram() : pGround->List()->front()->GetProgram();

    // enable the shader-program
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;

    // 
    pProgram->SendUniform("u_v1Time", m_fWaveTime);
    
    
            cShadow::EnableShadowRead(bInstanced ? SHADOW_HANDLE_OBJECT_WAVE_INST : SHADOW_HANDLE_OBJECT_WAVE);
}