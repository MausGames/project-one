///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// movement in reconstruction phase should be linear, smooth movement feels sluggish, and it's already smoothly interpolated anyway
// web should not revert too much, as the algorithm can just move forward
// first direction block wave from left-right should have one (any) pointing up, first wave from top-bottom should have one (bottom) pointing up
// orb-netz sollte sich nach kurzer zeit so schnell runter bewegen, dass man nicht mehr draufhalten kann, weil man sich sonst nicht schnell genug nach oben bewegen kann, aber nicht zu schnell, dass man sich nicht mehr traut anzugreifen
// boss sollte nur eine resurrection haben, bei mehreren fühlt sich der spieler eher verarscht als überrascht
// the flipped shooting during the swing phase is more interesting and removes safe spots
// make aiming in final phase not too precise, to reduce (unnecessary) pressure, and speed not too high, to allow player to dodge and attack properly
// in linearer einzelteil-phase, teile müssen sich bis zum rand bewegen, weil man sonst einfach stehen bleiben kann
// in stand-rotierender einzelteil-phase, alle 4 teile sollten nicht gleichzeitig in eine richtung feuern, weil man sonst nicht ausweichen kann
// nur eine wand sollte sich auf den spieler zubewegen, zu viele seiten sind zu kompliziert (vor allem mit zusätzlichen geschossen) (is schon heavy in normaler wave), außerdem könnte einschlag nur in mitte sein weil teile sonst langsam verdeckt werden, und einzelteile würden sich beim abschuss komisch überlagern
// einzelteil-phase sollt kohärente muster haben, damit die gefahr einfach einzuschätzen ist, vor allem weil der spieler die bewegung kontrolliert (er soll sich nicht so fühlen als hätte er keine kontrolle)
// TODO 1: rotation von körper oder flügel signalisieren bestimmten angriff -> andere bosse auch
// TODO 1: ram von boss und wings braucht flammen-wellen-effekt
// TODO 1: outline vom body is dünner als von flügeln (wegen shape)
// TODO 1: tod: gegner wird von seinen eigenen flügeln aufgespießt, tak-tak-tak-tak-boom
// TODO 1: tod: fliegt in lava, kommt wieder raus
// TODO 1: boss glüht nach dem einschlag in boden (und wand ?)
// TODO 1: MAIN: fragment, easy, hard (decision), coop, 3 badges, boss health, medal goal, intro, outro, foreshadow
// TODO 1: mehrere einzelne einschläge schon während der background sich noch bewegt, dann bleibt er stehen und alle 5 schlagen nochmal ein
// TODO 1: banner-anzeige bei finalem tod hat anderen text "Real Time Bonus" (Real farbig ? und separat rausstehend)
// TODO 1: wellen beim sturmangriff sind zu ähnlich wie von sword, vielleicht kreis-wellen wie beim alten leviathan ?
// TODO 1: orb-pad verlangsamen, dafür fliegen die spitzen manchmal nach oben ? oder boss fliegt herum ?
// TODO 1: bei ersten way-blöcke von unten-oben, da sollte der von unten nach oben zeigen
// TODO 1: während orb phase sollte boss sich langsam drehen
// TODO 1: enum for the wingstate
// TODO 1: better handle wing coloring, should always be the same as boss, but when they are detached and get a hit they get "their" color re-applied
// TODO 1: check if wave-phase (2.) is too fast, if yes, then add a bit more delay between dives
// TODO 1: wave-phase verwendet magenta bullets, direkt nach magenta way-phase
// TODO 1: einschlag-spuren wird durch lava verdeckt
// TODO 1: pause time during fake time-bonus (and hide all related interface items
// TODO 1: remove fake time-bonus early with an effect (slash ?) when boss returns
// TODO 1: in einzelteil-phase, vielleicht geschosse etwas mehr auseinander, aber dafür schnellere rotation in 2. sub-phase (1.0f*PI ? für die mittleren ?)
// TODO 1: einzelteil-phase muss auch punkte geben, aber mit konstantem multiplier vom boss
// TODO 1: wall-movement sollte smooth sein, besonders wenn man von ihr erwischt wird
// TODO 1: man sollte erst schaden von der wall nehmen, wenn alle bullets da sind (so wie bei wave)
// TODO 1: bei fake-death fliegen die einzelteile weg, bei resurrection sollen sie wieder zusammen fliegen (ein einem zug mit boss einflug)
// TODO 1: vielleicht sollte in finaler phase der geschoss-lerp erst bei 50% health starten
// TODO 1: flammen sound
// TODO 1: rotierende einzelteile kurz vorm abschuss brauchen einen weiter (glow) effekt, damit mans gscheit sieht
// TODO 1: remove the remaining squishy movement in orb-phase (in sync with wave part)
// TODO 1: wenn man die teile aus der wand schlägt, sollten sie zur seite fliegen, damit man nicht unabsichtlich glaubt man muss ihnen ausweichen
// TODO 1: wenn einzelteile rotiert werden (zb. bei explosion) sollten sie geshiftet werden wie bei einzelteil-phase


// ****************************************************************
// counter identifier
#define DEATH_COUNT     (0u)
#define STORM_COUNT     (1u)
#define PULL_COUNT      (2u)
#define INCURSION_COUNT (3u)


// ****************************************************************
// vector identifier
#define START_VALUES_POS (0u)   // # uses 0u - 4u
#define START_VALUES_DIR (5u)   // # uses 5u - 9u
#define WALL_OFFSET      (10u)
#define TRANSITION_TIME  (11u)


// ****************************************************************
// constructor
cCholBoss::cCholBoss()noexcept
: m_aiWingState   {}
, m_afWingTime    {}
, m_avWingReturn  {}
, m_iFireType     (0u)
, m_bFireActive   (false)
, m_fWebLevel     (0.0f)
, m_fWebReverse   (0.0f)
, m_iWebIndex     (0u)
, m_iPathIndex    (1u)
, m_iPathSide     (2u)
, m_fTilt         (0.0f)
, m_fFlap         (0.0f)
, m_fPush         (0.0f)
, m_fAnimation    (0.0f)
, m_fResurrection (0.0f)

{
    // load models
    this->DefineModelHigh("ship_boss_chol_body_high.md3");
    this->DefineModelLow ("ship_boss_chol_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.0f);

    // configure the boss
    this->Configure(5500, COLOR_SHIP_ORANGE);
    this->AddStatus(ENEMY_STATUS_DAMAGING);

    // 
    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
    {
        m_aWing[i].DefineModelHigh("ship_boss_chol_wing_high.md3");
        m_aWing[i].DefineModelLow ("ship_boss_chol_wing_low.md3");
        m_aWing[i].SetSize        (this->GetSize());
        m_aWing[i].Configure      (10000, COLOR_SHIP_ORANGE);
        m_aWing[i].AddStatus      (ENEMY_STATUS_DAMAGING);
        m_aWing[i].SetParent      (this);
    }

    // 
    m_Fire.DefineTexture(0u, "effect_energy.png");
    m_Fire.DefineProgram("effect_energy_flat_direct_program");
    m_Fire.SetColor3    (COLOR_ENERGY_ORANGE);
    m_Fire.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_apFireModel[0] = Core::Manager::Resource->Get<coreModel>("object_boss_chol_fire_01.md3");
    m_apFireModel[1] = Core::Manager::Resource->Get<coreModel>("object_boss_chol_fire_02.md3");
}


// ****************************************************************
// 
void cCholBoss::__ResurrectOwn()
{
    // 
    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
        this->__ChangeWingIntro(i);
}


// ****************************************************************
// 
void cCholBoss::__KillOwn(const coreBool bAnimated)
{
    cGeluMission* pMission = d_cast<cGeluMission*>(g_pGame->GetCurMission());

    // 
    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
        pMission->DisableFang(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < GELU_WAYS; ++i)
        pMission->DisableWay(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < GELU_ORBS; ++i)
        pMission->DisableOrb(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < GELU_LINES; ++i)
        pMission->DisableLine(i, bAnimated);

    // 
    g_pPostProcessing->SetWallOffset(2u, 0.0f);

    // 
    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->SetArea(PLAYER_AREA_DEFAULT);
        pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
    });

    // 
    this->__DisableFire(bAnimated);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cCholBoss::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    m_Fire.Render();
}


