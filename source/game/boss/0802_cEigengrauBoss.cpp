///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// for top enemies, on height 60, the maximum side can be 30, otherwise distance between player, enemies and bullets is hard to understand, the minimum side can be 10, otherwise player and bullets disappear inside enemy
// erste gegner fungieren als tutorial für geänderte rotation, wichtig, weil der kampf selbst sehr intensiv ist, und plötzliche gegner nicht als solche wahrgenommen werden könnten
// sub-phasen mit bullet-rotation sollten nicht hintereinander sein, da sie sich sonst gegenseitig beeinflussen
// random spread und random lines sollten keinen gezielten angriff haben, weil der zusammen mit der bullet-drehung nicht mehr richtig zielt
// random lines haben in der mitte eine kontinuierliche linie, mit geringem abstand zur ersten äußeren linie, um leichten safe-spot in mitte zu vermeiden, random spread (welches früher kommt) könnte uu. random in die mitte schießen (vom algorithmus her)
// kreuze mit ungerader anzahl an geschossen schaut besser aus, weil die mitte sich nicht komisch überlagert
// offset beim direkten angriff sollte nicht zu hoch sein, weil der spieler sonst einfach ruhig stehenbleiben kann und nicht getroffen wird
// rotations sub-phase (random spread, random lines, center wave, side wave) sollten möglichst abwechseln, besonders random spread und random lines sollten sich unterscheiden
// gegner-spawning sollte möglichst gut verteilt sein
// kamera-bewegung hilft beim ausweichen von geschossen, weil man die tiefe und perspektive besser erkennen kann
// tunnel-objekt hat die perspektive gut hervorgehoben, hat aber nicht zu der offenen weite gepasst, die das entfernen der seiten-teile ermöglicht
// wenn möglich, sollten geschosse nicht immer direkt vom boss spawnen, weil sie den boss zu sehr verdecken, und die perspektive verzerren (wodurch das "weite" feeling schlechter wird, und ausweichen schwerer wird)
// golden angle muster war schön, funktioniert aber nur wenn es von mitte ausgeht und gerade fliegt, und hatte somit wenig gameplay-nutzen
// kreuze mit 4 linien waren zu overwhelming
// kreuze mit gegenseitig bewegenden linien hatten nicht die erwartende tiefe, weil man einfacher ausweichen konnte als bei normalen kreuzen (sie funktionieren auch nur wenn sie aus mitte kommen, wegen rotation)
// wenn möglich, geschosse sollten bis zum rand gehn, bei dem der spieler sich noch positionieren kann
// there were some experiments with restricting game object visibility to the regular game-area, but it doesn't come close to the intensified feeling of using the whole screen
// attacks which move sideways during flight are too hard to predict, the rotation is easy enough though, maybe because of perspective influences
// creating a sinus-tunnel (e.g. left-right) was not feasible, if the wave was slow it was boring, if the wave was fast the player-ship disappeared behind the curve which was confusing
// the classical rotating hole attack was too similar to the rotating lines, but the lines are more versatile
// gegner die spawnen brauchen eine incentive um angegriffen zu werden (entweder sind sie unausweichlich, oder greifen selbst an)
// neither repair-enemy nor continue is properly handled, both situations are prevented by making players invincible
// TODO 1: hard: additional enemies and side-turning
// TODO 1: improve player-arrow appearance when looking into/outof z-axis
// TODO 1: background für time-score-banner hängt in leere bei vertikaler resolution
// TODO 1: spieler kann bei wänden hinaushängen, solange sie noch sichtbar sind (vor allem in coop) -> restrict with area ?
// TODO 1: MAIN: sound
// TODO 1: ACHIEVEMENT: name (), description (), 

// TODO 1: sound when fragment-board splits up, sound when bullet flies past player

// TODO 1: post-processing fehlt und farben sind anders, fix glow and distortion on extended viewport

// TODO 1: alle layer sollten getroffen werden können (custom-enemy geht aber nicht, wegen render-reihenfolge), adjust size+dir+ori depending on current layer
// TODO 1: boss sollte vielleicht eigenen shader haben, zum rand hin weniger transparenz (spherisch ?) statt ganz flach

// TODO 1: vielleicht doch nicht spieler unsterblich machen, repair enemy soll timer haben, und bei continue spielt man von der aktuellen stelle weiter mit eigenem einflug

// (TODO 1: sinus-line gegner sieht man schlecht von oben kommen (vielleicht beim ersten gegner ein loch lassen um (überraschendes) erstes ausweichen zu erleichtern))
// (TODO 1: handle farb-änderung von adds)


// ****************************************************************
// counter identifier
#define LINE_ROTA (0u)


// ****************************************************************
// vector identifier
#define ROTA_VALUE (0u)
#define PLAYER_POS (1u)   // # uses 1u - 2u
#define HEART_BEAT (3u)


