#pragma once

// om::ui
#include "ui/atl/omUiAtlLayoutResizable.h"

namespace om {
    namespace ast {

        //! Returns the registry key where the layout settings should be stored
        /*!
         *  \param key Reference to the registry key.
         *  \param wnd Not used.
         *  \param regPath Absolute or relative registry path. If an absolute
         *    path is passed (starting with a backslash) this one will be used.
         *    If a relative path is passed, it will be used to extend the path
         *    which is returned by GetModule().GetAppSectionKey(_T("Dialogs").
         *    In either case the registry key will be below HKEY_CURRENT_USER.
         *  \param templateID Resource id of the dialog/control from
         *    which the layout should be stored. Will be used as a suffix
         *    to the registry path
         *  \return
         */
        bool GetLayoutIORegistryKeyImpl(ATL::CRegKey &key, ATL::CWindow &wnd,
            LPCTSTR regPath, UINT templateId);

        OM_UI_ATL_LAYOUT_DEFINE_REGISTRY_IO_POLICY(ResizableIOPolicy, GetLayoutIORegistryKeyImpl)

        template<class T>
        struct Resizable {
            typedef om::ui::atl::layout::Resizable<T, ResizableIOPolicy<T> > Type;
        };

    } // namespace ast
} // namespace om
