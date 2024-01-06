#ifndef LSMAT_H_INCLUDED_
#define LSMAT_H_INCLUDED_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum lsmat_errno_ {
    LSMAT_OK,
    LSMAT_E_GEN,
    LSMAT_E_DUP,
} lsmat_errno_t;

typedef enum lsmat_axis_ {
    LSMAT_AXIS_0 = 0u,
    LSMAT_AXIS_1,
    LSMAT_AXIS_COUNT_,
} lsmat_axis_t;

typedef struct LSMat_AxisConn_ {
    size_t i;
    struct LSMatCell_ *next;
    struct LSMatCell_ *prev;
} LSMat_AxisConn_t;

typedef struct LSMatCell_ {
    LSMat_AxisConn_t axes[LSMAT_AXIS_COUNT_];
    double v;
} LSMatCell_t;

size_t LSMatCell_idx_of(const LSMatCell_t *restrict cell, lsmat_axis_t axis);
LSMatCell_t *LSMatCell_succ_of(const LSMatCell_t *restrict cell, lsmat_axis_t axis);
LSMatCell_t **LSMatCell_ref_succ_of(LSMatCell_t *restrict cell, lsmat_axis_t axis);
LSMatCell_t *LSMatCell_prec_of(const LSMatCell_t *restrict cell, lsmat_axis_t axis);
LSMatCell_t **LSMatCell_ref_prec_of(LSMatCell_t *restrict cell, lsmat_axis_t axis);
double LSMatCell_find_cont(LSMatCell_t **restrict cell, size_t i, lsmat_axis_t axis);

typedef struct LSMatHead_ {
    LSMatCell_t *first_cell;
} LSMatHead_t;

lsmat_errno_t LSMatHead_init(LSMatHead_t *restrict head);
lsmat_errno_t LSMatHead_destroy(LSMatHead_t *restrict head, lsmat_axis_t axis);
LSMatCell_t *LSMatHead_cell_at(const LSMatHead_t *restrict head, size_t i, lsmat_axis_t axis);
lsmat_errno_t LSMatHead_insert(LSMatHead_t *restrict head, LSMatCell_t *restrict cell,
                               lsmat_axis_t axis, LSMatCell_t **restrict out_dup);
lsmat_errno_t LSMatHead_remove(LSMatHead_t *restrict head, LSMatCell_t *restrict cell,
                               lsmat_axis_t axis);

typedef struct LSMat_ {
    size_t shape[LSMAT_AXIS_COUNT_];
    LSMatHead_t *heads[LSMAT_AXIS_COUNT_];
} LSMat_t;

LSMat_t *LSMat_new(size_t len_0, size_t len_1);
lsmat_errno_t LSMat_free(LSMat_t *restrict mat);
double LSMat_at(const LSMat_t *restrict mat, size_t i_0, size_t i_1);
lsmat_errno_t LSMat_set(LSMat_t *restrict mat, size_t i_0, size_t i_1, double v);
lsmat_errno_t LSMat_zero(LSMat_t *restrict mat);

typedef struct LSMatView_ {
    lsmat_axis_t axes_mapping[LSMAT_AXIS_COUNT_];
    LSMat_t *mat;
} LSMatView_t;

LSMatView_t LSMatView_from(LSMat_t *restrict mat);
size_t LSMatView_shape_of(const LSMatView_t view, lsmat_axis_t axis);
double LSMatView_at(const LSMatView_t view, size_t i_0, size_t i_1);
lsmat_errno_t LSMatView_set(const LSMatView_t view, size_t i_0, size_t i_1, double v);
LSMat_t *LSMatView_realize(const LSMatView_t view);

#endif /* LSMAT_H_INCLUDED_ */
