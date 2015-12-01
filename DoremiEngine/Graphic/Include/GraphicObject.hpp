#pragma once
namespace DoremiEngine
{
    namespace Graphic
    {
        class GraphicObject
        {
        public:
            GraphicObject(const int& p_meshID, const int& p_materialID) : m_meshID(p_meshID), m_materialID(p_materialID) 
            {
            };
            GraphicObject();
            ~GraphicObject();
        private:
            int m_meshID;
            int m_materialID;
        };
    }
}
