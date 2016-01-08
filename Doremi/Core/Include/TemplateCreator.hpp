#pragma once

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        class TemplateCreator
        {
            public:
            /**
                TODOCM doc
            */
            static TemplateCreator* GetInstance();

            /**
                TODOCM doc
            */
            void CreateTemplatesForClient(const DoremiEngine::Core::SharedContext& sharedContext);

            /**
                TODOCM doc
            */
            void CreateTemplatesForServer(const DoremiEngine::Core::SharedContext& sharedContext);

            private:
            /**
                TODOCM doc
            */
            TemplateCreator();

            /**
                TODOCM doc
            */
            ~TemplateCreator();

            /**
                TODOCM doc
            */
            static TemplateCreator* m_singleton;
        };
    }
}