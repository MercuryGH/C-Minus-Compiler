int gcd(int a, int b)
{
    if (b == 0)
    {
        return a;
    }
    // a % b == a - a / b * b
    else return gcd(b, a - a / b * b);
}

int main()
{
    int a;
    int b;

    a = input();
    b = input();

    output(gcd(a, b));
}
