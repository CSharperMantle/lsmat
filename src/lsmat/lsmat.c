#include "lsmat/lsmat.h"
#include <stdbool.h>
#include <stdlib.h>

#define FREE_NULLIFY_(ptr_)                                                                        \
    do {                                                                                           \
        free((ptr_));                                                                              \
        (ptr_) = NULL;                                                                             \
    } while (0)

size_t LSMatCell_idx_of(const LSMatCell_t *restrict cell, int axis) {
    switch (axis) {
    case 0:
        return cell->i_0;
    case 1:
        return cell->i_1;
    default:
        return false;
    }
}

LSMatCell_t *LSMatCell_succ_of(const LSMatCell_t *restrict cell, int axis) {
    if (cell == NULL) {
        return NULL;
    }
    switch (axis) {
    case 0:
        return cell->next_0;
    case 1:
        return cell->next_1;
    default:
        return NULL;
    }
}

LSMatCell_t **LSMatCell_ref_succ_of(LSMatCell_t *restrict cell, int axis) {
    if (cell == NULL) {
        return NULL;
    }
    switch (axis) {
    case 0:
        return &cell->next_0;
    case 1:
        return &cell->next_1;
    default:
        return NULL;
    }
}

LSMatCell_t *LSMatCell_prec_of(const LSMatCell_t *restrict cell, int axis) {
    if (cell == NULL) {
        return NULL;
    }
    switch (axis) {
    case 0:
        return cell->prev_0;
    case 1:
        return cell->prev_1;
    default:
        return NULL;
    }
}

LSMatCell_t **LSMatCell_ref_prec_of(LSMatCell_t *restrict cell, int axis) {
    if (cell == NULL) {
        return NULL;
    }
    switch (axis) {
    case 0:
        return &cell->prev_0;
    case 1:
        return &cell->prev_1;
    default:
        return NULL;
    }
}

lsmat_status_t LSMatHead_init(LSMatHead_t *restrict head) {
    if (head == NULL) {
        return ERR_GENERAL;
    }
    head->first_cell = NULL;
    return OK;
}

lsmat_status_t LSMatHead_destroy(LSMatHead_t *restrict head, int axis) {
    LSMatCell_t *p = head->first_cell;
    while (p != NULL) {
        LSMatCell_t *t = LSMatCell_succ_of(p, axis);
        free(p);
        p = t;
    }
    return OK;
}

LSMatCell_t *LSMatHead_cell_at(const LSMatHead_t *restrict head, size_t i, int axis) {
    if (head == NULL) {
        return NULL;
    }
    LSMatCell_t *p = head->first_cell;
    while (p != NULL) {
        if (LSMatCell_idx_of(p, axis) == i) {
            return p;
        }
        p = LSMatCell_succ_of(p, axis);
    }
    return NULL;
}

lsmat_status_t LSMatHead_insert(LSMatHead_t *restrict head, LSMatCell_t *restrict cell, int axis,
                                LSMatCell_t **restrict out_dup) {
    if (head == NULL || cell == NULL || out_dup == NULL) {
        return ERR_GENERAL;
    }
    *out_dup = NULL;
    const size_t cell_idx = LSMatCell_idx_of(cell, axis);
    LSMatCell_t *p = head->first_cell;
    LSMatCell_t *p_n = NULL;
    // Empty list
    if (p == NULL) {
        head->first_cell = cell;
        *LSMatCell_ref_prec_of(cell, axis) = NULL;
        *LSMatCell_ref_succ_of(cell, axis) = NULL;
        return OK;
    }
    // First element
    if (LSMatCell_idx_of(p, axis) == cell_idx) {
        *out_dup = p;
        return ERR_DUP;
    } else if (cell_idx < LSMatCell_idx_of(p, axis)) {
        *LSMatCell_ref_prec_of(cell, axis) = NULL;
        *LSMatCell_ref_prec_of(p, axis) = cell;
        *LSMatCell_ref_succ_of(cell, axis) = p;
        head->first_cell = cell;
        return OK;
    }
    // Body element
    while (p != NULL && (p_n = LSMatCell_succ_of(p, axis)) != NULL) {
        if (LSMatCell_idx_of(p, axis) == cell_idx) {
            *out_dup = p;
            return ERR_DUP;
        } else if (LSMatCell_idx_of(p, axis) < cell_idx && cell_idx < LSMatCell_idx_of(p_n, axis)) {
            *LSMatCell_ref_succ_of(p, axis) = cell;
            *LSMatCell_ref_prec_of(cell, axis) = p;
            *LSMatCell_ref_prec_of(p_n, axis) = cell;
            *LSMatCell_ref_succ_of(cell, axis) = p_n;
            return OK;
        }
        p = p_n;
    }
    // Tail element
    if (LSMatCell_idx_of(p, axis) == cell_idx) {
        *out_dup = p;
        return ERR_DUP;
    } else {
        *LSMatCell_ref_succ_of(p, axis) = cell;
        *LSMatCell_ref_prec_of(cell, axis) = p;
        *LSMatCell_ref_succ_of(cell, axis) = NULL;
        return OK;
    }
}

