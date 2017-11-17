#include "flipdot.h"

uint8_t Flipdot::ColumnHeight = 16;

Flipdot::Flipdot(uint8_t t_boardConfiguration[], uint8_t t_boardCount, RenderingFlags t_renderingFlags)
{
    m_boardCount = t_boardCount;
    if (m_boardCount < 1 || m_boardCount > 5) {
        return; // Invalid board count
    }
    m_boardConfiguration = t_boardConfiguration;
}

void Flipdot::setRenderingFlags(RenderingFlags t_renderingFlags)
{
    m_renderingFlags = t_renderingFlags;
}

Flipdot::RenderingFlags Flipdot::getRenderingFlags()
{
    return m_renderingFlags;
}

void Flipdot::begin()
{
    pinMode(Flipdot::IOPorts::SHIFT_RCK_PIN, OUTPUT);
    pinMode(Flipdot::IOPorts::SHIFT_OE_PIN, OUTPUT);
    pinMode(Flipdot::IOPorts::SHIFT_SCL_PIN, OUTPUT);
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2); // proofed to be working
    // SPI.setFrequency(80000000L); // not yet proofed to be working
    // GPMUX |= (1 << 9);
    m_columnBuffer = 0;
    m_controlBuffer = 0;
    flushRegisters();
    digitalWrite(Flipdot::IOPorts::SHIFT_SCL_PIN, HIGH);
    digitalWrite(SHIFT_OE_PIN, HIGH); // turn off shift register output
}

void Flipdot::flushRegisters() // TODO: use ICACHE_RAM_ATTR
{
    #if DEBUG_IO
        DEBUG_PORT << _BIN(m_controlBuffer) << " | " << _BIN(m_columnBuffer) << endl;
    #endif
    SPI.write(m_controlBuffer);
    SPI.write16(m_columnBuffer);
    pulseHigh(Flipdot::IOPorts::SHIFT_RCK_PIN);
}

void Flipdot::renderFrame(uint16_t t_frameBuffer[])
{
    uint8_t x=0;

    for (uint8_t board=0; board<m_boardCount; board++) {
        initializePanel(board);
        for (uint8_t boardX=0; boardX<m_boardConfiguration[board]; boardX++, x++) {
            renderColumn(t_frameBuffer[x], board);
        }
    }
}

/*uint8_t Flipdot::selectBoard(int board, int value) {
    if (!value)
        return 0b11111;
    switch (board) {
        case 0:
            return 0b11111;
        case 1:
            return 0b10111;
        case 2:
            return 0b11011;
        case 3:
            return 0b11101;
        case 4:
            return 0b11110;
        default:
            return 0b11111;
    }
}*/

void Flipdot::renderColumn(uint16_t column, uint8_t t_boardNumber)
{
    #if DEBUG
        DEBUG_PORT << "Rendering on board " << t_boardNumber << " content " << _BIN(column) << endl;
    #endif
    uint8_t SELECT = ControlRegister::SELECT1 - t_boardNumber;

    delay(2);

    // State J
    m_controlBuffer =  (1 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    delayMicroseconds(200);

    // State K
    m_controlBuffer =  (1 << CLEAR) | (0 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    delayMicroseconds(20);

    // State L
    m_controlBuffer =  (1 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    // State M
    m_columnBuffer = column;
    #if DEBUG
        DEBUG_PORT << _BIN(column) << endl;
    #endif
    flushRegisters();

    delay(2);

    // State N
    m_columnBuffer = 0;
    flushRegisters();

    // State O
    m_controlBuffer =  (0 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    delayMicroseconds(200);


    // State P
    m_controlBuffer =  (1 << CLEAR) | (0 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    delayMicroseconds(10);

    // State Q
    m_controlBuffer =  (1 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    delayMicroseconds(40);

    // State R
    m_controlBuffer =  (0 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();
}

void Flipdot::initializePanel(uint8_t t_boardNumber)
{
    #if DEBUG
        DEBUG_PORT << "Initializing board " << t_boardNumber << endl;
    #endif

    // TODO: State G and H might be merged
    // TODO: State D and E might be merged

    /* STATE A */
    m_columnBuffer = 0; // Turn off all high side pixel drivers
    m_controlBuffer = (0 << CLEAR) | (1 << CLOCK) | (0 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();
    digitalWrite(Flipdot::IOPorts::SHIFT_OE_PIN, LOW); // Turn on shift register output

    delay(30);

    /* STATE B */
    m_controlBuffer = (1 << CLEAR) | (1 << CLOCK) | (0 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters(); // turn off clear *before* turning on high side drivers
    m_columnBuffer = 0b1111111111111111; // Turn on all high side drivers
    flushRegisters();

    delayMicroseconds(200);

    /* STATE C */
    m_columnBuffer = 0;
    flushRegisters(); // turn off high side drivers *before* turning on CLEAR
    m_controlBuffer =  (0 << CLEAR) | (1 << CLOCK) | (0 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    delayMicroseconds(100);

    /* STATE D */
    m_controlBuffer = (0 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    /* STATE E */
    m_controlBuffer =  (0 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 0);
    flushRegisters();

    delayMicroseconds(200);

    /* STATE F */
    m_controlBuffer = (1 << CLEAR) | (0 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 0);
    flushRegisters();

    delayMicroseconds(20);

    /* STATE G */
    m_controlBuffer = (1 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 0);
    flushRegisters();

    /* STATE H */
    m_controlBuffer = (1 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();

    delayMicroseconds(30);

    /* STATE I */
    m_controlBuffer = (0 << CLEAR) | (1 << CLOCK) | (1 << RESET) | selectBoard(t_boardNumber, 1);
    flushRegisters();
}
