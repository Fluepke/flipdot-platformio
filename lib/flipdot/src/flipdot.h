/**
 * This is the library intended to be used with the hardware revision 1 boards
 * labelled "FLÜPDOT" using shift registers and externally connected controller
 *
 */
#ifndef FLIPDOT_H
#define FLIPDOT_H

#define DEBUG false
#define DEBUG_IO false
#define DEBUG_PORT Serial

#include <Arduino.h>
#include <SPI.h>

#if DEBUG
    #include <Streaming.h>
#endif

#define pulseHigh(pin) digitalWrite(pin, HIGH); digitalWrite(pin, LOW);
#define selectBoard(board, value) ((0b11111) & ~(!value << (4 - board)))

class Flipdot {
    static uint8_t ColumnHeight;
    public:
        //! Control shift register mapping
        /*!
         |     7    |     6    |     5    |     4    |     3    |     2    |     1    |     0
         |  CLEAR   |   CLOCK  |   RESET  | Flipdot::ControlRegister::SELECT21 1 | Flipdot::ControlRegister::SELECT21 2 | Flipdot::ControlRegister::SELECT21 3 | Flipdot::ControlRegister::SELECT21 4 | Flipdot::ControlRegister::SELECT21 5 |
        */
        enum ControlRegister : uint8_t {
            CLEAR = 7,
            CLOCK = 6,
            RESET = 5,
            SELECT1 = 4,
            SELECT2 = 3,
            SELECT3 = 2,
            SELECT4 = 1,
            SELECT5 = 0,
        };

        enum RenderingFlags {
            NONE = 0,
            FLIP_VERTICAL = 1
        };

        enum IOPorts {
            SHIFT_SCL_PIN = D3,
            SHIFT_OE_PIN = D1,
            SHIFT_RCK_PIN = D2
        };

        //! Constructor
        /*!
          \param t_boardConfiguration an array with maximum size of 5, containing the board width for each Flipdot::ControlRegister::SELECT21 line
          \param t_renderingFlags Rendering options. Defaults to @see RenderingFlags::NONE .
        */
        Flipdot(uint8_t t_boardConfiguration[], uint8_t t_boardCount, RenderingFlags t_renderingFlags = RenderingFlags::NONE);

        void setRenderingFlags(RenderingFlags t_renderingFlags);
        RenderingFlags getRenderingFlags();

        //! Initializes the hardware required to drive a flipdot. Run once in setup.
        void begin();

        //! Writes contents of @see m_columnBuffer and @see m_controlBuffer to the shift registers.
        void flushRegisters();

        //! Writes the current framebuffer to the flipdot
        void renderFrame(uint16_t t_frameBuffer[]);

        //! Renders a single column
        void renderColumn(uint16_t column, uint8_t t_boardNumber);

        //! Flipdot::ControlRegister::SELECT21s and initializes a flipdot panel before rendering to it
        /*!
         * @param t_boardNumber The board (0-4) to Flipdot::ControlRegister::SELECT21.
         */
        void initializePanel(uint8_t t_boardNumber);

        //uint8_t selectBoard(int board, int value);

    private:
        uint8_t *m_boardConfiguration;
        uint8_t m_boardCount = 0;
        uint16_t m_columnBuffer = 0;
        uint8_t m_controlBuffer = 0;
        RenderingFlags m_renderingFlags = RenderingFlags::NONE;

        void initializePanel();
};


#endif
