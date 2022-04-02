///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform mediump vec4 u_v4Scale;   // 

// shader output
varying vec3 v_v3AnimCoord;       // 


void VertexMain()
{
    // transform position
    gl_Position = coreObject2DPosition();

    // calculate basic texture coordinates (with alpha-value as fixed modifier)
    vec2 v2NewTexCoord = a_v2LowTexCoord * u_v2TexSize - vec2(u_v4Color.a, 0.0);

    // animate final texture coordinates into 3 different directions
    v_av2TexCoord[0] = v2NewTexCoord * u_v4Scale.zw - vec2(0.0, u_v2TexOffset.y);
    v_v3AnimCoord    = vec3(v2NewTexCoord.x + u_v2TexOffset.x,
                            v2NewTexCoord.x - u_v2TexOffset.x,
                            v2NewTexCoord.y);
}