//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cDesertBackground::cDesertBackground()noexcept
{
    coreBatchList* pList1;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("dust", "crack", 3u, 4.0f);

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
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 25.0f))
                {
                    // create object
                    coreObject3D* pObject = Core::Manager::Memory->New<coreObject3D>(oBase);

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
    pList1 = new coreBatchList(DESERT_SAND_NUM);
    pList1->DefineProgram("effect_decal_single_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowModel());
        oBase.DefineTexture(0u, "effect_sand.png");
        oBase.DefineProgram("effect_decal_single_program");

        for(coreUintW i = 0u; i < DESERT_SAND_NUM; ++i)
        {
            // create object
            coreObject3D* pObject = Core::Manager::Memory->New<coreObject3D>(oBase);

            // set object properties
            pObject->SetPosition(coreVector3(0.0f,0.0f,0.0f));
            pObject->SetSize    (coreVector3(1.0f,1.0f,1.0f) * SQRT2 * 80.0f);
            pObject->SetColor3  (coreVector3(200.0f/255.0f, 186.0f/255.0f, 156.0f/255.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        m_apAirObjectList.push_back(pList1);
    }

    // load wind sound-effect
    m_pWindSound = Core::Manager::Resource->Get<coreSound>("environment_wind.wav");
    m_pWindSound.GetHandle()->OnLoadOnce([this, pResource = m_pWindSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });
}


// ****************************************************************
// destructor
cDesertBackground::~cDesertBackground()
{
    // stop wind sound-effect
    if(m_pWindSound->EnableRef(this))
        m_pWindSound->Stop();
}


// ****************************************************************
// move the desert background
void cDesertBackground::__MoveOwn()
{
    // 
    coreBatchList*      pList = m_apAirObjectList[0];
    const coreObject3D* pBase = (*pList->List()) [0];

    // 
    const coreFloat   fStrength  = 0.5f;
    const coreVector2 vPosition  = g_pEnvironment->GetCameraPos().xy();
    const coreVector2 vDirection = coreVector2(-1.0f,1.0f).Normalize();
    const coreVector2 vMove      = vDirection * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexSize   = coreVector2(1.0f,1.0f) * (4.5f * fStrength);
    const coreVector2 vTexOffset = pBase->GetTexOffset() + (coreVector2(0.0f,-1.2f) + vMove) * coreVector2(1.0f,1.0f) * (Core::System->GetTime() * fStrength);

    // 
    for(coreUintW i = 0; i < DESERT_SAND_NUM; ++i)
    {
        coreObject3D* pSand = (*pList->List())[i];

        pSand->SetPosition (coreVector3(vPosition,  10.0f + 10.0f * I_TO_F(i)));
        pSand->SetDirection(coreVector3(vDirection, 0.0f));
        pSand->SetTexSize  ((vTexSize));
        pSand->SetTexOffset((vTexOffset + coreVector2(0.3f,0.3f) * I_TO_F(i)).Process(FRACT));
    }
    pList->MoveNormal();

    // 
    if(m_pWindSound->EnableRef(this))
        m_pWindSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}