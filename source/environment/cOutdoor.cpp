//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cOutdoor::cOutdoor(const coreChar* pcTextureTop, const coreChar* pcTextureBottom, const coreUint8& iAlgorithm, const coreFloat& fGrade)noexcept
: m_iVertexOffset (0u)
, m_iIndexOffset  (0u)
, m_fFlyOffset    (0.0f)
, m_iAlgorithm    (0u)
, m_fGrade        (0.0f)
{
    // load outdoor textures
    m_pNormalMap = Core::Manager::Resource->LoadNew<coreTexture>();
    this->LoadTextures(pcTextureTop, pcTextureBottom);

    // load outdoor geometry
    m_pModel = Core::Manager::Resource->LoadNew<coreModel>();
    this->LoadGeometry(iAlgorithm, fGrade);

    // load shader-program
    this->DefineProgram("environment_outdoor_program");
}


// ****************************************************************
// destructor
cOutdoor::~cOutdoor()
{
    // free resources
    this->DefineTexture(2u, NULL);
    Core::Manager::Resource->Free(&m_pNormalMap);
    Core::Manager::Resource->Free(&m_pModel);
}


// ****************************************************************
// render the outdoor-surface
void cOutdoor::Render()
{
    // check for model status
    if(!m_pModel.IsUsable()) return;

    // enable the shader-program
    if(!m_pProgram.IsUsable()) return;
    if(!m_pProgram->Enable())  return;

    // send shadow matrix to shader-program
    cShadow::EnableShadowRead(SHADOW_HANDLE_OUTDOOR);

    // enable all active textures
    for(coreUintW i = 0u; i < CORE_TEXTURE_UNITS; ++i)
        if(m_apTexture[i].IsUsable()) m_apTexture[i]->Enable(i);

    // draw the model
    m_pModel->Enable();
    glDrawRangeElements(m_pModel->GetPrimitiveType(), m_iVertexOffset, m_iVertexOffset + (OUTDOOR_VIEW+1u)*OUTDOOR_WIDTH, OUTDOOR_RANGE, m_pModel->GetIndexType(), I_TO_P(m_iIndexOffset));
}


// ****************************************************************
// load outdoor textures
void cOutdoor::LoadTextures(const coreChar* pcTextureTop, const coreChar* pcTextureBottom)
{
    // load color textures
    this->DefineTexture(0u, PRINT("environment_%s_diff.png", pcTextureTop));
    this->DefineTexture(1u, PRINT("environment_%s_diff.png", pcTextureBottom));

    // unbind normal map to prevent concurrency problems
    this->DefineTexture(2u, NULL);

    Core::Manager::Resource->AttachFunction([=]()
    {
        // FUN FACT:
        // apparently a "similar" method has an U.S. patent by HP (http://www.google.co.in/patents/US6337684)
        // so they "invented" the fundamental unit-sphere formula [1 = x*x + y*y + z*z]

        // delete old data
        m_pNormalMap->Unload();

        // retrieve normal map files
        coreFile* pFile1 = Core::Manager::Resource->RetrieveFile(PRINT("data/textures/environment_%s_norm.png", pcTextureTop));
        coreFile* pFile2 = Core::Manager::Resource->RetrieveFile(PRINT("data/textures/environment_%s_norm.png", pcTextureBottom));
        WARN_IF(!pFile1 || !pFile2) return CORE_OK;

        // decompress files to plain pixel data
        SDL_Surface* pSurface1 = IMG_LoadTyped_RW(SDL_RWFromConstMem(pFile1->GetData(), pFile1->GetSize()), true, coreData::StrExtension(pFile1->GetPath()));
        SDL_Surface* pSurface2 = IMG_LoadTyped_RW(SDL_RWFromConstMem(pFile2->GetData(), pFile2->GetSize()), true, coreData::StrExtension(pFile2->GetPath()));
        WARN_IF((pSurface1->format->BitsPerPixel != 24u) || (pSurface2->format->BitsPerPixel != 24u)) return CORE_OK;

        // allocate required memory
        const coreUintW iSize = pSurface1->w * pSurface1->h * 4u;
        coreByte* pOutput = new coreByte[iSize];
        coreByte* pInput1 = s_cast<coreByte*>(pSurface1->pixels);
        coreByte* pInput2 = s_cast<coreByte*>(pSurface2->pixels);

        // merge XY components of both normal maps (Z can be calculated in shader)
        for(coreUintW i = 0u, j = 0u; i < iSize; i += 4u, j += 3u)
        {
            std::memcpy(pOutput + i,      pInput1 + j, 2u);
            std::memcpy(pOutput + i + 2u, pInput2 + j, 2u);
        }

        // create final normal map
        m_pNormalMap->Create(pSurface1->w, pSurface1->h, CORE_TEXTURE_SPEC_RGBA, CORE_TEXTURE_MODE_FILTER | CORE_TEXTURE_MODE_REPEAT);
        m_pNormalMap->Modify(0u, 0u, pSurface1->w, pSurface1->h, iSize, pOutput);

        // wait until texture-upload is finished
        coreSync oSync;
        oSync.Create();
        oSync.Check(GL_TIMEOUT_IGNORED, CORE_SYNC_CHECK_FLUSHED);

        // bind normal map safely
        this->DefineTexture(2u, m_pNormalMap);

        // free all temporary resources
        SAFE_DELETE_ARRAY(pOutput)
        SDL_FreeSurface(pSurface1);
        SDL_FreeSurface(pSurface2);
        pFile1->UnloadData();
        pFile2->UnloadData();

        return CORE_OK;
    });
}


