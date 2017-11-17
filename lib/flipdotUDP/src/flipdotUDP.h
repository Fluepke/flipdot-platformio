/**
 * This is an adapter from UDP to flipdot
 */

#ifndef FLIPDOTUDP_H
#define FLIPDOTUDP_H

#include <WiFiUdp.h>
#include <flipdot.h>

class FlipdotUDP {

    public:
        static unsigned int Port;
        FlipdotUDP(Flipdot *t_flipdot);
        void begin(unsigned int t_port = Port);
        void poll();
    private:
        uint16_t m_framebuffer[115];
        WiFiUDP m_udp;
        Flipdot *m_flipdot;
};

#endif
