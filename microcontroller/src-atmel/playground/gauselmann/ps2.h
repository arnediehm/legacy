


//void key_press(ps2_key key);
void key_make(uint16_t data);
void key_break(uint16_t data);

void key_make_b(uint16_t data);
void key_break_b(uint16_t data);

void init_ps2();

void ps2_put(unsigned char size, uint8_t * data);
unsigned char ps2_get_nb(unsigned char * byte);


