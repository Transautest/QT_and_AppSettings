#pragma once
#ifndef TPSIMPLECOMMANDS_H__
#define TPSIMPLECOMMANDS_H__
#include <command/commandhandshake.h>
#include <command/commandimpl.h>
#include <command/commandoneshotimpl.h>
//document
#include <hcntcore/document.h>
#include <hcntcore\model.h>
/*!
* \file .h
*
* \author Leonardo Cecchinato
* \date 21/09/2017
*
* [your comment here]
*/

using namespace omsl::command;

namespace omsl
{
namespace hcntcore
{
    class Document;
}
namespace commands
{
namespace tp_analyzer
{
    enum tpVis
    {
          tpv_unknown  = 0x0000
        , tpv_Hide     = 0x0001
        , tpv_ShowAll  = 0x0002
        , tpv_Points   = 0x0004
        , tpv_Vector   = 0x0008
        , tpv_Normal   = 0x0010
        , tpv_G0_only  = 0x0020
        , tpv_G1_only  = 0x0040

    };

class TP_VisibilityImpl :
    public omsl::command::CommandOneShotImpl<TP_VisibilityImpl, omsl::command::Command::kMain, omsl::command::Command::kApplication>
{
    OMSL_FRIEND_COMMAND_FACTORY ( TP_Visibility )

public:
    static const wchar_t* m_name;

    OMSL_METHOD ( omsl::Bool ) CanRun ( int argc, void* argv [] );

    OMSL_METHOD ( void ) InternalRun ( int argc, void* argv [] );
    OMSL_METHOD ( void ) InternalExit ()
    {}
    OMSL_METHOD ( void ) InternalApply (bool resetAfterApply);


protected:
    TP_VisibilityImpl ( omsl::hcntcore::Application* pApplication, omsl::hcntcore::Document* pDocument );
    ~TP_VisibilityImpl ()
    {}

private:
    int m_type;
    WeakPtr<hcntcore::Document> m_pDocument;
    SharedPtr<hcntcore::Model>    m_pModel;
};


}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl



#endif // TPSIMPLECOMMANDS_H__
