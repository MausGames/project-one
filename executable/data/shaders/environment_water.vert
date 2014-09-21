//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// normal map resolution
const float c_fMapResolution = 0.42;

// overridden light direction
const vec3 c_v3LightDirection = vec3(0.583953857,-0.642349243,-0.496360779);

// water animation uniforms
uniform float u_fTime;
uniform float u_fOffset;

	
void main()
{
    vec3 v3ViewDir = vec3(u_m4ModelView     * vec4(a_v3Position, 1.0));
    gl_Position    =      u_m4ModelViewProj * vec4(a_v3Position, 1.0);

    coreDot3VertInit     (u_m3Normal, a_v3Normal, a_v4Tangent);
    coreDot3VertTransform(-c_v3LightDirection, v_av4LightDir[0].xyz);
    coreDot3VertTransform(-v3ViewDir,          v_v3ViewDir);

    vec2 vMapCoord = vec2(a_v2Texture.x * c_fMapResolution, 
                          a_v2Texture.y * c_fMapResolution + u_fOffset);
    
    v_av2TexCoord[0] = (vMapCoord + (vec2( u_fTime,      u_fTime)))     * 1.0;
    v_av2TexCoord[1] = (vMapCoord + (vec2( u_fTime*0.5, -u_fTime*0.3))) * 2.0;
    v_av2TexCoord[2] = (vMapCoord + (vec2(-u_fTime*0.1,  u_fTime*0.1))) * 8.0;
}