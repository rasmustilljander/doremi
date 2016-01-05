#pragma once
namespace Utility
{
    namespace MemoryManager
    {
        struct MemoryMarker
        {
            public:
            MemoryMarker() : adress(nullptr), manager(nullptr) {}
            MemoryMarker(void* p_adress, void* p_manager) : adress(p_adress), manager(p_manager) {}
            ~MemoryMarker() {}

            /**
                Verify that this is a marker for the this manager.
            */
            bool VerifyManager(void* p_manager) const { return (p_manager == manager); }

            /**
                Get the adress to rollback to.
            */
            void* GetMarkerAdress() const { return adress; }
            private:
            void* adress;
            void* manager;
        };
    }
}