lsmat_status_t LSMatHead_remove(LSMatHead_t *restrict head, LSMatCell_t *restrict cell, int axis) {
    if (head == NULL || cell == NULL) {
        return ERR_GENERAL;
    }
    LSMatCell_t *const prec = LSMatCell_prec_of(cell, axis);
    LSMatCell_t *const succ = LSMatCell_succ_of(cell, axis);
    if (succ != NULL) {
        *LSMatCell_ref_prec_of(succ, axis) = prec;
        *LSMatCell_ref_succ_of(cell, axis) = NULL;
    }
    if (prec != NULL) {
        *LSMatCell_ref_succ_of(prec, axis) = succ;
        *LSMatCell_ref_prec_of(cell, axis) = NULL;
    } else {
        // This is the first element
        head->first_cell = succ;
    }
    return OK;
}

LSMat_t *LSMat_new(size_t len_0, size_t len_1) {
    LSMat_t *const mat = malloc(sizeof(LSMat_t));
    mat->len_0 = len_0;
    mat->len_1 = len_1;
    // calloc has covered the trivial constructor for LSMatHead_t,
    // thus we skip it.
    mat->heads_0_arr = calloc(len_0, sizeof(LSMatHead_t));
    mat->heads_1_arr = calloc(len_0, sizeof(LSMatHead_t));
    return mat;
}

lsmat_status_t LSMat_free(LSMat_t *restrict mat) {
    if (mat == NULL) {
        return ERR_GENERAL;
    }
    for (size_t i = 0; i < mat->len_0; i++) {
        LSMatHead_destroy(mat->heads_0_arr + i, 1);
    }
    FREE_NULLIFY_(mat->heads_0_arr);
    // freeing the rows is equvalent to freeing the whole matrix.
    // thus we skip freeing the columns.
    FREE_NULLIFY_(mat->heads_1_arr);
    mat->len_0 = mat->len_1 = 0;
    free(mat);
    return OK;
}

double LSMat_at(const LSMat_t *restrict mat, size_t i_0, size_t i_1) {
    if (mat == NULL) {
        return 0.;
    }
    if (i_0 > mat->len_0 || i_1 > mat->len_1) {
        return 0.;
    }
    LSMatCell_t *cell = LSMatHead_cell_at(mat->heads_0_arr + i_0, i_1, 1);
    return cell == NULL ? 0. : cell->v;
}

static void LSMat_set_nonzero_(LSMat_t *restrict mat, size_t i_0, size_t i_1, double v) {
    LSMatHead_t *const head_0 = mat->heads_0_arr + i_0;
    LSMatHead_t *const head_1 = mat->heads_1_arr + i_1;
    LSMatCell_t *new_cell = calloc(1, sizeof(LSMatCell_t));
    new_cell->i_0 = i_0;
    new_cell->i_1 = i_1;
    new_cell->v = v;
    LSMatCell_t *dup = NULL;
    if (LSMatHead_insert(head_0, new_cell, 1, &dup) == ERR_DUP ||
        LSMatHead_insert(head_1, new_cell, 0, &dup) == ERR_DUP) {
        dup->v = v;
        free(new_cell);
    }
}

static void LSMat_set_zero_(LSMat_t *restrict mat, size_t i_0, size_t i_1) {
    LSMatHead_t *const head_0 = mat->heads_0_arr + i_0;
    LSMatHead_t *const head_1 = mat->heads_1_arr + i_1;
    LSMatCell_t *cell = LSMatHead_cell_at(head_0, i_1, 1);
    if (cell == NULL) {
        return;
    }
    LSMatHead_remove(head_0, cell, 1);
    LSMatHead_remove(head_1, cell, 0);
    free(cell);
}

void LSMat_set(LSMat_t *restrict mat, size_t i_0, size_t i_1, double v) {
    if (v == 0.0) {
        LSMat_set_zero_(mat, i_0, i_1);
    } else {
        LSMat_set_nonzero_(mat, i_0, i_1, v);
    }
}
