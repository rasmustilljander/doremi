#include "..\..\Include\MenuClasses\ObjectHolder.hpp"
#include <iostream>
namespace Doremi
{
    namespace Core
    {
        ObjectHolder::ObjectHolder(const std::vector<ScreenObject>& p_objects) : m_objects(p_objects), m_currentObject(0) {}
        ObjectHolder::ObjectHolder() {}
        ObjectHolder::~ObjectHolder() {}
        void ObjectHolder::SetCurrentObject(const int& p_objectIndex)
        {
            if(p_objectIndex >= 0 && p_objectIndex < m_objects.size())
            {
                m_currentObject = p_objectIndex;
            }
            else
            {
                std::cout << "Index for object holder is out of range" << std::endl;
            }
        }
        const ScreenObject& ObjectHolder::GetCurrentObject() { return m_objects[m_currentObject]; }
    }
}