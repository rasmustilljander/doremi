// Project specific
#include <Doremi/Core/Include/MenuClasses/Text.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>


// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;
        Text::Text(DoremiEngine::Graphic::MaterialInfo* p_textMaterial, DirectX::XMFLOAT2 p_textSize, XMFLOAT2 p_position, XMFLOAT2 p_origo,
                   XMFLOAT2 p_tableCharSize, XMFLOAT2 p_tableOffset)
            : m_textMaterial(p_textMaterial), m_textSize(p_textSize), m_position(p_position), m_origo(p_origo), m_tableCharSize(p_tableCharSize), m_tableOffset(p_tableOffset)
        {
        }

        Text::Text() {}

        Text::~Text() {}

        void Text::SetText(const DoremiEngine::Core::SharedContext& p_sharedContext, std::string p_text)
        {
            // If we don't already got it
            if(m_text == "")
            {
                DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

                size_t textLength = p_text.length();

                // TODO reuse the old ones
                for(size_t i = 0; i < textLength; i++)
                {

                    char t_curChar = p_text.at(i);

                    t_curChar -= 32;

                    DoremiEngine::Graphic::SpriteData t_data;
                    uint32_t y = floor(static_cast<float>(t_curChar) / 16.0f);
                    uint32_t x = t_curChar - 16 * y;

                    t_data.txtPos = XMFLOAT2(m_tableCharSize.x * static_cast<float>(x), m_tableCharSize.y * static_cast<float>(y));
                    t_data.txtSize = m_tableCharSize;

                    t_data.position = XMFLOAT2(m_position.x + m_origo.x + static_cast<float>(i) * m_textSize.x * 1.0f, m_position.y);
                    t_data.halfsize = m_textSize;
                    t_data.origo = XMFLOAT2(0.0f, 0.0f);


                    m_textInfo.push_back(t_meshManager.BuildSpriteInfo(t_data));
                }

                m_text = p_text;
            }
        }

        void Text::UpdateText(const DoremiEngine::Core::SharedContext& p_sharedContext, const std::string& p_text)
        {
            // If the text isn't the same
            if(m_text != p_text)
            {
                DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

                int32_t t_difference = m_text.size() - p_text.size();

                // If we got too many, we delete the excessive texts
                if(t_difference > 0)
                {
                    for(size_t i = m_text.size() - t_difference; i < m_text.size(); i++)
                    {
                        delete m_textInfo[i];
                    }
                    m_textInfo.resize(p_text.size());
                }
                else if(t_difference < 0) // If we got to few we create new, no need to set values, we update on update position
                {
                    t_difference *= -1;
                    for(size_t i = 0; i < t_difference; i++)
                    {
                        DoremiEngine::Graphic::SpriteData t_data;

                        m_textInfo.push_back(t_meshManager.BuildSpriteInfo(t_data));
                    }
                }

                m_text = p_text;
                size_t length = m_text.size();
                for(size_t i = 0; i < length; i++)
                {
                    char t_curChar = p_text.at(i);

                    t_curChar -= 32;

                    // Calculate location on font field
                    DoremiEngine::Graphic::SpriteData& t_data = m_textInfo[i]->GetData();
                    uint32_t y = floor(static_cast<float>(t_curChar) / 16.0f);
                    uint32_t x = t_curChar - 16 * y;

                    t_data.txtPos = XMFLOAT2(m_tableCharSize.x * static_cast<float>(x), m_tableCharSize.y * static_cast<float>(y));
                    t_data.txtSize = m_tableCharSize;
                }
            }
        }

        void Text::UpdatePosition(XMFLOAT2 p_position)
        {
            m_position = p_position;
            size_t prevTextLength = m_textInfo.size();
            for(size_t i = 0; i < prevTextLength; i++)
            {
                char t_curChar = m_text.at(i);

                t_curChar -= 32;

                // Calculate location on font field
                DoremiEngine::Graphic::SpriteData& t_data = m_textInfo[i]->GetData();
                uint32_t y = floor(static_cast<float>(t_curChar) / 16.0f);
                uint32_t x = t_curChar - 16 * y;

                t_data.txtPos = XMFLOAT2(m_tableCharSize.x * static_cast<float>(x), m_tableCharSize.y * static_cast<float>(y));
                t_data.txtSize = m_tableCharSize;


                t_data.position = XMFLOAT2(m_position.x + m_origo.x + static_cast<float>(i) * m_textSize.x * 1.0f, p_position.y);
                t_data.halfsize = m_textSize;
                t_data.origo = XMFLOAT2(0.0f, 0.0f);
            }
        }

        void Text::DeleteText()
        {
            size_t length = m_textInfo.size();
            for(size_t i = 0; i < length; i++)
            {
                delete m_textInfo[i];
            }
            m_textInfo.clear();
            m_text = "";
        }
    }
}