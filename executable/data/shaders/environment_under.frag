///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"
#include "shared.glsl"


// constant values
const vec3 c_v3Blue = vec3(0.0, 0.43, 0.69);   // default ambient color


void FragmentMain()
{
    vec2 v2ScreenCoord = gl_FragCoord.xy * u_v4Resolution.zw;

    // lookup normal map
    vec3 v3TexNormal = coreTexture2D(0, v_av2TexCoord[0]).xyz;
         v3TexNormal = normalize(v3TexNormal * 2.0 - 1.0);
         
         
   // float v1Depth       = coreTextureBase2D(3, v2ScreenCoord).r;

    // set distortion vector
    vec2 v2Distortion = v3TexNormal.xy * 0.015 * DistortionStrength(v2ScreenCoord);

    // lookup refraction texture
    vec3 v3Refraction = coreTextureBase2D(2, v2ScreenCoord + v2Distortion).rgb;

    // draw final color
    gl_FragColor = vec4(mix(c_v3Blue, v3Refraction, 0.78), 1.0);
    //gl_FragColor = vec4(vec3(smoothstep(0.62, 0.75, v1Depth)), 1.0);
}