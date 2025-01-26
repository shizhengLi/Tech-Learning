#include "mpc.h"
#include <math.h>  // 添加数学库支持pow函数

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
// #include <editline/history.h>
#endif

/* 扩展运算符处理函数 */
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  if (strcmp(op, "%") == 0) { return x % y; }
  if (strcmp(op, "^") == 0) { return (long)pow(x, y); } // 类型转换
  return 0;
}

long eval(mpc_ast_t* t) {
  
  /* 基础数字处理 */
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }
  
  char* op = t->children[1]->contents;
  
  /* 处理单参数负号 */
  if (strcmp(op, "-") == 0 && t->children_num == 3) {
    return -eval(t->children[2]);
  }
  
  /* 处理min/max函数 */
  if (strcmp(op, "min") == 0 || strcmp(op, "max") == 0) {
    long result = eval(t->children[2]);  // 第一个参数
    for (int i = 3; i < t->children_num; i++) {
      if (strstr(t->children[i]->tag, "expr")) {
        long val = eval(t->children[i]);
        result = (strcmp(op, "min") == 0) ? 
          (val < result ? val : result) : 
          (val > result ? val : result);
      }
    }
    return result;
  }
  
  /* 常规运算符处理 */
  long x = eval(t->children[2]);
  int i = 3;
  while (i < t->children_num && strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}

int main(int argc, char** argv) {
  
  /* 修改语法规则 */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' | '%' | '^'       \
               | \"min\" | \"max\" ;                     \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);
  
  puts("Lispy Version 0.0.0.0.4");
  puts("Press Ctrl+c to Exit\n");
  
  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
  }
  
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  
  return 0;
}