// ****************************************************************
// load outdoor geometry
void cOutdoor::LoadGeometry(const coreUint8& iAlgorithm, const coreFloat& fGrade)
{
    sVertex     aVertexData[OUTDOOR_TOTAL_VERTICES];
    coreUint16  aiIndexData[OUTDOOR_TOTAL_INDICES];
    coreVector3 avOrtho1   [OUTDOOR_TOTAL_VERTICES];
    coreVector3 avOrtho2   [OUTDOOR_TOTAL_VERTICES];

    // delete old data
    m_pModel->Unload();
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i) m_afHeight[i] = 0.0f;

    // save properties
    m_iAlgorithm = iAlgorithm;
    m_fGrade     = fGrade;

    // select algorithm function
    std::function<coreFloat(const coreFloat&, const coreFloat&)> nAlgorithmFunc;
    switch(iAlgorithm)
    {
    default: ASSERT(false)
    case 1u: nAlgorithmFunc = [](const coreFloat& x, const coreFloat& y) {coreFloat r = -(COS((x - I_TO_F(OUTDOOR_WIDTH)*0.5f)*0.075f*PI)*5.0f);                                                                                        return r;}; break;
    case 2u: nAlgorithmFunc = [](const coreFloat& x, const coreFloat& y) {coreFloat r =  (ABS(SIN(x*0.075f*PI)*8.0f + SIN(y*0.075f*PI)*8.0f) - 6.0f);                                                                                   return r;}; break;
    case 3u: nAlgorithmFunc = [](const coreFloat& x, const coreFloat& y) {coreFloat r =  (ABS(SIN(y*0.075f*PI)*0.25f -      (x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 13.0f);                                                      return r;}; break;
    case 4u: nAlgorithmFunc = [](const coreFloat& x, const coreFloat& y) {coreFloat r = -(ABS(SIN(y*0.150f*PI)*0.25f - 0.5f*(x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.150f*PI) - 0.0f; if(r <    0.0f) r = -1.0f; return r;}; break;
    case 5u: nAlgorithmFunc = [](const coreFloat& x, const coreFloat& y) {coreFloat r = -(ABS(SIN(y*0.075f*PI)*0.25f + 0.5f*(x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.075f*PI) - 1.5f; if(r < -100.0f) r = -1.0f; return r;}; break;
    case 6u: nAlgorithmFunc = [](const coreFloat& x, const coreFloat& y) {coreFloat r =  (ABS(SIN(y*0.075f*PI)*0.25f -      (x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.150f*PI) - 1.0f;                            return r;}; break;
    case 7u: nAlgorithmFunc = [](const coreFloat& x, const coreFloat& y) {coreFloat r =  (ABS(SIN(y*0.075f*PI)*0.25f -      (x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.075f*PI) + 3.0f;                            return r;}; break;
    case 8u: nAlgorithmFunc = [](const coreFloat& x, const coreFloat& y) {coreFloat r =  (ABS(SIN(y*0.075f*PI)*0.25f -      (x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.075f*PI) + 1.0f;                            return r;}; break;
    }

    // create vertices
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        const coreInt32 x = i % OUTDOOR_WIDTH;
        const coreInt32 y = i / OUTDOOR_WIDTH;

        // calculate level (height) of the vertex
        coreFloat fLevel = nAlgorithmFunc(I_TO_F(x), I_TO_F(y));
        if(fGrade)
        {
            // add randomness to the level and smooth out water-intersection-area
            coreFloat fSmoothLevel;
            do {fSmoothLevel = CLAMP(fLevel, -fGrade*1.5f, fGrade*1.5f) + Core::Rand->Float(-0.5f, 0.5f)*fGrade;}
            while(coreMath::InRange(fSmoothLevel, 0.0f, fGrade*0.25f));

            // forward smooth level
            fLevel = fSmoothLevel;
        }

        // center level at water
        fLevel += WATER_HEIGHT;

        // save height value
        m_afHeight[i] = fLevel;

        // set vertex position
        aVertexData[i].vPosition = coreVector3(I_TO_F(x - OUTDOOR_WIDTH/2) * OUTDOOR_DETAIL, I_TO_F(y - OUTDOOR_VIEW/2) * OUTDOOR_DETAIL, fLevel);
    }

    // sync beginning and ending height to create an infinite looking grid when resetting the position
    for(coreUintW i = 0u; i < OUTDOOR_VIEW; ++i)
    {
        for(coreUintW j = 0u; j < OUTDOOR_WIDTH; ++j)
        {
            const coreUintW f = i * OUTDOOR_WIDTH + j;
            const coreUintW t = f + OUTDOOR_HEIGHT * OUTDOOR_WIDTH;

            aVertexData[t].vPosition.z = aVertexData[f].vPosition.z;
            m_afHeight [t]             = m_afHeight [f];
        }
    }

    // create indices
    coreUintW iIndex = 0u;
    for(coreUintW y = 0u; y < OUTDOOR_BLOCKS_Y; ++y)
    {
        for(coreUintW x = 0u; x < OUTDOOR_BLOCKS_X; ++x)
        {
            const coreUint16 iVertex = x + y*OUTDOOR_WIDTH;

            aiIndexData[iIndex+0u] = iVertex;
            aiIndexData[iIndex+1u] = iVertex + 1u;
            aiIndexData[iIndex+2u] = iVertex + 1u + OUTDOOR_WIDTH;
            aiIndexData[iIndex+3u] = iVertex;
            aiIndexData[iIndex+4u] = iVertex + 1u + OUTDOOR_WIDTH;
            aiIndexData[iIndex+5u] = iVertex      + OUTDOOR_WIDTH;

            iIndex += 6u;
        }
    }

    // calculate normals
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        const coreInt32 x = i % OUTDOOR_WIDTH;
        const coreInt32 y = i / OUTDOOR_WIDTH;

        const coreUintW iLeft  = MAX(x-1, 0)                                + y*OUTDOOR_WIDTH;
        const coreUintW iRight = MIN(x+1, coreInt32(OUTDOOR_WIDTH)      -1) + y*OUTDOOR_WIDTH;
        const coreUintW iDown  = MAX(y-1, 0)                               *OUTDOOR_WIDTH + x;
        const coreUintW iUp    = MIN(y+1, coreInt32(OUTDOOR_HEIGHT_FULL)-1)*OUTDOOR_WIDTH + x;

        aVertexData[i].vNormal = coreVector3::Cross((aVertexData[iLeft].vPosition - aVertexData[iRight].vPosition).Normalize(),
                                                    (aVertexData[iDown].vPosition - aVertexData[iUp   ].vPosition).Normalize());
    }

    // calculate tangents
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_INDICES; i += 3u)
    {
        // calculate triangle sides
        const coreVector3 A1 = aVertexData[aiIndexData[i+1u]].vPosition - aVertexData[aiIndexData[i+0u]].vPosition;
        const coreVector3 A2 = aVertexData[aiIndexData[i+2u]].vPosition - aVertexData[aiIndexData[i+0u]].vPosition;
        const coreVector2 B1 = coreVector2(0.25f, (i%2u) ? 0.0f : 0.25f);
        const coreVector2 B2 = coreVector2((i%2u) ? 0.25f : 0.0f, 0.25f);

        // calculate local tangent vector parameters
        const coreFloat   R  = RCP(B1.s*B2.t - B2.s*B1.t);
        const coreVector3 D1 = (A1*B2.t - A2*B1.t) * R;
        const coreVector3 D2 = (A2*B1.s - A1*B2.s) * R;

        for(coreUintW j = 0u; j < 3u; ++j)
        {
            // add local values to each point of the triangle
            avOrtho1[aiIndexData[i+j]] += D1;
            avOrtho2[aiIndexData[i+j]] += D2;
        }
    }
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        // finish the Gram-Schmidt process to calculate the tangent vector and binormal sign (w)
        aVertexData[i].vTangent = coreVector4((avOrtho1[i] - aVertexData[i].vNormal * coreVector3::Dot(aVertexData[i].vNormal, avOrtho1[i])).Normalize(),
                                              SIGN(coreVector3::Dot(coreVector3::Cross(aVertexData[i].vNormal, avOrtho1[i]), avOrtho2[i])));
    }

    GLuint iNormFormat;
    std::function<coreUint32(const coreVector4&)> nPackFunc;
    if(CORE_GL_SUPPORT(ARB_vertex_type_2_10_10_10_rev))
    {
        // use high-precision packed format
        iNormFormat = GL_INT_2_10_10_10_REV;
        nPackFunc   = [](const coreVector4& vVector) {return vVector.PackSnorm210();};
    }
    else
    {
        // use low-precision byte format
        iNormFormat = GL_BYTE;
        nPackFunc   = [](const coreVector4& vVector) {return vVector.PackSnorm4x8();};
    }

    // reduce total vertex size
    sVertexPacked aPackedData[OUTDOOR_TOTAL_VERTICES];
    for(coreUintW i = 0u; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        // convert specific vertex attributes
        aPackedData[i].vPosition = aVertexData[i].vPosition;
        aPackedData[i].iNormal   = nPackFunc(coreVector4(aVertexData[i].vNormal, 0.0f));
        aPackedData[i].iTangent  = nPackFunc(aVertexData[i].vTangent);
    }

    // create vertex buffer
    coreVertexBuffer* pBuffer = m_pModel->CreateVertexBuffer(OUTDOOR_TOTAL_VERTICES, sizeof(sVertexPacked), aPackedData, CORE_DATABUFFER_STORAGE_STATIC);
    pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_POSITION_NUM, 3u, GL_FLOAT,    false, 0u);
    pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_NORMAL_NUM,   4u, iNormFormat, false, 3u*sizeof(coreFloat));
    pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_TANGENT_NUM,  4u, iNormFormat, false, 3u*sizeof(coreFloat) + 1u*sizeof(coreUint32));

    // create index buffer
    m_pModel->CreateIndexBuffer(OUTDOOR_TOTAL_INDICES, sizeof(coreUint16), aiIndexData, CORE_DATABUFFER_STORAGE_STATIC);

    Core::Log->Info("Outdoor-Surface loaded");
}


// ****************************************************************
// retrieve height value
coreFloat cOutdoor::RetrieveHeight(const coreVector2& vPosition)
{
    // convert real position to block position
    const coreFloat fX = (vPosition.x-this->GetPosition().x) / OUTDOOR_DETAIL + I_TO_F(OUTDOOR_WIDTH/2);
    const coreFloat fY = (vPosition.y-this->GetPosition().y) / OUTDOOR_DETAIL + I_TO_F(OUTDOOR_VIEW /2);

    // retrieve all four corners of the block
    const coreUintW iI00 = F_TO_UI(fY) * OUTDOOR_WIDTH + F_TO_UI(fX);   // bottom left
    const coreUintW iI01 = iI00 + 1u;                                   // bottom right
    const coreUintW iI10 = iI00      + OUTDOOR_WIDTH;                   // top left
    const coreUintW iI11 = iI00 + 1u + OUTDOOR_WIDTH;                   // top right
    ASSERT(iI00 < OUTDOOR_TOTAL_VERTICES && iI11 < OUTDOOR_TOTAL_VERTICES);

    // retrieve height values of the corners
    const coreFloat& fH00 = m_afHeight[iI00];
    const coreFloat& fH01 = m_afHeight[iI01];
    const coreFloat& fH10 = m_afHeight[iI10];
    const coreFloat& fH11 = m_afHeight[iI11];

    // interpolate between all height values
    const coreFloat fFractX = FRACT(fX);
    const coreFloat fFractY = FRACT(fY);
    return LERP(LERP(fH00, fH01, fFractX), LERP(fH10, fH11, fFractX), fFractY);
}


// ****************************************************************
// retrieve ray intersection point
coreVector3 cOutdoor::RetrieveIntersect(const coreVector3& vRayPosition, const coreVector3& vRayDirection)
{
    ASSERT(vRayDirection.z < 0.0f)
    coreVector3 vOutput = vRayPosition;

    // naive ray-tracing with fixed step-size
    for(coreUintW i = 100u; i--; )
    {
        ASSERT(i)

        // 
        if(ABS(vOutput.x) > I_TO_F(OUTDOOR_WIDTH/2) * OUTDOOR_DETAIL ||
           ABS(vOutput.y) > I_TO_F(OUTDOOR_VIEW /2) * OUTDOOR_DETAIL)
           break;

        // 
        const coreFloat& fCurHeight = this->RetrieveHeight(vOutput.xy());
        if(fCurHeight > vOutput.z)
        {
            // 
            vOutput += vRayDirection * (2.0f * (fCurHeight - vOutput.z) * RCP(vRayDirection.z));
            break;
        }

        // 
        vOutput += vRayDirection * 2.0f;
    }

    return vOutput;
}


// ****************************************************************
// set current fly offset
void cOutdoor::SetFlyOffset(const coreFloat& fFlyOffset)
{
    // set new value
    m_fFlyOffset = fFlyOffset;
    ASSERT(F_TO_UI(m_fFlyOffset) < OUTDOOR_HEIGHT)

    // 
    this->SetPosition(coreVector3(0.0f, m_fFlyOffset * -OUTDOOR_DETAIL, 0.0f));

    // calculate vertex and index offset
    m_iVertexOffset = F_TO_UI(m_fFlyOffset) * OUTDOOR_WIDTH;
    m_iIndexOffset  = F_TO_UI(m_fFlyOffset) * OUTDOOR_BLOCKS_X * OUTDOOR_PER_INDICES * sizeof(coreUint16);
}