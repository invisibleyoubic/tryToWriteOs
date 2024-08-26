#include <stdint.h>

#define PIC1_C 0x20
#define PIC1_D 0x21
#define PIC2_C 0xa0
#define PIC2_D 0xa1

#define ICW1_DEF 0x10
#define ICW1_ICW4 0x01
#define ICW4_x86 0x01

unsigned int SCREEN_SIZE;
char* VIDEO_MEMORY;
unsigned int SCREEN_PTR;
char NumberAscii[10];

struct IDT_ENTRY
{
    uint16_t base_lower;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_higher;
};

struct IDT_ENTRY _idt[256];
extern uint32_t _isr1;
uint32_t base;
extern void _loadIdt();

void clear()
{
    for (unsigned int i = 0; i < SCREEN_SIZE; i += 2)
    {
        VIDEO_MEMORY[i] = ' ';
    }
}

void setColor(char color)
{
    for (unsigned int i = 1; i < SCREEN_SIZE; i += 2)
    {
        VIDEO_MEMORY[i] = color;
    }
}

void scroll()
{
    for (unsigned int i = 160, y = 0; i < SCREEN_SIZE; i += 2, y += 2)
    {
        VIDEO_MEMORY[y] = VIDEO_MEMORY[i];
    }

    SCREEN_PTR = 2 * 80 * 24;

    for (unsigned int i = 0; i < 160; i += 2)
    {
        VIDEO_MEMORY[SCREEN_PTR + i] = ' ';
    }
}

void printChar(char ch)
{
    if (SCREEN_PTR >= SCREEN_SIZE)
        scroll();

    if (ch == '\n')
    {
        SCREEN_PTR = ((SCREEN_PTR + 160) - (SCREEN_PTR % 160));
        return;
    }

    VIDEO_MEMORY[SCREEN_PTR] = ch;
    SCREEN_PTR += 2;
}

void printColorChar(char ch, char color)
{
    if (SCREEN_PTR >= SCREEN_SIZE)
        scroll();

    if (ch == '\n')
    {
        SCREEN_PTR = ((SCREEN_PTR + 160) - (SCREEN_PTR % 160));
        return;
    }

    VIDEO_MEMORY[SCREEN_PTR] = ch;
    VIDEO_MEMORY[SCREEN_PTR + 1] = color;
    SCREEN_PTR += 2;
}

void printString(const char* str)
{
    while (*str)
    {
        printChar(*str++);
    }
}

void printColorString(const char* str, char color)
{
    while (*str)
    {
        printColorChar(*str++, color);
    }
}

void getDecAscii(uint32_t num)
{
    if (num == 0)
    {
        NumberAscii[0] = '0';
        NumberAscii[1] = '\0';
        return;
    }

    char NUM[10];
    uint32_t i = 0;

    while (num > 0)
    {
        NUM[i++] = '0' + (num % 10);
        num /= 10;
    }

    for (uint32_t j = 0; j < i; j++)
    {
        NumberAscii[j] = NUM[i - j - 1];
    }

    NumberAscii[i] = '\0';
}

uint8_t inportb(uint16_t port)
{
 	unsigned char rv;
 	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
 	return rv;
}

void outportb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

void pressed(char key)
{
    printColorChar(key, 0x0f);
}

void handleKeypress(int code)
{
    static const char Scancode[] =
    {
     	0 , 0 , '1' , '2' ,
 		'3' , '4' , '5' , '6' ,
 		'7' , '8' , '9' , '0' ,
 		'-' , '=' , 0 , 0 , 'Q' ,
 		'W' , 'E' , 'R' , 'T' , 'Y' ,
 		'U' , 'I' , 'O' , 'P' , '[' , ']' ,
 		0 , 0 , 'A' , 'S' , 'D' , 'F' , 'G' ,
 		'H' , 'J' , 'K' , 'L' , ';' , '\'' , '`' ,
 		0 , '\\' , 'Z' , 'X' , 'C' , 'V' , 'B' , 'N' , 'M' ,
 		',' , '.' , '/' , 0 , '*' , 0 , ' '
	};

    if (code == 0x1c)
        printChar('\n');
    else if (code < sizeof(Scancode))
        pressed(Scancode[code]);
}

void _isr1_Handler()
{
    handleKeypress(inportb(0x60));
    outportb(0x20, 0x20);
    outportb(0xa0, 0x20);
}

void picRemap()
{
    uint8_t a = inportb(PIC1_D);
    uint8_t b = inportb(PIC2_D);

    outportb(PIC1_C, ICW1_DEF | ICW1_ICW4);
    outportb(PIC2_C, ICW1_DEF | ICW1_ICW4);

    outportb(PIC1_D, 0);
    outportb(PIC2_D, 8);

    outportb(PIC1_D, 4);
    outportb(PIC2_D, 2);

    outportb(PIC1_D, ICW4_x86);
    outportb(PIC2_D, ICW4_x86);

    outportb(PIC1_D, a);
    outportb(PIC2_D, b);
}

void initIDT()
{
    _idt[1].base_lower = (base & 0xFFFF);
    _idt[1].base_higher = (base >> 16) & 0xFFFF;
    _idt[1].selector = 0x08;
    _idt[1].zero = 0;
    _idt[1].flags = 0x8e;

    picRemap();

    outportb(0x21, 0xfd);
    outportb(0xa1, 0xff);

    _loadIdt();
}

int _start()
{
    SCREEN_SIZE = 2 * 80 * 25;
    VIDEO_MEMORY = (char*) 0xb8000;
    SCREEN_PTR = 0;
    base = (uint32_t) &_isr1;

    // clear();
    setColor(0xa2);

	// char cpy[100];

	// uint8_t itr = 0;
	// while (itr < 99)
	// {
	// 	cpy[itr] = '6';
	// 	printChar(cpy[itr]);
	// 	++itr;
	// }

	// cpy[99] = '\n';

    // char str[] = "sOome9 stringl";
	// char str1[] = "sO0ome stringh";
	// char str2[] = "1234567890123\0";
    // printString(str);
	// printString(str1);
	// printString(cpy);
    // printColorString(str, 0xf0);
	// printColorString(str2, 0xf0);
    // printColorString(str, 0xa9);
    printChar('A');
    // printColorChar('B', 0x0a);

    initIDT();
}