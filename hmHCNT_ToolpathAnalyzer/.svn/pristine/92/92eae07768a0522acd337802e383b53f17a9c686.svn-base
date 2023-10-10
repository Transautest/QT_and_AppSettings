#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
// keys
#include <Commands\tpanalysiskeys.h>

// warnings

#include <hcntcore/pargeomentity.h>
#include <geom/arc3d.h>
#include <geom/line3d.h>
#include <wstringimpl.h>
#include <fstream>
#include <stdlib.h>
#include <tchar.h>
#include <wchar.h>
#include <string.h>


using namespace omsl::command;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            void TPAnalysisModel::fillToolpathPoints ( ToolpathSegmentPtr& pgep, int sel )
            {
                if ( !pgep.Get () )
                {
                    s_data.m[sel].pointVec.clear ();
                    s_data.m[sel].typeVec.clear ();
                    {
                        return;
                    }
                }

                if ( pgep.Get () == s_data.m[sel].m_pTpSeg.Get () )
                {
                    return;
                }

                s_data.m[sel].m_pTpSeg = pgep;

                s_data.m[sel].pointVec.clear ();
                s_data.m[sel].typeVec.clear ();

                omsl::ULong i = 0, j=0, k=0;
                omsl::ULong size = pgep->Size ();
                if ( size )
                {
                    s_data.m[sel].specialIndex [analyseData::dai_SXS] = 0;
                    s_data.m[sel].specialIndex [analyseData::dai_SXC] = 0;
                    s_data.m[sel].specialIndex [analyseData::dai_SXP1] = 0;
                    s_data.m[sel].specialIndex [analyseData::dai_SXE] = (int) size;
                    for ( i = 0; i < size; i++ )
                    {
                        const bgeo::Coord3d* pC = pgep->GetPoint ( i );
                        s_data.m[sel].pointVec.push_back ( *pC );
                        const omsl::geom::Curve3d* pc = pgep->GetCurve ( i );
                        if ( pc != nullptr )
                        {
                            j++;
                            const omsl::geom::Arc3d* pArc = ( const omsl::geom::Arc3d* ) pc->Cast ( L"Arc3d" );

                            if ( pArc != nullptr )
                            {
                                k++;
                                s_data.m[sel].points [analyseData::dai_CTR2] = pArc->Center ();
                                s_data.m[sel].typeVec.push_back ( analyseData::dai_tArc );
                            }
                            else if ( pc->IsLinear () )
                            {
                                s_data.m[sel].typeVec.push_back ( analyseData::dai_tLine );
                            }
                            else
                            {
                                s_data.m[sel].typeVec.push_back ( analyseData::dai_tCurve );
                            }
                        }
                        else
                            s_data.m[sel].typeVec.push_back ( analyseData::dai_tUnknown );

                        bgeo::Point3d p ( *pC );
                        p.SetError ( s_data.m[sel].points [analyseData::dai_SXP1].Error () );
                        if ( s_data.m[sel].points [analyseData::dai_SXP1].IsEquiv ( p ) == omsl::True )
                        {
                            if ( checkTPAMode (sel) == tpanalysis::cm_Movement )
                                s_data.m[sel].specialIndex [analyseData::dai_SXP1] = (int)i+1;
                            else
                                s_data.m[sel].specialIndex [analyseData::dai_SXP1] = (int)i;
                        }
                    }

//                    s_data.specialIndex [s_data.dai_1stSeg] = 0; // end of 1stsegment
//                    s_data.specialIndex [s_data.dai_3rdSeg] = s_data.specialIndex [analyseData::dai_SXS] + s_data.dai_2ndSegmentSize + 1;
                }
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
