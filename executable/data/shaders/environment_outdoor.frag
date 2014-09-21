//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shadow uniforms
//uniform sampler2D u_s2ShadowTex;
//uniform float u_fShadowVal;

// shader input
varying float v_fMix;
//varying vec4 v_v4ShadowPos;


void main()
{
    float fClampMix = clamp(v_fMix, 0.0, 1.0);
    
    vec3 v3Normal;
    vec4 v4Texture;
    if(fClampMix == 0.0)
    {
        v3Normal  = texture2D(u_as2Texture[3], v_av2TexCoord[0]).xyz;
        v4Texture = texture2D(u_as2Texture[2], v_av2TexCoord[0]);
    }
    else if(fClampMix == 1.0)
    {
        v3Normal  = texture2D(u_as2Texture[1], v_av2TexCoord[0]).xyz;
        v4Texture = texture2D(u_as2Texture[0], v_av2TexCoord[0]);
    }
    else
    {
        v3Normal  = mix(texture2D(u_as2Texture[3], v_av2TexCoord[0]).xyz, texture2D(u_as2Texture[1], v_av2TexCoord[0]).xyz, fClampMix);
        v4Texture = mix(texture2D(u_as2Texture[2], v_av2TexCoord[0]),     texture2D(u_as2Texture[0], v_av2TexCoord[0]),     fClampMix);
    }
    
    vec3 v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3 v3BumpNormal   = normalize(v3Normal * 2.0 - 1.0);
    float fBumpFactor   = dot(v3MathLightDir, v3BumpNormal); 
    
    vec3 v3MathViewDir = normalize(v_v3ViewDir);
    vec3 v3ReflNormal  = normalize((2.0 * fBumpFactor * v3BumpNormal) - v3MathLightDir); 
    float fReflFactor  = max(0.0, dot(v3MathViewDir, v3ReflNormal));
    
    float fShadow;
    //if(u_fShadowVal > 0.0 && u_v4Color.a > 0.0)
    //{
    //    vec2 v2TexCoord = (gl_FragCoord.xy * u_v4Resolution.zw) * 2.0 - 1.0;
    //
    //    vec3 v2ShadowCoord = v_v4ShadowPos.xyz/v_v4ShadowPos.w;
    //    float fDepth       = texture2D(u_s2ShadowTex, v2ShadowCoord.st).r;
    //    fShadow            = (fDepth < v2ShadowCoord.z) ? 0.3*u_fShadowValy : 0.0;
    //
    //    float fPos = max(abs(v2TexCoord.x-0.1), abs(v2TexCoord.y+0.3));
    //    fShadow   *= max(1.0 - max(fPos - 0.8, 0.0)*5.0, 0.0);
    //}
    //else
    {
        fShadow = 0.0;
    }
    
    vec3 v3Diffuse = v4Texture.rgb * ((1.0 - fShadow) * (1.3 * max(0.0, fBumpFactor) + 0.3));
    
    vec3 v3Specular;
    if(fReflFactor > 0.0) v3Specular = vec3(0.3 * v4Texture.a * pow(fReflFactor, 25.0));
                     else v3Specular = vec3(0.0);
    
    gl_FragColor = vec4(v3Diffuse + v3Specular, 1.0);
}