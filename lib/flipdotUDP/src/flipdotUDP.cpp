#include "flipdotUDP.h"

unsigned int FlipdotUDP::Port = 1337;

FlipdotUDP::FlipdotUDP(Flipdot *t_flipdot)
{
    m_flipdot = t_flipdot;
}

void FlipdotUDP::begin(unsigned int t_port)
{
    m_udp.begin(t_port);
}

void FlipdotUDP::poll()
{
    int packetSize = m_udp.parsePacket();
    if (packetSize == 0) {
        return;
    }
    if (packetSize != 230) {
        m_udp.beginPacket(m_udp.remoteIP(), m_udp.remotePort());
        m_udp.write("https://github.com/fluepke/flipdot-platformio/");
        m_udp.endPacket();
        return;
    }

    for (uint8_t i=0; i<115; i++) {
        m_framebuffer[i] = m_udp.read() << 8 | m_udp.read();
    }
    m_udp.beginPacket(m_udp.remoteIP(), m_udp.remotePort());
    m_udp.write("ACK");
    m_udp.endPacket();

    m_flipdot->renderFrame(m_framebuffer);
}
