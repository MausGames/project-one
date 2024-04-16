///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader output
flat varying vec2 v_v2Time;   // current simulation times (from 0.0 to 1.0)


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreParticlePosition();
    v_av2TexCoord[0] = coreParticleTexCoord();
    v_av2TexCoord[1] = vec2(1.0) - v_av2TexCoord[0];

    // calculate current simulation time
    float v1Rev = 1.0 - a_v1DivValue;
    v_v2Time = vec2(min(v1Rev * 3.0, 1.0), smoothstep(0.0, 0.05, v1Rev));
}