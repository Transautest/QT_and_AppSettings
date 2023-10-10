#pragma once
#ifndef TPSPEEDANALYSISGUI_H__
#define TPSPEEDANALYSISGUI_H__

/*!
* \file tp_speedanalysisgui.h
*
* \author Uwe Spiess
* \date 24/04/2017
*
* contains the functionality to act with the GUI
*/

#include <smartpointers.h>
#include <command/commandguihandshake.h>
// Base classes
#include <command/commandgui.h>
#include <refcounterimpl.h>
#include <command\commandsmartpointer.h>
#include <basetypes.h>
#include <composer\compositionexportdefine.h>
#include <gui/handshaker.h>
#include <boostwrapper/connection.h>
#include <vector>
#include <geom/typeofgeomiterators.h>
#include <bgeo/coord3diterators.h>
#include <hcntcore/typeofcadentity.h>
#include "userfilter.h"

#ifndef CadEntitiesIt_T
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntitiesIt_T;
#endif

namespace omsl { namespace intool { class IntoolsManager; } }
namespace omsl { namespace hcntcore { class Document; } }

namespace omsl {
namespace commands {
namespace tp_analyzer {
/*!
 * \class TPSpeedAnalysisgui
 *
 * \brief implementation of TPSpeedAnalysis gui
 *
 * \author Uwe Spieﬂ
 * \date 22/09/2017
 */

class TPSpeedAnalysisGUI:
  public omsl::command::CommandGUI
{
  OMSL_FRIEND_COMMANDGUI_FACTORY( TPSpeedAnalysis )
  OMSL_ALLOW_PROTECTED_DTOR( TPSpeedAnalysisGUI );

public:
  OMSL_IMPLEMENT_WEAKABLE_REFCOUNTER
    OMSL_IMPLEMENT_CAST() {
      OMSL_ADD_CAST( CommandGUI );
      OMSL_ADD_CAST( TPSpeedAnalysisGUI );
      OMSL_DEFAULT_CAST();
  }
  //////////////////////////////////////////////////////////////////////////
  //  CommandGUI Interface implementation
  //////////////////////////////////////////////////////////////////////////
  OMSL_METHOD( omsl::Bool ) Initialize();
  //////////////////////////////////////////////////////////////////////////
  //  Object methods
  //////////////////////////////////////////////////////////////////////////
  /**
  * @brief used to setup the filters
  * @param [in] const int& mode 
*/


protected:
  TPSpeedAnalysisGUI(void* pParent, omsl::command::Command* pCommand, omsl::hcntcore::Document* pDocument );
  ~TPSpeedAnalysisGUI( ) 
  {
      int i = 0;
      i++;
  }
  int Load( );

private:
  OMSL_DECLARE_WEAKABLE_REFCOUNTER;
  TPSpeedAnalysisGUI(const TPSpeedAnalysisGUI&)  {}
  const TPSpeedAnalysisGUI& operator= (const TPSpeedAnalysisGUI&);
  void SetupTPathSelector ();

public:
private:
  WeakPtr<omsl::command::Command>                   m_pCommand;
  WeakPtr<hcntcore::Document>                       m_pDocument;
  SharedPtr< omsl::gui::HandShaker>                 m_pParent; 
  std::vector<SharedPtr<boostwrapper::Connection>>  m_vConnections;
  SharedPtr<intool::IntoolsManager>                 m_pIntoolMgr;

};

}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl

#endif // TPANALYSISGUI_H__