// ****************************************************************
// 
void cCholBoss::__MoveOwn()
{
    cGeluMission* pMission = d_cast<cGeluMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(1.0f, 100.0f);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_LINEAR)
        {
            const auto nImpactFunc = [](coreObject3D* OUTPUT pObject, const coreVector2 vPos, const coreVector3 vDir, const coreFloat fTime, const coreFloat fTimeBefore, const coreBool bBig)
            {
                const coreFloat   fHeight  = g_pEnvironment->RetrieveSafeHeight(vPos);
                const coreVector3 vPoint   = coreVector3(vPos, fHeight + 8.0f);
                const coreVector3 vPosFrom = vPoint - vDir * 150.0f;
                const coreVector3 vPosTo   = vPoint;

                pObject->SetPosition (LERP(vPosFrom, vPosTo, LERPBR(0.0f, 1.0f, CLAMP(fTime, 0.0f, 1.0f))));
                pObject->SetDirection(vDir);

                if(InBetween(1.0f, fTimeBefore, fTime))
                {
                    if(bBig) g_pSpecialEffects->MacroExplosionColorBig  (vPosTo, COLOR_ENERGY_ORANGE);
                        else g_pSpecialEffects->MacroExplosionColorSmall(vPosTo, COLOR_ENERGY_ORANGE);

                        
                    const coreUintW i = 0u;
                    // 
                    const coreBool    bRotated   = i % 2u;
                    const coreBool    bFlipped   = (i / 2u) % 2u;
                    const coreVector3 vDecalPos  = coreVector3(vPos, fHeight);
                    const coreVector2 vDecalSize = coreVector2(1.0f,1.0f) * (bBig ? 20.0f : 15.0f);
                    const coreVector2 vDecalDir  = coreVector2(0.0f,1.0f);

                    // load object resources
                    coreObject3D* pObject2 = MANAGED_NEW(coreObject3D);
                    pObject2->DefineModel  (Core::Manager::Object->GetLowQuad());
                    pObject2->DefineTexture(0u, "effect_soot.png");
                    pObject2->DefineProgram("effect_decal_single_program");

                    // set object properties
                    pObject2->SetSize     (coreVector3((bRotated ? vDecalSize.yx()       : vDecalSize),                            1.0f));
                    pObject2->SetDirection(coreVector3((bRotated ? vDecalDir.Rotated90() : vDecalDir) * (bFlipped ? -1.0f : 1.0f), 0.0f));
                    pObject2->SetColor3   (coreVector3(0.0f,0.0f,0.0f));

                    // add object to background or windscreen
                    g_pEnvironment->GetBackground()->AddDecal(pObject2, vDecalPos, 8u, "effect_decal_single_inst_program", LIST_KEY);
                }
            };

            nImpactFunc(&m_aWing[0], coreVector2(-8.0f,-1.0f) * 2.0f, coreVector3(-0.5f,-1.0f,-2.5f).Normalized(), (fTime - 0.0f)  * 6.0f, (fTimeBefore - 0.0f)  * 6.0f, false);
            nImpactFunc(&m_aWing[1], coreVector2(-2.0f,-5.0f) * 2.0f, coreVector3(-1.0f,-0.5f,-2.5f).Normalized(), (fTime - 0.05f) * 6.0f, (fTimeBefore - 0.05f) * 6.0f, false);
            nImpactFunc(&m_aWing[2], coreVector2( 6.0f, 2.0f) * 2.0f, coreVector3(-1.5f,-1.0f,-2.5f).Normalized(), (fTime - 0.1f)  * 6.0f, (fTimeBefore - 0.1f)  * 6.0f, false);
            nImpactFunc(&m_aWing[3], coreVector2( 0.0f, 7.0f) * 2.0f, coreVector3(-1.0f,-1.5f,-2.5f).Normalized(), (fTime - 0.15f) * 6.0f, (fTimeBefore - 0.15f) * 6.0f, false);
            nImpactFunc(this,        coreVector2( 0.0f, 1.0f) * 2.0f, coreVector3(-1.0f,-1.0f,-2.5f).Normalized(), (fTime - 0.4f)  * 6.0f, (fTimeBefore - 0.4f)  * 6.0f, true);

            if(PHASE_TIME_AFTER(0.85f))
            {
                PHASE_CHANGE_INC

                this->_StartBoss();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
                {
                    const coreVector3 vWingDir = m_aWing[i].GetDirection();

                    m_avVector[START_VALUES_POS + i].xyz(m_aWing[i].GetPosition());
                    m_avVector[START_VALUES_DIR + i].xyz(coreVector3(MapToAxisInv(vWingDir.xy(), this->GetDirection().xy().Normalized()).Angle(), vWingDir.z, SQRT(1.0f - POW2(vWingDir.z))));

                    this->__ChangeWingInc(i);
                }

                m_avVector[START_VALUES_POS + 4u].xyz(this->GetPosition());
                m_avVector[START_VALUES_DIR + 4u].xyz(coreVector3(this->GetDirection().xy().Angle(), this->GetDirection().z, SQRT(1.0f - POW2(this->GetDirection().z))));
            }

            const coreVector3 vStartPos = m_avVector[START_VALUES_POS + 4u].xyz();
            const coreVector3 vStartDir = m_avVector[START_VALUES_DIR + 4u].xyz();

            const coreVector3 vNewPos = LERPH3(vStartPos, coreVector3(0.0f, 0.6f * FOREGROUND_AREA.y, 0.0f), fTime);
            const coreVector3 vNewDir = coreVector3(coreVector2::Direction(LERPH3(vStartDir.x, 1.0f*PI, fTime)) * vStartDir.z, LERPH3(vStartDir.y, 0.0f, fTime)).Normalized();

            this->SetPosition (vNewPos);
            this->SetDirection(vNewDir);

            g_pEnvironment->SetTargetSpeedNow(6.0f * BLENDBR(fTime));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->__EnableFire(0u);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(20u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        const coreInt32 iLostHealth = this->GetLostHealth();

        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_LINEAR)
        {
            if(iLostHealth < 2000)
            {
                m_avVector[TRANSITION_TIME].x = MIN1(m_avVector[TRANSITION_TIME].x + 0.2f * TIME);
            }
            else
            {
                m_avVector[TRANSITION_TIME].x = MAX0(m_avVector[TRANSITION_TIME].x - 0.4f * TIME);

                m_fTilt = LERPS(0.0f*PI, 2.0f*PI, m_avVector[TRANSITION_TIME].x);

                this->DefaultOrientate(m_fTilt);

                if(m_avVector[TRANSITION_TIME].x <= 0.0f)
                {
                    PHASE_CHANGE_TO(30u)
                }
            }

            const coreVector2 vNewPos = coreVector2(0.6f * SIN(fTime * 2.0f*PI) * LERPS(0.0f, 1.0f, m_avVector[TRANSITION_TIME].x), 0.6f);

            this->SetPosition(coreVector3(vNewPos * FOREGROUND_AREA, 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_RESET(0u)
            }
        });

        if(iLostHealth < 2000)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 0.22f, LERP_LINEAR)
            {
                const coreVector2 vDir   = StepRotated90(iTick % 4u);
                const coreVector2 vBase  = (iLostHealth < 1200) ? coreVector2(1.0f,0.0f) : coreVector2(0.0f,1.0f);
                const coreUintW   iStart = (iLostHealth < 1200) ? 0u : 12u;

                for(coreUintW i = 0u; i < 6u; ++i)
                {
                    const coreFloat   fHeight = (I_TO_F(i % 6u) - 2.5f) * GELU_WAY_STEP;
                    const coreVector2 vPos    = (vBase * 1.3f + vBase.Rotated90() * fHeight) * FOREGROUND_AREA;

                    pMission->DisableWay(iStart + i,      false);
                    pMission->DisableWay(iStart + i + 6u, false);

                    pMission->EnableWay(iStart + i,      -vPos,  vDir);
                    pMission->EnableWay(iStart + i + 6u,  vPos, -vDir);
                }
            });
        }

        PHASE_CONTROL_TICKER(2u, 0u, 3.0f, LERP_LINEAR)
        {
            if((iTick % 4u) < 2u) return;

            const coreVector2 vPos = this->GetPosition().xy();

            for(coreUintW j = 36u; j--; )
            {
                if((j % 2u) == (iTick % 2u)) continue;

                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 5.0f + 87.5f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, vPos,  vDir)->ChangeSize(1.7f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, vPos, -vDir)->ChangeSize(1.7f);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_LINEAR)
        {
            const coreFloat fSide = (m_aiCounter[INCURSION_COUNT] & 0x04) ? -1.0f : 1.0f;

            coreVector2 vTarget;
            switch(m_aiCounter[INCURSION_COUNT] % 4)
            {
            default: ASSERT(false)
            case 0: vTarget = coreVector2(-1.0f, 0.0f) * fSide; break;
            case 1: vTarget = coreVector2( 1.0f, 0.0f) * fSide; break;
            case 2: vTarget = coreVector2( 0.0f,-1.0f);         break;
            case 3: vTarget = coreVector2( 0.0f, 1.0f);         break;
            }

            if(PHASE_TIME_BEFORE(0.5f))
            {
                this->DefaultMoveLerp(m_vLastPosition, m_vLastPosition * 3.0f, BLENDBR(fTime * 2.0f));
            }
            else
            {
                const coreVector2 vNewPos = vTarget * 0.6f;

                this->DefaultMoveLerp(vNewPos * 3.0f, vNewPos, BLENDB((fTime - 0.5f) * 2.0f));
                this->SetDirection   (coreVector3(-vTarget, 0.0f));
            }

            if(PHASE_TIME_POINT(0.6f))
            {
                for(coreUintW j = 24u; j--; )
                {
                    const coreVector2 vPos = (vTarget * 1.2f + vTarget.Rotated90() * ((I_TO_F(j) - 11.5f) * 0.09f)) * FOREGROUND_AREA;
                    const coreVector2 vDir = -vTarget;

                    const coreFloat fSpeed = LERP(1.0f, 0.4f, ABS(I_TO_F(j) - 11.5f) / 11.5f) * ((j >= 7u && j < 17u) ? 2.0f : 1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.7f);
                }
            }

            if(PHASE_FINISHED)
            {
                const coreBool bChange = (this->GetLostHealth() >= 2500);

                if(bChange && ((m_aiCounter[INCURSION_COUNT] % 4) == 3))
                {
                    PHASE_CHANGE_TO(40u)
                }
                else
                {
                    PHASE_RESET(0u)

                    if(bChange) m_aiCounter[INCURSION_COUNT]  = 3;
                           else m_aiCounter[INCURSION_COUNT] += 1;

                    this->StorePosition();
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        if(PHASE_BEGINNING2)
        {
            this->__DisableFire(true);
        }

        PHASE_CONTROL_TICKER(0u, 4u, 2.0f, LERP_LINEAR)
        {
            coreUintW iIndex;
            switch(iTick)
            {
            default: ASSERT(false)
            case 0u: iIndex = 0u; break;
            case 1u: iIndex = 2u; break;
            case 2u: iIndex = 1u; break;
            case 3u: iIndex = 3u; break;
            }

            this->__ChangeWingThrow2(iIndex);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp     (m_vLastPosition, coreVector2(0.0f,1.7f), fTime);
            this->DefaultOrientateLerp(0.0f*PI,         6.0f*PI,                fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 42u)
    {
        PHASE_CONTROL_TICKER(0u, 20u, 15.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = coreVector2(1.2f,1.2f);
            const coreVector2 vDir = coreVector2(1.0f,0.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(3, 0.0f, this, vPos, vDir)->ChangeSize(1.5f)->AddStatus(BULLET_STATUS_GHOST);

            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
        });

        for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
        {
            if((m_aWing[i].GetLostHealth() > 50) && (m_aiWingState[i] < 41u))
            {
                this->__ChangeWingPull(i);
                m_aWing[i].AddStatus(ENEMY_STATUS_GHOST);

                const coreVector2 vPos = m_aWing[i].GetPosition().xy();

                for(coreUintW j = 40u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) - 19.5f) * 4.5f);

                    const coreFloat fSpeed = ((j % 4u) < 2u) ? 0.7f : 0.5f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.3f);
                }

                m_aiCounter[PULL_COUNT] += 1;
                if(m_aiCounter[PULL_COUNT] >= 4)
                {
                    PHASE_CHANGE_INC
                }

                m_avVector[WALL_OFFSET].z = MAX0(m_avVector[WALL_OFFSET].z - 0.01f);
            }
        }

        m_avVector[WALL_OFFSET].z = MIN1(m_avVector[WALL_OFFSET].z + 0.04f * TIME);
    }

    // ################################################################
    // 
    else if(m_iPhase == 43u)
    {
        constexpr coreFloat fSpeed = 0.3f;

        PHASE_CONTROL_PAUSE(0u, fSpeed)
        {
            PHASE_CHANGE_TO(100u)

            for(coreUintW i = 0u; i < CHOL_WINGS; ++i) m_aWing[i].RemoveStatus(ENEMY_STATUS_GHOST);
            for(coreUintW i = 0u; i < CHOL_WINGS; ++i) m_aWing[i].SetCurHealthPct(1.0f);
            for(coreUintW i = 1u; i < CHOL_WINGS; ++i) m_aWing[i].SetParent(&m_aWing[0]);
        });

        const coreFloat fPrev = m_avVector[WALL_OFFSET].z;

        m_avVector[WALL_OFFSET].z = MAX0(m_avVector[WALL_OFFSET].z - fSpeed * TIME);

        if((fPrev && !m_avVector[WALL_OFFSET].z) || ((m_iPhase != 43u) && m_avVector[WALL_OFFSET].z))
        {
            m_avVector[WALL_OFFSET].z = 0.0f;

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([](cConeBullet* OUTPUT pBullet)
            {
                pBullet->Deactivate(true);
            });

            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                pMission->EnableFang(i);

            this->ChangeToBottom();

            pMission->SetCrushFree(true);
            pMission->SetCrushLong(true);
        }

        constexpr coreFloat fDelay = 0.7f;

        coreFloat fStuckTime = 0.0f;

        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
        {
            coreObject3D* pFang = pMission->GetFang(i);
            if(!pFang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            const coreUintW X = i % GELU_FANGS_DIMENSION;
            const coreUintW Y = i / GELU_FANGS_DIMENSION;

            coreUintW iOrder;
            switch(X)
            {
            default: ASSERT(false)
            case 0u: iOrder = 0u; break;
            case 1u: iOrder = 2u; break;
            case 2u: iOrder = 4u; break;
            case 3u: iOrder = 3u; break;
            case 4u: iOrder = 1u; break;
            }

            const coreVector2 vOffset = coreVector2(I_TO_F(X) - 2.0f, I_TO_F(Y) - 2.0f) * GELU_FANG_STEP;

            const coreFloat fStompTimePrev = CLAMP((m_fPhaseTimeBefore * 0.9f * 3.0f/5.0f) - I_TO_F(iOrder) * 0.4f, 0.0f, 1.0f + fDelay);
            const coreFloat fStompTime     = CLAMP((m_fPhaseTime       * 0.9f * 3.0f/5.0f) - I_TO_F(iOrder) * 0.4f, 0.0f, 1.0f + fDelay);

            pFang->SetPosition(coreVector3((vOffset + coreVector2(0.0f, LERP(0.0f, I_TO_F(GELU_FANGS_DIMENSION) * GELU_FANG_STEP, ABS(COS(DelayTime(fStompTime, 0.5f, fDelay) * (1.0f*PI)))))) * FOREGROUND_AREA, 0.0f));

            if(InBetween(0.5f, fStompTimePrev, fStompTime))
            {
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

                if(i == 2u) g_pSpecialEffects->MacroExplosionPhysicalColorBig(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3, COLOR_FIRE_ORANGE);
            }

            if(i == 2u) fStuckTime = fStompTime;
        }

        if(fStuckTime < 0.5f)
        {
            const coreVector2 vFangPos = pMission->GetFang(2u)->GetPosition().xy();
            const coreVector2 vNewPos  = m_vLastPosition * FOREGROUND_AREA + coreVector2(0.0f, vFangPos.y - I_TO_F(GELU_FANGS_DIMENSION - 2u) * GELU_FANG_STEP * FOREGROUND_AREA.y);

            this->SetPosition(coreVector3(vNewPos, 0.0f));
        }
        else
        {
            this->SetPosition(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3);
        }

        if(fStuckTime >= 1.0f + fDelay)
        {
            for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                pMission->DisableFang(i, false);

            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        const coreFloat fSpeedMove  = MIN1(m_fPhaseTime);
        const coreFloat fSpeedShoot = LERP(15.0f, 1.0f, this->GetCurHealthPct());

        this->DefaultMoveTarget(this->NearestPlayerDual((FMOD(m_fPhaseTime, 10.0f) < 5.0f) ? 1u : 0u)->GetPosition().xy(), LERP(0.0f, 20.0f, fSpeedMove), LERP(0.0f, 1.0f, fSpeedMove));
        this->DefaultOrientate (m_fPhaseTime * (2.0f*PI));

        PHASE_CONTROL_TICKER(0u, 0u, fSpeedShoot, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos,  vDir)             ->ChangeSize(1.4f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos,  vDir.Rotated120())->ChangeSize(1.4f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos, -vDir.Rotated60 ())->ChangeSize(1.4f);
        });

        if(this->ReachedDeath()) this->Kill(true);
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        this->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        {
            this->SetPosition (coreVector3(0.0f, ParaLerp(1.5f, 0.6f, 0.7f, BLENDB(MIN1(fTime * 2.0f))), 0.0f) * FOREGROUND_AREA3);
            this->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

            m_fTilt = LERPS(0.0f*PI, 2.0f*PI, MAX0((fTime - 0.5f) * 2.0f));

            this->DefaultOrientate(m_fTilt);

            if(PHASE_TIME_POINT(0.5f)) this->__EnableFire(0u);
            if(PHASE_TIME_POINT(0.9f)) this->__DisableFire(true);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 62u)
    {
        if(PHASE_BEGINNING2)
        {
            this->__DisableFire(false);
            this->__EnableFire(1u);
        }

        PHASE_CONTROL_TIMER(0u, 0.8f, LERP_LINEAR)
        {
            coreVector2 vFrom, vTo;
            coreFloat fDelay;
            switch(m_aiCounter[STORM_COUNT])
            {
            default: ASSERT(false)
            case 0: vFrom = coreVector2( 0.0f, 0.7f); vTo = coreVector2( 0.0f,-1.7f); fDelay = 1.0f; break;
            case 1: vFrom = coreVector2(-0.5f, 1.7f); vTo = coreVector2(-0.5f,-1.7f); fDelay = 1.0f; break;
            case 2: vFrom = coreVector2( 0.5f, 1.7f); vTo = coreVector2( 0.5f,-1.7f); fDelay = 1.0f; break;
            case 3: vFrom = coreVector2(-1.7f, 0.5f); vTo = coreVector2( 1.7f, 0.5f); fDelay = 1.0f; break;
            case 4: vFrom = coreVector2( 1.7f,-0.5f); vTo = coreVector2(-1.7f,-0.5f); fDelay = 1.0f; break;
            case 5: vFrom = coreVector2( 0.0f,-1.7f); vTo = coreVector2( 0.0f, 1.4f); fDelay = 0.9f; break;
            }

            if(m_aiCounter[STORM_COUNT])
            {
                this->DefaultMoveLerp(vFrom, vTo, fTime);
                m_fFlap = 1.0f;
            }
            else
            {
                this->DefaultMoveLerp(vFrom, vTo, ParaLerp(0.0f, -0.05f, 1.0f, fTime));
                m_fFlap = BLENDS(MIN1(fTime * 1.0f));
            }

            const coreVector2 vDir = (vTo - vFrom).Normalized();

            this->SetDirection(coreVector3(vDir, 0.0f));

            if(PHASE_FINISHED || PHASE_TIME_AFTER(fDelay))
            {
                m_aiCounter[STORM_COUNT] += 1;

                if(m_aiCounter[STORM_COUNT] >= 6)
                {
                    PHASE_CHANGE_TO(50u)

                    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
                    {
                        this->__ChangeWingExplode(i);
                        m_aWing[i].AddStatus(ENEMY_STATUS_GHOST);
                    }

                    this->__DisableFire(true);

                    g_pSpecialEffects->MacroExplosionPhysicalColorBig(this->GetPosition(), COLOR_FIRE_ORANGE);
                }
                else
                {
                    PHASE_RESET(0u)
                }
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 30.0f, LERP_LINEAR)
        {
            if((iTick % 5u) < 2u) return;

            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy().Rotated90();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, this, vPos,  vDir)->ChangeSize(1.7f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, this, vPos, -vDir)->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        if(PHASE_BEGINNING2)
        {
            this->__DisableFire(true);

            for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
                this->__ChangeWingSpike(i);

            pMission->SetLineMode(1u);
        }

        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,-0.8f), fTime);
        });

        m_fWebReverse = MAX0(m_fWebReverse - 0.5f * TIME);

        const coreFloat   fSpeed  = LERP(1.0f, 1.3f, MIN1(m_fPhaseTime * 0.05f));
        const coreFloat   fChange = LERPS(fSpeed, -1.0f, m_fWebReverse) * TIME;
        const coreVector2 vMove   = coreVector2(0.0f, -fChange) * FOREGROUND_AREA * 0.5f;

        const coreFloat fWebLevelPrev = m_fWebLevel;
        m_fWebLevel += fChange;

        if(F_TO_UI(fWebLevelPrev) != F_TO_UI(m_fWebLevel))
        {
            constexpr coreUint8 aiPath[] = {2u, 0u, 2u, 0u, 0u, 2u, 1u, 2u, 1u, 1u, 2u, 2u, 0u, 0u, 2u, 1u, 2u, 0u, 0u, 2u, 1u, 1u, 1u, 2u, 0u, 2u, 0u, 2u, 0u};

            while(true)
            {
                const coreUintW iLineIndex = m_iWebIndex;
                if(++m_iWebIndex >= GELU_ORBS) m_iWebIndex = 0u;

                pMission->DisableOrb (m_iWebIndex, false);
                pMission->DisableLine(m_iWebIndex, false);
                pMission->DisableLine(iLineIndex,  false);

                pMission->EnableOrb (m_iWebIndex);
                pMission->EnableLine(iLineIndex);

                const coreVector2 vPos = coreVector2(I_TO_F(m_iPathSide) - 1.5f, 3.5f - FRACT(m_fWebLevel)) * FOREGROUND_AREA * 0.5f;

                pMission->GetOrb(m_iWebIndex)->SetPosition(coreVector3(vPos, 0.0f));

                if(++m_iPathIndex >= ARRAY_SIZE(aiPath) * 2u) m_iPathIndex = 0u;

                const coreUint8 iNextStep = aiPath[m_iPathIndex % ARRAY_SIZE(aiPath)];
                const coreBool  bMirror   = (m_iPathIndex < ARRAY_SIZE(aiPath));

                     if(iNextStep == (bMirror ? 0u : 1u)) m_iPathSide -= 1u;
                else if(iNextStep == (bMirror ? 1u : 0u)) m_iPathSide += 1u;
                else if(iNextStep == (2u))                break;   // #

                ASSERT(m_iPathSide < 4u)
            }
        }

        for(coreUintW i = 0u; i < GELU_ORBS; ++i)
        {
            coreObject3D* pOrb = pMission->GetOrb(i);
            if(!pOrb->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            pOrb->SetPosition(coreVector3(pOrb->GetPosition().xy() + vMove, 0.0f));
        }

        if(m_fWebLevel >= 4.0f)
        {
            static coreUint32  s_aiTarget [GAME_PLAYERS];
            static coreVector2 s_avOldMove[GAME_PLAYERS];

            if(fWebLevelPrev < 4.0f)
            {
                g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    s_aiTarget [i] = 1u;
                    s_avOldMove[i] = pPlayer->GetInput()->vMove;

                    pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE);
                });
            }

            const auto nChangeTargetFunc = [&](const coreUintW iIndex, const coreUint32 iNewTarget)
            {
                ASSERT(iNewTarget < GELU_ORBS)

                if(pMission->IsOrbEnabled(iNewTarget))
                    s_aiTarget[iIndex] = iNewTarget;
            };

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                if(!pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_MOVE)) return;

                const sGameInput* pInput = pPlayer->GetInput();

                const coreUintW iNextTarget = (s_aiTarget[i] + 1u) % GELU_ORBS;
                const coreUintW iPrevTarget = s_aiTarget[i] ? (s_aiTarget[i] - 1u) : (GELU_ORBS - 1u);

                const coreObject3D* pTarget     = pMission->GetOrb(s_aiTarget[i]);
                const coreObject3D* pNextTarget = pMission->GetOrb(iNextTarget);
                const coreObject3D* pPrevTarget = pMission->GetOrb(iPrevTarget);

                const coreVector2 vDiff1 = pTarget->GetPosition().xy() - pPlayer->GetPosition().xy();
                if(vDiff1.LengthSq() < POW2(3.0f))
                {
                    const coreBool bTabX = SIGNUM(s_avOldMove[i].x) != SIGNUM(pInput->vMove.x);
                    const coreBool bTabY = SIGNUM(s_avOldMove[i].y) != SIGNUM(pInput->vMove.y);

                    if(g_pForeground->IsVisiblePoint(pNextTarget->GetPosition().xy()))
                    {
                        const coreVector2 vNextDiff = pNextTarget->GetPosition().xy() - pTarget->GetPosition().xy();
                        if((coreVector2::Dot(vNextDiff, pInput->vMove) > 0.5f) && ((bTabX && IsHorizontal(vNextDiff)) || (bTabY && !IsHorizontal(vNextDiff))))
                        {
                            nChangeTargetFunc(i, iNextTarget);
                        }
                    }

                    if(g_pForeground->IsVisiblePoint(pPrevTarget->GetPosition().xy()))
                    {
                        const coreVector2 vPrevDiff = pPrevTarget->GetPosition().xy() - pTarget->GetPosition().xy();
                        if((coreVector2::Dot(vPrevDiff, pInput->vMove) > 0.5f) && ((bTabX && IsHorizontal(vPrevDiff)) || (bTabY && !IsHorizontal(vPrevDiff))))
                        {
                            nChangeTargetFunc(i, iPrevTarget);
                        }
                    }

                    s_avOldMove[i] = pInput->vMove;
                }

                pPlayer->SetPosition(coreVector3(pPlayer->GetPosition().xy() + vMove, 0.0f));

                const coreVector2 vDiff2 = pMission->GetOrb(s_aiTarget[i])->GetPosition().xy() - pPlayer->GetPosition().xy();
                if(!vDiff2.IsNull())
                {
                    const coreVector2 vPos = pPlayer->GetPosition().xy() + vDiff2.Normalized() * (3.0f * pPlayer->CalcMoveSpeed() * TIME * SmoothTowards(vDiff2.Length(), 3.0f));
                    pPlayer->SetPosition(coreVector3(vPos, 0.0f));
                }

                if(pPlayer->GetPosition().y < FOREGROUND_AREA.y * -0.7f)
                    m_fWebReverse = 1.0f;
            });
        }

        if(this->ReachedDeath())
        {
            PHASE_CHANGE_TO(60u)

            this->__KillFake();

            for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                pMission->DisableOrb(i, g_pForeground->IsVisiblePoint(pMission->GetOrb(i)->GetPosition().xy(), 1.3f));

            for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
                this->__ChangeWingReset(i);

            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_LINEAR)
        {
            const coreFloat   fLen   = 0.6f;
            const coreFloat   fAngle = LERPS(0.0f*PI, 2.0f*PI, STEP(0.0f, 0.4f, fTime)) + LERPS(0.0f*PI, -3.0f*PI, STEP(0.4f, 1.0f, fTime));
            const coreVector2 vDir   = coreVector2::Direction(fAngle);
            const coreVector2 vPos   = vDir * fLen;

            this->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            this->SetDirection(coreVector3(vDir * -1.0f,           0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(70u)
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 15.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy().InvertedX();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.6f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 100u)
    {
        if(m_fPush >= 3.0f)
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 101u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp     (coreVector2(0.0f,1.7f), coreVector2(0.0f,0.6f), fTime);
            this->DefaultOrientateLerp(0.0f*PI,                6.0f*PI,                fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->__EnableFire(0u);

                for(coreUintW i = 1u; i < CHOL_WINGS; ++i) m_aWing[i].SetParent(NULL);
                for(coreUintW i = 0u; i < CHOL_WINGS; ++i) m_aWing[i].SetParent(this);
                for(coreUintW i = 0u; i < CHOL_WINGS; ++i) this->__ChangeWingReset(i);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 102u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(80u)
        });
    }

    // ################################################################
    // ################################################################

    // 
    if(m_fResurrection)
    {
        m_fResurrection.Update(-1.0f);
        if(m_fResurrection <= 0.0f)
        {
            PHASE_CHANGE_TO(61u)

            this->__ResurrectFake();
        }
    }


    const coreVector2 vOffFrom  = 2.2f * FOREGROUND_AREA * coreVector2(0.0f, m_avVector[WALL_OFFSET].z);
    const coreVector2 vAreaFrom = -FOREGROUND_AREA + vOffFrom;

    const coreVector2 vPlayerAreaFrom = PLAYER_AREA_DEFAULT.xy() + vOffFrom;

    g_pPostProcessing->SetWallOffset(2u, m_avVector[WALL_OFFSET].z);

    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->SetArea(coreVector4(vPlayerAreaFrom, PLAYER_AREA_DEFAULT.zw()));
    });

    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        if(pPlayer->IsNormal())
        {
            const coreVector2 vPos = pPlayer->GetPosition().xy();

            if((vPos.y <= vPlayerAreaFrom.y + CORE_MATH_PRECISION) && m_avVector[WALL_OFFSET].z)
            {
                pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vPos);
                m_avVector[WALL_OFFSET].z = 0.0f;
            }
        }
    });

    if(m_avVector[WALL_OFFSET].z)
    {
        g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
        {
            const coreVector2 vPos = pBullet->GetPosition().xy();
            const coreVector2 vDir = pBullet->GetFlyDir();

            if((vPos.y < vAreaFrom.y) && (vDir.y < 0.0f)) pBullet->Deactivate(true);
        });
    }


    if(m_iPhase == 100u)
    {
        const coreFloat fTarget = (1.0f - m_aWing[0].GetCurHealthPct()) * 12.0f;
        const coreFloat fDiff   = fTarget - m_fPush;

        m_fPush += SIGN(fDiff) * TIME * SmoothTowards(ABS(fDiff), 0.2f);

        this->SetPosition(coreVector3(0.0f, 0.6f * FOREGROUND_AREA.y, 0.0f));
    }


    // 
    const coreVector3 vBodyDir  = this->GetDirection();
    const coreMatrix3 mBodyRota = coreMatrix4::RotationAxis(m_fTilt, vBodyDir).m123();
    const coreVector3 vBodyOri  = coreVector3::Cross(vBodyDir, coreVector3::Cross(vBodyDir, coreVector3(0.0f,0.0f,1.0f)).Normalized()) * mBodyRota;
    const coreVector3 vBodyTan  = coreVector3::Cross(vBodyOri, vBodyDir);

    // 
    const coreFloat fFinalFlap = m_fFlap + 0.05f * SIN(m_fLifeTime * 3.0f);

    // 
    const coreFloat fBottomWall = (m_avVector[WALL_OFFSET].z * 2.0f - 1.05f) * FOREGROUND_AREA.x;

    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
    {
        cLodObject& oWing = m_aWing[i];

        const coreFloat fSide = (i < 2u) ? 1.0f : -1.0f;

        const coreVector2 vBodyDirNorm = vBodyDir.xy().Normalized();

        const coreFloat fAngle1 = DEG_TO_RAD( 25.0f) * I_TO_F((i % 2u) + 1u) * fSide;
        const coreFloat fAngle2 = DEG_TO_RAD(-10.0f) * I_TO_F((i % 2u) + 1u) * fSide;
        const coreFloat fAngle3 = LERP(fAngle1, fAngle2, fFinalFlap);

        const coreVector3 vDir1 = coreVector3(MapToAxis(coreVector2::Direction(fAngle1), vBodyDirNorm), 0.0f) * mBodyRota;
        const coreVector3 vDir2 = coreVector3(MapToAxis(coreVector2::Direction(fAngle2), vBodyDirNorm), 0.0f) * mBodyRota;
        const coreVector3 vDir3 = coreVector3(MapToAxis(coreVector2::Direction(fAngle3), vBodyDirNorm), 0.0f) * mBodyRota;

        const coreVector3 vOff1 = vBodyDir *  15.0f - vDir1 *  15.0f;
        const coreVector3 vOff2 = vBodyDir * -16.0f - vDir2 * -15.0f;

        const coreVector3 vPos1 = this->GetPosition() + 1.2f * (vOff1 + vBodyTan * fSide * ((i % 2u) ? 2.0f : 1.7f));
        const coreVector3 vPos2 = this->GetPosition() + 1.2f * (vOff2 + vBodyTan * fSide * ((i % 2u) ? 5.5f : 3.0f) + vDir2 * ((i % 2u) ? -6.0f : -1.0f));
        const coreVector3 vPos3 = LERP(vPos1, vPos2, fFinalFlap);

        if(m_aiWingState[i])
        {
            m_afWingTime[i].Update(1.0f);

            const coreBool bThrow2 = ((m_aiWingState[i] >= 21u) && (m_aiWingState[i] <= 30u));

            switch(m_aiWingState[i])
            {
            // intro
            case 1u:
                {
                    // nothing
                }
                break;

            case 2u:
                {
                    const coreFloat fTime = MIN(m_afWingTime[i] * 0.5f, 1.0f);

                    const coreVector3 vStartPos = m_avVector[START_VALUES_POS + i].xyz() * coreMatrix4::RotationZ(fTime * 1.0f*PI).m123();
                    const coreVector3 vStartDir = m_avVector[START_VALUES_DIR + i].xyz();

                    const coreVector3 vNewPos = LERPH3(vStartPos, vPos3, fTime);
                    const coreVector3 vNewDir = coreVector3(MapToAxis(coreVector2::Direction(LERPH3(vStartDir.x, fAngle3, fTime)), vBodyDirNorm) * vStartDir.z, LERPH3(vStartDir.y, 0.0f, fTime)).Normalized();   // # no rota

                    oWing.SetPosition (vNewPos);
                    oWing.SetDirection(vNewDir);

                    if(m_afWingTime[i] >= 2.0f)
                    {
                        this->__ChangeWingReset(i);
                    }
                }
                break;

            // throw (1 = target player | 2 = target bottom wall)
            case 11u:
            case 21u:
                {
                    const coreVector2 vTarget = bThrow2 ? (coreVector2(((i % 2u) ? 0.75f : 0.25f) * ((i < 2u) ? 1.0f : -1.0f), -1.3f) * FOREGROUND_AREA) : this->NearestPlayerDual((i < 2u) ? 0u : 1u)->GetPosition().xy();
                    const coreVector2 vAim    = (vTarget - oWing.GetPosition().xy()).Normalized();

                    const coreVector2 vNewDir = SmoothAim(oWing.GetDirection().xy(), vAim, bThrow2 ? 1.0f : 5.0f);   // # no rota
                    const coreVector2 vNewOri = coreVector2::Direction(LERPBR(0.0f*PI, 10.0f*PI, MIN(m_afWingTime[i] * (1.0f/3.0f), 1.0f)));

                    oWing.SetDirection  (coreVector3(vNewDir, 0.0f));
                    oWing.SetOrientation(OriRoundDir(vNewOri, vNewDir));

                    if(m_afWingTime[i] >= 3.0f)
                    {
                        this->__ChangeWingInc(i);

                        g_pSpecialEffects->CreateBlowColor(oWing.GetPosition(), -oWing.GetDirection(), SPECIAL_BLOW_SMALL, COLOR_ENERGY_WHITE);

                        if(bThrow2)
                        {
                            m_aWing[i].ChangeToTop();
                        }
                    }
                }
                break;

            case 12u:
            case 22u:
                {
                    const coreVector2 vNewPos = oWing.GetPosition().xy() + oWing.GetDirection().xy() * ((120.0f + 300.0f * m_afWingTime[i]) * TIME);

                    oWing.SetPosition(coreVector3(vNewPos, 0.0f));

                    if(!g_pForeground->IsVisiblePoint(vNewPos, 1.05f))
                    {
                        this->__ChangeWingInc(i);

                        g_pSpecialEffects->CreateSplashColor(oWing.GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_WHITE);
                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

                        if(bThrow2)
                        {
                            m_aWing[i].SetParent(NULL);

                            const coreVector2 vCorrection = oWing.GetDirection().xy() * ((vNewPos.y - fBottomWall) / oWing.GetDirection().y);

                            oWing.SetPosition(coreVector3(vNewPos - vCorrection, 0.0f));
                        }
                    }
                }
                break;

            case 13u:
            case 23u:
                {
                    if(bThrow2)
                    {
                        oWing.SetPosition(coreVector3(oWing.GetPosition().x, fBottomWall, 0.0f));
                    }
                }
                break;

            // return
            case 31u:
                {
                    const coreVector2 vNewPos = oWing.GetPosition().xy() + oWing.GetDirection().xy() * (-20.0f * (1.0f - m_afWingTime[i]) * TIME);

                    oWing.SetPosition(coreVector3(vNewPos, 0.0f));

                    if(m_afWingTime[i] >= 1.0f)
                    {
                        this->__ChangeWingInc(i);

                        m_avWingReturn[i] = coreVector3(vNewPos, MapToAxisInv(oWing.GetDirection().xy(), vBodyDirNorm).Angle() + (2.0f*PI) * fSide);
                    }
                }
                break;

            case 32u:
                {
                    const coreVector3 vNewPos = LERPH3(coreVector3(m_avWingReturn[i].xy(), 0.0f), vPos3, MIN(m_afWingTime[i], 1.0f));
                    const coreVector3 vNewDir = coreVector3(MapToAxis(coreVector2::Direction(LERPH3(m_avWingReturn[i].z, fAngle3, MIN(m_afWingTime[i], 1.0f))), vBodyDirNorm), 0.0f);   // # no rota

                    oWing.SetPosition (vNewPos);
                    oWing.SetDirection(vNewDir);

                    if(m_afWingTime[i] >= 1.0f)
                    {
                        this->__ChangeWingReset(i);
                    }
                }
                break;

            // pull
            case 41u:
                {
                    const coreVector2 vNewPos = oWing.GetPosition ().xy() + coreVector2(0.0f,80.0f) * TIME;
                    const coreVector2 vNewDir = oWing.GetDirection().xy() * coreMatrix3::Rotation(10.0f * TIME).m12();   // # no rota

                    oWing.SetPosition (coreVector3(vNewPos, 0.0f));
                    oWing.SetDirection(coreVector3(vNewDir, 0.0f));

                    if(vNewPos.y >= FOREGROUND_AREA.y * 1.5f)
                    {
                        this->__ChangeWingInc(i);

                        m_aWing[i].ChangeToNormal();
                    }
                }
                break;

            case 42u:
                {
                    // nothing (also here during push phase)
                }
                break;

            // spike
            case 51u:
                {
                    const coreVector3 vNewPos = LERPH3(vPos3, coreVector3((I_TO_F(i % 2u) + 0.5f) * 0.5f * fSide, -0.7f, 0.0f) * FOREGROUND_AREA3, MIN(m_afWingTime[i], 1.0f));
                    const coreVector3 vNewDir = coreVector3(MapToAxis(coreVector2::Direction(LERPH3(fAngle3, 0.0f*PI, MIN(m_afWingTime[i], 1.0f))), vBodyDirNorm), 0.0f);   // # no rota

                    oWing.SetPosition (vNewPos);
                    oWing.SetDirection(vNewDir);

                    if(m_afWingTime[i] >= 1.0f)
                    {
                        this->__ChangeWingInc(i);
                    }
                }
                break;

            case 52u:
                {
                    const coreFloat fBlend = CLAMP01((m_afWingTime[i] - 5.0f) * 0.1f);

                    const coreVector3 vNewPos = coreVector3(oWing.GetPosition().x, (-0.7f + (0.1f * fBlend) * SIN(m_afWingTime[i] + I_TO_F(i))) * FOREGROUND_AREA.y, oWing.GetPosition().z);
                    const coreVector2 vNewOri = coreVector2::Direction((2.0f*PI) * m_afWingTime[i]);

                    oWing.SetPosition   (vNewPos);
                    oWing.SetOrientation(OriRoundDir(vNewOri, oWing.GetDirection().xy()));
                }
                break;

            // explode
            case 61u:
                {
                    const coreFloat   fTime  = MIN(m_afWingTime[i], 1.0f);
                    const coreVector3 vForce = coreVector3(1.5f * fSide, (i % 2u) ? -0.3f : -5.0f, 0.0f).Normalized() * FOREGROUND_AREA3 * 3.0f;

                    const coreVector3 vNewPos = LERP(vPos3, vPos3 + vForce, fTime);
                    const coreVector3 vNewDir = coreVector3(MapToAxis(coreVector2::Direction(fAngle3 + (4.0f*PI) * fSide * fTime), vBodyDirNorm), 0.0f);   // # no rota

                    oWing.SetPosition (vNewPos);
                    oWing.SetDirection(vNewDir);

                    if(m_afWingTime[i] >= 1.0f)
                    {
                        this->__ChangeWingInc(i);
                    }
                }
                break;

            case 62u:
                {
                    // nothing
                }
                break;

            default:
                ASSERT(false)
                break;
            }
        }
        else
        {
            oWing.SetPosition   (vPos3);
            oWing.SetDirection  (vDir3);
            oWing.SetOrientation(vBodyOri);
        }


        if(m_iPhase == 100u)
        {
            if(m_fPush < 1.0f)
            {
                const coreFloat fLocal = m_fPush - 0.0f;

                coreVector2       vPos = coreVector2(1.07f * (TriangleWave(fLocal * 2.25f) * 2.0f - 1.0f) * ((i % 2u) ? 1.0f : -1.0f), (I_TO_F(i) - 1.5f) * 0.55f);
                const coreVector2 vDir = coreVector2(0.0f, (i < 2u) ? 1.0f : -1.0f);

                vPos = LERPB(vPos * coreVector2(1.5f,1.0f), vPos, MIN1(m_fPhaseTime));

                oWing.SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                oWing.SetDirection(coreVector3(vDir,                   0.0f));
            }
            else if(m_fPush < 2.0f)
            {
                const coreFloat fLocal = m_fPush - 1.0f;

                const coreFloat   fAngle = LERP(0.0f, (i == 0u || i == 3u) ? (-2.5f*PI) : (3.5f*PI), fLocal);
                const coreVector2 vRota  = coreVector2::Direction(fAngle);

                const coreVector2 vPos = vRota * ((I_TO_F(i) - 1.5f) * 0.55f);
                const coreVector2 vDir = vRota * ((i < 2u) ? -1.0f : 1.0f);

                oWing.SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                oWing.SetDirection(coreVector3(vDir.InvertedY(),       0.0f));
            }
            else
            {
                const coreFloat fLocal = MIN1(m_fPush - 2.0f);

                const coreFloat   fFrom  = ((i == 1u || i == 3u) ? (-7.5f*PI) : (8.5f*PI)) * ((i < 2u) ? 1.0f : -1.0f);
                const coreFloat   fTo    = vDir3.xy().Angle();
                const coreFloat   fAngle = LERP(fFrom, fTo, fLocal);
                const coreVector2 vRota  = coreVector2::Direction(fAngle);

                coreVector2       vPos = coreVector2((I_TO_F(i % 2u) + 0.5f) * -0.55f * fSide * (1.0f + 0.5f * SIN(fLocal * (1.0f*PI))), 0.0f);
                const coreVector2 vDir = vRota;

                vPos = LERP(vPos * FOREGROUND_AREA + vDir * 4.0f, vPos3.xy(), fLocal);

                oWing.SetPosition (coreVector3(vPos, 0.0f));
                oWing.SetDirection(coreVector3(vDir, 0.0f));
            }

            if(m_fPush < 2.0f) oWing.SetPosition(oWing.GetPosition() + oWing.GetDirection() * 4.0f);
        }
    }

    if(m_iPhase == 100u)
    {
        this->SetPosition(coreVector3(HIDDEN_POS, 0.0f));

        if(m_fPush < 3.0f)
        {
            PHASE_CONTROL_TICKER(3u, 0u, 30.0f, LERP_LINEAR)
            {
                if((iTick % 10u) < 6u) return;

                for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
                {
                    const coreVector2 vPos = m_aWing[i].GetPosition ().xy();
                    const coreVector2 vDir = m_aWing[i].GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.6f);
                }
            });
        }
    }

    // 
    if(m_Fire.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        if(m_bFireActive)
        {
            m_Fire.SetAlpha(MIN1(m_Fire.GetAlpha() + 3.0f * TIME));
        }
        else
        {
            m_Fire.SetAlpha(MAX0(m_Fire.GetAlpha() - 5.0f * TIME));
        }

        if(m_iFireType == 0u)
        {
            m_Fire.SetPosition (this->GetPosition());
            m_Fire.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.16f));
        }
        else if(m_iFireType == 1u)
        {
            m_Fire.SetPosition (this->GetPosition() + this->GetDirection() * -19.0f);
            m_Fire.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.56f));
        }
        else ASSERT(false)

        m_Fire.SetDirection  (this->GetDirection());
        m_Fire.SetOrientation(vBodyOri);
        m_Fire.Move();

        if(!m_Fire.GetAlpha()) this->__DisableFire(false);
    }


    for(coreUintW i = 0u; i < GELU_WAYS; ++i)
    {
        coreObject3D* pWay = pMission->GetWay(i);
        if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !pMission->IsWayActive(i)) continue;

        const coreVector2 vMove   = coreVector2(1.0f,0.0f) * (25.0f * TIME) * (((i % 12u) < 6u) ? 1.0f : -1.0f);
        const coreVector2 vOldPos = pWay->GetPosition().xy();
        const coreVector2 vNewPos = vOldPos + ((i < 12u) ? vMove : vMove.yx());

        pWay->SetPosition(coreVector3(vNewPos, 0.0f));

        if(g_pForeground->IsVisiblePoint(vOldPos, 1.3f) && !g_pForeground->IsVisiblePoint(vNewPos, 1.3f))
        {
            pMission->DisableWay(i, false);
        }
    }


    coreUintW iIndex = 0u;
    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([&](cConeBullet* OUTPUT pBullet)
    {
        const coreUintW iWall = pBullet->GetDamage() - 1;
        ASSERT(iWall == 2u)

        const coreFloat fOffset = 1.07f - 2.2f * m_avVector[WALL_OFFSET].z;
        const coreFloat fSide   = (I_TO_F(iIndex) - 9.5f) / -10.0f;

        const coreVector2 vPos = -coreVector2(fSide, fOffset) * FOREGROUND_AREA;
        const coreVector2 vDir = -coreVector2(0.0f, -1.0f);

        pBullet->SetPosition(coreVector3(vPos, 0.0f));
        pBullet->SetFlyDir  (vDir);

        if(pBullet->GetFlyTime() == 0.0f) g_pSpecialEffects->CreateSplashColor(pBullet->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_ORANGE);

        ++iIndex;
    });
}


