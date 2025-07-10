// ES-exercise 06                                                            //
// Demo to initialize TFT-Display with ST7735R controller,                   //
// e.g. joy-it RB-TFT1.8-V2. 						     //
// configuration:  4-line serial interface, RGB-order: R-G-B,		     //

#include <SPI.h>
#include <string.h>
#define __AVR_ATmega2560__ 1
#if defined(__AVR_ATmega2560__)
  #include <SimpleTimer.h>
  #warning "__AVR_ATmega2560__ is defined"
#else
  #warning "__AVR_ATmega2560__ is NOT defined"
  #include <DueTimer.h>
#endif
//pin declarations
#define TFT_CS     10   //display: CS-pin
#define TFT_RST     9   //display: reset
#define TFT_DC      8   //display: Data/Command (D/C)

#if defined(__AVR_ATmega2560__)
 #define SS_SLAVE 	53
 //must be put into output mode; otherwise ATmega could assume
 //to be set into slave mode but SPI-lib doesn't support this
 //mode. So it breaks SPI-lib. Configured as output, the pin
 //can be used as normal output.
#endif



#define TFT_DC_HIGH()           digitalWrite(TFT_DC, HIGH)
#define TFT_DC_LOW()            digitalWrite(TFT_DC, LOW)
#define TFT_CS_HIGH()           digitalWrite(TFT_CS, HIGH)
#define TFT_CS_LOW()            digitalWrite(TFT_CS, LOW)


//SPI-Settings
#define SPI_DEFAULT_FREQ   1e6      ///< Default SPI data clock frequency
SPISettings settingsTFT(SPI_DEFAULT_FREQ, MSBFIRST, SPI_MODE0);

const int FB_X = 92;
const int FB_Y = 64;           //VCOM Voltage setting

uint8_t framebuffer[FB_X][FB_Y];

//TFT-area of 128 x 160 (1.8") TFT
const uint8_t FIRST_COL = 1;
const uint8_t FIRST_ROW = 2;
const uint8_t LAST_COL = FB_X;
const uint8_t LAST_ROW = FB_Y;

//TFT's commands
const uint8_t NOP = 0x00;               // no Operation
const uint8_t SWRESET = 0x01;           // Software reset
const uint8_t SLPOUT = 0x11;            //Sleep out & booster on
const uint8_t DISPOFF = 0x28;           //Display off
const uint8_t DISPON = 0x29;            //Display on
const uint8_t CASET = 0x2A;             //Column adress set
const uint8_t RASET = 0x2B;             //Row adress set
const uint8_t RAMWR = 0x2C;             //Memory write
const uint8_t MADCTL = 0x36;            //Memory Data Access Control
const uint8_t COLMOD = 0x3A;            //RGB-format, 12/16/18bit
const uint8_t INVOFF = 0x20;            // Display inversion off
const uint8_t INVON = 0x21;             // Display inversion on
const uint8_t INVCTR = 0xB4;            //Display Inversion mode control
const uint8_t NORON = 0x13;             //Partial off (Normal)

const uint8_t PWCTR1 = 0xC0;            //Power Control 1
const uint8_t PWCTR2 = 0xC1;            //Power Control 2
const uint8_t PWCTR3 = 0xC2;            //Power Control 3
const uint8_t PWCTR4 = 0xC3;            //Power Control 4
const uint8_t PWCTR5 = 0xC4;            //Power Control 5
const uint8_t VMCTR1 = 0xC5;



unsigned char font[95][6] =
{
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // space
{ 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 }, // !
{ 0x00, 0x07, 0x00, 0x07, 0x00, 0x00 }, // "
{ 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00 }, // #
{ 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00 }, // $
{ 0x23, 0x13, 0x08, 0x64, 0x62, 0x00 }, // %
{ 0x36, 0x49, 0x55, 0x22, 0x50, 0x00 }, // &
{ 0x00, 0x00, 0x07, 0x00, 0x00, 0x00 }, // '
{ 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00 }, // (
{ 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00 }, // )
{ 0x0A, 0x04, 0x1F, 0x04, 0x0A, 0x00 }, // *
{ 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 }, // +
{ 0x00, 0x50, 0x30, 0x00, 0x00, 0x00 }, // ,
{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 }, // -
{ 0x00, 0x60, 0x60, 0x00, 0x00, 0x00 }, // .
{ 0x20, 0x10, 0x08, 0x04, 0x02, 0x00 }, // slash
{ 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 }, // 0
{ 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 }, // 1
{ 0x42, 0x61, 0x51, 0x49, 0x46, 0x00 }, // 2
{ 0x21, 0x41, 0x45, 0x4B, 0x31, 0x00 }, // 3
{ 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 }, // 4
{ 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 }, // 5
{ 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00 }, // 6
{ 0x03, 0x71, 0x09, 0x05, 0x03, 0x00 }, // 7
{ 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 }, // 8
{ 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00 }, // 9
{ 0x00, 0x36, 0x36, 0x00, 0x00, 0x00 }, // :
{ 0x00, 0x56, 0x36, 0x00, 0x00, 0x00 }, // ;
{ 0x08, 0x14, 0x22, 0x41, 0x00, 0x00 }, // <
{ 0x14, 0x14, 0x14, 0x14, 0x14, 0x00 }, // =
{ 0x00, 0x41, 0x22, 0x14, 0x08, 0x00 }, // >
{ 0x02, 0x01, 0x51, 0x09, 0x06, 0x00 }, // ?
{ 0x32, 0x49, 0x79, 0x41, 0x3E, 0x00 }, // @
{ 0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00 }, // A
{ 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 }, // B
{ 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 }, // C
{ 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 }, // D
{ 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 }, // E
{ 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00 }, // F
{ 0x3E, 0x41, 0x41, 0x49, 0x7A, 0x00 }, // G
{ 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 }, // H
{ 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00 }, // I
{ 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00 }, // J
{ 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 }, // K
{ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 }, // L
{ 0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00 }, // M
{ 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00 }, // N
{ 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 }, // O
{ 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 }, // P
{ 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00 }, // Q
{ 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00 }, // R
{ 0x26, 0x49, 0x49, 0x49, 0x32, 0x00 }, // S
{ 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00 }, // T
{ 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 }, // U
{ 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 }, // V
{ 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00 }, // W
{ 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 }, // X
{ 0x07, 0x08, 0x70, 0x08, 0x07, 0x00 }, // Y
{ 0x61, 0x51, 0x49, 0x45, 0x43, 0x00 }, // Z
{ 0x00, 0x7F, 0x41, 0x41, 0x00, 0x00 }, // [
{ 0x02, 0x04, 0x08, 0x10, 0x20, 0x00 }, // backslash
{ 0x00, 0x41, 0x41, 0x7F, 0x00, 0x00 }, // ]
{ 0x04, 0x02, 0x01, 0x02, 0x04, 0x00 }, // ^
{ 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 }, // _
{ 0x00, 0x01, 0x02, 0x04, 0x00, 0x00 }, // `
{ 0x20, 0x54, 0x54, 0x54, 0x78, 0x00 }, // a
{ 0x7F, 0x48, 0x44, 0x44, 0x38, 0x00 }, // b
{ 0x38, 0x44, 0x44, 0x44, 0x20, 0x00 }, // c
{ 0x38, 0x44, 0x44, 0x48, 0x7F, 0x00 }, // d
{ 0x38, 0x54, 0x54, 0x54, 0x18, 0x00 }, // e
{ 0x08, 0x7E, 0x09, 0x01, 0x02, 0x00 }, // f
{ 0x08, 0x54, 0x54, 0x54, 0x3C, 0x00 }, // g
{ 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00 }, // h
{ 0x00, 0x48, 0x7D, 0x40, 0x00, 0x00 }, // i
{ 0x20, 0x40, 0x44, 0x3D, 0x00, 0x00 }, // j
{ 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00 }, // k
{ 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00 }, // l
{ 0x7C, 0x04, 0x78, 0x04, 0x78, 0x00 }, // m
{ 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00 }, // n
{ 0x38, 0x44, 0x44, 0x44, 0x38, 0x00 }, // o
{ 0x7C, 0x14, 0x14, 0x14, 0x08, 0x00 }, // p
{ 0x08, 0x14, 0x14, 0x18, 0x7C, 0x00 }, // q
{ 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00 }, // r
{ 0x48, 0x54, 0x54, 0x54, 0x20, 0x00 }, // s
{ 0x04, 0x3F, 0x44, 0x40, 0x20, 0x00 }, // t
{ 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00 }, // u
{ 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00 }, // v
{ 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00 }, // w
{ 0x44, 0x28, 0x10, 0x28, 0x44, 0x00 }, // x
{ 0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00 }, // y
{ 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00 }, // z
{ 0x00, 0x08, 0x36, 0x41, 0x00, 0x00 }, // {
{ 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00 }, // |
{ 0x00, 0x41, 0x36, 0x08, 0x00, 0x00 }, // }
{ 0x10, 0x08, 0x08, 0x10, 0x08, 0x00 } // ~
};

//global variables
uint8_t invState = 0;


void TFTwriteCommand(uint8_t cmd){
    TFT_DC_LOW();
     SPI.transfer(cmd);
    TFT_DC_HIGH();
}

void TFTwrite_saCommand(uint8_t cmd){
  SPI.beginTransaction(settingsTFT);
  TFT_CS_LOW();
    TFT_DC_LOW();
     SPI.transfer(cmd);
    TFT_DC_HIGH();
  TFT_CS_HIGH();
  SPI.endTransaction();
}

void TFTwriteWindow(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye) {
        //test weather parameters stay within address ranges;  should be implemented//
        TFTwriteCommand(NOP);   //normally not neccessary; but if not, the first command after eg. SD-access will be ignored (here: CASET)
        TFTwriteCommand(CASET);
        SPI.transfer(0x00); SPI.transfer(xs);
        SPI.transfer(0x00); SPI.transfer(xe);
        TFTwriteCommand(RASET);
        SPI.transfer(0x00); SPI.transfer(ys);
        SPI.transfer(0x00); SPI.transfer(ye);
}

void TFTinit(void) {
        //minimal configuration: only settings which are different from Reset Default Value
        //or not affected by HW/SW-reset
      SPI.beginTransaction(settingsTFT);
      TFT_CS_LOW();

        TFTwriteCommand(SWRESET);
        delay(120);                                     //mandatory delay
        TFTwriteCommand(SLPOUT);        //turn off sleep mode.
        delay(120);
        TFTwriteCommand(PWCTR1);
         SPI.transfer(0xA2);
         SPI.transfer(0x02);
         SPI.transfer(0x84);
        TFTwriteCommand(PWCTR4);
         SPI.transfer(0x8A);
         SPI.transfer(0x2A);
        TFTwriteCommand(PWCTR5);
         SPI.transfer(0x8A);
         SPI.transfer(0xEE);
        TFTwriteCommand(VMCTR1);
         SPI.transfer(0x0E);                    //VCOM = -0.775V

        //Memory Data Access Control D7/D6/D5/D4/D3/D2/D1/D0
        //                                                       MY/MX/MV/ML/RGB/MH/-/-
        // MY- Row Address Order; ‘0’ =Increment, (Top to Bottom)
        // MX- Column Address Order; ‘0’ =Increment, (Left to Right)
        // MV- Row/Column Exchange; '0’ = Normal,
        // ML- Scan Address Order; ‘0’ =Decrement,(LCD refresh Top to Bottom)
        //RGB - '0'= RGB color fill order
        // MH - '0'= LCD horizontal refresh left to right
        TFTwriteCommand(MADCTL);
         SPI.transfer(0x08);

        //RGB-format
        TFTwriteCommand(COLMOD);        //color mode
         SPI.transfer(0x55); //16-bit/pixel; high nibble don't care

        TFTwriteCommand(CASET);
         SPI.transfer(0x00); SPI.transfer(FIRST_COL);
         SPI.transfer(0x00); SPI.transfer(LAST_COL);
        TFTwriteCommand(RASET);
         SPI.transfer(0x00); SPI.transfer(FIRST_ROW);
         SPI.transfer(0x00); SPI.transfer(LAST_ROW);

        TFTwriteCommand(NORON);
        TFTwriteCommand(DISPON);

      TFT_CS_HIGH();
      SPI.endTransaction();
}

const uint16_t bgColor = 0b11111111;
const uint16_t fgColor = 0b11101000;
const uint16_t err1Color = 0b11100011;
const uint16_t err2Color = 0b00011110;

bool write_flag = false;

//  R   G   B
// 111|111|11
void  setPixel(uint8_t x, uint8_t y, uint8_t value) {
    // get blue
    // get msb
    //if (x > FB_X) x = FB_X;
    //if (y > FB_Y) y = FB_Y;
    framebuffer[x % FB_X][y % FB_Y] = value;
    //Serial.println(value);
}

uint8_t getPixel(uint8_t x, uint8_t y) {
  if (x >= FB_X) return err1Color;
  if (y >= FB_Y) return err2Color;
  return framebuffer[x % FB_X][y % FB_Y];
}

