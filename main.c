#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ID 256
#define NUM 257
#define INT 258
#define FLOAT 259
#define STRING 260
#define LT 261
#define LE 262
#define EQ 263
#define NE 264
#define GT 265
#define GE 266
#define RELOP 267
#define IF 268
#define ELSE 269
#define WHILE 270
#define PRINT 271
#define INICIO 272
#define FIM 273
#define READ 274

typedef struct token{
    int nome_token;
    int atributo;
} Token;

typedef struct palavra_reservada {
    char * nome;
    int nome_token;
} PalavraReservada;

PalavraReservada palavras_reservadas[] = {
    {"inicio", INICIO}, 
    {"fim", FIM}, 
    {"read", READ}, 
    {"print", PRINT}, 
    {"if", IF},
    {"while", WHILE},
    {"else", ELSE},
    {"int", INT},
    {"float", FLOAT},{"string", STRING}
};

int estado = 0;
int partida = 0;
int cont_simb_lidos = 0;
int valor_lexico = 0;
char * code;
char lexema[100];
char lexema_len = 0;

char * readFile(char * filename) 
{
    FILE * file = fopen(filename, "r");
    char * code;
    int n = 0;
    int c;

    if(file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    code = (char *)malloc((f_size + 1) * sizeof(char));

    while((c = fgetc(file)) != EOF) {
        code[n++] = (char) c;
    }
    code[n] = '\0';
    fclose(file);
    return code;

}

PalavraReservada * ehPalavraReservada(char * id) {
    int n = sizeof(palavras_reservadas) / sizeof(palavras_reservadas[0]);

    for(int i = 0; i < n; i++) {
        if(strcmp(palavras_reservadas[i].nome, id) == 0) {
            return &palavras_reservadas[i];
        }
    }

    return NULL;
}

int falhar() 
{
    switch (estado)
    {
    case 0:
        cont_simb_lidos++;
        break;
    
    default:
        printf("Error do compilador!");
        break;
    }
    return (partida);
}

Token lerToken() 
{
    Token token;
    char c;
    while (code[cont_simb_lidos] != '\0')
    {
        switch (estado)
        {
        case 0:
            c = code[cont_simb_lidos];
            if(c == ' ' || c == '\n') {
                estado = 0;
                cont_simb_lidos++;
            } 
            else if(c == '<') estado = 1;
            else if(c == '=') estado = 5;
            else if(c == '>') estado = 6;
            else if(isdigit(c)) estado = 13;
            else if(isalpha(c) || c == '_') estado = 10;
            else {
                // estado = falhar();
                cont_simb_lidos++;
            }
            break;
        case 1:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(c == '=') estado = 2;
            else if(c == '>') estado = 3;
            else estado = 4;
            break;
        case 2:
            cont_simb_lidos++;
            printf("<RELOP, LE>\n");
            token.nome_token = RELOP;
            token.atributo = LE;
            estado = 0;
            return token;
            break;
        case 3:
            cont_simb_lidos++;
            printf("<relop, NE>\n");
            token.nome_token = RELOP;
            token.atributo = NE;
            estado = 0;
            return(token);
            break;
        case 4:
            cont_simb_lidos++;
            printf("<relop, LT>\n");
            token.nome_token = RELOP;
            token.atributo = LT;
            estado = 0;
            return(token);
            break;
        case 5:
            cont_simb_lidos++;
            printf("<relop, EQ>\n");
            token.nome_token = RELOP;
            token.atributo = EQ;
            estado = 0;
            return(token);
            break;
        case 6:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c == '=') estado = 7;
            else estado = 8;
            break;
        case 7:
            cont_simb_lidos++;
            printf("<relop, GE>\n");
            token.nome_token = RELOP;
            token.atributo = GE;
            estado = 0;
            return(token);
            break;
        case 8:
            cont_simb_lidos++;
            printf("<relop, GT>\n");
            token.nome_token = RELOP;
            token.atributo = GT;
            estado = 0;
            return(token);
            break;

        case 9:
            c = code[cont_simb_lidos];
            if((c == ' ')||(c == '\n'))
            {
                estado = 0;
                cont_simb_lidos++;
            }
            else
            {
                /*implementar ações referentes aos estado */
                estado = falhar();
            }
        case 10:
            lexema[lexema_len++] = c;
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(isdigit(c) || isalpha(c) || c == '_') estado = 10;
            else if(c == ' ' || c == '\n') estado = 11;
            else {
                cont_simb_lidos++;
                // falhar();
            }
            break;
        case 11:
            lexema[lexema_len] = '\0';
            PalavraReservada * verificaPalavraReservada = ehPalavraReservada(lexema);
            estado = 0;
            lexema_len = 0;
            if(verificaPalavraReservada == NULL) {
                printf("<ID, %s>\n", lexema);
                token.nome_token = ID;
                // alterar
                token.atributo = 1;
                return token;
            } else {
                printf("<%s, >\n", verificaPalavraReservada->nome);
                token.nome_token = verificaPalavraReservada->nome_token;
                return token;
            }   
            break;
        case 13:
            // reconhecimento de numeros
            lexema[lexema_len++] = c;
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(isdigit(c)) {
                break;
            } else if(c == '.') estado = 13;
            else estado = 19;
            break;
        case 19:
            // numero inteiro reconhecido
            lexema[lexema_len] = '\0';
            cont_simb_lidos++;
            int valor = atoi(lexema);
            printf("<NUM, %d>\n", valor);
            token.nome_token = NUM;
            token.atributo = valor;
            estado = 0;
            lexema_len = 0;
            return token;
            break;
        default:
            cont_simb_lidos++;
            break;
        }
    }

    token.nome_token = EOF;
    token.atributo = -1;
    return token;
}

int main() 
{
    Token token;
    code = readFile("programa.txt");
    while((token = lerToken()).nome_token != EOF) {}

    free(code);
    return 0;
}
