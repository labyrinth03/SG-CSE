#include<stdio.h>

// #define MAX_STACK_SIZE 20 /*maximum stack size*/
// #define MAX_EXPR_SIZE 20 /*max size of expression*/

typedef enum {lparen, rparen, plus, minus, times, divide, mod, eos, minustimes, operand, end} precedence;
// int stack[MAX_STACK_SIZE]; /* global stack */
// char expr[MAX_EXPR_SIZE]; /* input string */
// int top = -1;

typedef struct _intstack{
    int data[20];
    int top;
}stack;

typedef struct _precedencestack{
    precedence data[20];
    int top;
}pstack;

void push(pstack* s, precedence token){
    if(s->top < 20){
        int i;
        s->top++;
        s->data[s->top] = token;
    }
}
precedence pop(pstack* s){
    if(s->top >= 0){
        precedence temp;
        temp = s->data[s->top];
        s->top--;
        return temp; 
    }
}

void printstack(pstack* s){
    printf("\nstack is :");
    for(int i = 0; i < s->top+1; i++){
        printf("%d ", s->data[i]);
    }
    printf("\n");
}
precedence getToken(char *symbol, int *n, char* expr)
{
/* get the next token, symbol is the character representation, which is returned, the 
token is represented by its enumerated value, which is returned in the function name */
    *symbol = expr[(*n)++];
    switch (*symbol) {
        case '(' : return lparen;
        case ')' : return rparen;
        case '+' : return plus;
        case '-' : 
            if (*n == 1 || expr[*n-2] == '('){
                return minustimes;
            }
            else{
                return minus;
            }
        case '/' : return divide;
        case '*' : return times;
        case '%' : return mod;
        case ' ' : return eos;
        case '\0': return end;
        default : return operand; /* no error checking, default is operand */
    }
}

void printToken(precedence token){
    switch(token){
        case lparen: 
            printf("("); break;
        case rparen: 
            printf(")"); break;
        case plus: 
            printf("+"); break;
        case minus: 
            printf("-"); break;
        case divide: 
            printf("/"); break;
        case times: 
            printf("*"); break;
        case mod: 
            printf("%"); break;
        case eos: 
            printf("eos"); break;
        case minustimes: 
            printf("#"); break;
    }
}

void postfix(char* expr)
{
/* output the postfix of the expression. The expression string, stack, and the top are global */
    int isp[] = {0, 19, 12, 12, 13, 13, 13, 0, 18};
    int icp[] = {20, 19, 12, 12, 13, 13, 13, 0, 18};
    char symbol;
    int n = 0;
    int top = 0;
    int postnum = 0;
    precedence token;
    pstack stack;
    stack.top = 0;
    stack.data[0] = eos;
    int i = 0;
    for (token = getToken(&symbol, &n, expr); token != end; token = getToken(&symbol, &n, expr)) {
        if (token == operand)
            printf("%c", symbol);
        else if (token == rparen) {
            /* unstack tokens until left parenthesis */
            while (stack.data[stack.top] != lparen)
                printToken(pop(&stack));
            pop(&stack); /* discard the left parenthesis */
        }
        else {
        /* remove and print symbols whose isp is greater 
        than or equal to the current token’s icp */
            while (isp[stack.data[stack.top]] >= icp[token]){
                printToken(pop(&stack)); 
            }
            push(&stack, token);
        }
    }
     while ( (token = pop(&stack)) != eos){
        printToken(token);
     }

    printf("\n");
}

// int eval(char* expr)
// {
// /* evaluate a postfix expression, expr, maintained as a global variable. ‘\0’ is the end of the 
// expression. The stack and top of the stack are global variables. getToken is used to 
// return the tokentype and the character symbol. Operands are assumed to be single 
// character digits */
//     precedence token;
//     char symbol;
//     int op1, op2;
//     int n = 0; /* counter for the expression string */

//     stack fixstack;
//     token = getToken(&symbol, &n, expr);
//     while (token != eos) {
//         if (token == operand)
//             push(fixstack, symbol-'0'); /* stack insert */
//         else {
//         /* remove two operands, perform operation, and return result to the stack */
//             op2 = pop(fixstack); /* stack delete */
//             op1 = pop(fixstack);
//             switch (token) {
//                 case plus : push(fixstack, op1+op2); break;
//                 case minus : push(fixstack, op1-op2); break;
//                 case times : push(fixstack, op1*op2); break;
//                 case divide: push(fixstack, op1/op2); break;
//                 case minustimes: push(fixstack, op1); break; // 수정필요
//                 case mod : push(fixstack, op1%op2);
//             }
//         }
//         token = getToken(&symbol, &n, expr);
//     }
//     return pop(fixstack); /* return result */
// }

int main(){
    char expr[20];
    printf("Input: ");
    scanf("%s", expr);
    printf("Postfix: ");
    postfix(expr);

    return 0;
}