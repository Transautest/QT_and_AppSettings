#pragma once
#ifndef TPSPEEDANALYSISIMPL_H__
#define TPSPEEDANALYSISIMPL_H__
#include <command/commandhandshake.h>
#include <command/commandimpl.h>

#include <redirectoutput.h>

/*!
 * \file .h
 *
 * \author Uwe Spieﬂ
 * \date 21/09/2017
 *
 * [your comment here]
 */

namespace omsl {
namespace hcntcore{class Document;}
namespace command {class CommandGUI; class CommandModel; class CommandPreview;}
namespace commands {
namespace tp_analyzer {
/*!
 * \class TPSpeedAnalysis
 *
  * \brief implementation of TPSpeedAnalysis command
  *
 * [detailed description]
 *
 * \author Uwe Spieﬂ
 * \date 21/09/2017
 */

class TPSpeedAnalysisImpl :
    public command::CommandImpl<TPSpeedAnalysisImpl, command::Command::kMain, command::Command::kDocument>
{

    OMSL_FRIEND_COMMAND_FACTORY ( TPSpeedAnalysis )

public:
    static const wchar_t* m_name;
    OMSL_METHOD ( propertyservice::Context* ) GetContext ();
    OMSL_METHOD ( omsl::Bool ) CanRun ( int argc, void* argv [] );
    OMSL_METHOD ( void ) InternalRun ( int argc, void* argv [] );
    OMSL_METHOD ( void ) InternalExit ();
    OMSL_METHOD ( void ) InternalApply ();
protected:
    TPSpeedAnalysisImpl ( void* pGUI, omsl::hcntcore::Application* pApplication, hcntcore::Document* pDocument );
private:

    WeakPtr<hcntcore::Document> m_pDocument;
    void* m_pRootGUI;
    SharedPtr<omsl::command::CommandModel>    m_pModel;
    SharedPtr<omsl::command::CommandGUI>      m_pCommandGUI;
    SharedPtr<omsl::command::CommandPreview>  m_pPreview;
};

}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl



#endif // TPSPEEDANALYSISIMPL_H__
