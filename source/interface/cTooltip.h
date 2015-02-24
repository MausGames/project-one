//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_TOOLTIP_H_
#define _P1_GUARD_TOOLTIP_H_

// TODO: transfer to Core Engine


// ****************************************************************
// tooltip class
class cTooltip final : public coreObject2D
{
public:
    cTooltip()noexcept;

    DISABLE_COPY(cTooltip)
};


#endif // _P1_GUARD_TOOLTIP_H_