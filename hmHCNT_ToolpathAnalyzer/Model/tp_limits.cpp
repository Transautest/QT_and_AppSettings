#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
// keys
#include <Commands\tpanalysiskeys.h>
#include "../Commands/tp_listener.h"
#include "../Commands/tp_signalemitter.h"

using namespace omsl::command;


namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            int TPAnalysisModel::OnChange_CB_MoveLimits ( const int& bFlag )
            {
                callStackString ( L"OnChange_CB_MoveLimits", 0 );
                m_moveLimits = bFlag ? true : false;
            
                return 0;
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl