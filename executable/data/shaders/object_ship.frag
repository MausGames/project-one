//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void FragmentMain()
{
    // lookup texture (rgb = base-color, a = color-modifier)
    vec4 v4TexColor = coreTexture2D(0, v_av2TexCoord[0]);
    
    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3BumpNormal   = normalize(vec3(0.5,0.5,1.0) * 2.0 - 1.0);
    float v1BumpFactor   = dot(v3MathLightDir, v3BumpNormal); 

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize((2.0 * v1BumpFactor * v3BumpNormal) - v3MathLightDir); 
    float v1ReflFactor  = max(0.0, dot(v3MathViewDir, v3ReflNormal));
    
    // calculate diffuse and specular value
    vec3 v3Diffuse  = v4TexColor.rgb * (1.4 * max(0.0, v1BumpFactor) + 0.4);
	vec3 v3Specular = vec3(0.2 * 0.5 * pow(v1ReflFactor, 25.0));

    // draw final color
    gl_FragColor = vec4((v3Diffuse + v3Specular) * mix(vec3(1.0), u_v4Color.rgb*1.5, v4TexColor.a), u_v4Color.a);
}