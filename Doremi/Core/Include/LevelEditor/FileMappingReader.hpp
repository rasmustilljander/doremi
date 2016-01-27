#pragma once
#include <Windows.h>

namespace Doremi
{
    namespace Core
    {
        struct MessageHeader;
        struct MeshMessage;
        struct NameMessage;
        struct TransformMessage;
        struct MaterialMessage;
        struct LightMessage;
        struct CameraMessage;

        class FileMappingReader
        {
        public:
            FileMappingReader();
            virtual ~FileMappingReader();

            /**
            Intializes the filemapping reader with a path.
            */
            void Initialize();

        protected:
            HANDLE m_essageFileMap = nullptr;
            LPVOID m_messageData = nullptr;
            const size_t m_size = 1 << 13;

            HANDLE m_infoFileMap = nullptr;
            LPVOID m_infoData = nullptr;
            const size_t m_infoSize = 1 << 6;

            size_t m_localTail = 0; // hur långt ifrån starten i bytes den är på filemapen med meddelanden
            int m_delay = 10; // hur länge den ska sleepa mellan varje läsnings försök

            void MeshChange(MessageHeader mh, MeshMessage* mm); // lägger till ett nytt objekt om det inte redan finns eller updaterar en gammal, tar
            // hand om den aktualla meshen
            void MeshAdded(MessageHeader mh, MeshMessage* mm);
            void MeshRenamed(MessageHeader mh, NameMessage* mm);

            void TransformAdded(MessageHeader mh, TransformMessage* mm);
            void TransformChange(MessageHeader mh, TransformMessage* mm);
            void TransformDeleted(MessageHeader mh, NameMessage* mm);
            void TransformRenamed(MessageHeader mh, NameMessage* mm);

            void MaterialAdded(MessageHeader mh, MaterialMessage* mm);
            void MaterialChange(MessageHeader mh, MaterialMessage* mm);
            void MaterialDeleted(MessageHeader mh, NameMessage* mm);
            void MaterialRenamed(MessageHeader mh, NameMessage* mm);

            void LightAdded(MessageHeader mh, LightMessage* mm);
            void LightChange(MessageHeader mh, LightMessage* mm);
            void LightRenamed(MessageHeader mh, NameMessage* mm);

            void CameraAdded(MessageHeader mh, CameraMessage* mm);
            void CameraChange(MessageHeader mh, CameraMessage* mm);
            // ingen delete coz på transformen
            void CameraRenamed(MessageHeader mh, NameMessage* mm);
            void CameraSwitch(MessageHeader mh, CameraMessage* mm);
        };
    }
}