#pragma once
#ifndef WP_USRFILTER_H__
#define WP_USRFILTER_H__

#include <hcntcore/document.h>


//////////////////////////////////////////////////////////////////////////
#include <hcntcore/model.h>
#include <containers/stringiteratorsreadonly.h>
#include <selectionservice/entityselector.h>
#include <selectionservice/entityfilter.h>
#include <selectionservice/filters.h>
#include <selectionservice/filtert.h>
#include <selectionservice/selection.h>
#include <hcntcore/cadentity.h>
#include <wstringptr.h>
#include <command\commandsmartpointer.h>
namespace omsl
{
    class ToolpathUsrFilter :
        public selectionservice::FilterT < ToolpathUsrFilter >
    {
    public:
        inline ToolpathUsrFilter ( SharedPtr<iterator::hcntcore::CadEntityForward> mySelectedPoint, const int type )
            : m_mySelectedPoint ( mySelectedPoint )
            , m_filterType (type)
        {
        }
        inline ToolpathUsrFilter ( WeakPtr<omsl::command::Command> myCommand, const int type ) 
            : m_pCommand ( myCommand )
            , m_filterType (type)
        {
        }
        
        OMSL_METHOD ( hcntcore::CadEntity* ) Apply ( hcntcore::CadEntity* entity );

    private:

        SharedPtr<iterator::hcntcore::CadEntityForward> m_mySelectedPoint;
        WeakPtr<omsl::command::Command>                   m_pCommand;
        int m_filterType;
    };
}

typedef omsl::SharedPtr<omsl::ToolpathUsrFilter> ToolpathUsrFilterPtr;

#endif // WP_USRFILTER_H__
