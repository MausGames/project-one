//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// normal map resolution
const float c_fMapResolution = 0.5;

// water animation uniforms
uniform float u_fTime;
uniform float u_fOffset;

	
void main()
{
    vec3 v3ViewDir = vec3(u_m4ModelView     * vec4(a_v3Position, 1.0));
    gl_Position    =      u_m4ModelViewProj * vec4(a_v3Position, 1.0);

    coreDot3Init();
    coreDot3Transform(u_asLight[0].v4Direction.xyz, v_av4LightDir[0].xyz);
    coreDot3Transform(v3ViewDir,                    v_v3ViewDir);

    vec2 vMapCoord = vec2(a_v2Texture.x * c_fMapResolution, 
                          a_v2Texture.y * c_fMapResolution + u_fOffset);
    
    v_av2TexCoord[0] = (vMapCoord + (vec2( u_fTime,      u_fTime)))     * 1.0;
    v_av2TexCoord[1] = (vMapCoord + (vec2( u_fTime*0.5, -u_fTime*0.3))) * 2.0;
    v_av2TexCoord[2] = (vMapCoord + (vec2(-u_fTime*0.1,  u_fTime*0.1))) * 8.0;
}