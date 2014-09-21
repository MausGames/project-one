//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// default surface color
const vec3 c_v3Blue = vec3(0.0, 0.43, 0.69);

// water smoothing uniform
uniform float u_fSmooth;


void main()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;
    
    vec3 v3BumpNormal1 = normalize(texture2D(u_as2Texture[0], v_av2TexCoord[0].st).xyz * 2.0 - 1.0);
    vec3 v3BumpNormal2 = normalize(texture2D(u_as2Texture[0], v_av2TexCoord[1].st).xyz * 2.0 - 1.0);
    vec3 v3BumpNormal3 = normalize(texture2D(u_as2Texture[0], v_av2TexCoord[2].st).xyz * 2.0 - 1.0);
    float fDepth       = texture2D(u_as2Texture[3], v2ScreenCoord).r;

    vec3 v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3 v3BumpNormal   = normalize(v3BumpNormal1 + v3BumpNormal2 + v3BumpNormal3);
    float fBumpFactor   = dot(v3MathLightDir, v3BumpNormal);

    vec3 v3MathViewDir = normalize(v_v3ViewDir);
    vec3 v3ReflNormal  = normalize((2.0 * fBumpFactor * v3BumpNormal) - v3MathLightDir); 
    float fReflFactor  = max(0.0, dot(v3MathViewDir, v3ReflNormal));
          fReflFactor  = 0.8 * pow(fReflFactor, 55.0);
          
    vec2 v2Distortion = v3BumpNormal.xy * 0.015;

    fDepth  = smoothstep(0.64, 0.735, fDepth) * 0.8 * (1.0 + fReflFactor);
    fDepth -= smoothstep(0.4,  0.3,   fDepth) * u_fSmooth;
   
    vec3 v3Reflection = texture2D(u_as2Texture[1], clamp(v2ScreenCoord + v2Distortion,          0.0, 1.0)).rgb;
    vec3 v3Refraction = texture2D(u_as2Texture[2], clamp(v2ScreenCoord + v2Distortion * fDepth, 0.0, 1.0)).rgb;
    
    gl_FragColor = vec4(mix(v3Refraction, 0.85 * (0.4 * c_v3Blue + 0.7 * v3Reflection + vec3(fReflFactor)), fDepth), 1.0);
}