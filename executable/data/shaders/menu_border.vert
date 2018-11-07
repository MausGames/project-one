///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// constant values
const float c_v1TileRate = 1.4;    // 

// shader output
varying      vec4  v_v4Interior;   // raw object coordinates (xy = normal, zw = inverse)
flat varying vec4  v_v4Border;     // border distance values (xy = inner, zw = outer)
varying      float v_v1Factor;     // 


void VertexMain()
{
    vec2 v2Aspect = u_v4Resolution.xy * u_v4Resolution.wz;

    // calculate non-uniform size-factor and offset
    vec2 v2Size   = abs(max(v2Aspect, 1.0) * coreMat3to2(u_m3ScreenView));
    vec2 v2Offset = u_v2TexOffset * 2.0;

    // 
    vec4 v4Pixel = abs(u_v4Resolution.xy * coreMat3to2(u_m3ScreenView)).xyxy * 0.5;

    // transform position and texture coordinates
    gl_Position      = coreObject2DPosition();
    v_av2TexCoord[0] = (a_v2LowTexCoord * v2Size + v2Offset) * c_v1TileRate;

    // calculate border properties
    v_v4Interior = vec4(a_v2LowTexCoord, vec2(1.0) - a_v2LowTexCoord * u_v2TexSize);
    v_v4Border   = vec4(vec2(24.0/1080.0), vec2(14.0/1080.0)) / v2Size.xyxy;
    v_v4Border   = floor(v_v4Border * v4Pixel + vec4(0.001)) / v4Pixel;

    // 
    v_v1Factor = a_v2LowTexCoord.y * 2.2 - 0.2;
}