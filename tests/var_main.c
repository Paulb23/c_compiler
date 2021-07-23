//result=32
int main()
{
	int x = 5;
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
	z--;
	z++;
	z++;
	z++;

	z -= 4;
	z += 1; // z == 4

	int k = 4;
	k = k * 5 + 3; // = 23
	k = (4 * 5 + 3);
	k = (4 * (5 + 3));

	z *= 5 + 3; // = 32
	return z;
}
