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
cExhaustManager::cExhaustManager()noexcept
: m_fAnimation (0.0f)
{
    // 
    for(coreUintW i = 0u; i < EXHAUST_BASES; ++i)
    {
        m_aRenderList[i].Reallocate(EXHAUST_RENDER_CAPACITY);
        g_pGlow->BindList(&m_aRenderList[i]);
    }
    m_aRenderList[0].DefineProgram("effect_energy_flat_direct_inst_program");
    m_aRenderList[1].DefineProgram("effect_energy_flat_inst_program");
    m_aRenderList[2].DefineProgram("effect_energy_flat_inst_program");
    STATIC_ASSERT(EXHAUST_BASES == 3u)

    // 
    m_aPrototype[0].DefineModel  ("object_tube_open.md3");
    m_aPrototype[0].DefineTexture(0u, "effect_energy.png");
    m_aPrototype[0].DefineProgram("effect_energy_flat_direct_program");

    m_aPrototype[1].DefineModel  ("object_sphere_center.md3");
    m_aPrototype[1].DefineTexture(0u, "effect_energy.png");
    m_aPrototype[1].DefineProgram("effect_energy_flat_program");

    m_aPrototype[2].DefineModel  ("object_sphere.md3");
    m_aPrototype[2].DefineTexture(0u, "effect_energy.png");
    m_aPrototype[2].DefineProgram("effect_energy_flat_program");

    // 
    m_ObjectPool.Configure(sizeof(coreObject3D), EXHAUST_POOL_PAGESIZE);

    // 
    m_aData.reserve(EXHAUST_DATA_CAPACITY);
}


// ****************************************************************
// destructor
cExhaustManager::~cExhaustManager()
{
    // 
    ASSERT(m_aData.empty())
    m_aData.clear();

    // 
    for(coreUintW i = 0u; i < EXHAUST_BASES; ++i)
    {
        g_pGlow->UnbindList(&m_aRenderList[i]);
    }
}


// ****************************************************************
// 
void cExhaustManager::Render()
{
    // 
    for(coreUintW i = 0u; i < EXHAUST_BASES; ++i)
    {
        m_aRenderList[i].Render();
    }
}


