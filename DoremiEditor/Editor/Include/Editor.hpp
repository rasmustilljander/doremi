#pragma once
#include <Doremi/Core/Include/GameCore.hpp>
#include <vector>

#include <DoremiEditor/Editor/Include/TA files/MayaLoader.h>
namespace Doremi
{
    namespace Core
    {
        class Manager;
        class EntityInterface;
    }
}
namespace DoremiEditor
{
    class EditorMain : public Doremi::Core::GameCore
    {
    public:
        /**
        Constructor
        */
        EditorMain();

        /**
        Destructor
        */
        virtual ~EditorMain();

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

        MayaLoader* m_mayaLoader;
    };
}