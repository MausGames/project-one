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
cOutdoor::cOutdoor(const char* pcTextureTop, const char* pcTextureBottom, const coreByte& iAlgorithm, const float& fGrade)noexcept
: m_iRenderOffset (0)
, m_fFlyOffset    (0.0f)
, m_iAlgorithm    (0)
, m_fGrade        (0.0f)
{
    // load outdoor geometry
    m_pModel = Core::Manager::Resource->LoadNew<coreModel>();
    this->LoadGeometry(iAlgorithm, fGrade);

    // load object resources
    this->DefineTexture(0, PRINT("environment_%s_diff.png", pcTextureTop));
    this->DefineTexture(1, PRINT("environment_%s_norm.png", pcTextureTop));
    this->DefineTexture(2, PRINT("environment_%s_diff.png", pcTextureBottom));
    this->DefineTexture(3, PRINT("environment_%s_norm.png", pcTextureBottom));
    this->DefineProgram("environment_outdoor_program");
}


// ****************************************************************
// destructor
cOutdoor::~cOutdoor()
{
    // free model
    Core::Manager::Resource->Free(&m_pModel);
}


// ****************************************************************
// render the outdoor-surface
void cOutdoor::Render()
{
    // enable all resources
    if(this->Enable())
    {
        // send shadow matrix to shader-program
        cShadow::EnableShadowRead(SHADOW_HANDLE_OUTDOOR);

        // draw the model
        glDrawRangeElements(m_pModel->GetPrimitiveType(), 0, OUTDOOR_TOTAL_VERTICES, OUTDOOR_RANGE, m_pModel->GetIndexType(), r_cast<const GLvoid*>(m_iRenderOffset));
    }
}


