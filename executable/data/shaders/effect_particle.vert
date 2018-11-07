///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader output
flat varying float v_v1Time;   // current animation time (from 0.0 to 1.0)


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreParticlePosition();
    v_av2TexCoord[0] = coreParticleTexCoord();
    v_av2TexCoord[1] = vec2(1.0) - v_av2TexCoord[0];

    // calculate current animation time
    v_v1Time = 1.0 - a_v1DivValue;
}