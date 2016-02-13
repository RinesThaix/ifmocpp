/* 
 * File:   main.c
 * Author: RinesThaix
 *
 * Created on 13 февраля 2016 г., 16:26
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 
 */
int main(int argc, char** argv) {
    int n;
    scanf("%d", &n);
    int* table = (int*) malloc(n * n * sizeof(int));
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < n; ++j)
            *(table + i * n + j) = (i + 1) * (j + 1);
    char helper[4];
    while(1) {
        int x1, y1, x2, y2;
        scanf("%d", &x1);
        if(x1 == 0)
            break;
        scanf("%d %d %d", &y1, &x2, &y2);
        int length = 1, max = *(table + (y2 - 1) * n + x2 - 1);
        do {
            max /= 10;
            ++length;
        }while(max);
        sprintf(helper, "%%%dd", length);
        for(int i = y1 - 1; i < y2; ++i) {
            for(int j = x1 - 1; j < x2; ++j)
                printf(helper, *(table + i * n + j));
            printf("\n");
        }
    }
    free(table);
    return (EXIT_SUCCESS);
}