// ****************************************************************
// move the outdoor-surface
void cOutdoor::Move()
{
    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// load outdoor geometry
void cOutdoor::LoadGeometry(const coreByte& iAlgorithm, const float& fGrade)
{
    sVertex     aVertexData[OUTDOOR_TOTAL_VERTICES];
    coreUshort  aiIndexData[OUTDOOR_TOTAL_INDICES];
    coreVector3 avOrtho1   [OUTDOOR_TOTAL_VERTICES];
    coreVector3 avOrtho2   [OUTDOOR_TOTAL_VERTICES];

    // delete old data
    m_pModel->Unload();
    for(int i = 0; i < OUTDOOR_TOTAL_VERTICES; ++i) m_afHeight[i] = 0.0f;

    // save properties
    m_iAlgorithm = iAlgorithm;
    m_fGrade     = fGrade;

    // select algorithm function
    std::function<float(const float&, const float&)> pAlgorithmFunc;
    switch(iAlgorithm)
    {
    default: ASSERT(false)
    case 3:  pAlgorithmFunc = [](const float& x, const float& y) {float r = -(COS((x - I_TO_F(OUTDOOR_WIDTH)*0.5f)*0.075f*PI)*5.0f);                                                                                        return r;}; break;
    case 1:  pAlgorithmFunc = [](const float& x, const float& y) {float r =  (ABS(SIN(x*0.075f*PI)*8.0f + SIN(y*0.075f*PI)*8.0f) - 6.0f);                                                                                   return r;}; break;
    case 2:  pAlgorithmFunc = [](const float& x, const float& y) {float r =  (ABS(SIN(y*0.075f*PI)*0.25f -      (x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 13.0f);                                                      return r;}; break;
    case 4:  pAlgorithmFunc = [](const float& x, const float& y) {float r = -(ABS(SIN(y*0.150f*PI)*0.25f - 0.5f*(x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.150f*PI) - 0.0f; if(r <    0.0f) r = -1.0f; return r;}; break;
    case 7:  pAlgorithmFunc = [](const float& x, const float& y) {float r = -(ABS(SIN(y*0.075f*PI)*0.25f + 0.5f*(x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.075f*PI) - 1.5f; if(r < -100.0f) r = -1.0f; return r;}; break;
    case 5:  pAlgorithmFunc = [](const float& x, const float& y) {float r =  (ABS(SIN(y*0.075f*PI)*0.25f -      (x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.150f*PI) - 1.0f;                            return r;}; break;
    case 6:  pAlgorithmFunc = [](const float& x, const float& y) {float r =  (ABS(SIN(y*0.075f*PI)*0.25f -      (x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.075f*PI) + 3.0f;                            return r;}; break;
    case 8:  pAlgorithmFunc = [](const float& x, const float& y) {float r =  (ABS(SIN(y*0.075f*PI)*0.25f -      (x/I_TO_F(OUTDOOR_WIDTH) - 0.5f)*4.0f)*20.0f - 10.0f) * SIN(y*0.075f*PI) + 1.0f;                            return r;}; break;
    }

    // create vertices
    for(int i = 0; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        const int x = i % OUTDOOR_WIDTH;
        const int y = i / OUTDOOR_WIDTH;

        // calculate level (height) of the vertex
        float fLevel = pAlgorithmFunc(I_TO_F(x), I_TO_F(y));
        if(fGrade)
        {
            // add randomness to the level and smooth out water-intersection-area
            float fSmoothLevel;
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
    for(int i = 0; i < OUTDOOR_VIEW; ++i)
    {
        for(int j = 0; j < OUTDOOR_WIDTH; ++j)
        {
            const int f = i * OUTDOOR_WIDTH + j;
            const int t = f + OUTDOOR_HEIGHT * OUTDOOR_WIDTH;

            aVertexData[t].vPosition.z  = aVertexData[f].vPosition.z;
            m_afHeight[t]               = m_afHeight[f];
        }
    }

    // create indices
    int iIndex = 0;
    for(int y = 0; y < OUTDOOR_BLOCKS_Y; ++y)
    {
        for(int x = 0; x < OUTDOOR_BLOCKS_X; ++x)
        {
            const int iVertex = x + y*OUTDOOR_WIDTH;

            aiIndexData[iIndex+0] = iVertex;
            aiIndexData[iIndex+1] = iVertex + 1;
            aiIndexData[iIndex+2] = iVertex + 1 + OUTDOOR_WIDTH;
            aiIndexData[iIndex+3] = iVertex;
            aiIndexData[iIndex+4] = iVertex + 1 + OUTDOOR_WIDTH;
            aiIndexData[iIndex+5] = iVertex     + OUTDOOR_WIDTH;

            iIndex += 6;
        }
    }

    // calculate normals
    for(int i = 0; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        const int x = i % OUTDOOR_WIDTH;
        const int y = i / OUTDOOR_WIDTH;

        const int iLeft  = MAX(x-1, 0)                     + y*OUTDOOR_WIDTH;
        const int iRight = MIN(x+1, OUTDOOR_WIDTH      -1) + y*OUTDOOR_WIDTH;
        const int iDown  = MAX(y-1, 0)                    *OUTDOOR_WIDTH + x;
        const int iUp    = MIN(y+1, OUTDOOR_HEIGHT_FULL-1)*OUTDOOR_WIDTH + x;

        aVertexData[i].vNormal = coreVector3::Cross((aVertexData[iLeft].vPosition - aVertexData[iRight].vPosition).Normalize(),
                                                    (aVertexData[iDown].vPosition - aVertexData[iUp   ].vPosition).Normalize());
    }

    // calculate tangents
    for(int i = 0; i < OUTDOOR_TOTAL_INDICES; i += 3)
    {
        // calculate triangle sides
        const coreVector3 A1 = aVertexData[aiIndexData[i+1]].vPosition - aVertexData[aiIndexData[i+0]].vPosition;
        const coreVector3 A2 = aVertexData[aiIndexData[i+2]].vPosition - aVertexData[aiIndexData[i+0]].vPosition;
        const coreVector2 B1 = coreVector2(0.25f, (i%2) ? 0.0f : 0.25f);
        const coreVector2 B2 = coreVector2((i%2) ? 0.25f : 0.0f, 0.25f);

        // calculate local tangent vector parameters
        const float R = RCP(B1.s*B2.t - B2.s*B1.t);
        const coreVector3 D1 = (A1*B2.t - A2*B1.t) * R;
        const coreVector3 D2 = (A2*B1.s - A1*B2.s) * R;

        for(int j = 0; j < 3; ++j)
        {
            // add local values to each point of the triangle
            avOrtho1[aiIndexData[i+j]] += D1;
            avOrtho2[aiIndexData[i+j]] += D2;
        }
    }
    for(coreUint i = 0; i < OUTDOOR_TOTAL_VERTICES; ++i)
    {
        // finish the Gram-Schmidt process to calculate the tangent vector and binormal sign (w)
        aVertexData[i].vTangent = coreVector4((avOrtho1[i] - aVertexData[i].vNormal * coreVector3::Dot(aVertexData[i].vNormal, avOrtho1[i])).Normalize(),
                                              SIGN(coreVector3::Dot(coreVector3::Cross(aVertexData[i].vNormal, avOrtho1[i]), avOrtho2[i])));
    }

    if(CORE_GL_SUPPORT(ARB_vertex_type_2_10_10_10_rev))
    {
        // reduce total vertex size
        sVertexPacked aPackedData[OUTDOOR_TOTAL_VERTICES];
        for(coreUint i = 0; i < OUTDOOR_TOTAL_VERTICES; ++i)
        {
            // convert specific vertex attributes
            aPackedData[i].vPosition = aVertexData[i].vPosition;
            aPackedData[i].iNormal   = coreVector4(aVertexData[i].vNormal, 0.0f).PackSnorm210();
            aPackedData[i].iTangent  = aVertexData[i].vTangent.PackSnorm210();
        }

        // create compressed vertex buffer
        coreVertexBuffer* pBuffer = m_pModel->CreateVertexBuffer(OUTDOOR_TOTAL_VERTICES, sizeof(sVertexPacked), aPackedData, CORE_DATABUFFER_STORAGE_STATIC);
        pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_POSITION_NUM, 3, GL_FLOAT,              0);
        pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_NORMAL_NUM,   4, GL_INT_2_10_10_10_REV, 3*sizeof(float));
        pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_TANGENT_NUM,  4, GL_INT_2_10_10_10_REV, 3*sizeof(float) + 1*sizeof(coreUint));
    }
    else
    {
        // create vertex buffer
        coreVertexBuffer* pBuffer = m_pModel->CreateVertexBuffer(OUTDOOR_TOTAL_VERTICES, sizeof(sVertex), aVertexData, CORE_DATABUFFER_STORAGE_STATIC);
        pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_POSITION_NUM, 3, GL_FLOAT, 0);
        pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_NORMAL_NUM,   3, GL_FLOAT, 3*sizeof(float));
        pBuffer->DefineAttribute(CORE_SHADER_ATTRIBUTE_TANGENT_NUM,  4, GL_FLOAT, 6*sizeof(float));
    }

    // create index buffer
    m_pModel->CreateIndexBuffer(OUTDOOR_TOTAL_INDICES, sizeof(coreUshort), aiIndexData, CORE_DATABUFFER_STORAGE_STATIC);

    Core::Log->Info("Outdoor-Surface loaded");
}

// ****************************************************************
// retrieve height value
float cOutdoor::RetrieveHeight(const coreVector2& vPosition)
{
    // convert real position to block position
    const float fX = (vPosition.x-this->GetPosition().x) / OUTDOOR_DETAIL + I_TO_F(OUTDOOR_WIDTH/2);
    const float fY = (vPosition.y-this->GetPosition().y) / OUTDOOR_DETAIL + I_TO_F(OUTDOOR_VIEW /2);

    // retrieve all four corners of the block
    const int iI00 = F_TO_SI(fY) * OUTDOOR_WIDTH + F_TO_SI(fX);   // bottom left
    const int iI01 = iI00 + 1;                                    // bottom right
    const int iI10 = iI00     + OUTDOOR_WIDTH;                    // top left
    const int iI11 = iI00 + 1 + OUTDOOR_WIDTH;                    // top right
    ASSERT(0 <= iI00 && iI11 < OUTDOOR_TOTAL_VERTICES);

    // retrieve height values of the corners
    const float& fH00 = m_afHeight[iI00];
    const float& fH01 = m_afHeight[iI01];
    const float& fH10 = m_afHeight[iI10];
    const float& fH11 = m_afHeight[iI11];

    // interpolate between all height values
    const float fFractX = FRACT(fX);
    const float fFractY = FRACT(fY);
    return LERP(LERP(fH00, fH01, fFractX), LERP(fH10, fH11, fFractX), fFractY);
}


// ****************************************************************
// set current fly offset
void cOutdoor::SetFlyOffset(const float& fFlyOffset)
{
    // set new value
    m_fFlyOffset = fFlyOffset;

    // calculate render offset
    m_iRenderOffset = (F_TO_SI(m_fFlyOffset) % OUTDOOR_HEIGHT) * OUTDOOR_BLOCKS_X * OUTDOOR_PER_INDICES * 2;
}