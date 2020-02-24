
PROGMEM iconButton homeButton =   { 128, 96, 32, 32, INTERFACE_COLOR, BACKGROUND_COLOR,
  {
    (0b00000000 << 8) | 0b00000000,
    (0b00000000 << 8) | 0b00000000,
    (0b00000001 << 8) | 0b10000000,
    (0b00000011 << 8) | 0b11000000,
    (0b00000111 << 8) | 0b11100000,
    (0b00001111 << 8) | 0b11110000,
    (0b00111111 << 8) | 0b11111100,
    (0b01111111 << 8) | 0b11111110,
    (0b11111111 << 8) | 0b11111111,
    (0b00111111 << 8) | 0b11111100,
    (0b00111110 << 8) | 0b00111100,
    (0b00111110 << 8) | 0b00111100,
    (0b00111110 << 8) | 0b00111100,
    (0b00111110 << 8) | 0b00111100,
    (0b00111110 << 8) | 0b00111100,
    (0b00000000 << 8) | 0b00000000
  }
};

int page = 0; //this tells the program which page to render

//page definitions 
#define HOME 0
#define NOTIFICATIONS 1
#define SETTINGS 2
#define APPS 3
