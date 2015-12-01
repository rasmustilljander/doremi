#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Only intended as an example of what a component might look like
        Should not be used anywhere. Could be removed once exmaples are no longer necessary*/
        struct ExampleComplexStruct
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