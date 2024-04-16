///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_LODOBJECT_H_
#define _P1_GUARD_LODOBJECT_H_

// TODO: change high-resolution models with pre-baked normal-maps or tessellation ?
// TODO: find better way to handle using high for normal rendering and low for special effects + reflection + collision


// ****************************************************************
// LOD object class
class cLodObject : public coreObject3D
{
private:
    coreModelPtr m_pModelHigh;   // high-polygon model object (used for default shading)
    coreModelPtr m_pModelLow;    // low-polygon model object (used for shadow, outline, glow)


public:
    cLodObject() = default;
    virtual ~cLodObject()override = default;

    ENABLE_COPY(cLodObject)

    // define the visual appearance
    inline void DefineModelHigh(std::nullptr_t)               {m_pModelHigh = NULL;                                           this->coreObject3D::DefineModel(m_pModelHigh);}
    inline void DefineModelHigh(const coreModelPtr&   pModel) {m_pModelHigh = pModel;                                         this->coreObject3D::DefineModel(m_pModelHigh);}
    inline void DefineModelHigh(const coreHashString& sName)  {m_pModelHigh = Core::Manager::Resource->Get<coreModel>(sName); this->coreObject3D::DefineModel(m_pModelHigh);}
    inline void DefineModelLow (std::nullptr_t)               {m_pModelLow  = NULL;}
    inline void DefineModelLow (const coreModelPtr&   pModel) {m_pModelLow  = pModel;}
    inline void DefineModelLow (const coreHashString& sName)  {m_pModelLow  = Core::Manager::Resource->Get<coreModel>(sName);}

    // render the LOD object (low-polygon only)
    using coreObject3D::Render;
    void Render(const coreProgramPtr& pProgram)final;

    // 
    inline void ActivateModelDefault() {this->coreObject3D::DefineModel(m_pModelHigh);}
    inline void ActivateModelLowOnly() {this->coreObject3D::DefineModel(m_pModelLow);}

    // get object properties
    inline const coreModelPtr& GetModelHigh()const {return m_pModelHigh;}
    inline const coreModelPtr& GetModelLow ()const {return m_pModelLow;}


private:
    // 
    inline void DefineModel(std::nullptr_t)               {ASSERT(false)}
    inline void DefineModel(const coreModelPtr&   pModel) {ASSERT(false)}
    inline void DefineModel(const coreHashString& sName)  {ASSERT(false)}
};


#endif // _P1_GUARD_LODOBJECT_H_