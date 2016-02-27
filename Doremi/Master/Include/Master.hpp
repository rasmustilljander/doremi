#pragma once
#include <Doremi/Core/Include/GameCore.hpp>
#include <vector>

namespace Doremi
{
    namespace Core
    {
        class Manager;
        class EntityInterface;
    }
}
namespace Doremi
{

    class MasterMain : public Core::GameCore
    {
    public:
        /**
        Constructor
        */
        MasterMain();

        /**
        Destructor
        */
        virtual ~MasterMain();

        /**
        doc
        */
        void Start();

    private:
        /**
        TODOCM doc
        */
        void Initialize();

        /**
        TODOCM doc
        */
        void Run();

        /**
        TODOCM doc
        */
        void Update(double p_deltaTime);

        /**
        TOODCM doc
        */
        std::vector<Core::Manager*> m_managers;
    };
}