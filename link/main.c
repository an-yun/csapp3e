/* main.c */
/* $begin main */
int sum(int *a, int n);

int array[2] = {1, 2};

int main() 
{
    int val = sum(array, 2);
    int val2 = 2;
    array[0] = 3;
    val2 += sum(array, 2);
    return val2 + val;
}
/* $end main */