void write_framebuffer(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye) {
    SPI.beginTransaction(settingsTFT);
    TFT_CS_LOW();
    TFTwriteWindow(xs, xe, ys, ye);
    TFTwriteCommand(RAMWR);

    for (uint8_t j = ys - FIRST_ROW; j < ye; j++) {
      for (uint8_t i = xs - FIRST_COL; i < xe; i++) {
            uint8_t value = getPixel(i,j);
            uint8_t blue = value & 0b00000011;
            uint8_t b_shifted = (blue << 3) & 0b00010000;
            uint8_t b_lower = (blue & 0b00000001) << 2;
            uint8_t blue_v = b_shifted | b_lower; //00000|000000|10100

            uint8_t green = value & 0b00011100;
            uint16_t g_upper = (green  & 0b00010000) << 6;
            uint16_t g_middle = (green & 0b00001000) << 5;
            uint16_t g_lower = (green & 0b00000100) << 4;
            uint16_t green_v = g_upper | g_middle | g_lower; //00000|101010|00000


            uint8_t red = value & 0b11100000;
            uint16_t red_upper = (red & 0b10000000) << 8;
            uint16_t red_middle = (red & 0b01000000) << 7;
            uint16_t red_lower = (red & 0b00100000) << 6;
            uint16_t red_v = red_upper | red_middle | red_lower; //10101|000000|00000

            uint16_t rgb565_c = red_v | green_v | blue_v;

            SPI.transfer(rgb565_c >> 8);
            SPI.transfer(rgb565_c);


        }
    }


    /*for (uint16_t i=((xs - FIRST_COL) * FB_Y + (ys - FIRST_ROW)); i<((xe+1-xs)*(ye+1-ys)) + ((xs - FIRST_COL) * FB_Y + (ys - FIRST_ROW)); i++) {
        uint8_t value = framebuffer[i];
        uint8_t blue = value & 0b00000011;
        uint8_t b_shifted = (blue << 3) & 0b00010000;
        uint8_t b_lower = (blue & 0b00000001) << 2;
        uint8_t blue_v = b_shifted | b_lower; //00000|000000|10100

        uint8_t green = value & 0b00011100;
        uint16_t g_upper = (green  & 0b00010000) << 6;
        uint16_t g_middle = (green & 0b00001000) << 5;
        uint16_t g_lower = (green & 0b00000100) << 4;
        uint16_t green_v = g_upper | g_middle | g_lower; //00000|101010|00000


        uint8_t red = value & 0b11100000;
        uint16_t red_upper = (red & 0b10000000) << 8;
        uint16_t red_middle = (red & 0b01000000) << 7;
        uint16_t red_lower = (red & 0b00100000) << 6;
        uint16_t red_v = red_upper | red_middle | red_lower; //10101|000000|00000

        uint16_t rgb565_c = red_v | green_v | blue_v;

        SPI.transfer(rgb565_c >> 8);
        SPI.transfer(rgb565_c);}*/
    TFT_CS_HIGH();
    SPI.endTransaction();
}

int print_char(uint8_t x, uint8_t y, char value, uint8_t fgColor, uint8_t bgColor) {
    //TODO manipulated for simulator
    if ((x+6) > FB_Y || (y+8) > FB_X) {
        return -1;
    }
    for (uint8_t i=0; i<6; i++) {
        for (uint8_t j=0; j<8; j++) {
            uint8_t pixel = font[value - 32][i] & (0b00000001 << (7 - j));
            setPixel(x + i, y + j, pixel ? fgColor : bgColor);
        }
    }
    return 0;
}

int print_string(uint8_t x, uint8_t y, char *c_str, uint8_t fgColor, uint8_t bgColor) {
    if (x + (strlen(c_str) * 6) > FB_Y || y + 8 > FB_X) return -1;
    for (uint8_t i=0; i<strlen(c_str); i++) {
        print_char(x + (i * 6), y, c_str[i], fgColor, bgColor);
    }
    return 0;
}

#if defined(__AVR_ATmega2560__)
SimpleTimer simple_timer;
int timer4;
int timer5;
#define USE_ST 1
#else
DueTimer Timer4;
DueTimer Timer5;
#define USE_ST 0
#endif



char * names[][2] {
  //{"Philip Hedram", "7503095"},
  //{"Lovis Koenig", "7056609"}
  {"PH", "01"},
  {"LK", "02"}
};

uint8_t names_ptr = 0;

uint8_t timer_counter = 4;

void clear_display() {
    for (uint8_t i = 0; i < FB_X; i++) {
        for (uint8_t j = 0; j < FB_Y; j++) {
            setPixel(i, j, bgColor);
        }
    }
    write_framebuffer(FIRST_COL, LAST_COL, FIRST_ROW, LAST_ROW);
}

