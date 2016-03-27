//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader output
varying float v_v1Strength;   // 
varying float v_v1Blink;      // 


void VertexMain()
{
    // transform position normal-resized
    vec4 v4NewPosition = vec4(coreQuatApply(u_v4Rotation, (a_v3RawPosition + a_v3RawNormal * u_v2TexOffset.x) * u_v3Size) + u_v3Position, 1.0);
    gl_Position        = u_m4ViewProj * v4NewPosition;

    // forward texture coordinates
    v_av2TexCoord[0] = a_v2RawTexCoord;
    
    // forward blink intensity
    v_v1Blink = u_v2TexOffset.y;
    
    // 
    vec3 v3ViewDir   = normalize(u_v3CamPosition - v4NewPosition.xyz);
    vec3 v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    
    // calculate light and color intensity
    float v1Base = dot(vec3(v3ViewDir.xy, abs(v3ViewDir.z)), vec3(v3NewNormal.xy, abs(v3NewNormal.z)));
    v_v1Strength = 3.0 - 2.7 * v1Base;
}