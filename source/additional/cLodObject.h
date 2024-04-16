///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_LODOBJECT_H_
#define _P1_GUARD_LODOBJECT_H_

// TODO 5: change high-resolution models with pre-baked normal-maps or tessellation ?


// ****************************************************************
// LOD object class
class cLodObject : public coreObject3D
{
private:
    coreModelPtr m_pModelHigh;      // high-polygon model object (used for regular shading)
    coreModelPtr m_pModelLow;       // low-polygon model object (used for shadow, outline, glow)

    static coreBool s_bAllowHigh;   // 


public:
    cLodObject() = default;
    virtual ~cLodObject()override = default;

    ENABLE_COPY(cLodObject)

    // define the visual appearance
    inline void DefineModelHigh(std::nullptr_t)               {m_pModelHigh = NULL;}
    inline void DefineModelHigh(const coreModelPtr&   pModel) {m_pModelHigh = pModel;}
    inline void DefineModelHigh(const coreHashString& sName)  {m_pModelHigh = Core::Manager::Resource->Get<coreModel>(sName);}
    inline void DefineModelLow (std::nullptr_t)               {m_pModelLow  = NULL;                                            this->coreObject3D::DefineModel(m_pModelLow);}
    inline void DefineModelLow (const coreModelPtr&   pModel) {m_pModelLow  = pModel;                                          this->coreObject3D::DefineModel(m_pModelLow);}
    inline void DefineModelLow (const coreHashString& sName)  {m_pModelLow  = Core::Manager::Resource->Get<coreModel>(sName);  this->coreObject3D::DefineModel(m_pModelLow);}

    // 
    inline void ActivateModelHigh() {if(s_bAllowHigh) this->coreObject3D::DefineModel(m_pModelHigh);}
    inline void ActivateModelLow () {this->coreObject3D::DefineModel(m_pModelLow);}

    // get object properties
    inline const coreModelPtr& GetModelHigh()const {return m_pModelHigh;}
    inline const coreModelPtr& GetModelLow ()const {return m_pModelLow;}

    // 
    static inline void RenderHighObject(cLodObject*    pObject) {pObject->ActivateModelHigh(); pObject->coreObject3D::Render(); pObject->ActivateModelLow();}
    static inline void RenderHighList  (coreBatchList* pList)   {FOR_EACH(it, *pList->List()) d_cast<cLodObject*>(*it)->ActivateModelHigh(); pList->Render(); FOR_EACH(it, *pList->List()) d_cast<cLodObject*>(*it)->ActivateModelLow();}

    // 
    static inline void AllowHigh(const coreBool bState) {ASSERT(s_bAllowHigh != bState) s_bAllowHigh = bState;}


private:
    // 
    inline void DefineModel(std::nullptr_t)               {UNREACHABLE}
    inline void DefineModel(const coreModelPtr&   pModel) {UNREACHABLE}
    inline void DefineModel(const coreHashString& sName)  {UNREACHABLE}
};


// ****************************************************************
// 
inline coreBool cLodObject::s_bAllowHigh = true;


#endif // _P1_GUARD_LODOBJECT_H_