// ****************************************************************
// constructor
cEigengrauBoss::cEigengrauBoss()noexcept
: m_afParasiteAlpha   {}
, m_afFollowerAlpha   {}
, m_fAnimation        (0.0f)
, m_fRotation         (0.0f)
, m_fRotationSpeed    (0.0f)
, m_fRangeAnim        (0.0f)
, m_fRangeAnimPlayer  (0.0f)
, m_fRangeSpeed       (1.0f)
, m_fRangeSpeedPlayer (1.0f)
, m_fLightningDelay   (0.0f)
, m_fRotaSpeed        (0.0f)
, m_fAttackSpeed      (0.0f)
, m_iBurstTick        (0u)
{
    // load models
    this->DefineModelHigh("object_cube_rota.md3");
    this->DefineModelLow ("object_cube_rota.md3");
    this->DefineVolume   ("object_cube_volume.md3");

    // set object properties
    this->SetPosition(coreVector3(0.0f,0.0f,EIGENGRAU_DEPTH));
    this->SetSize    (coreVector3(0.0f,0.0f,0.0f));

    // configure the boss
    this->Configure(12000, 0u, coreVector3(0.0f,0.0f,0.0f), false, true);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    for(coreUintW i = 0u; i < EIGENGRAU_LAYERS; ++i)
    {
        // 
        m_aLayer[i].DefineModelHigh("object_cube_rota.md3");
        m_aLayer[i].DefineModelLow ("object_cube_rota.md3");
        m_aLayer[i].DefineVolume   ("object_cube_volume.md3");
        m_aLayer[i].DefineTexture  (0u, "effect_energy.png");
        m_aLayer[i].DefineProgram  ("effect_energy_spheric_program");
        m_aLayer[i].SetSize        (coreVector3(0.0f,0.0f,0.0f));
        m_aLayer[i].SetColor3      (COLOR_ENERGY_WHITE * (0.1f + 0.1f * I_TO_F(EIGENGRAU_LAYERS - i)));
    }

    // 
    m_Range.DefineModel  ("object_cube_top.md3");
    m_Range.DefineTexture(0u, "effect_energy.png");
    m_Range.DefineProgram("effect_energy_flat_invert_program");
    m_Range.SetColor3    (COLOR_PLAYER_BLUE);
    m_Range.SetAlpha     (0.0f);
    m_Range.SetTexSize   (coreVector2(0.1f,0.1f));

    // 
    m_RangePlayer.DefineModel  ("object_penta_top.md3");
    m_RangePlayer.DefineTexture(0u, "effect_energy.png");
    m_RangePlayer.DefineProgram("effect_energy_flat_invert_program");
    m_RangePlayer.SetColor3    (COLOR_PLAYER_GREEN);
    m_RangePlayer.SetAlpha     (0.0f);
    m_RangePlayer.SetTexSize   (coreVector2(0.1f,0.1f));
    m_RangePlayer.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    for(coreUintW i = 0u; i < BOSS_PLAYERS; ++i)
    {
        m_aArrow[i].DefineModel  ("object_arrow.md3");
        m_aArrow[i].DefineTexture(0u, "effect_energy.png");
        m_aArrow[i].DefineProgram("effect_energy_flat_invert_program");
        m_aArrow[i].SetSize      (coreVector3(1.0f,1.0f,1.0f) * 2.2f);
        m_aArrow[i].SetTexSize   (coreVector2(1.0f,1.0f) * 0.4f);
        m_aArrow[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    for(coreUintW i = 0u; i < EIGENGRAU_PARASITES; ++i)
    {
        // 
        m_aParasite[i].DefineModelHigh     ("object_tetra_top.md3");
        m_aParasite[i].DefineModelLow      ("object_tetra_top.md3");
        m_aParasite[i].DefineVolume        ("object_tetra_volume.md3");
        m_aParasite[i].DefineTexture       (0u, "effect_energy.png");
        m_aParasite[i].DefineProgram       ("effect_energy_blink_flat_program");
        m_aParasite[i].SetSize             (coreVector3(coreVector2(1.0f,1.0f) * 5.0f, 0.01f));
        m_aParasite[i].SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.1f);
        m_aParasite[i].SetTexSize          (coreVector2(1.0f,1.0f) * 0.7f);
        m_aParasite[i].Configure           (4, 0u, COLOR_ENERGY_WHITE * 0.4f);
        m_aParasite[i].AddStatus           (ENEMY_STATUS_ENERGY | ENEMY_STATUS_TOP | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_LIGHT);
    }

    for(coreUintW i = 0u; i < EIGENGRAU_FOLLOWERS; ++i)
    {
        // 
        m_aFollower[i].DefineModelHigh("object_cube_top.md3");
        m_aFollower[i].DefineModelLow ("object_cube_top.md3");
        m_aFollower[i].DefineVolume   ("object_cube_volume.md3");
        m_aFollower[i].DefineTexture  (0u, "effect_energy.png");
        m_aFollower[i].DefineProgram  ("effect_energy_blink_flat_program");
        m_aFollower[i].SetSize        (coreVector3(coreVector2(1.0f,1.0f) * 5.5f, 0.01f));
        m_aFollower[i].SetTexSize     (coreVector2(1.0f,1.0f) * 0.7f);
        m_aFollower[i].Configure      (30, 0u, COLOR_ENERGY_WHITE * 0.4f);
        m_aFollower[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_TOP | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_LIGHT | ENEMY_STATUS_SECRET);
    }

    // 
    m_Lightning.DefineProgram("menu_color_program");
    m_Lightning.SetPosition  (coreVector2(0.0f,0.0f));
    m_Lightning.SetSize      (coreVector2(1.0f,1.0f));
    m_Lightning.SetAlpha     (0.0f);
    m_Lightning.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_PlayerPath.Reserve(6u);
    m_PlayerPath.AddNode(1.8f * coreVector2(-0.5f,  0.0f),  coreVector2( 1.0f, 0.0f));
    m_PlayerPath.AddNode(4.5f * coreVector2( 0.0f,  0.0f),  coreVector2( 1.0f, 0.0f));
    m_PlayerPath.AddNode(4.5f * coreVector2( 0.5f,  0.25f), coreVector2( 0.0f, 1.0f));
    m_PlayerPath.AddNode(4.5f * coreVector2( 0.25f, 0.5f),  coreVector2(-1.0f, 0.0f));
    m_PlayerPath.AddNode(4.5f * coreVector2( 0.0f,  0.0f),  coreVector2( 0.0f,-1.0f));
    m_PlayerPath.AddNode(5.0f * coreVector2( 0.0f, -0.5f),  coreVector2( 0.0f,-1.0f));   // # height affects collision
    m_PlayerPath.Refine();

    // 
    m_pHeartSound = Core::Manager::Resource->Get<coreSound>("effect_heart.wav");
}


// ****************************************************************
// 
void cEigengrauBoss::__ResurrectOwn()
{
    // 
    this->SetAlpha(0.0f);

    // 
    g_bTiltMode = true;

    // 
    m_fAttackSpeed = g_pGame->IsEasy() ? 1.1f : 1.5f;

    // 
    this->_ResurrectBoss();
}


// ****************************************************************
// 
void cEigengrauBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < EIGENGRAU_PARASITES; ++i)
        m_aParasite[i].Kill(bAnimated);

    // 
    for(coreUintW i = 0u; i < EIGENGRAU_FOLLOWERS; ++i)
        m_aFollower[i].Kill(bAnimated);

    // 
    g_bTiltMode  = false;
    g_fShiftMode = 0.0f;

    // # no postprocessing reset, for credits
}


// ****************************************************************
// 
void cEigengrauBoss::__RenderOwnUnder()
{
    for(coreUintW i = EIGENGRAU_LAYERS; i--; )
    {
        if(m_aLayer[i].IsEnabled(CORE_OBJECT_ENABLE_RENDER))
        {
            DEPTH_PUSH

            m_aLayer[i].Render();
            g_pOutline->GetStyle(OUTLINE_STYLE_LIGHT_THICK)->ApplyObject(&m_aLayer[i]);
        }
    }

    if(m_Range.IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        DEPTH_PUSH

        // 
        m_Range.Render();
        g_pOutline->GetStyle(m_bActive ? OUTLINE_STYLE_FLAT_THICK : OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Range);
    }

    if(m_RangePlayer.IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        DEPTH_PUSH

        // 
        m_RangePlayer.Render();
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_RangePlayer);
    }
}


