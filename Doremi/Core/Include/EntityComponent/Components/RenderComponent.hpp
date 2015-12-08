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
            int data = 0;
            float floatData = 0;
        };
        struct Example2Component
        {
            ExampleComplexStruct complexStruct;
        };
    }
}