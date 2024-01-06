#include "lsmat/lsarith.h"
#include "lsmat/lsmat.h"
#include <stdbool.h>
#include <stdlib.h>

static lsarith_errno_t LSArith_mat_is_same_shape_2_(const LSMat_t *const restrict a,
                                                    const LSMat_t *const restrict b) {
    if (a == NULL || b == NULL) {
        return LSARITH_E_GEN;
    }
    for (size_t i = 0; i < LSMAT_AXIS_COUNT_; i++) {
        if (a->shape[i] != b->shape[i]) {
            return LSARITH_E_SHAPE;
        }
    }
    return LSARITH_OK;
}

static lsarith_errno_t LSArith_mat_is_same_shape_3_(const LSMat_t *const restrict a,
                                                    const LSMat_t *const restrict b,
                                                    const LSMat_t *const restrict c) {
    return LSArith_mat_is_same_shape_2_(a, b) == LSARITH_OK &&
                   LSArith_mat_is_same_shape_2_(b, c) == LSARITH_OK
               ? LSARITH_OK
               : LSARITH_E_SHAPE;
}

static lsarith_errno_t LSArith_mat_addsub_(const LSMat_t *restrict a, const LSMat_t *restrict b,
                                           LSMat_t *restrict out, bool sub) {
    if (LSArith_mat_is_same_shape_3_(a, b, out) != LSARITH_OK) {
        return LSARITH_E_SHAPE;
    }
    LSMat_zero(out);
    for (size_t i = 0; i < a->shape[LSMAT_AXIS_0]; i++) {
        LSMatCell_t *pa = a->heads[LSMAT_AXIS_0][i].first_cell;
        LSMatCell_t *pb = b->heads[LSMAT_AXIS_0][i].first_cell;
        if (pa == NULL && pb == NULL) {
            continue;
        }
        size_t j = 0;
        while ((pa != NULL || pb != NULL) && j < a->shape[LSMAT_AXIS_1]) {
            double va = LSMatCell_find_cont(&pa, j, LSMAT_AXIS_1);
            double vb = LSMatCell_find_cont(&pb, j, LSMAT_AXIS_1);
            LSMat_set(out, i, j, sub ? va - vb : va + vb);
        }
    }
    return LSARITH_OK;
}

lsarith_errno_t LSArith_mat_add(const LSMat_t *restrict a, const LSMat_t *restrict b,
                                LSMat_t *restrict out) {
    return LSArith_mat_addsub_(a, b, out, false);
}

lsarith_errno_t LSArith_mat_sub(const LSMat_t *restrict a, const LSMat_t *restrict b,
                                LSMat_t *restrict out) {
    return LSArith_mat_addsub_(a, b, out, true);
}

lsarith_errno_t LSArith_mat_mul(const LSMat_t *restrict a, const LSMat_t *restrict b,
                                LSMat_t *restrict out) {
    if (a->shape[LSMAT_AXIS_1] != b->shape[LSMAT_AXIS_0] ||
        out->shape[LSMAT_AXIS_0] != a->shape[LSMAT_AXIS_0] ||
        out->shape[LSMAT_AXIS_1] != b->shape[LSMAT_AXIS_1]) {
        return LSARITH_E_SHAPE;
    }
    LSMat_zero(out);
    for (size_t i = 0; i < a->shape[LSMAT_AXIS_0]; i++) {
        for (size_t j = 0; j < b->shape[LSMAT_AXIS_1]; j++) {
            double sum = 0.0;
            LSMatCell_t *pa = a->heads[LSMAT_AXIS_0][i].first_cell;
            LSMatCell_t *pb = b->heads[LSMAT_AXIS_1][j].first_cell;
            size_t k = 0;
            while (pa != NULL && pb != NULL && k < a->shape[LSMAT_AXIS_1]) {
                const double va = LSMatCell_find_cont(&pa, k, LSMAT_AXIS_1);
                const double vb = LSMatCell_find_cont(&pb, k, LSMAT_AXIS_0);
                sum += va * vb;
                k++;
            }
            LSMat_set(out, i, j, sum);
        }
    }

    return LSARITH_OK;
}

LSMatView_t LSArith_mat_T(LSMat_t *restrict a) {
    LSMatView_t v = LSMatView_from(a);
    v.axes_mapping[LSMAT_AXIS_0] ^= v.axes_mapping[LSMAT_AXIS_1];
    v.axes_mapping[LSMAT_AXIS_1] ^= v.axes_mapping[LSMAT_AXIS_0];
    v.axes_mapping[LSMAT_AXIS_0] ^= v.axes_mapping[LSMAT_AXIS_1];
    return v;
}
