//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const float c_fMapResolution   = 0.42;                                          // normal map resolution
const vec3  c_v3LightDirection = vec3(0.583953857,-0.642349243,-0.496360779);   // overridden light direction

// water uniforms
uniform float u_fTime;     // current animation value
uniform float u_fOffset;   // current y-position offset

	
void VertexMain()
{
    // transform view direction and position
    vec3 v3ViewDir = coreObject3DViewDir().xyz;
    gl_Position    = coreObject3DPosition();

    // dot-3 transform lighting vectors (resolved)
    v_av4LightDir[0].xyz = vec3(-c_v3LightDirection.x, c_v3LightDirection.y, -c_v3LightDirection.z);
    v_v3ViewDir          = normalize(vec3(-v3ViewDir.x, v3ViewDir.y, -v3ViewDir.z));

    // calculate current mapping base
    vec2 vMapCoord = vec2(a_v2Texture.x * c_fMapResolution, 
                          a_v2Texture.y * c_fMapResolution + u_fOffset);
    
    // transform texture coordinates with different animation properties
    v_av2TexCoord[0] = (vMapCoord + (vec2( u_fTime,      u_fTime)))     * 1.0;
    v_av2TexCoord[1] = (vMapCoord + (vec2( u_fTime*0.5, -u_fTime*0.3))) * 2.0;
    v_av2TexCoord[2] = (vMapCoord + (vec2(-u_fTime*0.1,  u_fTime*0.1))) * 8.0;
}