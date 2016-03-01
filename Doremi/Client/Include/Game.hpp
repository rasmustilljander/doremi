#pragma once
#include <Doremi/Core/Include/GameCore.hpp>
#include <vector>
#include <DirectXMath.h>

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
        class Manager;
        class ScreenSpaceDrawer;
    }

    class GameMain : public Core::GameCore
    {
    public:
        /**
            Constructor
        */
        GameMain();

        /**
            Destructor
        */
        virtual ~GameMain();

        /**
            TODOCM doc
        */
        void Start();

        /**
        TODOCM doc
        */
        void Stop();

        /**
        TODOCM doc
        */
        void Run();


    private:
        /**
            TODOCM doc
        */
        void Initialize();

        /**
            Responsible for building the world.
        */
        void BuildWorld(const DoremiEngine::Core::SharedContext& sharedContext);


        /**
            TODOCM doc
        */
        void Update(double p_deltaTime);

        /**
            A method to send the draw to the correct path using the menustate
        */
        void Draw(double p_deltaTime);

        /**
        TODOCM doc
        */
        void UpdateGame(double p_deltaTime);

        void UpdateServerBrowser(double p_deltaTime);

        /**
            TODOCM doc
        */
        void DrawGame(double p_deltaTime);

        /**
            TOODCM doc
        */
        std::vector<Core::Manager*> m_managers;

        std::vector<Core::Manager*> m_serverBrowserManagers;

        /**
            TODOCM doc
        */
        std::vector<Core::Manager*> m_graphicalManagers;

        Core::ScreenSpaceDrawer* m_screenSpaceDrawer;

        DirectX::XMFLOAT2 m_screenRes;

        const DoremiEngine::Core::SharedContext* m_sharedContext;

    protected:
        void AddToManagerList(Core::Manager* p_manager);
        void AddToServerBrowserList(Core::Manager* p_manager);
        void AddToGraphicalManagerList(Core::Manager* p_manager);

        bool m_gameRunning;
    };
}