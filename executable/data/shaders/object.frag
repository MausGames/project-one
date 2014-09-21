//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void main()
{
    vec3 v3Normal  = texture2D(u_as2Texture[1], v_av2TexCoord[0]).xyz;
    vec4 v4Texture = texture2D(u_as2Texture[0], v_av2TexCoord[0]);

    vec3 v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3 v3BumpNormal   = normalize(v3Normal * 2.0 - 1.0);
    float fBumpFactor   = dot(v3MathLightDir, v3BumpNormal); 

    vec3 v3MathViewDir = normalize(v_v3ViewDir);
    vec3 v3ReflNormal  = normalize((2.0 * fBumpFactor * v3BumpNormal) - v3MathLightDir); 
    float fReflFactor  = max(0.0, dot(v3MathViewDir, v3ReflNormal));
    
    vec3 v3Diffuse = v4Texture.rgb * (1.5 * max(0.0, fBumpFactor) + 0.3);

	vec3 v3Specular;
	if(fReflFactor > 0.0) v3Specular = vec3(0.3 * v4Texture.a * pow(fReflFactor, 25.0));
				     else v3Specular = vec3(0.0);

#ifdef _CORE_OPTION_INSTANCING_

    gl_FragColor = vec4((v3Diffuse + v3Specular) * v_v4VarColor.rgb, v_v4VarColor.a);
    
#else

    gl_FragColor = vec4((v3Diffuse + v3Specular) * u_v4Color.rgb, u_v4Color.a);

#endif
}