uint8_t rotation_state = 0;

void draw_slash(uint8_t x, uint8_t y, uint8_t diameter, uint8_t color, uint8_t rotation) {
    if (diameter % 2 != 0) {
        diameter += 1;
    }
    if (rotation == 0) {
        // Draw a |
        //start at x + (diameter / 2), y
        //stop at x + (diameter / 2), y + diameter
        for (int i = y; i <= (y+diameter); i++) {
            setPixel(x + (diameter / 2), i, color);
            //Serial.println(i);
        }
    } else if (rotation == 1) {
        //Draw a /
        for (int i = x; i <= (x+diameter); i++) {
            setPixel(i, y + (diameter - (i - x)), color);
        }
    } else if (rotation == 2) {
        //Draw a -
        for (int i = x; i <= (x+diameter); i++) {
            setPixel(i, y + (diameter / 2), color);
        }
    } else {
        //Draw a '\'
        for (int i = x; i <= (x+diameter); i++) {
            setPixel(i, y + (i - x), color);
        }
    }
}

void rotbar_timer_routine() {
    draw_slash(50, 10, 10, bgColor, rotation_state);
    draw_slash(50, 10, 10, fgColor, (rotation_state+1) % 4);
    write_framebuffer(50 + FIRST_COL, FIRST_COL + 60, 10 + FIRST_ROW, FIRST_ROW + 20);
    rotation_state = (rotation_state + 1) % 4;
}

const int x_pos_offset = 20;
const int y_pos = 1;

void studid_timer_routine() {
    if (timer_counter == 4 || USE_ST) {
        clear_display();

        char **name = names[names_ptr];

        uint8_t name_length = strlen(name[0]) * 6;
        uint8_t name_x_pos = x_pos_offset - (name_length / 2);

        uint8_t matnr_length = strlen(name[1]) * 6;
        uint8_t matnr_x_pos = x_pos_offset - (matnr_length / 2);

        int i;

        i |= print_string(name_x_pos, y_pos + 10, name[0], fgColor, bgColor);
        i |= print_string(matnr_x_pos, y_pos, name[1], fgColor, bgColor);
        if (i != 0) Serial.println("Display too small");
        write_framebuffer(FIRST_COL, LAST_COL, FIRST_ROW, LAST_ROW);

        names_ptr = (names_ptr + 1) % 2;
        timer_counter = 0;
    } else {
        timer_counter++;
    }
}

