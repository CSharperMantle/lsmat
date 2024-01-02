#include "lsmat/lsmat.h"
#include <mcheck.h>
#include <stdio.h>

static const double TARGET_MAT_1[4][5] = {{0., 2., 3., 4., 0.},
                                          {1., 0., 3., 0., 1919.810},
                                          {0., 0.2, 0., 0.4, 0.},
                                          {114.514, -2., -3., -4., 1.}};
static const double TARGET_MAT_2[4][5] = {{10., -5., 3., 4., 0.},
                                          {0., 10., 0., 10., 0.},
                                          {10., 0.2, 10., 0.4, 0.},
                                          {0., -21., -31., 0., 0.}};

int main(void) {
    LSMat_t *const mat = LSMat_new(5, 4);
    for (size_t n = 0; n < 1; n++) {
        for (size_t j = 0; j < sizeof(TARGET_MAT_1) / sizeof(TARGET_MAT_1[0]); j++) {
            for (size_t i = 0; i < sizeof(TARGET_MAT_1[0]) / sizeof(TARGET_MAT_1[0][0]); i++) {
                LSMat_set(mat, i, j, TARGET_MAT_1[j][i]);
            }
        }
        for (size_t j = 0; j < sizeof(TARGET_MAT_1) / sizeof(TARGET_MAT_1[0]); j++) {
            for (size_t i = 0; i < sizeof(TARGET_MAT_1[0]) / sizeof(TARGET_MAT_1[0][0]); i++) {
                if (LSMat_at(mat, i, j) != TARGET_MAT_1[j][i]) {
                    printf("ERROR: 1: (%zu,%zu)\n", i, j);
                    LSMat_free(mat);
                    return 1;
                }
            }
        }
        for (size_t j = 0; j < sizeof(TARGET_MAT_2) / sizeof(TARGET_MAT_2[0]); j++) {
            for (size_t i = 0; i < sizeof(TARGET_MAT_2[0]) / sizeof(TARGET_MAT_2[0][0]); i++) {
                LSMat_set(mat, i, j, TARGET_MAT_2[j][i]);
            }
        }
        for (size_t j = 0; j < sizeof(TARGET_MAT_2) / sizeof(TARGET_MAT_2[0]); j++) {
            for (size_t i = 0; i < sizeof(TARGET_MAT_2[0]) / sizeof(TARGET_MAT_2[0][0]); i++) {
                if (LSMat_at(mat, i, j) != TARGET_MAT_2[j][i]) {
                    printf("ERROR: 2: (%zu,%zu)\n", i, j);
                    LSMat_free(mat);
                    return 1;
                }
            }
        }
    }
    LSMat_free(mat);
    return 0;
}
