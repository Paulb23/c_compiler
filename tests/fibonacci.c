//result=98
int fibonacci(int n)
{
	if (n == 0)
	{
		return n;
	}

	if (n == 1)
	{
		return n;
	}
	return fibonacci(n - 1) + fibonacci(n - 2);
}

int main()
{
	int n = 15;
	return fibonacci(n);
}