// ****************************************************************
// 
void cExhaustManager::Move()
{
    constexpr coreFloat fStrength = 0.07f;

    // 
    m_fAnimation.UpdateMod(0.05f, 1.0f);

    // 
    FOR_EACH(it, m_aData)
    {
        cEnemy*       OUTPUT pEnemy  = it->pEnemy;
        coreObject3D* OUTPUT pObject = it->pObject;
        ASSERT(pEnemy != pObject)

        // 
        const coreFloat fOffset = I_TO_F((P_TO_UI(pObject) / sizeof(coreObject3D) % 10u)) * 0.1f;

        // 
        switch(it->eType)
        {
        default: ASSERT(false)

        case EXHAUST_TYPE_SCOUT:
            {
                // TODO 1: cleanup
                const coreFloat fLen  = fStrength * 25.0f;
                const coreFloat fSize = 1.0f - fStrength * 0.25f;

                pObject->SetSize       (coreVector3(fSize, fLen, fSize) * (0.9f * pEnemy->GetSize().x));
                pObject->SetTexOffset  (coreVector2(0.0f, FRACT(m_fAnimation * 15.0f + fOffset)));
                pObject->SetPosition   (pEnemy->GetPosition   () - pEnemy->GetDirection() * (pObject->GetSize().y + 2.5f * pEnemy->GetSize().x));
                pObject->SetDirection  (pEnemy->GetDirection  ());
                pObject->SetOrientation(pEnemy->GetOrientation());
                pObject->SetColor3     (COLOR_ENERGY_WHITE * 0.7f);
                pObject->SetAlpha      (pEnemy->GetAlpha      () * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize    (coreVector2(0.5f,0.25f));
            }
            break;

        case EXHAUST_TYPE_WARRIOR:
            {
                // TODO 1: cleanup
                const coreFloat fLen  = fStrength * 30.0f;
                const coreFloat fSize = 1.0f - fStrength * 0.25f;

                const coreVector3 vTan = coreVector3::Cross(pEnemy->GetDirection(), pEnemy->GetOrientation()).Normalized();

                pObject->SetSize       (coreVector3(fSize, fLen, fSize) * (0.9f * pEnemy->GetSize().x));
                pObject->SetTexOffset  (coreVector2(0.0f, FRACT(m_fAnimation * 15.0f + fOffset)));
                pObject->SetPosition   (pEnemy->GetPosition   () - pEnemy->GetDirection() * (pObject->GetSize().y + 1.85f * pEnemy->GetSize().x) + vTan * ((it->iSubType ? -1.4f : 1.4f) * pEnemy->GetSize().x));
                pObject->SetDirection  (pEnemy->GetDirection  ());
                pObject->SetOrientation(pEnemy->GetOrientation());
                pObject->SetColor3     (COLOR_ENERGY_WHITE * 0.7f);
                pObject->SetAlpha      (pEnemy->GetAlpha      () * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize    (coreVector2(0.5f,0.25f));
            }
            break;

        case EXHAUST_TYPE_STAR:
            {
                // TODO 1: cleanup
                pObject->SetSize     (coreVector3(1.0f,1.0f,1.0f) * pEnemy->GetVisualRadius() * 0.9f);
                pObject->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * -2.0f + fOffset)));
                pObject->SetPosition (pEnemy->GetPosition());
                pObject->SetDirection(pEnemy->GetDirection());
                pObject->SetColor3   (COLOR_ENERGY_WHITE * 0.5f);
                pObject->SetAlpha    (pEnemy->GetAlpha() * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize  (coreVector2(2.0f,1.0f) * 0.2f);
            }
            break;

        case EXHAUST_TYPE_ARROW:
            {
                // TODO 1: cleanup
                const coreFloat fLen  = fStrength * 40.0f;
                const coreFloat fSize = 1.0f - fStrength * 0.25f;

                pObject->SetSize     (coreVector3(fSize, fLen, fSize) * (0.9f * pEnemy->GetSize().x));
                pObject->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * 15.0f + fOffset)));
                pObject->SetPosition (pEnemy->GetPosition () - pEnemy->GetDirection() * (pObject->GetSize().y + 1.65f * pEnemy->GetSize().x));
                pObject->SetDirection(pEnemy->GetDirection());
                pObject->SetColor3   (COLOR_ENERGY_WHITE * 0.7f);
                pObject->SetAlpha    (pEnemy->GetAlpha    () * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize  (coreVector2(0.5f,0.25f));
            }
            break;

        case EXHAUST_TYPE_MINER:
            {
                // TODO 1: cleanup
                const coreFloat fLen  = fStrength * 25.0f;
                const coreFloat fSize = (1.0f - fStrength * 0.25f) * 1.5f;

                pObject->SetSize       (coreVector3(fSize, fLen, fSize) * (0.9f * pEnemy->GetSize().x));
                pObject->SetTexOffset  (coreVector2(0.0f, FRACT(m_fAnimation * 9.0f + fOffset)));
                pObject->SetPosition   (pEnemy->GetPosition   () - pEnemy->GetDirection() * (pObject->GetSize().y + 1.9f * pEnemy->GetSize().x));
                pObject->SetDirection  (pEnemy->GetDirection  ());
                pObject->SetOrientation(pEnemy->GetOrientation());
                pObject->SetColor3     (COLOR_ENERGY_WHITE * 0.7f);
                pObject->SetAlpha      (pEnemy->GetAlpha      () * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize    (coreVector2(0.75f,0.15f));
            }
            break;

        case EXHAUST_TYPE_FREEZER:
            {
                // TODO 1: cleanup
                const coreFloat fLen  = fStrength * 25.0f;
                const coreFloat fSize = 1.0f - fStrength * 0.25f;

                pObject->SetSize     (coreVector3(fSize, fLen, fSize) * (0.9f * pEnemy->GetSize().x));
                pObject->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * 15.0f + fOffset)));
                pObject->SetPosition (pEnemy->GetPosition () - pEnemy->GetDirection() * (pObject->GetSize().y + 2.35f * pEnemy->GetSize().x));
                pObject->SetDirection(pEnemy->GetDirection());
                pObject->SetColor3   (COLOR_ENERGY_WHITE * 0.7f);
                pObject->SetAlpha    (pEnemy->GetAlpha    () * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize  (coreVector2(0.5f,0.25f));
            }
            break;

        case EXHAUST_TYPE_CINDER:
            {
                // TODO 1: cleanup
                pObject->SetSize     (coreVector3(1.0f,1.0f,1.0f) * pEnemy->GetVisualRadius() * 1.15f);
                pObject->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * -2.0f + fOffset)));
                pObject->SetPosition (pEnemy->GetPosition ());
                pObject->SetDirection(coreVector3(1.0f,0.0f,0.0f));
                pObject->SetColor3   (COLOR_ENERGY_WHITE * 0.5f);
                pObject->SetAlpha    (pEnemy->GetAlpha    () * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize  (coreVector2(2.0f,1.0f) * 0.3f);
            }
            break;

        case EXHAUST_TYPE_METEOR:
            {
                // TODO 1: cleanup
                pObject->SetSize     (coreVector3(1.0f,1.0f,1.0f) * pEnemy->GetVisualRadius() * 1.15f);
                pObject->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * -2.0f + fOffset)));
                pObject->SetPosition (pEnemy->GetPosition ());
                pObject->SetDirection(coreVector3(1.0f,0.0f,0.0f));
                pObject->SetColor3   (COLOR_ENERGY_WHITE * 0.5f);
                pObject->SetAlpha    (pEnemy->GetAlpha    () * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize  (coreVector2(2.0f,1.0f) * pEnemy->GetVisualRadius() * 0.3f);
                //pObject->SetTexSize  (coreVector2(2.0f,1.0f) * SQRT(pEnemy->GetVisualRadius()) * 0.77f);
            }
            break;

        case EXHAUST_TYPE_UFO:
            {
                // TODO 1: cleanup
                pObject->SetSize     (coreVector3(1.0f,1.0f,1.0f) * pEnemy->GetVisualRange() * 1.4f);
                pObject->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * -2.0f + fOffset)));
                pObject->SetPosition (pEnemy->GetPosition ());
                pObject->SetDirection(pEnemy->GetDirection());
                pObject->SetColor3   (COLOR_ENERGY_WHITE * 0.5f);
                pObject->SetAlpha    (pEnemy->GetAlpha    () * (pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
                pObject->SetTexSize  (coreVector2(2.0f,1.0f) * pEnemy->GetVisualRadius() * 0.3f);
            }
            break;
        }
    }

    // 
    for(coreUintW i = 0u; i < EXHAUST_BASES; ++i)
    {
        m_aRenderList[i].MoveNormal();
    }
}