// ****************************************************************
// 
void cEigengrauBoss::__RenderOwnTop()
{
    DEPTH_PUSH

    // 
    for(coreUintW i = 0u; i < BOSS_PLAYERS; ++i) m_aArrow[i].Render();
    for(coreUintW i = 0u; i < BOSS_PLAYERS; ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aArrow[i]);

    if(m_Lightning.IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        glDisable(GL_DEPTH_TEST);
        {
            // 
            m_Lightning.Render();
        }
        glEnable(GL_DEPTH_TEST);
    }
}


// ****************************************************************
// 
void cEigengrauBoss::__MoveOwn()
{
    cDarkBackground* pBackground = d_cast<cDarkBackground*>(g_pEnvironment->GetBackground());

    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(1.0f, 100.0f);

    const coreFloat fCurHealthPct = this->GetCurHealthPct();

    if(fCurHealthPct > 0.0f)
    {
        this->SetSize(coreVector3(1.0f,1.0f,1.0f) * LERP(150.0f, 30.0f, fCurHealthPct));

        g_pSpecialEffects->CreateGust(1.0f - fCurHealthPct, 0.0f);

        g_pGame->SetMusicVolume(STEP(0.01f, 0.06f, fCurHealthPct));
    }

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        if(PHASE_BEGINNING2)
        {
            g_pEnvironment->SetTargetSpeed(0.0f, 1.0f);

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
                m_avVector[PLAYER_POS + i].xy(pPlayer->GetPosition().xy());
            });
        }

        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            const coreVector2 vTarget = m_PlayerPath.CalcPosition(0.0f);

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreFloat   fSide = g_pGame->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;
                const coreVector2 vPos  = LERP(m_avVector[PLAYER_POS + i].xy(), coreVector2(fSide, (vTarget.x - 1.0f) * FOREGROUND_AREA.y), fTime);

                pPlayer->SetPosition(coreVector3(vPos, 0.0f));
            });

            g_fShiftMode = fTime;

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        if(PHASE_BEGINNING2)
        {
            pBackground->Dissolve();
        }

        PHASE_CONTROL_TIMER(0u, 0.2f, LERP_LINEAR)
        {
            const coreVector4 vPlayerArea = PLAYER_AREA_DEFAULT + coreVector4(-FOREGROUND_AREA, FOREGROUND_AREA) * fTime;

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                if(PHASE_TIME_POINT(0.2f)) g_pSpecialEffects->PlaySound(pPlayer->GetPosition(), 1.0f, 1.0f, SOUND_SHIP_FLY);
                
                coreVector2 vPos, vDir;
                m_PlayerPath.CalcPosDirLerp(BLENDS(fTime), &vPos, &vDir);                       

                const coreFloat   fSide   = g_pGame->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;
                const coreVector2 vHeight = coreVector2(-1.0f, BLENDS(STEP(0.5f, 1.0f, fTime)) * 2.5f);
                const coreVector2 vOri    = coreVector2::Direction(BLENDS(STEP(0.4f, 0.8f, fTime)) * (2.0f*PI));

                pPlayer->SetPosition   (coreVector3(fSide, (vPos + vHeight) * FOREGROUND_AREA));
                pPlayer->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
                pPlayer->SetOrientation(coreVector3(vOri.x, 0.0f, vOri.y));

                pPlayer->SetArea     (vPlayerArea);
                pPlayer->SetMoveSpeed(1.5f);
                pPlayer->SetTilt     ((1.5f*PI) - vDir.Angle());
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);
                    pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_TURN);
                });

                g_pGame->GetInterface()->SetVisible(true);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f*2.0f, LERP_LINEAR)
        {
            /*for(coreUintW i = 0u; i < POST_WALLS; ++i)
            {
                const coreVector2 vResolution = Core::System->GetResolution();
                const coreVector2 vSize       = coreVector2(0.0f, ((vResolution - g_vGameResolution) / vResolution.yx()).Max() * 0.5f);

                g_pPostProcessing->SetWallOffset(i, -vSize.Max() * BLENDH3(fTime));
            }*/

            g_pPostProcessing->SetFrameValue(fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        /*
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < 4u; ++i)
                this->__ResurrectFollower(i);
        }

        if(std::all_of(m_aFollower, m_aFollower + 4u, [](const cCustomEnemy& oFollower) {return oFollower.HasStatus(ENEMY_STATUS_DEAD);}))
        {
            PHASE_CHANGE_INC
        }
         */
        PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 4u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 5u)
    {
        if(PHASE_BEGINNING2)
        {
            m_fRotation      = 0.0f;
            m_fRotationSpeed = 0.0f;

            this->RemoveStatus(ENEMY_STATUS_GHOST);
        }

        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            this->SetAlpha(BLENDH3(MIN1(fTime * 2.0f)));

            g_pPostProcessing->SetFrameValue(1.0f + CLAMP01(fTime * 2.0f - 1.0f));
            
            for(coreUintW i = 0u; i < POST_WALLS; ++i)
            {
                const coreVector2 vResolution = Core::System->GetResolution();
                const coreVector2 vSize       = coreVector2(0.0f, ((vResolution - g_vGameResolution) / vResolution.yx()).Max() * 0.5f);

                g_pPostProcessing->SetWallOffset(i, -vSize.Max() * BLENDBR(fTime));
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->_StartBoss();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 6u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.2f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 7u)
    {
        PHASE_CONTROL_TICKER(0u, 1u, 1.0f, LERP_LINEAR)
        {
            this->__AddBulletBurst();

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 8u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(10u)
            this->__PhaseChange();
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        // rotating lines
        PHASE_CONTROL_TICKER(0u, 0u, 1.0f * m_fAttackSpeed, LERP_LINEAR)
        {
            const coreVector2 vPos = this->NearestPlayerDual(iTick % 2u)->GetPosition().xy();
            const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);

            const coreInt32 iDamage = (iTick % 2u) ? 6 : 7;

            this->__AddBulletLine(iDamage, 5.0f, vPos, vDir);
            this->__AddBulletLine(iDamage, 5.0f, vPos, vDir.Rotated90());

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        m_fRotaSpeed = 1.5f * (1.0f - STEP(0.8f, 0.9f, fCurHealthPct));

        if(fCurHealthPct <= 0.8f)
        {
            PHASE_CHANGE_INC
            this->__PhaseChange();
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(20u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        if(PHASE_BEGINNING2)
        {
            //for(coreUintW i = 4u; i < 20u; ++i)
            //    this->__ResurrectFollower(i);
        }

        // direct attack
        PHASE_CONTROL_TICKER(0u, 0u, 15.0f * m_fAttackSpeed, LERP_LINEAR)
        {
            const coreVector3 vOffset = coreVector3(coreVector2::Direction(I_TO_F(iTick) * GA) * 0.01f, 0.0f);

            const coreVector3 vPos = this->GetPosition();
            const coreVector3 vDir = ((this->NearestPlayerDual((iTick / 10u) % 2u)->GetPosition() - this->GetPosition()).Normalized() + vOffset).Normalized();

            this->__AddBullet(5, 5.0f, vPos, vDir);
            
            
            const coreVector2 vRota  = coreVector2::Direction(I_TO_F(iTick) *  0.12f) * 50.0f;
            const coreVector2 vRota2 = coreVector2::Direction(I_TO_F(iTick) * -0.08f) * 100.0f;
            const coreVector2 vRota3 = coreVector2::Direction(I_TO_F(iTick) *  0.04f) * 150.0f;
            if(fCurHealthPct <= 0.74f) this->__AddBullet(5, 5.0f, vPos, ((coreVector3( vRota,             0.0f) - this->GetPosition()).Normalized() + vOffset).Normalized());
            if(fCurHealthPct <= 0.74f) this->__AddBullet(5, 5.0f, vPos, ((coreVector3(-vRota,             0.0f) - this->GetPosition()).Normalized() + vOffset).Normalized());
            if(fCurHealthPct <= 0.77f) this->__AddBullet(5, 5.0f, vPos, ((coreVector3( vRota2, 0.0f) - this->GetPosition()).Normalized() + vOffset).Normalized());
            if(fCurHealthPct <= 0.77f) this->__AddBullet(5, 5.0f, vPos, ((coreVector3(-vRota2, 0.0f) - this->GetPosition()).Normalized() + vOffset).Normalized());
            this->__AddBullet(5, 5.0f, vPos, ((coreVector3( vRota3, 0.0f) - this->GetPosition()).Normalized() + vOffset).Normalized());
            this->__AddBullet(5, 5.0f, vPos, ((coreVector3(-vRota3, 0.0f) - this->GetPosition()).Normalized() + vOffset).Normalized());
            
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        if(fCurHealthPct <= 0.7f)
        {
            PHASE_CHANGE_INC
            this->__PhaseChange();
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(30u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        // random spread
        PHASE_CONTROL_TICKER(0u, 0u, 3.0f * m_fAttackSpeed, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < 24u; ++i)
            {
                const coreVector2 vBase = coreVector2((I_TO_F(i) - 11.5f) * 1.5f, 14.5f * SIN(I_TO_F(i) + I_TO_F(iTick) * 0.4f));

                const coreVector3 vPos = this->GetPosition() + coreVector3(vBase * 5.0f, 0.0f);
                const coreVector3 vDir = coreVector3(0.0f,0.0f,1.0f);

                this->__AddBullet(6, 5.0f, vPos, vDir);
            }

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        m_fRotaSpeed = 1.0f * (1.0f - STEP(0.6f, 0.69f, fCurHealthPct));

        if(fCurHealthPct <= 0.6f)
        {
            PHASE_CHANGE_INC
            this->__PhaseChange();
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        // center wave
        PHASE_CONTROL_TICKER(0u, 0u, 12.0f * m_fAttackSpeed, LERP_LINEAR)
        {
            m_avVector[ROTA_VALUE].x += 0.09f * LERP(2.0f, 0.0f, STEP(0.5f, 0.6f, fCurHealthPct));

            const coreVector2 vPos = coreVector2(0.0f,0.0f);
            const coreVector2 vDir = coreVector2::Direction(m_avVector[ROTA_VALUE].x);

            this->__AddBulletLine(5, 5.0f, vPos, vDir);

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        if(fCurHealthPct <= 0.5f)
        {
            PHASE_CHANGE_INC
            this->__PhaseChange();
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(50u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        if(PHASE_BEGINNING2)
        {
            //for(coreUintW i = 20u; i < 36u; ++i)
            //    this->__ResurrectFollower(i);
        }

        // block attack
        if(PHASE_MAINTIME_AFTER(1.0f))
        {
            const coreFloat fSpeed = LERP(6.0f, 3.0f, STEP(0.4f, 0.5f, fCurHealthPct));

            PHASE_CONTROL_TICKER(0u, 0u, fSpeed * m_fAttackSpeed, LERP_LINEAR)
            {
                constexpr coreUintW aiTile[] = {9u, 2u, 15u, 8u, 6u, 13u, 0u, 7u, 10u, 4u, 3u, 14u, 5u, 11u, 12u, 1u};

                const coreBool   bAimed   = !(iTick % 4u);
                const coreUint16 iNewTick = iTick - (iTick / 4u);

                const coreUintW iSelect = aiTile[iNewTick % ARRAY_SIZE(aiTile)];

                coreVector2 vOffset;
                coreVector3 vTarget;
                if(bAimed)
                {
                    vOffset = coreVector2(0.0f,0.0f);
                    vTarget = coreVector3(this->NearestPlayerDual((iTick / 4u) % 2u)->GetPosition().xy(), this->GetPosition().z);
                }
                else
                {
                    vOffset = coreVector2(I_TO_F(iSelect % 4u) - 1.5f, I_TO_F(iSelect / 4u) - 1.5f) * 6.0f;
                    vTarget = this->GetPosition();
                }

                const coreVector2 vRota = coreVector2::Direction(I_TO_F(iTick) * 0.09f);

                for(coreUintW i = 0u; i < 5u; ++i)
                {
                    for(coreUintW j = 0u; j < 5u; ++j)
                    {
                        if(((i == 0u) || (i == 4u)) && ((j == 0u) || (j == 4u))) continue;

                        const coreVector2 vBase = coreVector2(I_TO_F(i) - 2.0f, I_TO_F(j) - 2.0f);

                        const coreVector3 vPos = vTarget + coreVector3(MapToAxis(vBase + vOffset, vRota) * 9.0f, 0.0f);
                        const coreVector3 vDir = coreVector3(0.0f,0.0f,1.0f);

                        this->__AddBullet(5, 5.0f, vPos, vDir);
                    }
                }

                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            });
        }

        if(fCurHealthPct <= 0.4f)
        {
            PHASE_CHANGE_INC
            this->__PhaseChange();
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(60u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        // random rays
        PHASE_CONTROL_TICKER(0u, 0u, 10.0f * m_fAttackSpeed, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < 24u; ++i)
            {
                const coreVector2 vBase = coreVector2::Direction(I_TO_F(i + (iTick / 10u) * 3u) * GA) * ((I_TO_F(i) + 1.3f) * 1.5f);

                const coreVector3 vPos = this->GetPosition() + coreVector3(vBase * 5.0f, 0.0f);
                const coreVector3 vDir = coreVector3(0.0f,0.0f,1.0f);

                this->__AddBullet(7, 5.0f, vPos, vDir);
            }

            this->__AddBullet(5, 5.0f, coreVector3(0.0f, 0.0f, this->GetPosition().z), coreVector3(0.0f,0.0f,1.0f));

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        m_fRotaSpeed = 1.0f * (1.0f - STEP(0.3f, 0.39f, fCurHealthPct));

        if(fCurHealthPct <= 0.3f)
        {
            PHASE_CHANGE_INC
            this->__PhaseChange();
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(70u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        if(PHASE_BEGINNING2)
        {
            //for(coreUintW i = 36u; i < 52u; ++i)
            //    this->__ResurrectFollower(i);

            m_avVector[ROTA_VALUE].y = 0.0f;
        }

        // side wave
        PHASE_CONTROL_TICKER(0u, 0u, 6.0f * m_fAttackSpeed, LERP_LINEAR)
        {
            const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * -0.18f);
            const coreVector2 vPos = vDir.Rotated90() * LERP(25.0f, 60.0f, STEP(0.2f, 0.3f, fCurHealthPct));

            this->__AddBulletLine(5, 5.0f,  vPos, vDir);
            this->__AddBulletLine(5, 5.0f, -vPos, vDir);

            if((iTick % 6u) == 5u)
            {
                const coreVector2 vPos2 = coreVector2(0.0f,0.0f);
                const coreVector2 vDir2 = vDir;

                this->__AddBulletLine(5, 5.0f, vPos2, vDir2);
            }

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        if(fCurHealthPct <= 0.2f)
        {
            PHASE_CHANGE_INC
            this->__PhaseChange();
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 71u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(80u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        const coreFloat fSpeed = LERP(10.0f, 5.0f, STEP(0.05f, 0.2f, fCurHealthPct));

        // static lines
        PHASE_CONTROL_TICKER(0u, 0u, fSpeed * m_fAttackSpeed, LERP_LINEAR)
        {
            coreVector2 vBase;
            if(fCurHealthPct > 0.15f)
            {
                vBase = coreVector2(0.0f,1.0f);
            }
            else if(fCurHealthPct > 0.1f)
            {
                vBase = coreVector2(1.0f,0.0f);
            }
            else
            {
                m_aiCounter[LINE_ROTA] += 1;
                vBase = coreVector2::Direction(I_TO_F(m_aiCounter[LINE_ROTA]) * GA);
            }

            const coreBool   bAimed   = !(iTick % 8u);
            const coreUint16 iNewTick = iTick - (iTick / 8u);

            if(bAimed)
            {
                const coreFloat fSide = coreVector2::Dot(this->NearestPlayerDual((iTick / 8u) % 2u)->GetPosition().xy(), vBase.Rotated90());
                this->__AddBulletLine(5, 5.0f, vBase.Rotated90() * fSide, vBase);
            }
            else
            {
                const coreFloat fSide = I_TO_F(iNewTick * 3u % 8u) - 3.5f;
                this->__AddBulletLine(5, 5.0f, vBase.Rotated90() * LERP(fSide, SIGN(fSide) * 3.5f, 0.1f) * 30.0f, vBase);
            }

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        if(fCurHealthPct <= 0.05f)
        {
            PHASE_CHANGE_TO(90u)

            for(coreUintW i = 0u; i < EIGENGRAU_PARASITES; ++i)
                this->__KillParasite(i);

            for(coreUintW i = 0u; i < EIGENGRAU_FOLLOWERS; ++i)
                this->__KillFollower(i);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 90u)
    {
        // nothing
    }

    // ################################################################
    // 
    else if(m_iPhase == 200u)
    {
        if(PHASE_BEGINNING2)
        {
            m_fLightningDelay = 10.0f;

            this->_EndBoss();

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);

            g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                ASSERT(!pPlayer->GetScoreTable()->GetCurChain())

                pPlayer->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                pPlayer->AddStatus  (PLAYER_STATUS_NO_INPUT_ALL);
                pPlayer->SetRainbow (false);
            });

            g_pGame->GetInterface()->SetVisible (false);
            g_pGame->GetInterface()->SetAlphaAll(0.0f);

            g_MusicPlayer.Stop();
            g_pGame->SetMusicVolume(1.0f);

            g_pPostProcessing->SetFrameValue(1.0f);

            this->SetPosition(coreVector3(0.0f,0.2f,0.0f) * FOREGROUND_AREA3);
            this->SetSize    (coreVector3(1.0f,1.0f,1.0f) * 50.0f);

            m_RangePlayer.SetPosition(coreVector3(0.0f,-0.2f,0.0f) * FOREGROUND_AREA3);
            m_RangePlayer.SetEnabled (CORE_OBJECT_ENABLE_ALL);

            m_fRangeSpeed       = 0.4f;
            m_fRangeSpeedPlayer = 0.4f;

            g_fShiftMode = 0.0f;

            for(coreUintW i = 0u; i < EIGENGRAU_PARASITES; ++i)
                m_aParasite[i].Kill(false);

            for(coreUintW i = 0u; i < EIGENGRAU_FOLLOWERS; ++i)
                m_aFollower[i].Kill(false);

            //g_pGame->GetInterface()->GetDialogText(0u)->SetPosition(coreVector2(0.0f,0.2f));
            //g_pGame->GetInterface()->GetDialogText(0u)->SetColor3(COLOR_MENU_BLUE);
            //g_pGame->GetInterface()->GetDialogText(0u)->SetText("I LOVE YOU");
            //g_pGame->GetInterface()->GetDialogText(1u)->SetPosition(coreVector2(0.0f,-0.2f));
            //g_pGame->GetInterface()->GetDialogText(1u)->SetColor3(COLOR_MENU_GREEN);
            //g_pGame->GetInterface()->GetDialogText(1u)->SetText("I LOVE YOU TOO");
        }

        PHASE_CONTROL_PAUSE(0u, 0.1f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 201u)
    {
        PHASE_CONTROL_TIMER(0u, 0.2f, LERP_SMOOTH)
        {
            m_fRangeSpeed = LERP(0.4f, 0.0f, fTime);
            m_Range.SetColor3(LERP(COLOR_PLAYER_BLUE, COLOR_ENERGY_WHITE * 0.2f, fTime));

            //g_pGame->GetInterface()->GetDialogText(0u)->SetColor3(LERP(COLOR_MENU_BLUE, COLOR_MENU_WHITE * 0.5f, fTime));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 202u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 203u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fHeight = LERPH3(-0.2f, 0.15f, fTime) * FOREGROUND_AREA.y;

            m_RangePlayer.SetPosition(coreVector3(0.0f, fHeight, 0.0f));

            //g_pGame->GetInterface()->GetDialogText(0u)->SetAlpha(LERPS(1.0f, 0.0f, fTime));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 204u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 205u)
    {
        PHASE_CONTROL_TIMER(0u, 0.2f, LERP_SMOOTH)
        {
            m_fRangeSpeedPlayer = LERP(0.4f, 0.0f, fTime);
            m_RangePlayer.SetColor3(LERP(COLOR_PLAYER_GREEN, COLOR_ENERGY_WHITE * 0.2f, fTime));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 206u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC

            g_MusicPlayer.SelectName("ending_secret.ogg");
            g_MusicPlayer.Play();
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 207u)
    {
        PHASE_CONTROL_TIMER(0u, 0.2f, LERP_HERMITE3)
        {
            this->SetAlpha(1.0f - fTime);

            if(PHASE_FINISHED)
            {
                this->Kill(false);
            }
        });
    }

    // ################################################################
    // ################################################################

    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(200u)
    }

    m_avVector[ROTA_VALUE].y += 0.7f * TIME * LERP(2.0f, 1.0f, STEP(0.2f, 0.3f, fCurHealthPct));

    for(coreUintW i = 0u; i < EIGENGRAU_PARASITES; ++i)
    {
        cEnemy& oParasite = m_aParasite[i];
        if(oParasite.HasStatus(ENEMY_STATUS_DEAD)) continue;

        // 
        if(oParasite.HasStatus(ENEMY_STATUS_GHOST)) m_afParasiteAlpha[i].UpdateMax(-1.0f, 0.0f);
                                               else m_afParasiteAlpha[i].UpdateMin( 1.0f, 1.0f);

        if(!m_afParasiteAlpha[i]) oParasite.Kill(false);

        const coreFloat fTime = oParasite.GetLifeTime();

        coreVector2 vPos;
        if(i < 10u)
        {
            // spread move from top
            const coreUintW iIndex = i - 0u;
            const coreFloat fReal  = ((I_TO_F(iIndex) / 10.0f) * 2.0f - 1.0f) - fTime * 0.27f + 2.0f;

            vPos = (fReal > 1.0f) ? HIDDEN_POS : coreVector2((I_TO_F((iIndex * 2u) % 5u) - 2.0f) * 0.2f, FmodRange(fReal, -1.0f, 1.0f));

            vPos *= FOREGROUND_AREA * 1.8f;
        }
        else if(i < 18u)
        {
            // circle
            const coreUintW   iIndex = i - 10u;
            const coreVector2 vDir   = coreVector2::Direction(((I_TO_F(iIndex) / 8.0f) + fTime * 0.05f * ((iIndex % 2u) ? -2.0f : 1.0f)) * (2.0f*PI));
            const coreFloat   fLen   = (iIndex % 2u) ? 20.0f : 35.0f;

            vPos = vDir * fLen;
        }
        else ASSERT(false)

        const coreVector2 vDir    = s_RotaCache.Direction(-fTime);
        const coreFloat   fOffset = I_TO_F(i) / I_TO_F(EIGENGRAU_PARASITES);
        const coreFloat   fView   = 1.0f - STEPH3(FOREGROUND_AREA.x * 1.6f, FOREGROUND_AREA.x * 1.8f, MAX(ABS(vPos.x), ABS(vPos.y)));

        oParasite.SetPosition (coreVector3(vPos, 60.0f));
        oParasite.SetDirection(coreVector3(vDir, 0.0f));
        oParasite.SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));
        oParasite.SetAlpha    (BLENDH3(m_afParasiteAlpha[i]) * fView);
    }

    for(coreUintW i = 0u; i < EIGENGRAU_FOLLOWERS; ++i)
    {
        cEnemy& oFollower = m_aFollower[i];
        if(oFollower.HasStatus(ENEMY_STATUS_DEAD)) continue;

        // 
        if(oFollower.HasStatus(ENEMY_STATUS_GHOST)) m_afFollowerAlpha[i].UpdateMax(-1.0f, 0.0f);
                                               else m_afFollowerAlpha[i].UpdateMin( 1.0f, 1.0f);

        if(!m_afFollowerAlpha[i]) oFollower.Kill(false);

        const coreFloat fTime = oFollower.GetLifeTime();

        coreVector2 vPos;
        if(i < 4u)
        {
            // intro
            const coreUintW   iIndex = i - 0u;
            const coreVector2 vDir   = coreVector2::Direction((I_TO_F(iIndex) / 4.0f + 0.125f - fTime * 0.02f) * (2.0f*PI));
            const coreFloat   fLen   = LERPB(2.0f, 0.5f, MIN1(fTime * 0.3f)) * 0.7f;

            vPos = vDir * fLen;
        }
        else if(i < 20u)
        {
            // kreuz und quer
            const coreUintW   iIndex = i - 4u;
            const coreFloat   fReal  = fTime * -0.3f + ((iIndex < 8u) ? 0.0f : 0.16f) + (((iIndex % 4u) < 2u) ? 0.0f : 1.0f) + 1.0f;
            const coreVector2 vBase  = coreVector2((iIndex % 2u) ? -0.1f : 0.3f, FmodRange(fReal, -1.0f, 1.0f));

            vPos = (fReal > 1.0f) ? HIDDEN_POS : MapStepRotated90(vBase, (iIndex / 2u) % 4u);
        }
        else if(i < 36u)
        {
            // circle wave
            const coreUintW   iIndex = i - 20u;
            const coreVector2 vDir   = coreVector2::Direction(((I_TO_F(iIndex) / 32.0f) - fTime * 0.11f * ((iIndex % 2u) ? -1.0f : 1.0f)) * (2.0f*PI));
            const coreFloat   fLen   = LERPB(2.0f, 0.55f, MIN1(fTime * 0.3f)) * ((iIndex % 2u) ? 0.7f : 1.0f);

            vPos = vDir * fLen;
        }
        else if(i < 52u)
        {
            // sinus line
            const coreUintW iIndex = i - 36u;
            const coreFloat fReal  = ((I_TO_F(iIndex) / 16.0f) * 2.0f - 1.0f) - fTime * 0.21f + 2.0f;

            vPos = (fReal > 1.0f) ? HIDDEN_POS : coreVector2(SIN(m_avVector[ROTA_VALUE].y) * 0.5f, FmodRange(fReal, -1.0f, 1.0f));
        }
        else ASSERT(false)

        vPos *= FOREGROUND_AREA * 2.4f;

        const coreVector2 vDir    = s_RotaCache.Direction(fTime);
        const coreFloat   fOffset = I_TO_F(i) / I_TO_F(EIGENGRAU_FOLLOWERS);
        const coreFloat   fView   = 1.0f - STEPH3(FOREGROUND_AREA.x * 2.2f, FOREGROUND_AREA.x * 2.4f, MAX(ABS(vPos.x), ABS(vPos.y)));

        oFollower.SetPosition (coreVector3(vPos, 0.0f));
        oFollower.SetDirection(coreVector3(vDir, 0.0f));
        oFollower.SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));
        oFollower.SetAlpha    (BLENDH3(m_afFollowerAlpha[i]) * fView);
    }


    m_fRotationSpeed.UpdateMin(1.0f, 10.0f);
    m_fRotation.Update(CLAMP01((m_fRotationSpeed - 4.0f) * 0.3f));

    for(coreUintW i = 0u; i < EIGENGRAU_LAYERS; ++i)
    {
        const coreVector2 vDir   = s_RotaCache.Direction(m_fRotation * (0.5f + 0.3f * I_TO_F(EIGENGRAU_LAYERS - i - 1u)));
        const coreFloat   fScale = 1.0f - 0.2f * I_TO_F(EIGENGRAU_LAYERS - i - 1u);
        const coreFloat   fAlpha = STEPH3(0.0f, 0.01f, fCurHealthPct - I_TO_F(i) * 0.01f);

        m_aLayer[i].SetPosition   (this->GetPosition());
        m_aLayer[i].SetSize       (this->GetSize() * fScale);
        m_aLayer[i].SetDirection  (coreVector3(vDir, 0.0f));
        m_aLayer[i].SetOrientation(OriRoundDir(vDir, vDir));
        m_aLayer[i].SetAlpha      (this->GetAlpha() * fAlpha);
        m_aLayer[i].SetTexSize    (coreVector2(1.0f,1.0f) * fScale);
        m_aLayer[i].SetTexOffset  (coreVector2(0.0f,0.4f) * fScale * m_fRotation);
        m_aLayer[i].SetEnabled    (fAlpha ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aLayer[i].Move();
    }

    this->SetDirection  (m_aLayer[EIGENGRAU_LAYERS - 1u].GetDirection  ());
    this->SetOrientation(m_aLayer[EIGENGRAU_LAYERS - 1u].GetOrientation());


    if(m_Range.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_fRangeAnim.Update(m_fRangeSpeed);

        if(fCurHealthPct > 0.0f)
        {
            const coreFloat fOldBeat = m_avVector[HEART_BEAT].x;
            m_avVector[HEART_BEAT].x += 3.0f * TIME;

            const coreUint32 iTickOld = F_TO_UI(fOldBeat);
            const coreUint32 iTickNew = F_TO_UI(m_avVector[HEART_BEAT].x);

            if(iTickOld != iTickNew)
            {
                const coreFloat fPower = 1.0f - STEP(0.0f, 0.05f, fCurHealthPct);

                if((iTickNew % 4u) < 2u) m_avVector[HEART_BEAT].y = 0.5f * fPower;

                if((iTickNew % 4u) == 0u && fPower) m_pHeartSound->PlayRelative(NULL, fPower, 1.0f, false, SOUND_EFFECT);
            }

            m_avVector[HEART_BEAT].y *= FrictionFactor(3.0f);
        }
        else
        {
            m_avVector[HEART_BEAT].y = 0.0f;
        }

        // 
        const coreVector2 vDir = coreVector2::Direction(m_fRangeAnim * (-1.6f*PI));

        m_Range.SetPosition (this->GetPosition());
        m_Range.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 0.03f * this->GetSize().x * (1.0f + m_avVector[HEART_BEAT].y));
        m_Range.SetDirection(coreVector3(vDir, 0.0f));
        m_Range.SetAlpha    (this->GetAlpha());
        m_Range.SetTexOffset(m_Range.GetTexOffset() - m_Range.GetDirection().xy() * (0.1f * m_fRangeSpeed * TIME));
        m_Range.Move();
    }
    if(m_RangePlayer.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_fRangeAnimPlayer.Update(m_fRangeSpeedPlayer);

        // 
        const coreVector2 vDir = coreVector2::Direction(m_fRangeAnimPlayer * (-1.6f*PI));

        m_RangePlayer.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 0.032f * this->GetSize().x);
        m_RangePlayer.SetDirection(coreVector3(vDir, 0.0f));
        m_RangePlayer.SetAlpha    (this->GetAlpha());
        m_RangePlayer.SetTexOffset(m_RangePlayer.GetTexOffset() - m_RangePlayer.GetDirection().xy() * (0.1f * m_fRangeSpeedPlayer * TIME));
        m_RangePlayer.Move();
    }

    // 
    if(g_pGame->IsMulti())
    {
        g_pGame->ForEachPlayerAll([&](const cPlayer* pPlayer, const coreUintW i)
        {
            const coreVector2 vRatio = Core::System->GetResolution().HighRatio();

            const coreVector2 vFrom = g_pGame->CalculateCamShift().xy() * 0.5f;
            const coreVector2 vTo   = pPlayer->GetPosition().xy();
            const coreVector2 vDiff = vTo - vFrom;

            const coreVector2 vClamp = coreVector2(CLAMP(vDiff.x, -vRatio.x * 46.0f + 9.0f, vRatio.x * 46.0f - 9.0f),
                                                   CLAMP(vDiff.y, -vRatio.y * 46.0f + 9.0f, vRatio.y * 46.0f - 9.0f));

            const coreVector2 vCorr = vDiff * (vDiff.IsAligned() ? 1.0f : (vClamp / vDiff).Min());
            const coreVector2 vPos   = vFrom + vCorr;
            const coreVector2 vDir   = vDiff.Normalized();
            const coreFloat   fAlpha = (pPlayer->HasStatus(PLAYER_STATUS_DEAD) || pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_ALL)) ? 0.0f : STEPH3(42.0f, 46.0f, (vDiff / vRatio).Processed(ABS).Max());

            m_aArrow[i].SetPosition (coreVector3(vPos, 0.0f));
            m_aArrow[i].SetDirection(coreVector3(vDir, 0.0f));
            m_aArrow[i].SetColor3   (pPlayer->GetEnergyColor());
            m_aArrow[i].SetAlpha    (fAlpha);
            m_aArrow[i].SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.2f));
            m_aArrow[i].SetEnabled  (fAlpha ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aArrow[i].Move();
        });
    }
    const coreMatrix2 mRota    = coreMatrix3::Rotation(m_fRotaSpeed * TIME).m12();
    const coreMatrix2 mRotaRev = mRota.Transposed();

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTiltBullet>([&](cTiltBullet* OUTPUT pBullet)
    {
        if(pBullet->GetDamage() == 6)
        {
            pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mRota, pBullet->GetPosition().z));
            pBullet->SetFlyDir3D(coreVector3(pBullet->GetFlyDir3D().xy() * mRota, pBullet->GetFlyDir3D().z));
        }
        else if(pBullet->GetDamage() == 7)
        {
            pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mRotaRev, pBullet->GetPosition().z));
            pBullet->SetFlyDir3D(coreVector3(pBullet->GetFlyDir3D().xy() * mRotaRev, pBullet->GetFlyDir3D().z));
        }
    });

    // 
    m_fLightningDelay.UpdateMax(-1.0f, 0.0f);

    // 
    m_Lightning.SetSize   (coreVector2(1.0f,1.0f) * MaxAspectRatio(Core::System->GetResolution()));
    m_Lightning.SetColor3 (g_CurConfig.Graphics.iFlash ? (coreVector3(1.0f,1.0f,1.0f) * 0.9f) : COLOR_MENU_BLACK);
    m_Lightning.SetAlpha  (BLENDH3(MIN1(m_fLightningDelay * 0.4f)));
    m_Lightning.SetEnabled(m_fLightningDelay ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Lightning.Move();
}


// ****************************************************************
// 
coreVector3 cEigengrauBoss::__AimAtPlayerDual(const coreUintW iIndex)const
{
    // 
    return (this->NearestPlayerDual(iIndex)->GetPosition() - this->GetPosition());
}


// ****************************************************************
// 
cBullet* cEigengrauBoss::__AddBullet(const coreInt32 iDamage, const coreFloat fSpeed, const coreVector3 vPos, const coreVector3 vDir)
{
    constexpr coreVector2 vFakePos = coreVector2(0.0f,0.0f);
    constexpr coreVector2 vFakeDir = coreVector2(0.0f,1.0f);

    return g_pGame->GetBulletManagerEnemy()->AddBullet<cTiltBullet>(iDamage, fSpeed * m_fAttackSpeed, this, vFakePos, vFakeDir)->SetTiltProperties(vPos, vDir)->ChangeSize(4.0f);
}


// ****************************************************************
// 
void cEigengrauBoss::__AddBulletLine(const coreInt32 iDamage, const coreFloat fSpeed, const coreVector2 vLinePos, const coreVector2 vLineDir)
{
    for(coreUintW i = 0u; i < 45u; ++i)
    {
        const coreVector2 vBase = vLinePos + vLineDir * ((I_TO_F(i) - 22.0f) * 7.2f);

        const coreVector3 vPos = coreVector3(vBase, this->GetPosition().z);
        const coreVector3 vDir = coreVector3(0.0f,0.0f,1.0f);

        this->__AddBullet(iDamage, fSpeed, vPos, vDir);
    }
}


// ****************************************************************
// 
void cEigengrauBoss::__AddBulletBurst()
{
    const coreVector2 vRota = coreVector2::Direction(I_TO_F(m_iBurstTick++) * (0.125f*PI));

    for(coreUintW i = 0u; i < 19u; ++i)
    {
        for(coreUintW j = 0u; j < 19u; ++j)
        {
            const coreVector2 vBase = MapToAxis(coreVector2(I_TO_F(i) - 9.0f, I_TO_F(j) - 9.0f), vRota);

            const coreVector3 vPos = this->GetPosition();
            const coreVector3 vDir = coreVector3(vBase, 10.0f).Normalized();

            this->__AddBullet(5, 5.0f, vPos, vDir);
        }
    }
}


// ****************************************************************
// 
void cEigengrauBoss::__PhaseChange()
{
    // 
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->StartRolling(pPlayer->GetInput()->vMove);
    });
}


// ****************************************************************
// 
void cEigengrauBoss::__ResurrectParasite(const coreUintW iIndex)
{
    ASSERT(iIndex < EIGENGRAU_PARASITES)
    cEnemy& oParasite = m_aParasite[iIndex];

    // 
    ASSERT(oParasite.HasStatus(ENEMY_STATUS_DEAD))
    oParasite.Resurrect();

    // 
    oParasite.SetPosition (coreVector3(HIDDEN_POS, 0.0f));
    oParasite.SetAlpha    (0.0f);
    oParasite.RemoveStatus(ENEMY_STATUS_GHOST);

    // 
    m_afParasiteAlpha[iIndex] = 0.0f;
}


// ****************************************************************
// 
void cEigengrauBoss::__KillParasite(const coreUintW iIndex)
{
    ASSERT(iIndex < EIGENGRAU_PARASITES)
    cEnemy& oParasite = m_aParasite[iIndex];

    // 
    if(oParasite.HasStatus(ENEMY_STATUS_GHOST) || oParasite.HasStatus(ENEMY_STATUS_DEAD)) return;

    // 
    oParasite.AddStatus(ENEMY_STATUS_GHOST);
}


// ****************************************************************
// 
void cEigengrauBoss::__ResurrectFollower(const coreUintW iIndex)
{
    ASSERT(iIndex < EIGENGRAU_FOLLOWERS)
    cEnemy& oFollower = m_aFollower[iIndex];

    // 
    ASSERT(oFollower.HasStatus(ENEMY_STATUS_DEAD))
    oFollower.Resurrect();

    // 
    oFollower.SetPosition (coreVector3(HIDDEN_POS, 0.0f));
    oFollower.SetAlpha    (0.0f);
    oFollower.RemoveStatus(ENEMY_STATUS_GHOST);

    // 
    m_afFollowerAlpha[iIndex] = 0.0f;
}


// ****************************************************************
// 
void cEigengrauBoss::__KillFollower(const coreUintW iIndex)
{
    ASSERT(iIndex < EIGENGRAU_FOLLOWERS)
    cEnemy& oFollower = m_aFollower[iIndex];

    // 
    if(oFollower.HasStatus(ENEMY_STATUS_GHOST) || oFollower.HasStatus(ENEMY_STATUS_DEAD)) return;

    // 
    oFollower.AddStatus(ENEMY_STATUS_GHOST);
}