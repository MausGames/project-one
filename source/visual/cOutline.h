//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_OUTLINE_H_
#define _P1_GUARD_OUTLINE_H_


// ****************************************************************
// outline definitions
#define OUTLINE_SHADER_FULL   "effect_outline_program",        "effect_outline_inst_program"          // shader-program names for full outlining
#define OUTLINE_SHADER_DIRECT "effect_outline_direct_program", "effect_outline_direct_inst_program"   // shader-program names for directional interpolated outlining


// ****************************************************************
// outline-effect class
class cOutline final : public cBindContainer
{
private:
    coreProgramPtr s_pProgramSingle;      // shader-program for single outlined objects
    coreProgramPtr s_pProgramInstanced;   // shader-program for instanced outlined objects


public:
    cOutline(const coreChar* pcProgramSingleName, const coreChar* pcProgramInstancedName)noexcept;

    DISABLE_COPY(cOutline)

    // apply outline-effect
    void Apply();
};


#endif // _P1_GUARD_OUTLINE_H_