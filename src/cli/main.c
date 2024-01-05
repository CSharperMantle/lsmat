#include "lsmat/lsarith.h"
#include "lsmat/lsmat.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N_MATS 512
#define MAX_LEN_IDENT 16
#define S_UPR_ALPHANUMERIC "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define S_OPS "+-*."

#define ARR_LIT_LEN_(a_) ((sizeof(a_)) / (sizeof(a_[0])))

typedef enum cmd_errno_ {
    CONT,
    QUIT,
} cmd_errno_t;

typedef cmd_errno_t (*cmd_handler_t)(void);

typedef struct CmdHandlerPair_ {
    const char *const cmd;
    const cmd_handler_t handler;
    const char *const help_str;
} CmdHandlerPair_t;

static cmd_errno_t cmd_handler_new(void);
static cmd_errno_t cmd_handler_fillrand(void);
static cmd_errno_t cmd_handler_fillident(void);
static cmd_errno_t cmd_handler_set(void);
static cmd_errno_t cmd_handler_eval(void);
static cmd_errno_t cmd_handler_shapeof(void);
static cmd_errno_t cmd_handler_disp(void);
static cmd_errno_t cmd_handler_dispnzt(void);
static cmd_errno_t cmd_handler_dbg_nodes(void);
static cmd_errno_t cmd_handler_quit(void);
static cmd_errno_t cmd_handler_help(void);
static cmd_errno_t cmd_handler_license(void);
static cmd_errno_t cmd_handler_null(void);

static const CmdHandlerPair_t CMDS[] = {
    {.cmd = "new", .handler = cmd_handler_new, .help_str = "new <ID> <DIM0> <DIM1>"},
    {.cmd = "fillrand", .handler = cmd_handler_fillrand, .help_str = "fillrand <ID>"},
    {.cmd = "fillident", .handler = cmd_handler_fillident, .help_str = "fillident <ID>"},
    {.cmd = "set", .handler = cmd_handler_set, .help_str = "set <ID> <I0> <I1> <VAL>"},
    {.cmd = "eval", .handler = cmd_handler_eval, .help_str = "eval <DEST>=<EXPR>"},
    {.cmd = "shapeof", .handler = cmd_handler_shapeof, .help_str = "shapeof <ID>"},
    {.cmd = "disp", .handler = cmd_handler_disp, .help_str = "disp <ID> <PREC>"},
    {.cmd = "dispnzt", .handler = cmd_handler_dispnzt, .help_str = "dispnzt <ID> <PREC>"},
    {.cmd = "dbg_nodes", .handler = cmd_handler_dbg_nodes, .help_str = "dbg_nodes <ID>"},
    {.cmd = "quit", .handler = cmd_handler_quit, .help_str = "quit"},
    {.cmd = "help", .handler = cmd_handler_help, .help_str = "help"},
    {.cmd = "license", .handler = cmd_handler_license, .help_str = "license"},
    {.cmd = NULL, .handler = cmd_handler_null, .help_str = NULL},
};

static char mat_idents[MAX_LEN_IDENT][N_MATS] = {0};
static LSMat_t *mats[N_MATS] = {0};
static size_t n_mats = 0;

static bool find_ident(const char *restrict ident, size_t *restrict out) {
    size_t idx_mat = SIZE_MAX;
    bool found = false;
    for (size_t i = 0; i < n_mats; i++) {
        if (strcmp(mat_idents[i], ident) == 0) {
            idx_mat = i;
            found = true;
            break;
        }
    }
    if (out != NULL) {
        *out = idx_mat;
    }
    return found;
}

static void push_ident_and_mat(const char *restrict ident, LSMat_t *restrict mat) {
    strcpy(mat_idents[n_mats], ident);
    mats[n_mats] = mat;
    n_mats++;
}

