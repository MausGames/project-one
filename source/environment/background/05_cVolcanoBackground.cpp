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
cVolcanoBackground::cVolcanoBackground()noexcept
: m_Smoke      (512u)
, m_fSparkTime (Core::Rand->Float(10.0f))
{
    coreBatchList* pList1;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("rock", "lava", 7u, 4.0f);
    m_pOutdoor->LoadProgram(true);

    // allocate smoke list
    pList1 = new coreBatchList(VOLCANO_SMOKE_RESERVE);
    {
        for(coreUintW i = 0u; i < VOLCANO_SMOKE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, VOLCANO_SMOKE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -21.0f) && (fHeight < -18.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 25.0f))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D);

                    // set object properties
                    pObject->SetPosition(coreVector3(vPosition, 0.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, VOLCANO_SMOKE_RESERVE);
        m_apGroundObjectList.push_back(pList1);
    }

    // allocate spark list
    pList1 = new coreBatchList(VOLCANO_SPARK_RESERVE);
    pList1->DefineProgram("effect_decal_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "effect_particle_32.png");
        oBase.DefineProgram("effect_decal_program");

        for(coreUintW i = 0u; i < VOLCANO_SPARK_NUM; ++i)
        {
            for(coreUintW j = 10u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, VOLCANO_SPARK_NUM);
                const coreFloat   fHeight   = Core::Rand->Float(10.0f, 40.0f);

                // test for valid values
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 680.0f))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition(coreVector3(vPosition, fHeight));
                    pObject->SetSize    (coreVector3(1.0f,1.0f,1.0f)); 

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

        // post-process list and add it to the air
        cBackground::_FillInfinite(pList1, VOLCANO_SPARK_RESERVE);
        m_apAirObjectList.push_back(pList1);
    }




    // 
    m_Smoke.DefineProgram("effect_particle_smoke_program"); 
    m_Smoke.DefineTexture(0u, "effect_particle_128.png"); 

    pList1 = m_apGroundObjectList[0]; 
    m_aSmokeEffect.reserve(pList1->List()->size()); 
    FOR_EACH(it, *pList1->List()) 
    {
        m_aSmokeEffect.emplace_back(&m_Smoke); 
        m_aSmokeEffect.back().SetOrigin(*it); 
    }


    
    m_pLavaSound = Core::Manager::Resource->Get<coreSound>("environment_lava.wav");
    m_pLavaSound.OnUsableOnce([this, pResource = m_pLavaSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });
}


// ****************************************************************
// destructor
cVolcanoBackground::~cVolcanoBackground()
{

    m_Smoke.ClearAll();  

    m_aSmokeEffect.clear();


    if(m_pLavaSound->EnableRef(this))
        m_pLavaSound->Stop();
}


// ****************************************************************
// 
void cVolcanoBackground::__RenderOwnBefore()
{
    // 
    m_Lava.Render();

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Smoke.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cVolcanoBackground::__MoveOwn()
{
    // 
    m_Lava.SetFlyOffset(g_pEnvironment->GetFlyOffset());
    m_Lava.Move();

    // 
    coreBatchList* pList = m_apGroundObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pSmoke = (*pList->List())[i];
        if(!pSmoke->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        m_aSmokeEffect[i].CreateParticle(1, 6.0f, [](coreParticle* OUTPUT pParticle)
        {
            constexpr coreFloat fScale = 10.0f;
            pParticle->SetPositionRel(coreVector3::Rand(0.0f), coreVector3::Rand(1.0f) + coreVector3::Rand(-fScale, fScale) + coreVector3(-10.0f,20.0f,10.0f) * 2.0f);
            pParticle->SetScaleAbs   (3.0f,                              12.5f * 2.0f);
            pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI*0.1f, PI*0.1f));
            pParticle->SetColor4Abs  (coreVector4(0.2f,0.2f,0.2f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
            pParticle->SetSpeed      (0.1f * Core::Rand->Float(0.9f, 1.1f));
            // TODO: rounding-error: color goes below 0.0f                            
        });
    }

    //Core::Debug->InspectValue("Smoke", (coreUint32)m_Smoke.GetNumActiveParticles());

    m_Smoke.Move();




    // 
    m_fSparkTime.Update(1.0f);

    // 
  //  coreBatchList* 
        pList = m_apAirObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pSpark = (*pList->List())[i];
        if(!pSpark->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        // 
        const coreFloat   fOffset = I_TO_F((i*i) % m_iSparkNum);
        const coreFloat   fTime   = m_fSparkTime * ((i % 2u) ? 1.0f : -1.0f) + fOffset;
        const coreFloat   fPos    = SIN(fTime * 0.1f + fOffset) * (I_TO_F(OUTDOOR_WIDTH) * OUTDOOR_DETAIL * 0.2f);
        const coreVector2 vDir    = coreVector2::Direction(fTime);

        // 
        pSpark->SetPosition   (coreVector3(fPos, pSpark->GetPosition().yz()));
        pSpark->SetDirection  (coreVector3(vDir, 0.0f));
    }
    pList->MoveNormal();


    if(m_pLavaSound->EnableRef(this))
        m_pLavaSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}