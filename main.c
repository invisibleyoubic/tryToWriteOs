int _start()
{
    const unsigned short int sizeOfScreen = 2 * 80 * 25;
    
    char *screnPtr = (char*)0xb8000;

    unsigned short int itr = 1;
    unsigned char color = 0;

    while (itr < sizeOfScreen)
    {
        *(screnPtr + itr) = color;
        itr += 2;
        color += 25;
    } 
}