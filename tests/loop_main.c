//result=20

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
	return x;
}
