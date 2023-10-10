#pragma once
#ifndef TPCOMMON_H__
#define TPCOMMON_H__

/*!
* \file tp_speedanalysisgui.h
*
* \author Uwe Spiess
* \date 24/04/2017
*
* contains the functionality to act with the GUI
*/
#include <geom/toolpathsegment.h>
#include <hcntcore/cadentity.h>
#include <hcntcore/toolpathentity.h>
#include <geom/attributes.h>

class ToolpathData;
typedef omsl::SharedPtr<omsl::geom::ToolpathSegment> ToolpathSegmentPtr;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            void callStackString ( const wchar_t* s, int inout );
            void clear_callStackString ();
            omsl::hcntcore::CadEntity* GetReferencedToolpath ( SharedPtr<hcntcore::CadEntity>& pEntsIn , bool withSignal = false );
            omsl::geom::ToolpathSegment* GetReferencedToolpathSegment ( SharedPtr<hcntcore::CadEntity>& pEntsIn );
            omsl::hcntcore::CadEntity* GetReferencedToolpathSegmentEntity ( SharedPtr<hcntcore::CadEntity>& pEntsIn, bool withSignal = false );
            ToolpathData* GetToolpathData ( ToolpathSegmentPtr pSegment );
            ToolpathData* GetToolpathData ( SharedPtr<omsl::geom::Attribute> pAttr );
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
#endif // TPCOMMON_H__
