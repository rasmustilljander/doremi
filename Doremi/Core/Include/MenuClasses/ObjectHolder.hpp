#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>

#include <vector>

namespace Doremi
{
    namespace Core
    {
        class ObjectHolder
        {
        public:
            ObjectHolder(const std::vector<ScreenObject>& p_objects);
            ObjectHolder();
            virtual ~ObjectHolder();
            void SetCurrentObject(const int& p_objectIndex);
            const ScreenObject& GetCurrentObject();

        private:
            std::vector<ScreenObject> m_objects;
            int m_currentObject;
        };
    }
}