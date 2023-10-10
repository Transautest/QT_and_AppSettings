#pragma once

// om::ast
#include "omAstRegisteredApplication.h"

namespace om {
    namespace ast {

        class ApplicationSettings {
        public:
            ApplicationSettings();
            ApplicationSettings(const ApplicationSettings &other);
            ~ApplicationSettings();

            ApplicationSettings &operator=(const ApplicationSettings &other);
            void swap(ApplicationSettings &other);

            bool Init(const om::tstring &version);
            bool Save(const om::tstring &version);

            bool SetApp(RegisteredApplicationPtr pApp);
            bool EraseApp(RegisteredApplicationPtr pApp);

            RegisteredApplicationPtr FindApp(LPCTSTR key, SettingsType type = kUnknownSettingsType) const;

            void GetUserRelevantApps(RegisteredApplications &apps) const;
            void GetMachineRelevantApps(RegisteredApplications &apps) const;

        private:
            void UpdateChangedFlags(RegisteredApplication &app);

            RegisteredApplications m_apps;

            bool m_currentVersionUserAppsChanged;
            bool m_currentVersionMachineAppsChanged;
            bool m_commonUserAppsChanged;
            bool m_commonMachineAppsChanged;
        };

    } // namespace ast
} // namespace om
