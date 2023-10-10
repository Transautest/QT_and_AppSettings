#pragma once
#if !defined(__TP_PROPERTIESIMPL_H__)
#define __TP_PROPERTIESIMPL_H__
#include "tpanalyseproperties.h"
#include <propertyservice/typednametdef.h>
#include <propertyservice/context/context.h>
#include <propertyservice/context/contextsmartpointer.h>
#include <propertyservice/context/applicable.h>
#include <boostwrapper/sharedconnection.h>
#include <vector>
#include <basetypes.h>
#include <memory>

namespace omsl {

namespace propertyservice {class Value; class PropertyBag; class BagChangeConnection;}

namespace commands
{
    class TPAnalysisModel;
    namespace tp_analyzer
    {
        class TPAnalysisModel;

        class PropertiesImpl : public Properties
        {

        public:
            PropertiesImpl ();
            propertyservice::PropertyBag* GetBag () const
            {
                
                return m_context.get ();
            }

            void InitConnections ();

            OMSL_METHOD (void) SetModel ( TPAnalysisModel* pModel );
            void SetStrength ( const TCHAR* keyname, const omsl::propertyservice::Applicable::Strength& strength );
            OMSL_METHOD (bool) IsInactive ( const TCHAR* keyname );
        protected:
            PropertiesImpl ( const PropertiesImpl& /*copy*/ )
            {}
            PropertiesImpl& operator=( const PropertiesImpl& /*copy*/ )
            {}

            template <class VALUETYPE> propertyservice::Value* AddValue ( const omsl::propertyservice::TypedNameT<VALUETYPE> &par, const VALUETYPE& value );
            template <class VALUETYPE> const VALUETYPE& GetValue ( const omsl::propertyservice::TypedNameT<VALUETYPE> &par ) const;

            void OnPropertyChange ( propertyservice::PropertyBag*, const wchar_t* );

        private:
            propertyservice::SharedContext m_context;
            omsl::WeakPtr<TPAnalysisModel> m_pModel;
            std::vector<boostwrapper::SharedConnection > m_Connections;
            std::shared_ptr< propertyservice::BagChangeConnection > m_bagConnection;
        };

    } //namespace tpanalysis
} // commands
} //namespace omsl

#endif //__TP_PROPERTIESIMPL_H__