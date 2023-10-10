
#ifndef _MODELENTITYCONNECTION_H_
#define _MODELENTITYCONNECTION_H_


#include <wchar.h>

#include <hcntcore/model.h>

#include <smartpointers.h>

//#include "tpanalysismodel.h"

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
//            class TPAnalysisModel;

            typedef omsl::SharedPtr<omsl::boostwrapper::Connection> ConnectionPtr;


            class TPAnalyzerConnection : public omsl::RefCounter
            {

            public:

                TPAnalyzerConnection ();
                virtual ~TPAnalyzerConnection ();

                int Connect ( omsl::boostwrapper::Connection* );
                virtual int Disconnect ();

                OMSL_IMPLEMENT_WEAKABLE_REFCOUNTER
                    OMSL_IMPLEMENT_CAST ()
                {
                    OMSL_ADD_CAST ( TPAnalyzerConnection );
                    OMSL_DEFAULT_CAST ();
                }

            protected:

                OMSL_DECLARE_WEAKABLE_REFCOUNTER;

                ConnectionPtr m_cp;
            };


            typedef omsl::SharedPtr<TPAnalyzerConnection> TPAnalyzerConnectionPtr;

            class TPAnalyzerModelEntityConnection : public TPAnalyzerConnection
            {

            public:

                TPAnalyzerModelEntityConnection ();
                ~TPAnalyzerModelEntityConnection ();

                int Init ( void* pCommandModel );
                int Connect ( omsl::hcntcore::Model* );
                virtual int Disconnect ();

                // signals
                virtual bool OnEntitySignal ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnEntityCreated ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnEntityGraphicBuilt ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnEntityPersist ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnEntityModified ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnModifiedByOther ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnEntityDeleted ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnEntityHideUnhide ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnEntityBuildGraphicPlease ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                virtual bool OnEntityUpdateGraphicPlease ( omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );

                inline int isInHM_mode ()
                {
                    return m_pre_unusedType; 
                }

            private:
                bool checkEntity ( omsl::hcntcore::CadEntity* pEnt, omsl::hcntcore::Model::EntitySignalType );
                static bool _entity_signal ( void* obj, omsl::signal::Phase mode, omsl::hcntcore::Model::EntitySignalType type, omsl::hcntcore::CadEntity* pEnt, const omsl::hcntcore::entitysignal::EntitySignalData* pESD );
                void* m_pCommandModel;

                int m_pre_unusedType = -1;
            };

            typedef omsl::SharedPtr<TPAnalyzerModelEntityConnection> TPAnalyzerModelEntityConnectionPtr;

            int CreateAnalyzerModelEntityConnection ( omsl::hcntcore::Model*, TPAnalyzerModelEntityConnectionPtr& );
            int CreateAnalyzerModelEntityConnection ( omsl::hcntcore::Model*, TPAnalyzerModelEntityConnection*);
            int CreateAnalyzerModelEntityConnection ( omsl::hcntcore::Model*, TPAnalyzerModelEntityConnection*, TPAnalyzerModelEntityConnectionPtr& );

        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl


#endif // #ifdef _MODELENTITYCONNECTION_H_