// ****************************************************************
// 
void cCholBoss::__EnableFire(const coreUint8 iType)
{
    WARN_IF(m_Fire.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Fire.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Fire);

    // 
    m_iFireType   = iType;
    m_bFireActive = true;

    // 
    if(iType == 0u)
    {
        m_Fire.DefineModel(m_apFireModel[0]);
        m_Fire.SetSize    (this->GetSize());
        m_Fire.SetTexSize (coreVector2(1.0f,0.3f) * 1.5f);
    }
    else if(iType == 1u)
    {
        m_Fire.DefineModel(m_apFireModel[1]);
        m_Fire.SetSize    (this->GetSize() * coreVector3(15.0f,10.0f,15.0f));
        m_Fire.SetTexSize (coreVector2(1.5f,0.2f) * 2.5f);
    }
    else ASSERT(false)

    // 
    m_Fire.SetAlpha(0.0f);

    // 
    if(iType == 0u)
    {
        for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
        {
            g_pSpecialEffects->CreateBlowColor(m_aWing[i].GetPosition() + m_aWing[i].GetDirection() * 5.0f, -m_aWing[i].GetDirection(), SPECIAL_BLOW_BIG, COLOR_ENERGY_ORANGE);
        }
    }
}


// ****************************************************************
// 
void cCholBoss::__DisableFire(const coreBool bAnimated)
{
    if(!m_Fire.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_bFireActive = false;

    if(!bAnimated)
    {
        // 
        m_Fire.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_Fire);
    }
}


// ****************************************************************
// 
void cCholBoss::__ResurrectFake()
{
    // 
    this->SetMaxHealth(1200);
    this->SetCurHealth(1200);
    this->RefreshColorAll(1.0f);

    // 
    m_fResurrection = 0.0f;

    // 
    g_pGame->GetInterface()->ShowBoss(this, true);
}


// ****************************************************************
// 
void cCholBoss::__KillFake()
{
    // 
    const coreFloat  fTimeShifted = g_pGame->GetTimeTable()->GetTimeShiftedSegmentSafe();
    const coreUint32 iBonus       = cGame::CalcBonusTime(fTimeShifted);

    // 
    g_pGame->GetInterface()->ShowScore(iBonus, 1u, MEDAL_TYPE_BOSS);

    // 
    m_fResurrection = INTERFACE_BANNER_DURATION_SCORE + 1.0f;
    m_aiCounter[DEATH_COUNT] += 1u;

    // 
    g_pSpecialEffects->MacroDestructionDark(this);

    // 
    this->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
}