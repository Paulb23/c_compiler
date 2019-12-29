//result=30

int main()
{
	int x = 0;
	int y = 0;

	while (y == 0)
	{
		x = x + 1;
		if (x == 10)
		{
			y = 1;
		}
	}

	y = 0;
	do
	{
		x = x + 1;
		if (x == 20)
		{
			y = 1;
		}
	} while (y == 0);

	for (y = 0; y < 10; y = y + 1)
	{
		x = x + 1;
	}

	//for (;;) {
	//	break;
	//}

	//for (;y;)
	//{
	//}

	return x;
}
