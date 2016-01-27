#pragma once

namespace DoremiEditor
{
    namespace Core
    {
        struct MessageHeader
        {
            MessageHeader()
            {
                nodeType = 0;
                messageType = 0;
                msgConfig = 0;
                byteTotal = 0;
                byteSize = 0;
                bytePadding = 0;
            }

            int nodeType;
            int messageType;
            int msgConfig;
            size_t byteTotal;
            size_t byteSize;
            size_t bytePadding;
        };
    }
}