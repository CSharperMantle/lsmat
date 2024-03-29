#ifndef LSARITH_H_INCLUDED_
#define LSARITH_H_INCLUDED_

#include "lsmat.h"
#include <stdbool.h>

typedef enum lsarith_errno_ {
    LSARITH_OK,
    LSARITH_E_GEN,
    LSARITH_E_SHAPE,
    LSARITH_E_NOINV,
} lsarith_errno_t;

lsarith_errno_t LSArith_mat_add(const LSMat_t *restrict a, const LSMat_t *restrict b,
                                LSMat_t *restrict out);
lsarith_errno_t LSArith_mat_sub(const LSMat_t *restrict a, const LSMat_t *restrict b,
                                LSMat_t *restrict out);
lsarith_errno_t LSArith_mat_mul(const LSMat_t *restrict a, const LSMat_t *restrict b,
                                LSMat_t *restrict out);
LSMatView_t LSArith_mat_T(LSMat_t *restrict a);

#endif /* LSARITH_H_INCLUDED_ */
