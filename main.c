int _start()
{
	const screen_size = 2 * 80 * 25;
	char *video_memory = (char*)0xb8000;
	unsigned int i = 0;
	char obj = 0;
	while (1)
	{
		i = 0;
		while (i < screen_size)
		{
			*(video_memory + i) = obj;
			++i;
			++obj;
		}
	}

	return 0;
}