
#define SIN_LEN 30
static const uint8_t sin_table[] PROGMEM =
{
    0,  28,  54,  77,  98, 113, 123, 128, 126, 119, 106,  88,  66,  41,
  14,  14,  41,  66,  88, 106, 119, 126, 128, 123, 113,  98,  77,  54,
  28,   0
};

inline static uint8_t sinSample(uint16_t i) {
    uint16_t newI = i % (SIN_LEN/2);
    newI = (newI >= (SIN_LEN/4)) ? (SIN_LEN/2 - newI -1) : newI;
    uint8_t sine = pgm_read_byte(&sin_table[newI]);
    return (i >= (SIN_LEN/2)) ? (255 - sine) : sine;
}
