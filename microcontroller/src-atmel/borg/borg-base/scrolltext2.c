#include "config.h"

#ifdef AVR
#   include <avr/pgmspace.h>
#else 
#   define PROGMEM
#   define pgm_read_byte(B) *(B)      
#endif

#define SCROLLTEXT_C

#include "scrolltext.h"
#ifdef AVR
#   include "borg_hw.h"
#endif

#include "pixel.h"
#include "util.h"

struct FONT_DEF 
{
   unsigned char store_width;            /* glyph storage width in bytes */
   unsigned char glyph_height;  		 /* glyph height for storage */
   const unsigned char * glyph_table;      /* font table start address in memory */
   const unsigned char *width_table; 	 /* variable width table start adress */
   unsigned char glyph_beg;			 	 /* start ascii offset in table */
   unsigned char glyph_end;				 /* end ascii offset in table */
   unsigned char glyph_def;				 /* code for undefined glyph code */
};

unsigned char PROGMEM five_dot_glyph_table[] = {
		/* ' ' charwidth: 2 */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		
		/* '!' charwidth: 2 */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		
		/* '"' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0xA0, 	/*  [* * ]  */
		0xA0, 	/*  [* * ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		
		/* '#' charwidth: 8 */
		0x00, 	/*  [        ]  */
		0x14, 	/*  [   * *  ]  */
		0x7E, 	/*  [ ****** ]  */
		0x28, 	/*  [  * *   ]  */
		0xFC, 	/*  [******  ]  */
		0x50, 	/*  [ * *    ]  */
		0x00, 	/*  [        ]  */
		
		/* '$' charwidth: 4 */
		0x40, 	/*  [ *  ]  */
		0x60, 	/*  [ ** ]  */
		0x80, 	/*  [*   ]  */
		0x40, 	/*  [ *  ]  */
		0x20, 	/*  [  * ]  */
		0xC0, 	/*  [**  ]  */
		0x40, 	/*  [ *  ]  */
		
		/* '%' charwidth: 8 */
		0x00, 	/*  [        ]  */
		0x64, 	/*  [ **  *  ]  */
		0xA8, 	/*  [* * *   ]  */
		0xD6, 	/*  [** * ** ]  */
		0x2A, 	/*  [  * * * ]  */
		0x4C, 	/*  [ *  **  ]  */
		0x00, 	/*  [        ]  */
		
		/* '&' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x60, 	/*  [ **   ]  */
		0x90, 	/*  [*  *  ]  */
		0x40, 	/*  [ *    ]  */
		0x98, 	/*  [*  ** ]  */
		0x60, 	/*  [ **   ]  */
		0x00, 	/*  [      ]  */
		
		/* ''' charwidth: 2 */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		
		/* '(' charwidth: 3 */
		0x40, 	/*  [ * ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0x40, 	/*  [ * ]  */
		
		/* ')' charwidth: 3 */
		0x80, 	/*  [*  ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x80, 	/*  [*  ]  */
		
		/* '*' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x50, 	/*  [ * *  ]  */
		0x20, 	/*  [  *   ]  */
		0xF8, 	/*  [***** ]  */
		0x20, 	/*  [  *   ]  */
		0x50, 	/*  [ * *  ]  */
		0x00, 	/*  [      ]  */
		
		/* '+' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x20, 	/*  [  *   ]  */
		0x20, 	/*  [  *   ]  */
		0xF8, 	/*  [***** ]  */
		0x20, 	/*  [  *   ]  */
		0x20, 	/*  [  *   ]  */
		0x00, 	/*  [      ]  */
		
		/* ',' charwidth: 3 */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x80, 	/*  [*  ]  */
		
		/* '-' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0xE0, 	/*  [*** ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		
		/* '.' charwidth: 2 */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		
		/* '/' charwidth: 4 */
		0x20, 	/*  [  * ]  */
		0x20, 	/*  [  * ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0x00, 	/*  [    ]  */
		
		/* '0' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* '1' charwidth: 3 */
		0x00, 	/*  [   ]  */
		0x40, 	/*  [ * ]  */
		0xC0, 	/*  [** ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x00, 	/*  [   ]  */
		
		/* '2' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x20, 	/*  [  *  ]  */
		0x40, 	/*  [ *   ]  */
		0xF0, 	/*  [**** ]  */
		0x00, 	/*  [     ]  */
		
		/* '3' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x20, 	/*  [  *  ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* '4' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x20, 	/*  [  *  ]  */
		0x60, 	/*  [ **  ]  */
		0xA0, 	/*  [* *  ]  */
		0xF0, 	/*  [**** ]  */
		0x20, 	/*  [  *  ]  */
		0x00, 	/*  [     ]  */
		
		/* '5' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0xE0, 	/*  [*** ]  */
		0x80, 	/*  [*   ]  */
		0xE0, 	/*  [*** ]  */
		0x20, 	/*  [  * ]  */
		0xC0, 	/*  [**  ]  */
		0x00, 	/*  [    ]  */
		
		/* '6' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x20, 	/*  [  *  ]  */
		0x40, 	/*  [ *   ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* '7' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0xE0, 	/*  [*** ]  */
		0x20, 	/*  [  * ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x00, 	/*  [    ]  */
		
		/* '8' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* '9' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x70, 	/*  [ *** ]  */
		0x20, 	/*  [  *  ]  */
		0x40, 	/*  [ *   ]  */
		0x00, 	/*  [     ]  */
		
		/* ':' charwidth: 2 */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		
		/* ';' charwidth: 3 */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		0x40, 	/*  [ * ]  */
		0x00, 	/*  [   ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x80, 	/*  [*  ]  */
		
		/* '<' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x20, 	/*  [  * ]  */
		0x40, 	/*  [ *  ]  */
		0x80, 	/*  [*   ]  */
		0x40, 	/*  [ *  ]  */
		0x20, 	/*  [  * ]  */
		0x00, 	/*  [    ]  */
		
		/* '=' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x00, 	/*  [     ]  */
		0xF0, 	/*  [**** ]  */
		0x00, 	/*  [     ]  */
		0xF0, 	/*  [**** ]  */
		0x00, 	/*  [     ]  */
		0x00, 	/*  [     ]  */
		
		/* '>' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x80, 	/*  [*   ]  */
		0x40, 	/*  [ *  ]  */
		0x20, 	/*  [  * ]  */
		0x40, 	/*  [ *  ]  */
		0x80, 	/*  [*   ]  */
		0x00, 	/*  [    ]  */
		
		/* '?' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0xC0, 	/*  [**  ]  */
		0x20, 	/*  [  * ]  */
		0x40, 	/*  [ *  ]  */
		0x00, 	/*  [    ]  */
		0x40, 	/*  [ *  ]  */
		0x00, 	/*  [    ]  */
		
		/* '@' charwidth: 8 */
		0x38, 	/*  [  ***   ]  */
		0x44, 	/*  [ *   *  ]  */
		0x9A, 	/*  [*  ** * ]  */
		0xAA, 	/*  [* * * * ]  */
		0xB4, 	/*  [* ** *  ]  */
		0x40, 	/*  [ *      ]  */
		0x30, 	/*  [  **    ]  */
		
		/* 'A' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x20, 	/*  [  *   ]  */
		0x20, 	/*  [  *   ]  */
		0x50, 	/*  [ * *  ]  */
		0x70, 	/*  [ ***  ]  */
		0x88, 	/*  [*   * ]  */
		0x00, 	/*  [      ]  */
		
		/* 'B' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0xE0, 	/*  [***  ]  */
		0x00, 	/*  [     ]  */
		
		/* 'C' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x80, 	/*  [*    ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* 'D' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0xE0, 	/*  [***  ]  */
		0x00, 	/*  [     ]  */
		
		/* 'E' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0xE0, 	/*  [*** ]  */
		0x80, 	/*  [*   ]  */
		0xE0, 	/*  [*** ]  */
		0x80, 	/*  [*   ]  */
		0xE0, 	/*  [*** ]  */
		0x00, 	/*  [    ]  */
		
		/* 'F' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0xE0, 	/*  [*** ]  */
		0x80, 	/*  [*   ]  */
		0xE0, 	/*  [*** ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0x00, 	/*  [    ]  */
		
		/* 'G' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x70, 	/*  [ *** ]  */
		0x80, 	/*  [*    ]  */
		0xB0, 	/*  [* ** ]  */
		0x90, 	/*  [*  * ]  */
		0x70, 	/*  [ *** ]  */
		0x00, 	/*  [     ]  */
		
		/* 'H' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0xF0, 	/*  [**** ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x00, 	/*  [     ]  */
		
		/* 'I' charwidth: 2 */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		
		/* 'J' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x10, 	/*  [   * ]  */
		0x10, 	/*  [   * ]  */
		0x10, 	/*  [   * ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* 'K' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x90, 	/*  [*  * ]  */
		0xA0, 	/*  [* *  ]  */
		0xC0, 	/*  [**   ]  */
		0xA0, 	/*  [* *  ]  */
		0x90, 	/*  [*  * ]  */
		0x00, 	/*  [     ]  */
		
		/* 'L' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0xE0, 	/*  [*** ]  */
		0x00, 	/*  [    ]  */
		
		/* 'M' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x88, 	/*  [*   * ]  */
		0xD8, 	/*  [** ** ]  */
		0xA8, 	/*  [* * * ]  */
		0x88, 	/*  [*   * ]  */
		0x88, 	/*  [*   * ]  */
		0x00, 	/*  [      ]  */
		
		/* 'N' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x88, 	/*  [*   * ]  */
		0xC8, 	/*  [**  * ]  */
		0xA8, 	/*  [* * * ]  */
		0x98, 	/*  [*  ** ]  */
		0x88, 	/*  [*   * ]  */
		0x00, 	/*  [      ]  */
		
		/* 'O' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* 'P' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0xE0, 	/*  [***  ]  */
		0x80, 	/*  [*    ]  */
		0x80, 	/*  [*    ]  */
		0x00, 	/*  [     ]  */
		
		/* 'Q' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x10, 	/*  [   * ]  */
		
		/* 'R' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0xE0, 	/*  [***  ]  */
		0xA0, 	/*  [* *  ]  */
		0x90, 	/*  [*  * ]  */
		0x00, 	/*  [     ]  */
		
		/* 'S' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x60, 	/*  [ ** ]  */
		0x80, 	/*  [*   ]  */
		0x40, 	/*  [ *  ]  */
		0x20, 	/*  [  * ]  */
		0xC0, 	/*  [**  ]  */
		0x00, 	/*  [    ]  */
		
		/* 'T' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0xE0, 	/*  [*** ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x00, 	/*  [    ]  */
		
		/* 'U' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* 'V' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x88, 	/*  [*   * ]  */
		0x50, 	/*  [ * *  ]  */
		0x50, 	/*  [ * *  ]  */
		0x20, 	/*  [  *   ]  */
		0x20, 	/*  [  *   ]  */
		0x00, 	/*  [      ]  */
		
		/* 'W' charwidth: 8 */
		0x00, 	/*  [        ]  */
		0x82, 	/*  [*     * ]  */
		0x54, 	/*  [ * * *  ]  */
		0x54, 	/*  [ * * *  ]  */
		0x28, 	/*  [  * *   ]  */
		0x28, 	/*  [  * *   ]  */
		0x00, 	/*  [        ]  */
		
		/* 'X' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x88, 	/*  [*   * ]  */
		0x50, 	/*  [ * *  ]  */
		0x20, 	/*  [  *   ]  */
		0x50, 	/*  [ * *  ]  */
		0x88, 	/*  [*   * ]  */
		0x00, 	/*  [      ]  */
		
		/* 'Y' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x88, 	/*  [*   * ]  */
		0x50, 	/*  [ * *  ]  */
		0x20, 	/*  [  *   ]  */
		0x20, 	/*  [  *   ]  */
		0x20, 	/*  [  *   ]  */
		0x00, 	/*  [      ]  */
		
		/* 'Z' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0xE0, 	/*  [*** ]  */
		0x20, 	/*  [  * ]  */
		0x40, 	/*  [ *  ]  */
		0x80, 	/*  [*   ]  */
		0xE0, 	/*  [*** ]  */
		0x00, 	/*  [    ]  */
		
		/* '[' charwidth: 3 */
		0xC0, 	/*  [** ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0xC0, 	/*  [** ]  */
		
		/* '\' charwidth: 4 */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x20, 	/*  [  * ]  */
		0x20, 	/*  [  * ]  */
		0x00, 	/*  [    ]  */
		
		/* ']' charwidth: 3 */
		0xC0, 	/*  [** ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0xC0, 	/*  [** ]  */
		
		/* '^' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x20, 	/*  [  *   ]  */
		0x50, 	/*  [ * *  ]  */
		0x88, 	/*  [*   * ]  */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		
		/* '_' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0xF8, 	/*  [***** ]  */
		
		/* '`' charwidth: 3 */
		0x00, 	/*  [   ]  */
		0x80, 	/*  [*  ]  */
		0x40, 	/*  [ * ]  */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		
		/* 'a' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x00, 	/*  [     ]  */
		0x70, 	/*  [ *** ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x70, 	/*  [ *** ]  */
		0x00, 	/*  [     ]  */
		
		/* 'b' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x80, 	/*  [*    ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0xE0, 	/*  [***  ]  */
		0x00, 	/*  [     ]  */
		
		/* 'c' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x60, 	/*  [ ** ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0x60, 	/*  [ ** ]  */
		0x00, 	/*  [    ]  */
		
		/* 'd' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x10, 	/*  [   * ]  */
		0x70, 	/*  [ *** ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x70, 	/*  [ *** ]  */
		0x00, 	/*  [     ]  */
		
		/* 'e' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x60, 	/*  [ ** ]  */
		0xA0, 	/*  [* * ]  */
		0xC0, 	/*  [**  ]  */
		0x60, 	/*  [ ** ]  */
		0x00, 	/*  [    ]  */
		
		/* 'f' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x60, 	/*  [ ** ]  */
		0x80, 	/*  [*   ]  */
		0xC0, 	/*  [**  ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0x00, 	/*  [    ]  */
		
		/* 'g' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x70, 	/*  [ *** ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x70, 	/*  [ *** ]  */
		0x10, 	/*  [   * ]  */
		0x60, 	/*  [ **  ]  */
		
		/* 'h' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x80, 	/*  [*    ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x00, 	/*  [     ]  */
		
		/* 'i' charwidth: 2 */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		
		/* 'j' charwidth: 3 */
		0x40, 	/*  [ * ]  */
		0x00, 	/*  [   ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x40, 	/*  [ * ]  */
		0x80, 	/*  [*  ]  */
		
		/* 'k' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x80, 	/*  [*    ]  */
		0x90, 	/*  [*  * ]  */
		0xE0, 	/*  [***  ]  */
		0xA0, 	/*  [* *  ]  */
		0x90, 	/*  [*  * ]  */
		0x00, 	/*  [     ]  */
		
		/* 'l' charwidth: 2 */
		0x00, 	/*  [  ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		
		/* 'm' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0xF0, 	/*  [****  ]  */
		0xA8, 	/*  [* * * ]  */
		0xA8, 	/*  [* * * ]  */
		0xA8, 	/*  [* * * ]  */
		0x00, 	/*  [      ]  */
		
		/* 'n' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x00, 	/*  [     ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x00, 	/*  [     ]  */
		
		/* 'o' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x00, 	/*  [     ]  */
		0x60, 	/*  [ **  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x60, 	/*  [ **  ]  */
		0x00, 	/*  [     ]  */
		
		/* 'p' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0xE0, 	/*  [***  ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0xE0, 	/*  [***  ]  */
		0x80, 	/*  [*    ]  */
		0x80, 	/*  [*    ]  */
		
		/* 'q' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x70, 	/*  [ *** ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x70, 	/*  [ *** ]  */
		0x10, 	/*  [   * ]  */
		0x10, 	/*  [   * ]  */
		
		/* 'r' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0xA0, 	/*  [* * ]  */
		0xC0, 	/*  [**  ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		0x00, 	/*  [    ]  */
		
		/* 's' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x60, 	/*  [ ** ]  */
		0xC0, 	/*  [**  ]  */
		0x20, 	/*  [  * ]  */
		0xC0, 	/*  [**  ]  */
		0x00, 	/*  [    ]  */
		
		/* 't' charwidth: 3 */
		0x00, 	/*  [   ]  */
		0x80, 	/*  [*  ]  */
		0xC0, 	/*  [** ]  */
		0x80, 	/*  [*  ]  */
		0x80, 	/*  [*  ]  */
		0x40, 	/*  [ * ]  */
		0x00, 	/*  [   ]  */
		
		/* 'u' charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x00, 	/*  [     ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x90, 	/*  [*  * ]  */
		0x70, 	/*  [ *** ]  */
		0x00, 	/*  [     ]  */
		
		/* 'v' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0xA0, 	/*  [* * ]  */
		0xA0, 	/*  [* * ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x00, 	/*  [    ]  */
		
		/* 'w' charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0xA8, 	/*  [* * * ]  */
		0xA8, 	/*  [* * * ]  */
		0x50, 	/*  [ * *  ]  */
		0x50, 	/*  [ * *  ]  */
		0x00, 	/*  [      ]  */
		
		/* 'x' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0xA0, 	/*  [* * ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0xA0, 	/*  [* * ]  */
		0x00, 	/*  [    ]  */
		
		/* 'y' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0xA0, 	/*  [* * ]  */
		0xA0, 	/*  [* * ]  */
		0x40, 	/*  [ *  ]  */
		0x80, 	/*  [*   ]  */
		0x80, 	/*  [*   ]  */
		
		/* 'z' charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0xE0, 	/*  [*** ]  */
		0x40, 	/*  [ *  ]  */
		0x80, 	/*  [*   ]  */
		0xE0, 	/*  [*** ]  */
		0x00, 	/*  [    ]  */
		
		/* '{' charwidth: 4 */
		0x20, 	/*  [  * ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x80, 	/*  [*   ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x20, 	/*  [  * ]  */
		
		/* '|' charwidth: 2 */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x80, 	/*  [* ]  */
		0x00, 	/*  [  ]  */
		
		/* '}' charwidth: 4 */
		0x80, 	/*  [*   ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x20, 	/*  [  * ]  */
		0x40, 	/*  [ *  ]  */
		0x40, 	/*  [ *  ]  */
		0x80, 	/*  [*   ]  */
		
		/* '~' charwidth: 7 */
		0x00, 	/*  [       ]  */
		0x64, 	/*  [ **  * ]  */
		0x98, 	/*  [*  **  ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
};

unsigned char PROGMEM five_dot_width_table[] = {
		 3, /* ' ' */
		 2, /* '!' */
		 4, /* '"' */
		 8, /* '#' */
		 4, /* '$' */
		 8, /* '%' */
		 6, /* '&' */
		 2, /* ''' */
		 3, /* '(' */
		 3, /* ')' */
		 6, /* '*' */
		 6, /* '+' */
		 3, /* ',' */
		 4, /* '-' */
		 2, /* '.' */
		 4, /* '/' */
		 5, /* '0' */
		 3, /* '1' */
		 5, /* '2' */
		 5, /* '3' */
		 5, /* '4' */
		 4, /* '5' */
		 5, /* '6' */
		 4, /* '7' */
		 5, /* '8' */
		 5, /* '9' */
		 2, /* ':' */
		 3, /* ';' */
		 4, /* '<' */
		 5, /* '=' */
		 4, /* '>' */
		 4, /* '?' */
		 8, /* '@' */
		 6, /* 'A' */
		 5, /* 'B' */
		 5, /* 'C' */
		 5, /* 'D' */
		 4, /* 'E' */
		 4, /* 'F' */
		 5, /* 'G' */
		 5, /* 'H' */
		 2, /* 'I' */
		 5, /* 'J' */
		 5, /* 'K' */
		 4, /* 'L' */
		 6, /* 'M' */
		 6, /* 'N' */
		 5, /* 'O' */
		 5, /* 'P' */
		 5, /* 'Q' */
		 5, /* 'R' */
		 4, /* 'S' */
		 4, /* 'T' */
		 5, /* 'U' */
		 6, /* 'V' */
		 8, /* 'W' */
		 6, /* 'X' */
		 6, /* 'Y' */
		 4, /* 'Z' */
		 3, /* '[' */
		 4, /* '\' */
		 3, /* ']' */
		 6, /* '^' */
		 6, /* '_' */
		 3, /* '`' */
		 5, /* 'a' */
		 5, /* 'b' */
		 4, /* 'c' */
		 5, /* 'd' */
		 4, /* 'e' */
		 4, /* 'f' */
		 5, /* 'g' */
		 5, /* 'h' */
		 2, /* 'i' */
		 3, /* 'j' */
		 5, /* 'k' */
		 2, /* 'l' */
		 6, /* 'm' */
		 5, /* 'n' */
		 5, /* 'o' */
		 5, /* 'p' */
		 5, /* 'q' */
		 4, /* 'r' */
		 4, /* 's' */
		 3, /* 't' */
		 5, /* 'u' */
		 4, /* 'v' */
		 6, /* 'w' */
		 4, /* 'x' */
		 4, /* 'y' */
		 4, /* 'z' */
		 4, /* '{' */
		 2, /* '|' */
		 4, /* '}' */
		 7, /* '~' */
};

struct FONT_DEF font = {1,  7, five_dot_glyph_table, five_dot_width_table,' ','~','.'};

void shift_in(char glyph, unsigned int delay){
	unsigned char i, y, ror;
	unsigned char len;
	if ((glyph < font.glyph_beg) || (glyph > font.glyph_end)) {
            glyph = font.glyph_def;
    } 
	glyph -= font.glyph_beg;
	len = pgm_read_byte(font.width_table+glyph);
	ror = (1 << 7);
	for (i = 0; i < len; i++) {
		shift_pixmap_l();
		for (y = 0; y < font.glyph_height; y++) {
			setpixel((pixel){NUM_COLS-1, y},(pgm_read_byte(font.glyph_table+y+glyph*font.glyph_height) & ror)?3:0 );
		}
		ror >>= 1;
		wait(delay);
	}
}


void shift_out(unsigned char cols, unsigned int delay){
	unsigned char i;
	for(i=0;i<cols;i++){
		shift_pixmap_l();
		wait(delay);
	}
}

void scrolltext(char *str, unsigned int delay) {
     char *tmp = str;
     while (*tmp) {
        shift_in(*tmp++, delay);
     }
     shift_out(NUM_COLS+8, delay);
}
