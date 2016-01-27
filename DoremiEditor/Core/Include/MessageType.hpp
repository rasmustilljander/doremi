#pragma once

namespace DoremiEditor
{
    namespace Core
    {
        enum MessageType
        {
            msgAdded = 1,
            msgEdited = 2,
            msgDeleted = 3,
            msgRenamed = 4,
            msgSwitched = 5
        };
    }
}