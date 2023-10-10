#pragma once
#ifndef TPANALYSISIMPL_H__
#define TPANALYSISIMPL_H__
#include <command/commandhandshake.h>
#include <command/commandimpl.h>


/*!
 * \file .h
 *
 * \author Leonardo Cecchinato
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
 * \class TPAnalysis
 *
  * \brief implementation of TPAnalysis command
  *
 * [detailed description]
 *
 * \author Leonardo Cecchinato
 * \date 21/09/2017
 */

class TPAnalysisImpl:
//    public command::CommandImpl<TPAnalysisImpl, command::Command::kTool, command::Command::kDocument>
    public command::CommandImpl<TPAnalysisImpl, command::Command::kMain, command::Command::kDocument>
{

  OMSL_FRIEND_COMMAND_FACTORY( TPAnalysis )

public:
  static const wchar_t* m_name;
  OMSL_METHOD ( propertyservice::Context* ) GetContext( );
  OMSL_METHOD ( omsl::Bool ) CanRun( int argc, void* argv[] );
  OMSL_METHOD( void ) InternalRun( int argc, void* argv[] );
  OMSL_METHOD( void ) InternalExit();
  OMSL_METHOD( void ) InternalApply(bool resetAfterApply);

  OMSL_METHOD ( omsl::command::CommandPolicy* ) GetPolicy () const;

protected:
TPAnalysisImpl(void* pGUI, omsl::hcntcore::Application* pApplication, hcntcore::Document* pDocument);
private:

  WeakPtr<hcntcore::Document> m_pDocument;
  void* m_pRootGUI;
  SharedPtr<omsl::command::CommandModel>    m_pModel;
  SharedPtr<omsl::command::CommandGUI>      m_pCommandGUI;
  SharedPtr<omsl::command::CommandPreview>  m_pPreview;

  omsl::SharedPtr<omsl::command::CommandPolicy> m_policy;
};

}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl

#endif // TPANALYSISIMPL_H__
