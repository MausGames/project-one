///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_VALIDATE_H_
#define _P1_GUARD_VALIDATE_H_

// TODO: validate score, but never force, just mark invalid for shadow-banning
// TODO: replay and save: just mark invalid for shadow-banning, don't shuffle
// TODO: check sums of boss/mission scores and times with total and boss with mission
// TODO: check if num of streams and keyframes and sizes per stream would exceed anything

// NOTE: coreProtect only on ship health, score table and time table


#if defined(_CORE_DEBUG_) && 0


// ****************************************************************
// validation disabled
inline coreBool ValidateReplay(cReplay* OUTPUT pReplay) {return true;}
inline coreBool ValidateSave  (cSave*   OUTPUT pSave)   {return true;}


#else


// ****************************************************************
// 
FORCE_INLINE coreBool ValidateReplay(cReplay* OUTPUT pReplay)
{
    cReplay::sHeader& oHeader = pReplay->m_Header;

    const coreBool bValid = (oHeader.iReplayHash == pReplay->__CalculateReplayHash()) &&
                            ((oHeader.iNumPlayers >= 1u) && (oHeader.iNumPlayers <= 2u));

    /*
        after loading header, before setting data
        totalscore <> bossscores
        replayhash
        actions <> stream-entries
        adapt iNumStreams
        keyframe+stream size <> body-size
    */

    if(!bValid)
    {
        //for(coreUintW i = 0u; i < REPLAY_STREAMS; ++i)
        //    coreData::Shuffle(pReplay->m_aaStreamPacket[i].begin(), pReplay->m_aaStreamPacket[i].end(), CORE_RAND_SEED_ALWAYS + i);
    }

    return bValid;
}


// ****************************************************************
// 
FORCE_INLINE coreBool ValidateSave(cSave* OUTPUT pSave)
{
    return true;
}


#endif


#endif // _P1_GUARD_VALIDATE_H_