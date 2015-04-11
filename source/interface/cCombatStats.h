//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_COMBATSTATS_H_
#define _P1_GUARD_COMBATSTATS_H_


// ****************************************************************
// combat stats class
class cCombatStats final
{
public:
    cCombatStats()noexcept;

    DISABLE_COPY(cCombatStats)

    // apply combat stats
    void Apply();
};


#endif // _P1_GUARD_COMBATSTATS_H_