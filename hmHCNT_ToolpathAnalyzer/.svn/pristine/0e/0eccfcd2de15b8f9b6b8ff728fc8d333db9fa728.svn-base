#include "userfilter.h"
// keys
#include <Commands\tpanalysiskeys.h>
// command context
#include <propertyservice/context/context.h>
#include <hcntcore/cadentitymgr.h>
#include <hcntcore/point.h>
#include <hcntcore/toolpathmovement.h>
#include <commands/common_functions.h>

using namespace omsl::command;
using namespace omsl::commands::tp_analyzer;

namespace omsl
{
    OMSL_IMPL ( hcntcore::CadEntity* ) ToolpathUsrFilter::Apply ( hcntcore::CadEntity* entity )
    {
        SharedPtr<hcntcore::CadEntity> retVal;
        if ( entity )
        {
            if ( m_filterType == 0 )
            {
                {
                    omsl::SharedPtr<omsl::hcntcore::CadEntity> cent ( entity );

                    cent = GetReferencedToolpath ( cent, true );
                    if ( cent.Get () )
                    {
                        // !! return the original object , otherwise the filterresult confused
                        retVal = entity;
                        return retVal.get ();
                    }
                    cent = entity ;
                    cent = GetReferencedToolpathSegmentEntity ( cent, true );
                    if ( cent.Get () )
                    {
                        // !! return the original object , otherwise the filterresult confused
                        retVal = entity;
                        return retVal.get ();
                    }
                }
            }
            else
            {
            }
        }
        return retVal.get ();
    }
}
