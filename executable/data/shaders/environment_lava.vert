///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// constant values
const float c_v1MapResolution = 0.42;   // normal map resolution

// shader uniforms
uniform float u_v1Time;                 // current animation value
uniform float u_v1Offset;               // current y-position offset


void VertexMain()
{
    // transform position
    vec4 v4NewPosition = vec4(coreObject3DTransformLow(), 1.0);
    gl_Position        = u_m4ViewProj * v4NewPosition;

    // calculate current mapping base
    vec2 v2MapCoord = vec2(a_v2LowTexCoord.x * c_v1MapResolution,
                           a_v2LowTexCoord.y * c_v1MapResolution + u_v1Offset);

    // transform texture coordinates with different animation properties
    v_av2TexCoord[0] = (v2MapCoord + u_v1Time * vec2( 0.5, -0.5)) * 1.0;
    v_av2TexCoord[1] = (v2MapCoord + u_v1Time * vec2(-0.1,  0.1)) * 2.0;
    v_av2TexCoord[2] = v4NewPosition.xy * 0.03125;
}