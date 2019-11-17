//result=12
int main()
{
	int x = 10;
	int y = 2;
	int z = 1;
	z = x + y;
	{
		int z = 1;
	}

	{
	}

	return z;
}
