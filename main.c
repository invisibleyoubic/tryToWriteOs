unsigned int SCREEN_SIZE; 
char* VIDEO_MEMORY;
unsigned int SCREEN_PTR;


void clear()
{
	unsigned int i = 0;
	SCREEN_PTR = 0;
	while(i < SCREEN_SIZE){
		*(VIDEO_MEMORY + i) = ' ';
		i += 2;
	}
}

void setColor(char color)
{
	unsigned int i = 1;
	while(i < SCREEN_SIZE){
		*(VIDEO_MEMORY + i) = color;
		i += 2;
	}
}

void scroll()
{
	unsigned int i = 160;
	unsigned int y = 0;

	while(i < SCREEN_SIZE)
	{
		*(VIDEO_MEMORY + y) = *(VIDEO_MEMORY + i);
		
		i += 2;
		y += 2;
	}
	
	SCREEN_PTR = 2 * 80 * 24;
	
	i = 0;
	
	while(i < 160)
	{
		*(VIDEO_MEMORY + SCREEN_PTR + i) = ' ';
		i += 2;
	}
}

void printChar(char ch)
{
	if(SCREEN_PTR == SCREEN_SIZE)
		scroll();
	
	if(ch == '\n')
	{
		SCREEN_PTR = ((SCREEN_PTR + 160) - (SCREEN_PTR % 160));
		return;
	}

	*(VIDEO_MEMORY + SCREEN_PTR) = ch;
	SCREEN_PTR += 2;	
}

void printColorChar(char ch, char color)
{
	if(SCREEN_PTR == SCREEN_SIZE)
		scroll();
	
	if(ch == '\n'){
		SCREEN_PTR = ((SCREEN_PTR + 160) - (SCREEN_PTR % 160));
		return;
	}
	
	*(VIDEO_MEMORY + SCREEN_PTR) = ch;
	*(VIDEO_MEMORY + SCREEN_PTR + 1) = color;
	SCREEN_PTR += 2;	
}

void printString(char* str)
{
	int i = 0;

	while(*(str + i) != '\0')
	{
		printChar(*(str + i));
		i++;
	}
}

void printColorString(char* ch, char color)
{
	int i = 0;
	
	while(*(ch + i) != '\0')
	{
		printColorChar(*(ch + i) , color);
		i++;
	}
}


int _start()
{
	SCREEN_SIZE = 2 * 80 * 25;
	VIDEO_MEMORY = (char*)0xb8000;
	SCREEN_PTR = 0;

	clear();
	setColor(0x0f);

	char str[] = "some string\n";
	printString(str);
	printColorString(str, 0xf0);
	printColorString(str, 0xa9);
	printChar('A');
	printColorChar('B', 0x0a);

	return 0;
}