#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bench.h"

/* timing helper function */
double tvgetf(void)
{
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;

    return sec;
}

int bench_test(const tst_node *root,
               char *search_out,
               char **a,
               int *n,
               const int max)
{
    char word[] = "A";
    int i;
    double t1, t2, searchtime;
    searchtime = 0.0;

    FILE *fp = fopen (search_out, "a+");
    if (!fp) {
        fprintf(stderr, "error: file open failed in '%s'.\n", search_out);
        return 1;
    }
    for (i = 0; i < 25; i++) {
        t1 = tvgetf();
        tst_search_prefix(root, word, a, n, max);
        t2 = tvgetf();
        searchtime += t2 - t1;
        word[0]++;
    }
    word[0] = 'a';
    for (i = 0; i < 25; i++) {
        t1 = tvgetf();
        tst_search_prefix(root, word, a, n, max);
        t2 = tvgetf();
        searchtime += t2 - t1;
        word[0]++;
    }
    fprintf(fp, "%.6f\n",searchtime);
    fclose(fp);

    return 0;
}

