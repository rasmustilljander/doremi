#include <DoremiEngine/Network/Include/Adress.hpp>


namespace DoremiEngine
{
    namespace Network
    {
        Adress::Adress()
        {

        }

        Adress::Adress(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port)
        {
            SetIP(p_a, p_b, p_c, p_d);

            SetNetPort(p_port);

            ComposeAdress();
        }

        Adress::~Adress()
        {

        }

        void Adress::SetIP(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d)
        {
            // TODOCM maybe save IP parts, and compose string when needed
            // Save IP as string
            m_IPString = std::to_string(p_a) + "." + std::to_string(p_b) + "." 
                + std::to_string(p_c) + "." + std::to_string(p_d);

            // Moves bits so that the IP segments(255 max, one byte) to be placed after eachother
            int32_t ToConvertIP = ((p_a << 24) | (p_b << 16) | (p_c << 8) | p_d);

            // Performs big-endian on the IP (network necessary)
            m_IP = htonl(ToConvertIP);
        }

        void Adress::SetNetPort(uint16_t p_port)
        {
            m_port = p_port;
        }

        void Adress::ComposeAdress()
        {
            m_Adress = {0};
            m_Adress.sin_family = AF_INET;
            m_Adress.sin_port = m_port;
            m_Adress.sin_addr.s_addr = m_IP;
        }
    }
}