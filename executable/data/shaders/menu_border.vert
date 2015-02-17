//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader output
varying vec4 v_v4Interior;   // raw object coordinates (xy = normal, zw = reciprocal)
varying vec4 v_v4Border;     // border distance values (xy = inner, zw = outer)


void VertexMain()
{
    // calculate non-uniform size-factor
    vec2 v2Size = vec2(u_m3ScreenView[0][0], u_m3ScreenView[1][1] * (u_v4Resolution.y/u_v4Resolution.x));

    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = a_v2LowTexCoord * v2Size * 2.0;

    // calculate border properties
    v_v4Interior = vec4(a_v2LowTexCoord, vec2(1.0) - a_v2LowTexCoord);
    v_v4Border   = vec4(vec2(0.0168), vec2(0.008)) / v2Size.xyxy;
}