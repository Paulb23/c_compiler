//result=13
int main()
{
	int x = 10;
	int y = 2;
	int z = 3;
	z = x + y;
	{
		int z = 1;
	}

	{
	}

	z--;
	z--;
	z++;
	z++;
	z++;
	return z;
}
