#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tst.h"
#include "bench.h"

/** constants insert, delete, max word(s) & stack nodes */
enum { INS, DEL, WRDMAX = 256, STKMAX = 512, LMAX = 1024, CircularBufferSize = 3000000 };
#define REF INS
#define CPY DEL

/* simple trim '\n' from end of buffer filled by fgets */
static void rmcrlf(char *s)
{
    size_t len = strlen(s);
    if (len && s[len - 1] == '\n')
        s[--len] = 0;
}

#define IN_FILE "cities.txt"
#define BENCH_LOAD_OUT "bench_load_ref.txt"
#define BENCH_SEARCH_OUT "bench_search_ref.txt"

// Circular buffer object
typedef struct {
    int size; // maximum number of elements
    int start; // index of oldest element
    unsigned int end; // index at which to write new element
    char *elems; // vector of elements
} CircularBuffer;

void cbInit(CircularBuffer *cb, int size)
{
    cb->size = size;
    cb->start = 0;
    cb->end = 0;
    cb->elems = (char *)calloc(cb->size, sizeof(char));
}

int main(int argc, char **argv)
{
    char word[WRDMAX] = "";
    char *sgl[LMAX] = {NULL};
    tst_node *root = NULL, *res = NULL;
    int rtn = 0, idx = 0, sidx = 0;
    FILE *fp = fopen(IN_FILE, "r");
    double t1, t2;
    CircularBuffer buffer;
    cbInit(&buffer, CircularBufferSize);
    char *buffer_head = buffer.elems;
    if (!fp) { /* prompt, open, validate file for reading */
        fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
        return 1;
    }
    /* start store from the middle */
    buffer.start = CircularBufferSize >> 1;
    buffer.end = CircularBufferSize >> 1;
    buffer_head += (CircularBufferSize >> 1);

    t1 = tvgetf();
    while ((rtn = fscanf(fp, "%s", buffer_head)) != EOF) {
        char *p = buffer_head;
        /* FIXME: insert reference to each string */
        if (!tst_ins_del(&root, &p, INS, REF)) {
            fprintf(stderr, "error: memory exhausted, tst_insert.\n");
            fclose(fp);
            return 1;
        }
        idx++;
        int word_len = strlen(buffer_head) + 1;
        if( (WRDMAX + buffer.end) >= CircularBufferSize) {
            buffer_head -= buffer.end;
            buffer.end = 0;
        } else {
            buffer.end = word_len + buffer.end;
            buffer_head += word_len;
        }
    }

    t2 = tvgetf();

    fclose(fp);
    printf("ternary_tree, loaded %d words in %.6f sec\n", idx, t2 - t1);

    if (argc == 2 && strcmp(argv[1], "--bench") == 0) {
        /* build loading time txt */
        fp = fopen (BENCH_LOAD_OUT,"a+");
        if (!fp) {
            fprintf(stderr, "error: file open failed in '%s'.\n", BENCH_LOAD_OUT);
            return 1;
        }
        fprintf(fp, "%.6f\n",t2 - t1);
        fclose(fp);
        /* build searching time txt */
        int r = bench_test(root, BENCH_SEARCH_OUT, sgl, &sidx, LMAX);
        tst_free(root);
        free(buffer.elems);
        return r;
    }

    for (;;) {
        printf(
            "\nCommands:\n"
            " a  add word to the tree\n"
            " f  find word in tree\n"
            " s  search words matching prefix\n"
            " d  delete word from the tree\n"
            " q  quit, freeing all data\n\n"
            "choice: ");
        fgets(word, sizeof word, stdin);

        switch (*word) {
            char *p = NULL;
        case 'a':
            printf("enter word to add: ");
            if (!fgets(buffer_head, sizeof buffer_head, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(buffer_head);
            p = buffer_head;
            t1 = tvgetf();
            /* FIXME: insert reference to each string */
            res = tst_ins_del(&root, &p, INS, REF);
            t2 = tvgetf();
            if (res) {
                idx++;
                printf("  %s - inserted in %.6f sec. (%d words in tree)\n",
                       (char *) res, t2 - t1, idx);
            } else
                printf("  %s - already exists in list.\n", (char *) res);
            break;
        case 'f':
            printf("find word in tree: ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search(root, word);
            t2 = tvgetf();
            if (res)
                printf("  found %s in %.6f sec.\n", (char *) res, t2 - t1);
            else
                printf("  %s not found.\n", word);
            break;
        case 's':
            printf("find words matching prefix (at least 1 char): ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search_prefix(root, word, sgl, &sidx, LMAX);
            t2 = tvgetf();
            if (res) {
                printf("  %s - searched prefix in %.6f sec\n\n", word, t2 - t1);
                for (int i = 0; i < sidx; i++)
                    printf("suggest[%d] : %s\n", i, sgl[i]);
            } else
                printf("  %s - not found\n", word);
            break;
        case 'd':
            printf("enter word to del: ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            p = word;
            printf("  deleting %s\n", word);
            t1 = tvgetf();
            /* FIXME: remove reference to each string */
            res = tst_ins_del(&root, &p, DEL, REF);
            t2 = tvgetf();
            if (res)
                printf("  delete failed.\n");
            else {
                printf("  deleted %s in %.6f sec\n", word, t2 - t1);
                idx--;
            }
            break;
        case 'q':
            tst_free(root);
            free(buffer.elems);
            return 0;
            break;
        default:
            fprintf(stderr, "error: invalid selection.\n");
            break;
        }
    }

    return 0;
}
