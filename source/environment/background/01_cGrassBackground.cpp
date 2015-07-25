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
cGrassBackground::cGrassBackground()noexcept
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("grass", "dust", 2u, 4.0f);

    // create water-surface object
    m_pWater = new cWater();

    // allocate stone list
    pList1 = new coreBatchList(GRASS_STONES_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        for(coreUintW j = 0u; j < 2u; ++j)   // types
        {
            const coreUintW iStoneTries = j ? GRASS_STONES_2_NUM : GRASS_STONES_1_NUM;
            const coreFloat fStoneSize  = j ? 2.0f               : 2.2f;

            // select position and height test function
            std::function<coreFloat(const coreFloat&, const coreFloat&)> nTestFunc;
            if(j) nTestFunc = [](const coreFloat& h, const coreFloat& y) {return (h > -23.0f && h < -18.0f && (F_TO_SI(y+160.0f) % 80 < 40));};
             else nTestFunc = [](const coreFloat& h, const coreFloat& y) {return (h > -20.0f && h < -18.0f);};

            for(coreUintW i = 0u; i < iStoneTries; ++i)
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(iStoneTries)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(nTestFunc(fHeight, vPosition.y))
                {
                    if(!j || !cBackground::_CheckIntersection(pList1, vPosition, 16.0f))
                    {
                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  ("environment_stone_01.md3");
                        pObject->DefineTexture(0u, "environment_stone_diff.png");
                        pObject->DefineTexture(1u, "environment_stone_norm.png");
                        pObject->DefineProgram("object_ground_program");

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, fHeight+0.2f));
                        pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(1.0f, 1.3f) * fStoneSize);
                        pObject->SetDirection  (coreVector3::Rand());
                        pObject->SetOrientation(coreVector3::Rand());
                        pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                    }
                }
            }
        }

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1);
        pList1->ShrinkToFit();
        m_apGroundObjectList.push_back(pList1);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate reed lists
    pList1 = new coreBatchList(GRASS_REEDS_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(GRASS_REEDS_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        for(coreUintW i = 0u; i < GRASS_REEDS_NUM; ++i)
        {
            for(coreUintW j = 0u; j < 3u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_REEDS_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(fHeight > -23.0f && fHeight < -18.0f && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 25.0f))
                    {
                        // determine object type
                        const coreBool bType = (Core::Rand->Int(3) || fHeight >= -20.0f) ? true : false;

                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  (bType ? "environment_reed_01.md3" : "environment_reed_02.md3");
                        pObject->DefineTexture(0u, "environment_reed.png");
                        pObject->DefineTexture(1u, "environment_grass_norm.png");
                        pObject->DefineProgram("object_ground_program");

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, fHeight-0.8f));
                        pObject->SetSize       (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 2.0f);
                        pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
                        pObject->SetColor3     (coreVector3(1.0f, 1.0f * Core::Rand->Float(0.55f, 0.7f), 0.5f));

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1);
        pList1->ShrinkToFit();
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2);
        pList2->ShrinkToFit();
        m_apGroundObjectList.push_back(pList2);

        // bind reed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // allocate flower list
    pList1 = new coreBatchList(GRASS_FLOWERS_RESERVE);
    pList1->DefineProgram("effect_decal_spheric_inst_program");
    {
        for(coreUintW i = GRASS_FLOWERS_NUM; i--; )
        {
            for(coreUintW j = 0u; j < 15u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_FLOWERS_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(fHeight > -14.5f)
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 225.0f))
                    {
                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  (Core::Manager::Object->GetLowModel());
                        pObject->DefineTexture(0u, "environment_flowers.png");
                        pObject->DefineProgram("effect_decal_spheric_program");

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, fHeight-0.8f));
                        pObject->SetSize     (coreVector3(coreVector2(1.65f,1.65f) * Core::Rand->Float(9.0f, 10.0f), 1.0f));
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1);
        pList1->ShrinkToFit();
        m_apDecalObjectList.push_back(pList1);
    }

    // allocate cloud list
    pList1 = new coreBatchList(GRASS_CLOUDS_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        for(coreUintW i = 0u; i < GRASS_CLOUDS_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.3f, 0.3f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_CLOUDS_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // load object resources
            coreObject3D* pObject = new coreObject3D();
            pObject->DefineModel  (Core::Manager::Object->GetLowModel());
            pObject->DefineTexture(0u, "environment_clouds_mid.png");
            pObject->DefineProgram("environment_clouds_program");

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.1f,2.1f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite(pList1);
        ASSERT(pList1->GetCurCapacity() == GRASS_CLOUDS_RESERVE)
        m_apAirObjectList.push_back(pList1);
    }


    m_White.DefineTexture(0u, "environment_clouds_blue.png"); 
    m_White.DefineProgram("menu_grey_program"); 
    m_White.SetSize(coreVector2(1.0f,1.0f)); 
    m_White.SetTexSize(coreVector2(1.0f,1.0f) * 1.2f); 
    //m_White.SetColor3(coreVector3(0.0f, 0.43f, 0.69f) /*coreVector3(112.0f/255.0f,66.0f/255.0f,20.0f/255.0f)* 2.0f*/ ); 
    //m_White.SetColor3(LERP(coreVector3(1.0f,1.0f,1.0f), coreVector3(0.0f, 0.43f, 0.69f), 0.1f)); 
    m_White.Move(); 


    // load nature sound-effect
    //m_pNatureSound = Core::Manager::Resource->Get<coreSound>("environment_nature.wav"); 
    m_pNatureSound = Core::Manager::Resource->Get<coreSound>("environment_wind.wav"); 
    m_pNatureSound.GetHandle()->OnLoadOnce([&]()
    {
        m_pNatureSound->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });
}


// ****************************************************************
// destructor
cGrassBackground::~cGrassBackground()
{
    // stop nature sound-effect
    if(m_pNatureSound->EnableRef(this))
        m_pNatureSound->Stop();
}


// ****************************************************************
// render the grass background
void cGrassBackground::__RenderOwn()
{
    glDisable(GL_DEPTH_TEST); 
    //m_White.Render(); 
    glEnable(GL_DEPTH_TEST); 
}


// ****************************************************************
// move the grass background
void cGrassBackground::__MoveOwn()
{
    //g_pEnvironment->SetTargetSpeed    (8.0f - 2.0f*m_White.GetAlpha()); 

     
    m_White.SetTexOffset(coreVector2(0.0f, m_White.GetTexOffset().y - 0.65f/8.0f * Core::System->GetTime() * g_pEnvironment->GetSpeed()));

     
    //m_White.SetColor3(LERP(coreVector3(1.0f,1.0f,1.0f), /*coreVector3(0.0f, 0.43f, 0.69f)*/coreVector3(112.0f/255.0f,66.0f/255.0f,20.0f/255.0f)* 2.0f, m_White.GetAlpha()));


    // TODO # sound-volume per config value 
    // adjust volume of the nature sound-effect
    if(m_pNatureSound->EnableRef(this))
        m_pNatureSound->SetVolume(MAX(6.0f /6.0f* g_pEnvironment->GetTransition().GetValue((g_pEnvironment->GetBackground() == this) ? CORE_TIMER_GET_NORMAL : CORE_TIMER_GET_REVERSED), 0.0f));
}