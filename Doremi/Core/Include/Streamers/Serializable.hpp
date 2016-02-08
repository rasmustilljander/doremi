#pragma once
#include <Doremi/Core/Include/Streamers/Streamer.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
            Abstract class for objects that are needed to be abele to write or read to a stream
        */
        class Serializable
        {
        public:
            /**
                Write object to stream
            */
            virtual void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) = 0;

            /**
                Read object from stream
            */
            virtual void Read(Streamer* p_streamer, uint32_t& op_bitsRead) = 0;
        };
    }
}