///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader uniforms
uniform vec4 u_v4Scale;       // 

// shader input
varying vec3 v_v3AnimCoord;   // 


void FragmentMain()
{
    // prepare texture coordinates
    float v1Dir  = coreAllInvocations(v_v3AnimCoord.z > 0.5) ? v_v3AnimCoord.x : v_v3AnimCoord.y;
    float v1Lerp = (coreLinearStep(0.0, u_v4Scale.x, v_v3AnimCoord.z) +
                    coreLinearStep(u_v4Scale.y, 1.0, v_v3AnimCoord.z)) * 0.5;

    // lookup textures
    vec4  v4Shape  = coreTexture2D(0, vec2(v1Dir, v1Lerp));
    float v1Detail = coreTexture2D(1, v_av2TexCoord[0]).r;

    // draw shape with detail map and subtle white glow
    gl_FragColor = vec4(mix(vec3(1.0), v4Shape.rgb + vec3(v1Detail * 1.1), v4Shape.a), v4Shape.a * 0.9);
}