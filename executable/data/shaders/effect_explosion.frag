///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// shader input
smooth varying float v_v1Light;   // 


void FragmentMain()
{
    // 
    //gl_FragColor = vec4(1.0, 0.3, 0.1, 0.6) * v_v1Light * pow(u_v4Color.a, 1.4);
    gl_FragColor = vec4(0.4, 0.4, 0.4, 0.6) * v_v1Light * pow(u_v4Color.a, 1.4);
}