void setup() {
  // set pin-modes
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_CS, OUTPUT);
  #if defined(__AVR_ATmega2560__)
     pinMode(SS_SLAVE, OUTPUT);
  #endif

  // set inactive levels
  digitalWrite(TFT_RST, HIGH);
  digitalWrite(TFT_DC, HIGH);
  digitalWrite(TFT_CS, HIGH);

  // initialize serial port 0
  Serial.begin(9600);
  Serial.println("Exercise init TFT template\n");

  // initialize SPI:
  // several devices: multiple SPI.begin(nn_CS) possible
  SPI.begin();
  delay(10);

  //power-on-reset of Display
  digitalWrite(TFT_RST, HIGH);
  delay(100);
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(100);

  TFTinit();
  Serial.println("Display Initialized");
  delay(100);

  //clear display
    uint8_t xs = FIRST_COL;
    uint8_t xe = LAST_COL;
    uint8_t ys = FIRST_ROW;
    uint8_t ye = LAST_ROW;
  uint16_t time = millis();
    SPI.beginTransaction(settingsTFT);
    TFT_CS_LOW();
    TFTwriteWindow(xs, xe, ys, ye);
    TFTwriteCommand(RAMWR);  //assign background-color to every element of writewindow
        for (uint16_t i=0; i<(xe+1-xs)*(ye+1-ys); i++) {
            SPI.transfer(0xFF);
            SPI.transfer(0xFF);}
    TFT_CS_HIGH();
    SPI.endTransaction();
  time = millis() - time;
  Serial.print("time consumption of clear-display: "); Serial.print(time, DEC); Serial.println(" ms");

    //draw blue rectangle (50x50pixel) centered on Display
    uint8_t  xc = FIRST_COL + ((LAST_COL+1 - FIRST_COL) >>1);
    uint8_t  yc = FIRST_ROW + ((LAST_ROW+1 - FIRST_ROW) >>1);
    xs = xc - 25;
    xe = xc + 25;
    ys = yc - 25;
    ye = yc + 25;

    SPI.beginTransaction(settingsTFT);
    TFT_CS_LOW();
    TFTwriteWindow(xs, xe, ys, ye);
    TFTwriteCommand(RAMWR);  //assign blue-color to every element of writewindow
        for (uint16_t i=0; i<(xe+1-xs)*(ye+1-ys); i++) {
            SPI.transfer((uint8_t)(0x001F>>8));
            SPI.transfer((uint8_t)0x001F); }
    TFT_CS_HIGH();
    SPI.endTransaction();

    delay(500);

    //draw centered red colored cross with one pixel space to border
    xs = FIRST_COL+1;
    xe = LAST_COL-1;
    ys = FIRST_ROW+1;
    ye = LAST_ROW-1;
    SPI.beginTransaction(settingsTFT);
    TFT_CS_LOW();
    //horizontal line
    TFTwriteWindow(xs, xe, yc, yc);
    TFTwriteCommand(RAMWR);  //assign background-color to every element of writewindow
        for (uint16_t i=0; i<(xe+1-xs)*(ye+1-ys); i++) {
            SPI.transfer((uint8_t)(0xF800>>8));
            SPI.transfer((uint8_t)0xF800); }
    //vertical line
    TFTwriteWindow(xc, xc, ys, ye);
    TFTwriteCommand(RAMWR);  //assign background-color to every element of writewindow
        for (uint16_t i=0; i<(xe+1-xs)*(ye+1-ys); i++) {
            SPI.transfer((uint8_t)(0xF800>>8));
            SPI.transfer((uint8_t)0xF800); }
    TFT_CS_HIGH();
    SPI.endTransaction();

    clear_display();
    #if !defined(__AVR_ATmega2560__)
     if (!Timer4.configure(1, studid_timer_routine) || !Timer5.configure(10, rotbar_timer_routine)) {
        Serial.println("ERROR: Timer configure failed");
    }
    #else
    timer4 = simple_timer.setInterval(5000, studid_timer_routine);
    simple_timer.disable(timer4);
    timer5 = simple_timer.setInterval(100, rotbar_timer_routine);
    simple_timer.disable(timer5);
  #endif

  Serial.println("\nSetup finished\n");
}

void start_student_id_demo() {
  #if !defined(__AVR_ATmega2560__)
     timer_counter = 4;
    Timer4.start();
  #else
    simple_timer.enable(timer4);
  #endif
}

void stop_student_id_demo() {
    #if !defined(__AVR_ATmega2560__)

    Timer4.stop();
  #else
  simple_timer.disable(timer4);
  #endif
}

void start_rotbar_demo() {
    #if !defined(__AVR_ATmega2560__)
    Timer5.start();
  #else
  simple_timer.enable(timer5);
  #endif
}

void stop_rotbar_demo() {
     #if !defined(__AVR_ATmega2560__)

    Timer5.stop();
  #else
  simple_timer.disable(timer5);
  #endif
}

void print_help() {
    Serial.println("Available commands:");
    Serial.println("  clear | clears display");
    Serial.println("  runRBD | runs rotating bar demo");
    Serial.println("  runID | runs student id demo");
    Serial.println("  stop | stops all running demos");
}

char input[64] = "";
int next = 0;

void parse_string() {
    char *ptr = strtok(input, " ");

    String command = String(ptr);


    if (strcmp(ptr, "help") == 0) {
        print_help();
    } else if (strcmp(ptr, "clear") == 0) {
        clear_display();
    } else if (strcmp(ptr, "runRBD") == 0) {
        start_rotbar_demo();
    } else if (strcmp(ptr, "runID") == 0) {
        start_student_id_demo();
    } else if (strcmp(ptr, "stop") == 0) {
        stop_student_id_demo();
        stop_rotbar_demo();
    } else {
        Serial.println("Invalid command " + command);
        print_help();
    }

    memset(&input[0], 0, sizeof(input));
}

void loop() {
    #if defined(__AVR_ATmega2560__)

    simple_timer.run();
  #endif
    if (Serial.available() > 0 && next < 63) {
        char c = Serial.read();
        if (c == '\r') {
            input[next] = '\0';
            Serial.print("\n");
            parse_string();
            next = 0;
        } else if (c != '\n') {
            input[next++] = c;
            Serial.print(c);
        }
    } else if (next == 63) {
        input[next] = '\0';
        Serial.print("\n");
        parse_string();
        next = 0;
    }
}   
