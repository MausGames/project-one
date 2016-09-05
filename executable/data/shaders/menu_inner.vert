//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader output
varying vec2 v_v2Interior;   // raw object coordinates (x = normal, y = inverse)
varying vec2 v_v2Border;     // border distance values (x = inner, y = outer)


void VertexMain()
{
    float v1Aspect = u_v4Resolution.y * u_v4Resolution.z;

    // calculate non-uniform size-factor and offset
    vec2 v2Size = vec2(u_m3ScreenView[0][0], u_m3ScreenView[1][1] * v1Aspect);
    vec2 v2Offset = u_v2TexOffset * (min(v1Aspect, 1.0) * 2.0);

    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = a_v2LowTexCoord * vec2(v2Size.x / v2Size.y * 0.25, 1.0);
    v_av2TexCoord[1] = (a_v2LowTexCoord * v2Size + v2Offset) * 2.0;

    // calculate border properties
    v_v2Interior = vec2(a_v2LowTexCoord.x, 1.0 - a_v2LowTexCoord.x);
    v_v2Border   = vec2(0.0168, 0.008) / v2Size.x * (min(v1Aspect, 1.0) * 1.333);
}