///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


void FragmentMain()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;

    // lookup normal map
    vec3 v3TexNormal = coreTexture2D(1, v_av2TexCoord[1]).xyz;

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3BumpNormal   = normalize(v3TexNormal * 2.0 - 1.0);
    float v1BumpFactor   = max(dot(v3MathLightDir, v3BumpNormal), 0.0);

    // 
    vec2 v2Distortion = v3BumpNormal.xy * 0.018 * DistortionStrength(v2ScreenCoord);
    vec3 v3Refraction = coreTextureBase2D(0, v2ScreenCoord + v2Distortion).rgb;

    // calculate dot-3 reflection factor
    vec3  v3MathViewDir = normalize(v_v3ViewDir);
    vec3  v3ReflNormal  = normalize(v3MathLightDir + v3MathViewDir);
    float v1ReflFactor  = max(dot(v3ReflNormal, v3BumpNormal), 0.0);

    // 
    vec2  v2Center = abs((v_av2TexCoord[0] * 2.0 - 1.0) + v2Distortion * 10.0);
    float v1Border = smoothstep(0.8, 1.05, max(v2Center.x, v2Center.y)) + max(v1BumpFactor - 0.5, 0.0) + 0.0;

    // calculate diffuse and specular value
    vec3 v3Diffuse  = mix(v3Refraction, vec3(1.0), v1Border) * (v1BumpFactor * 0.1 + 0.95);
    vec3 v3Specular = vec3(2.0 * min(coreGGX(v1ReflFactor, 0.005), 1.0));

    // draw final color
    gl_FragColor = vec4(v3Diffuse + v3Specular, 1.0);
}