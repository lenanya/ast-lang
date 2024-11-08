#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

typedef struct Node Node;

typedef enum {
    NK_PRINT, // temporary
    NK_CONSTANT,
    NK_VARIABLE,
    NK_VARIABLE_ASSIGNMENT,
    NK_ERROR,
} Node_Kind;

typedef enum {
    T_INT,
    T_CHAR,
    T_BOOL,
    T_FLOAT,
} Type;

typedef union {
    char *v_char;
    int v_int;
    bool v_bool;
    float v_float;
} Value;

typedef struct {
    Type type;
    char *name;
    Value value;
} NodeVariable;

typedef struct {
    Type type;
    Value value;
} NodeConstant;

typedef struct {
    char *error_text;
} NodeError;

typedef struct {
    Node *fmt;
    Node *ops;   
} NodePrint;

typedef struct {
    char *name;
    Type type;
    Node *value;
} NodeVariableAssigmnent;

typedef union {
    NodePrint print;
    NodeVariable var;
    NodeConstant constant;
    NodeError error;
} InnerNode;

struct Node {
    Node_Kind kind;
    InnerNode inner_node;
};

typedef struct {
    size_t count;
    size_t capacity;
    Node **items;
} Program;

typedef struct {
    size_t count;
    size_t capacity;
    Node **items;
} Variables;

typedef struct {
    size_t count;
    size_t capacity;
    char **items;
} VariableNames;

Node ErrorConstructor(char *error) {
    Node return_node;
    NodeError return_err;
    return_node.kind = NK_ERROR;
    return_err.error_text = error;
    return_node.inner_node.error = return_err;
    return return_node;
}

Node BoolConstructor(bool val) {
    Node return_node;
    NodeConstant return_const;
    return_node.kind = NK_CONSTANT;
    return_const.type = T_BOOL;
    return_const.value.v_bool = val;
    return_node.inner_node.constant = return_const;
    return return_node;
}

Node IntConstructor(int val) {
    Node return_node;
    NodeConstant return_const;
    return_node.kind = NK_CONSTANT;
    return_const.type = T_INT;
    return_const.value.v_int = val;
    return_node.inner_node.constant = return_const;
    return return_node;
}

Node FloatConstructor(float val) {
    Node return_node;
    NodeConstant return_const;
    return_node.kind = NK_CONSTANT;
    return_const.type = T_FLOAT;
    return_const.value.v_float = val;
    return_node.inner_node.constant = return_const;
    return return_node;
}

void CharConstructor(Node *ret, NodeConstant *ret_const, char *val) {
    ret->kind = NK_CONSTANT;
    ret_const->type = T_CHAR;
    ret_const->value.v_char = val;
    ret->inner_node.constant = *ret_const;
}                   

Node ConstFromVar(Node variable) {
    Node return_node;
    NodeConstant return_const;
    return_node.kind = NK_CONSTANT;
    return_const.type = variable.inner_node.var.type;
    switch (return_const.type) {
        case (T_BOOL):
            return_const.value.v_bool = variable.inner_node.var.value.v_bool;
            break;
        case (T_INT):
            return_const.value.v_int = variable.inner_node.var.value.v_int;
            break;
        case (T_FLOAT):
            return_const.value.v_float = variable.inner_node.var.value.v_float;
            break;
        case (T_CHAR):
            return_const.value.v_char = variable.inner_node.var.value.v_char;
            break;
    }
    return_node.inner_node.constant = return_const;
    return return_node;
}

Node VarFromName(Variables *vars, VariableNames *var_names, char *name) {
    for (size_t i = 0; i < var_names->count; ++i) {
        if (strcmp(var_names->items[i], name) == 0) {
            return *vars->items[i];
        }
    }
    Node err = ErrorConstructor("Unknown Variable");
    return err;
}

Node eval(Node node) {
    switch (node.kind) {
        case NK_PRINT:
            char *fmt = eval(*node.inner_node.print.fmt).inner_node.constant.value.v_char;
            Node ops_node = eval(*node.inner_node.print.ops); 
            switch (ops_node.inner_node.constant.type) {
                case (T_BOOL):
                    bool ops_bool = ops_node.inner_node.constant.value.v_bool;
                    printf(fmt, ops_bool);
                    break;
                case (T_CHAR):
                    char *ops_char = ops_node.inner_node.constant.value.v_char;
                    printf(fmt, ops_char);
                    break;
                case (T_FLOAT):
                    float ops_float = ops_node.inner_node.constant.value.v_float;
                    printf(fmt, ops_float);
                    break;
                case (T_INT):
                    int ops_int = ops_node.inner_node.constant.value.v_int;
                    printf(fmt, ops_int);
                    break;
                default:
                    return BoolConstructor(true);
            } 
            return BoolConstructor(false);
        case NK_VARIABLE:
            return ConstFromVar(node);
        case NK_CONSTANT:
        case NK_ERROR:
            return node;
        default:
            UNREACHABLE("why u here");
    }
}

bool exec(Program prog) {
    Variables vars = {0};
    VariableNames var_names = {0};

    for (size_t i = 0; i < prog.count; ++i) {
        Node result = eval(*prog.items[i]);
        if (result.kind == NK_ERROR) {
            nob_log(ERROR, result.inner_node.error.error_text);
            return 1;
        }
        if (result.kind != NK_CONSTANT) {
            return 1;
        }
        if (result.inner_node.constant.type != T_BOOL) {
            return 1;
        }
        if (result.inner_node.constant.value.v_bool) {
            return 1;
        }
    }
    return 0;
}

int main(void) {
    Program prog = {0};

    Node error = ErrorConstructor("err");

    da_append(&prog, &error);

    //Node entry;
    //entry.kind = NK_PRINT;

    //Node fmt;
    //NodeConstant fmt_const;
    //CharConstructor(&fmt, &fmt_const, "%s\n");

    //Node fmt_bool;
    //NodeConstant fmt_bool_const;
    //CharConstructor(&fmt_bool, &fmt_bool_const, "%d\n");

    //Node ops;
    //ops.kind = NK_PRINT;

    //Node ops2;
    //NodeConstant ops2_const;
    //CharConstructor(&ops2, &ops2_const, "Hello World");

    //ops.inner_node.print = (NodePrint) {&fmt, &ops2};

    //entry.inner_node.print = (NodePrint) {&fmt_bool, &ops}; 
    //Node ops;
    //NodeConstant ops_const;
    //CharConstructor(&ops, &ops_const, "Hello World");

    //entry.inner_node.print = (NodePrint){&fmt, &ops};

    //da_append(&prog, &entry);
    
    if (exec(prog)) return 1;

    da_free(prog);

    return 0; 
}