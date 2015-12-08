#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Should contain data on what mech and what material to use
        */
        struct RenderComponent
        {
            size_t meshID;
            size_t materialID;
            RenderComponent(int p_meshID, int p_materialID) : meshID(p_meshID), materialID(p_materialID)
            {
            }
            RenderComponent() : meshID(0), materialID(0)
            {
            }
        };

    }
}