// ****************************************************************
// 
void cExhaustManager::BindEnemy(cEnemy* pEnemy, const eExhaustType eType)
{
    for(coreUintW i = 0u, ie = (eType == EXHAUST_TYPE_WARRIOR) ? 2u : 1u; i < ie; ++i)
    {
        // 
        coreUint8 iBase;
        switch(eType)
        {
        default: ASSERT(false)
        case EXHAUST_TYPE_SCOUT:   iBase = 0u; break;
        case EXHAUST_TYPE_WARRIOR: iBase = 0u; break;
        case EXHAUST_TYPE_STAR:    iBase = 1u; break;
        case EXHAUST_TYPE_ARROW:   iBase = 0u; break;
        case EXHAUST_TYPE_MINER:   iBase = 0u; break;
        case EXHAUST_TYPE_FREEZER: iBase = 0u; break;
        case EXHAUST_TYPE_CINDER:  iBase = 1u; break;
        case EXHAUST_TYPE_METEOR:  iBase = 2u; break;
        case EXHAUST_TYPE_UFO:     iBase = 2u; break;
        }
        ASSERT(iBase < EXHAUST_BASES)

        // 
        coreObject3D* pObject = POOLED_NEW(m_ObjectPool, coreObject3D, m_aPrototype[iBase]);
        m_aRenderList[iBase].BindObject(pObject);

        // 
        sExhaustData oData;
        oData.pEnemy   = pEnemy;
        oData.pObject  = pObject;
        oData.eType    = eType;
        oData.iSubType = i;
        oData.iBase    = iBase;

        // 
        m_aData.push_back(oData);
    }
}


// ****************************************************************
// 
void cExhaustManager::UnbindEnemy(cEnemy* pEnemy, const eExhaustType eType)
{
    // 
    coreUintW iNum = (eType == EXHAUST_TYPE_WARRIOR) ? 2u : 1u;

    // 
    FOR_EACH_DYN(it, m_aData)
    {
        if(it->pEnemy == pEnemy)
        {
            ASSERT(it->eType == eType)

            // 
            m_aRenderList[it->iBase].UnbindObject(it->pObject);
            POOLED_DELETE(m_ObjectPool, it->pObject)

            // 
            DYN_REMOVE(it, m_aData)

            // 
            if(--iNum == 0u) return;
        }
        else
        {
            // 
            DYN_KEEP(it)
        }
    }

    // 
    WARN_IF(true) {}
}