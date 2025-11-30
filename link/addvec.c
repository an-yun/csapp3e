/* addvec.c */
/* $begin addvec */
int addcnt = 0;

void addvec(int *x, int *y,
	    int *z, int n) 
{
    int i;

    addcnt++;

    for (i = 0; i < n; i++)
	z[i] = x[i] + y[i];
}

int addTest(int *x, int *y) 
{
    return x[0] + y[0];
}
/* $end addvec */

