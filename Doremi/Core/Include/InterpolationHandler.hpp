#pragma once

namespace Doremi
{
    namespace Core
    {
        class InterpolationHandler
        {
            public:
            /**
                TODOCM doc
            */
            static InterpolationHandler* GetInstance();

            /**
                TODOCM doc
            */
            void InterpolateFrame(double p_alpha);

            private:
            /**
                TODOCM doc
            */
            InterpolationHandler();

            /**
                TODOCM doc
            */
            ~InterpolationHandler();

            /**
                TODOCM doc
            */
            static InterpolationHandler* m_singleton;
        };
    }
}