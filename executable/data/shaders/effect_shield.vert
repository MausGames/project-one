//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader uniforms
uniform float u_v1Expansion;   // 

// shader output
varying float v_v1Strength;    // 


void VertexMain()
{
    // transform position normal-resized
    vec4 v4NewPosition = vec4(coreQuatApply(u_v4Rotation, (a_v3RawPosition * u_v3Size) + (a_v3RawNormal * u_v1Expansion)) + u_v3Position, 1.0);
    gl_Position        = u_m4ViewProj * v4NewPosition;

    // forward texture coordinates
    v_av2TexCoord[0] = a_v2RawTexCoord;
    
    // 
    vec3 v3ViewDir = normalize(u_v3CamPosition - v4NewPosition.xyz);
    
    // calculate light and color intensity
    vec3  v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    float v1Base      = dot(vec3(v3ViewDir.xy, abs(v3ViewDir.z)), vec3(v3NewNormal.xy, abs(v3NewNormal.z)));
    v_v1Strength      = (v1Base * 0.6 + 0.4) * 2.25;
}