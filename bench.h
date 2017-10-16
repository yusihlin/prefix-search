#ifndef BENCH_H
#define BENCH_H

#include "tst.h"

double tvgetf();
int bench_test(const tst_node *root,
               char *search_out,
               char **a,
               int *n,
               const int max);

#endif
