#ifndef LSMAT_H_INCLUDED_
#define LSMAT_H_INCLUDED_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum lsmat_status_ {
    OK,
    ERR_GENERAL,
    ERR_DUP,
} lsmat_status_t;

typedef struct LSMatCell_ {
    size_t i_0;
    size_t i_1;
    struct LSMatCell_ *next_0;
    struct LSMatCell_ *prev_0;
    struct LSMatCell_ *next_1;
    struct LSMatCell_ *prev_1;
    double v;
} LSMatCell_t;

size_t LSMatCell_idx_of(const LSMatCell_t *restrict cell, int axis);
LSMatCell_t *LSMatCell_succ_of(const LSMatCell_t *restrict cell, int axis);
LSMatCell_t **LSMatCell_ref_succ_of(LSMatCell_t *restrict cell, int axis);
LSMatCell_t *LSMatCell_prec_of(const LSMatCell_t *restrict cell, int axis);
LSMatCell_t **LSMatCell_ref_prec_of(LSMatCell_t *restrict cell, int axis);

typedef struct LSMatHead_ {
    LSMatCell_t *first_cell;
} LSMatHead_t;

lsmat_status_t LSMatHead_init(LSMatHead_t *restrict head);
lsmat_status_t LSMatHead_destroy(LSMatHead_t *restrict head, int axis);
LSMatCell_t *LSMatHead_cell_at(const LSMatHead_t *restrict head, size_t i, int axis);
lsmat_status_t LSMatHead_insert(LSMatHead_t *restrict head, LSMatCell_t *restrict cell, int axis,
                                LSMatCell_t **restrict out_dup);
lsmat_status_t LSMatHead_remove(LSMatHead_t *restrict head, LSMatCell_t *restrict cell, int axis);

typedef struct LSMat_ {
    size_t len_0;
    size_t len_1;
    LSMatHead_t *heads_0_arr;
    LSMatHead_t *heads_1_arr;
} LSMat_t;

LSMat_t *LSMat_new(size_t len_0, size_t len_1);
lsmat_status_t LSMat_free(LSMat_t *restrict mat);
double LSMat_at(const LSMat_t *restrict mat, size_t i_0, size_t i_1);
void LSMat_set(LSMat_t *restrict mat, size_t i_0, size_t i_1, double v);

#endif /* LSMAT_H_INCLUDED_ */
