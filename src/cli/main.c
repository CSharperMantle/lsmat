#include "lsmat/lsmat.h"
#include <mcheck.h>
#include <stdio.h>

static const double TARGET_MAT[4][5] = {{0., 2., 3., 4., 0.},
                                        {1., 0., 3., 0., 1919.810},
                                        {0., 0.2, 0., 0.4, 0.},
                                        {114.514, -2., -3., -4., 1.}};

int main(void) {
    LSMat_t *const mat = LSMat_new(5, 4);
    for (size_t n = 0; n < 10; n++) {
        for (size_t j = 0; j < sizeof(TARGET_MAT) / sizeof(TARGET_MAT[0]); j++) {
            for (size_t i = 0; i < sizeof(TARGET_MAT[0]) / sizeof(TARGET_MAT[0][0]); i++) {
                LSMat_set(mat, i, j, TARGET_MAT[j][i]);
            }
        }
    }
    for (size_t j = 0; j < sizeof(TARGET_MAT) / sizeof(TARGET_MAT[0]); j++) {
        for (size_t i = 0; i < sizeof(TARGET_MAT[0]) / sizeof(TARGET_MAT[0][0]); i++) {
            printf("%.6f\t", LSMat_at(mat, i, j));
        }
        putchar('\n');
    }
    LSMat_free(mat);
    return 0;
}