static char *new_fmt_into(const char *restrict fmt, ...) {
    va_list args1;
    va_list args2;
    va_start(args1, fmt);
    va_copy(args2, args1);
    const int fmt_len = vsnprintf(NULL, 0, fmt, args1) + 1;
    char *fmt_buf = calloc(fmt_len, sizeof(char));
    vsnprintf(fmt_buf, fmt_len, fmt, args2);
    va_end(args1);
    va_end(args2);
    return fmt_buf;
}

static cmd_errno_t cmd_handler_new(void) {
    const char *name = strtok(NULL, " ");
    const char *s_dim0 = strtok(NULL, " ");
    const char *s_dim1 = strtok(NULL, " ");
    if (!name || !s_dim0 || !s_dim1) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }
    const long dim0 = strtol(s_dim0, NULL, 10);
    const long dim1 = strtol(s_dim1, NULL, 10);

    bool found = find_ident(name, NULL);
    if (found) {
        printf("ERROR: Identifier already defined: '%s'\n", name);
        return CONT;
    }
    unsigned long name_len = strlen(name);
    if (name_len > MAX_LEN_IDENT - 1) {
        printf("ERROR: Identifier too long (%d max)\n", MAX_LEN_IDENT);
        return CONT;
    }
    if (strspn(name, S_UPR_ALPHANUMERIC) != name_len) {
        puts("ERROR: Invalid identifier; allowed chars: '" S_UPR_ALPHANUMERIC "'");
        return CONT;
    }

    if (dim0 <= 0 || dim1 <= 0) {
        puts("ERROR: Invalid dimension size");
        return CONT;
    }

    LSMat_t *m = LSMat_new(dim0, dim1);
    if (m == NULL) {
        puts("FATAL: Matrix creation failed");
        return QUIT;
    }
    push_ident_and_mat(name, m);
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_fillrand(void) {
    const char *name = strtok(NULL, " ");
    if (!name) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }
    size_t idx_mat = SIZE_MAX;
    bool found = find_ident(name, &idx_mat);
    if (!found) {
        printf("ERROR: Undefined identifier '%s'\n", name);
        return CONT;
    }
    LSMat_t *mat = mats[idx_mat];
    srand(time(NULL));
    for (size_t i = 0; i < mat->shape[LSMAT_AXIS_0]; i++) {
        for (size_t j = 0; j < mat->shape[LSMAT_AXIS_1]; j++) {
            if (LSMat_set(mat, i, j, (double)rand() / (double)RAND_MAX) != LSMAT_OK) {
                puts("FATAL: Matrix element set failed");
                return QUIT;
            }
        }
    }
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_fillident(void) {
    const char *name = strtok(NULL, " ");
    if (!name) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }
    size_t idx_mat = SIZE_MAX;
    bool found = find_ident(name, &idx_mat);
    if (!found) {
        printf("ERROR: Undefined identifier '%s'\n", name);
        return CONT;
    }
    LSMat_t *mat = mats[idx_mat];
    if (mat->shape[LSMAT_AXIS_0] != mat->shape[LSMAT_AXIS_1]) {
        puts("ERROR: Not a square matrix");
        return CONT;
    }
    if (LSMat_zero(mat) != LSMAT_OK) {
        puts("FATAL: Matrix zeroing failed");
        return QUIT;
    }
    for (size_t i = 0; i < mat->shape[LSMAT_AXIS_0]; i++) {
        if (LSMat_set(mat, i, i, 1.0) != LSMAT_OK) {
            puts("FATAL: Matrix element set failed");
            return QUIT;
        }
    }
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_set(void) {
    const char *name = strtok(NULL, " ");
    const char *s_i0 = strtok(NULL, " ");
    const char *s_i1 = strtok(NULL, " ");
    const char *s_val = strtok(NULL, " ");
    if (!name || !s_i0 || !s_i1 || !s_val) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }
    const long i0 = strtol(s_i0, NULL, 10);
    const long i1 = strtol(s_i1, NULL, 10);
    const double val = strtold(s_val, NULL);
    size_t idx_mat = SIZE_MAX;
    bool found = find_ident(name, &idx_mat);
    if (!found) {
        printf("ERROR: Undefined identifier '%s'\n", name);
        return CONT;
    }
    lsmat_errno_t err = LSMat_set(mats[idx_mat], i0, i1, val);
    if (err != LSMAT_OK) {
        puts("ERROR: Failed to set value");
        return CONT;
    }
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_eval(void) {
    const char *dest_name = strtok(NULL, "=");
    char *expr = strtok(NULL, "=");
    if (!dest_name || !expr) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }

    bool found = find_ident(dest_name, NULL);
    if (found) {
        printf("ERROR: Identifier already defined: '%s'\n", dest_name);
        return CONT;
    }
    // Check if the second part contains .T
    if (strstr(expr, ".T")) {
        char *arg1 = strtok(expr, ".T");
        if (!arg1) {
            puts("ERROR: Invalid syntax; missing unary operand");
            return CONT;
        }
        size_t idx_arg1 = SIZE_MAX;
        bool found = find_ident(arg1, &idx_arg1);
        if (!found) {
            printf("ERROR: Undefined identifier: '%s'\n", arg1);
            return CONT;
        }
        LSMat_t *m = LSMatView_realize(LSArith_mat_T(mats[idx_arg1]));
        push_ident_and_mat(dest_name, m);
    } else {
        const char *op = strpbrk(expr, "+-*");
        if (!op) {
            puts("ERROR: Invalid syntax; missing operator");
            return CONT;
        }
        const char ch_op = op[0];

        char *arg1 = strtok(expr, "+-*");
        if (!arg1) {
            puts("ERROR: Invalid syntax; missing 1st binary operand");
            return CONT;
        }
        size_t idx_arg1 = SIZE_MAX;
        bool found = find_ident(arg1, &idx_arg1);
        if (!found) {
            printf("ERROR: Undefined identifier: '%s'\n", arg1);
            return CONT;
        }
        LSMat_t *mat_arg1 = mats[idx_arg1];

        char *arg2 = strtok(NULL, "+-*");
        if (!arg2) {
            puts("ERROR: Invalid syntax; missing 2nd binary operand");
            return CONT;
        }
        size_t idx_arg2 = SIZE_MAX;
        found = find_ident(arg2, &idx_arg2);
        if (!found) {
            printf("ERROR: Undefined identifier: '%s'\n", arg2);
            return CONT;
        }
        LSMat_t *mat_arg2 = mats[idx_arg2];

        switch (ch_op) {
        case '+': {
            LSMat_t *m = LSMat_new(mat_arg1->shape[LSMAT_AXIS_0], mat_arg1->shape[LSMAT_AXIS_1]);
            lsarith_errno_t err = LSArith_mat_add(mat_arg1, mat_arg2, m);
            switch (err) {
            case LSARITH_OK:
                push_ident_and_mat(dest_name, m);
                break;
            case LSARITH_E_SHAPE:
                printf("ERROR: Inconsistent shapes for '%c': (%zu,%zu) and (%zu,%zu)\n", ch_op,
                       mat_arg1->shape[LSMAT_AXIS_0], mat_arg1->shape[LSMAT_AXIS_1],
                       mat_arg2->shape[LSMAT_AXIS_0], mat_arg2->shape[LSMAT_AXIS_1]);
                return CONT;
            default:
                puts("FATAL: General arithmetic error");
                return QUIT;
            }
            break;
        }
        case '-': {
            LSMat_t *m = LSMat_new(mat_arg1->shape[LSMAT_AXIS_0], mat_arg1->shape[LSMAT_AXIS_1]);
            lsarith_errno_t err = LSArith_mat_sub(mat_arg1, mat_arg2, m);
            switch (err) {
            case LSARITH_OK:
                push_ident_and_mat(dest_name, m);
                break;
            case LSARITH_E_SHAPE:
                printf("ERROR: Inconsistent shapes for '%c': (%zu,%zu) and (%zu,%zu)\n", ch_op,
                       mat_arg1->shape[LSMAT_AXIS_0], mat_arg1->shape[LSMAT_AXIS_1],
                       mat_arg2->shape[LSMAT_AXIS_0], mat_arg2->shape[LSMAT_AXIS_1]);
                return CONT;
            default:
                puts("FATAL: General arithmetic error");
                return QUIT;
            }
            break;
        }
        case '*': {
            LSMat_t *m = LSMat_new(mat_arg1->shape[LSMAT_AXIS_0], mat_arg2->shape[LSMAT_AXIS_1]);
            lsarith_errno_t err = LSArith_mat_mul(mat_arg1, mat_arg2, m);
            switch (err) {
            case LSARITH_OK:
                push_ident_and_mat(dest_name, m);
                break;
            case LSARITH_E_SHAPE:
                printf("ERROR: Inconsistent shapes for '%c': (%zu,%zu) and (%zu,%zu)\n", ch_op,
                       mat_arg1->shape[LSMAT_AXIS_0], mat_arg1->shape[LSMAT_AXIS_1],
                       mat_arg2->shape[LSMAT_AXIS_0], mat_arg2->shape[LSMAT_AXIS_1]);
                return CONT;
            default:
                puts("FATAL: General arithmetic error");
                return QUIT;
            }
            break;
        }
        default:
            printf("FATAL: Unreachable: '%c'\n", ch_op);
            return QUIT;
        }
    }
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_shapeof(void) {
    const char *name = strtok(NULL, " ");
    if (!name) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }
    size_t idx_mat = SIZE_MAX;
    bool found = find_ident(name, &idx_mat);
    if (!found) {
        printf("ERROR: Undefined identifier '%s'\n", name);
        return CONT;
    }
    const LSMat_t *mat = mats[idx_mat];
    printf("(%zu,%zu)\n", mat->shape[LSMAT_AXIS_0], mat->shape[LSMAT_AXIS_1]);
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_disp(void) {
    const char *name = strtok(NULL, " ");
    const char *s_prec = strtok(NULL, " ");
    if (!name || !s_prec) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }
    const long prec = strtol(s_prec, NULL, 10);
    if (prec < 0) {
        puts("ERROR: Invalid PREC; non-negative integer wanted");
        return CONT;
    }
    size_t idx_mat = SIZE_MAX;
    bool found = find_ident(name, &idx_mat);
    if (!found) {
        printf("ERROR: Undefined identifier '%s'\n", name);
        return CONT;
    }
    char *fmt_buf = new_fmt_into("%%.%ldf ", prec);
    const LSMat_t *mat = mats[idx_mat];
    for (size_t i = 0; i < mat->shape[LSMAT_AXIS_0]; i++) {
        for (size_t j = 0; j < mat->shape[LSMAT_AXIS_1]; j++) {
            printf(fmt_buf, LSMat_at(mat, i, j));
        }
        putchar('\n');
    }
    free(fmt_buf);
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_dispnzt(void) {
    const char *name = strtok(NULL, " ");
    const char *s_prec = strtok(NULL, " ");
    if (!name || !s_prec) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }
    const long prec = strtol(s_prec, NULL, 10);
    if (prec < 0) {
        puts("ERROR: Invalid PREC; non-negative integer wanted");
        return CONT;
    }
    size_t idx_mat = SIZE_MAX;
    bool found = find_ident(name, &idx_mat);
    if (!found) {
        printf("ERROR: Undefined identifier '%s'\n", name);
        return CONT;
    }
    char *fmt_buf = new_fmt_into("(%%zu,%%zu): %%.%ldf\n", prec);
    const LSMat_t *mat = mats[idx_mat];
    for (size_t i = 0; i < mat->shape[LSMAT_AXIS_0]; i++) {
        LSMatHead_t h = mat->heads[LSMAT_AXIS_0][i];
        LSMatCell_t *p = h.first_cell;
        while (p != NULL) {
            printf(fmt_buf, p->axes[LSMAT_AXIS_0].i, p->axes[LSMAT_AXIS_1].i, p->v);
            p = p->axes[LSMAT_AXIS_0].next;
        }
    }
    free(fmt_buf);
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_dbg_nodes(void) {
    const char *name = strtok(NULL, " ");
    if (!name) {
        puts("ERROR: Missing arguments; type help to learn more");
        return CONT;
    }
    size_t idx_mat = SIZE_MAX;
    bool found = find_ident(name, &idx_mat);
    if (!found) {
        printf("ERROR: Undefined identifier '%s'\n", name);
        return CONT;
    }
    size_t n = 0;
    const LSMat_t *mat = mats[idx_mat];
    for (size_t i = 0; i < mat->shape[LSMAT_AXIS_0]; i++) {
        LSMatHead_t h = mat->heads[LSMAT_AXIS_0][i];
        LSMatCell_t *p = h.first_cell;
        while (p != NULL) {
            n++;
            p = p->axes[LSMAT_AXIS_0].next;
        }
    }
    printf("%zu\n", n);
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_quit(void) {
    puts("OK");
    return QUIT;
}

static cmd_errno_t cmd_handler_help(void) {
    for (size_t i = 0; i < ARR_LIT_LEN_(CMDS); i++) {
        if (CMDS[i].cmd != NULL) {
            puts(CMDS[i].help_str);
        }
    }
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_license(void) {
    puts("BSD 3-Clause License");
    puts("");
    puts("Copyright (c) 2023-2024, Rong \"Mantle\" Bao <baorong2005@126.com>");
    puts("");
    puts("Redistribution and use in source and binary forms, with or without");
    puts("modification, are permitted provided that the following conditions are met:");
    puts("");
    puts("1. Redistributions of source code must retain the above copyright notice, this");
    puts("   list of conditions and the following disclaimer.");
    puts("");
    puts("2. Redistributions in binary form must reproduce the above copyright notice,");
    puts("   this list of conditions and the following disclaimer in the documentation");
    puts("   and/or other materials provided with the distribution.");
    puts("");
    puts("3. Neither the name of the copyright holder nor the names of its");
    puts("   contributors may be used to endorse or promote products derived from");
    puts("   this software without specific prior written permission.");
    puts("");
    puts("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"");
    puts("AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE");
    puts("IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE");
    puts("DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE");
    puts("FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL");
    puts("DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR");
    puts("SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER");
    puts("CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,");
    puts("OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE");
    puts("OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.");
    puts("");
    puts("OK");
    return CONT;
}

static cmd_errno_t cmd_handler_null(void) {
    puts("ERROR: Invalid command; type help to learn more");
    return CONT;
}

static char *readline_gets(const char *restrict prompt) {
    static char *line_read = NULL;
    if (line_read) {
        free(line_read);
        line_read = NULL;
    }
    line_read = readline(prompt);
    if (line_read && *line_read) {
        add_history(line_read);
    }
    return line_read;
}

static cmd_errno_t main_loop(void) {
    char *buf_input = readline_gets("lsmat_cli > ");

    const char *cmd_str = strtok(buf_input, " ");
    cmd_errno_t e = QUIT;
    for (size_t i = 0; i < ARR_LIT_LEN_(CMDS); i++) {
        if (CMDS[i].cmd == NULL || strcmp(CMDS[i].cmd, cmd_str) == 0) {
            e = CMDS[i].handler();
            break;
        }
    }

    return e;
}

int main(void) {
    while (main_loop() == CONT) {
        ;
    }
    puts("INFO: Cleaning up and quitting");
    for (size_t i = 0; i < n_mats; i++) {
        LSMat_free(mats[i]);
    }
    return 0;
}
