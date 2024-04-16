///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// one hand, one foot (or something else), one eye (links awakening)
// die einzelteile des fragments bilden die hand oder ähnliches
// white outlines, own shader
// im laufe des kampfes werden seitenteile immer dünkler, bis zu der phase wo man ins auge fliegt und alles verschmilzt, spieler kann dann überall fliegen
// boss is im background, moved weiter weg, rotation wechselt von XY zu XZ, transition wird mit flachem quad (oder 3 bunten achsen, beides) in spielerschiff dargestellt, pseudo-movement nach unten (partikel), gegner/hände und bullets kommen von links und rechts, boss ist unten, greift mit geschossen von unten an, manchmal auch was von oben am windshield (yoshi's story)
// P1 is von allen helfern umgeben, sie opfern sich stück für stück, in P1 um ihm die fähigkeit zu geben sich nach unten zu drehen
// boss kann auf händen um am windschild erscheinen
// boss ist auge, auf würfel im hintergrund, hat zwei hände die aus einzelteilen bestehen, die man vorher einzeln angreift
// wenn der boss besiegt wurde fliegt man ins auge in totenstille (wird immer größer und umschließt alles in dunkelheit), wo der weiße würfel in der mitte das finale objekt ist, man kann nicht mehr schießen, wenn man den würfel berührt verschmilzt man mit ihm, leuchtet weiß und kann sich nicht mehr bewegen (lerp), dann erscheint in der mitte "Thank you for playing", und der bildschirm zerreißt (ausgehen von spieler) samt text in ein weißes bild, seite fährt langsam wieder in mitte, dann wird grass background eingeblendet, credits starten mit melodie, wärend credits werden background durchgeblendet bis zum cloud background, es fliegen nochmal alle helfer durchs bild mit ENDE (deutsch) in der mitte, danach score eingabe
// hand will create balls (grab) and shoot at player, multiple times, which fly like mischief-makers ball, and create wave on border
// hand can slap on ground to create bullets and gray wave
// hand can drag game screen into side
// Eigengrau -> Eye gengrau / Auge ngrau -> Auge sieht farben und formen (vierecke, dreiecke, kreise)
// gegner-welle von links/rechts die oben/unten startet, man muss rotieren um sie abzuwehren
// TODO 1: MAIN: fragment, easy, hard (decision), coop, 3 badges, boss health, medal goal, intro, outro, foreshadow

// der tunnel und alle objekte werden gegen flug-richtung geshiftet, wodurch bewegungen mehr impact haben, außerdem kann man den rand des tunnels mit dem verschwundenen rand des spiel-felds gleichsetzen
// boss hat mehrere health-bars, erster healthbar ist approach zum boss, wenn healthbar weg ändert sich musik das auge geht auf und boss-name erscheint, welches auch als tutorial fürs drehen (dinge kommen von links und recht) dient


// ****************************************************************
// counter identifier


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cEigengrauBoss::cEigengrauBoss()noexcept
{
    // load models
    this->DefineModelHigh("ship_projectone.md3");
    this->DefineModelLow ("ship_projectone.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f));

    // configure the boss
    this->Configure(10000, COLOR_SHIP_GREY);
}


// ****************************************************************
// 
void cEigengrauBoss::__ResurrectOwn()
{

}


// ****************************************************************
// 
void cEigengrauBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    //this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cEigengrauBoss::__MoveOwn()
{

}