//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const vec3 c_v3Blue     = vec3(0.0, 0.43, 0.69);                           // default surface color
const vec3 c_v3LightDir = vec3(-0.583953857, -0.642349243, 0.496360779);   // overridden light direction

// shader input
varying float v_v1Smooth;   // height offset for smooth shores


void FragmentMain()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;

    // lookup normal map (multiple times) and depth map
    vec3 v3BumpNormal = coreTexture2D(0, v_av2TexCoord[0].st).xyz;
    
    // calculate dot-3 bump factor
      v3BumpNormal   = normalize(v3BumpNormal * 2.0 - 1.0);

    float v1Fall = coreMin3(v2ScreenCoord.x*0.5, (1.0-v2ScreenCoord.x)*0.5, coreMin3(v2ScreenCoord.y*0.1, (1.0-v2ScreenCoord.y)*0.1, 0.02));
      
    // set distortion vector and lookup reflection texture
    vec2 v2Distortion      = v3BumpNormal.xy * v1Fall;

    // lookup refraction texture
    vec3 v3BelowRefraction = coreTexture2D(2, v2ScreenCoord + v2Distortion).rgb;

    // draw final color
    gl_FragColor = vec4(mix(c_v3Blue, v3BelowRefraction, (1.0-0.2*coreLengthSq(v2ScreenCoord*2.0-1.0)) * 0.75), 1.0);
}