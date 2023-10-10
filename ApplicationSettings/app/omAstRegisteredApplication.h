#pragma once

// std
#include <vector>

// boost
#include "boost/shared_ptr.hpp"

// om
#include "utils/omTString.h"

// om::ui
#include "ui/atl/omUiAtlHandle.h"

// om::ast
#include "omAstDefines.h"

namespace om {
    namespace ast {

        class RegisteredApplication {
        public:
            RegisteredApplication(
                SettingsType settingsType,
                ConfigurationType configurationType,
                const om::tstring &key,
                const om::tstring &name,
                const om::tstring &executable,
                const om::tstring &arguments);
            RegisteredApplication(
                SettingsType settingsType,
                ConfigurationType configurationType,
                const om::tstring &key,
                const om::tstring &name,
                const om::tstring &call);
            RegisteredApplication(const RegisteredApplication &other);
            ~RegisteredApplication();

            RegisteredApplication &operator=(const RegisteredApplication &other);
            void Swap(RegisteredApplication &other);

            bool operator<(const RegisteredApplication &other) const;
            bool operator==(const RegisteredApplication &other) const;

            SettingsType GetSettingsType() const { return m_settingsType; }
            void SetSettingsType(SettingsType type) { m_settingsType = type; }

            ConfigurationType GetConfigurationType() const { return m_configurationType; }
            void SetConfigurationType(ConfigurationType type) { m_configurationType = type; }

            const om::tstring &GetKey() const { return m_key; }
            void SetKey(const om::tstring &key) { m_key = key; }

            const om::tstring &GetName() const { return m_name; }
            void SetName(const om::tstring &name) { m_name = name; }

            const om::tstring &GetExecutable() const { return m_executable; }
            void SetExecutable(const om::tstring &exe) { m_executable = exe; }

            const om::tstring &GetArguments() const { return m_arguments; }
            void SetArguments(const om::tstring &args) { m_arguments = args; }

            HICON GetIcon() const;
            om::tstring GetCall() const;

        private:
            SettingsType m_settingsType;
            ConfigurationType m_configurationType;

            om::tstring m_key;
            om::tstring m_name;
            om::tstring m_executable;
            om::tstring m_arguments;

            mutable om::ui::atl::IconHandle m_icon;
        };

        typedef boost::shared_ptr<RegisteredApplication> RegisteredApplicationPtr;
        typedef std::vector<RegisteredApplicationPtr> RegisteredApplications;

        inline int CompareRegisteredApplicationKey(LPCTSTR key1, LPCTSTR key2)
        {
            return _tcsicmp(key1, key2);
        }

        struct FindRegisteredApplicationByKey {
            FindRegisteredApplicationByKey(LPCTSTR key) :
                m_key(key)
            {
            }

            bool operator()(const RegisteredApplicationPtr &pApp) const
            {
                return 0 == CompareRegisteredApplicationKey(pApp->GetKey().c_str(), m_key);
            }

        private:
            LPCTSTR m_key;
        };

        struct LessRegisteredApplicationByKey {
            bool operator()(const RegisteredApplicationPtr &pApp, LPCTSTR key) const
            {
                return _tcsicmp(pApp->GetKey().c_str(), key) < 0;
            }

            bool operator()(LPCTSTR key, const RegisteredApplicationPtr &pApp) const
            {
                return _tcsicmp(key, pApp->GetKey().c_str()) < 0;
            }
        };

        struct LessRegisteredApplication {
            bool operator()(const RegisteredApplicationPtr &pApp1, const RegisteredApplicationPtr &pApp2) const
            {
                return *pApp1 < *pApp2;
            }
        };

    } // namespace ast
} // namespace om
