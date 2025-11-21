#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// CLASSES
#define ID 256
#define NUM 257
#define STRING_VALOR 275
// TIPOS
#define STRING 260
#define INT 258
#define FLOAT 259
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
#define LIMITE_CASAS_DECIMAIS 5

typedef struct token{
    int nome_token;
    double atributo;
} Token;

// Struct para palavras reservadas
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
    {"float", FLOAT},
    {"string", STRING}
};

// Tabela de simbolos
typedef struct simbolo
{
    char * lexema;
    int tipo_token;
    double valor;
    struct simbolo * prox;
} Simbolo;

typedef struct string 
{
    char * valor;
    struct string * prox;
} String;

Simbolo * tabelaDeSimbolos = NULL;
String * tabelaDeStrings = NULL;
int quantidadeDeSimbolos = 0;
int quantidadeDeStrings = 0;

Simbolo * criarSimbolo(char * nome, int atributo) {
    Simbolo * el = (Simbolo *)malloc(sizeof(Simbolo));
    el->lexema = malloc(strlen(nome) + 1);
    strcpy(el->lexema, nome);
    el->tipo_token = atributo;
    el->prox = NULL;
    return el;
}

String * criarString(char * valor) {
    String * el = (String *)malloc(sizeof(String));
    el->valor = (char *)malloc(strlen(valor) + 1);
    if (el->valor == NULL) { 
        free(el); 
        return NULL; 
    }
    strcpy(el->valor, valor);
    el->prox = NULL;
    return el;
}

void inserirSimbolo(char * nome, int atributo) {
    Simbolo * el = criarSimbolo(nome, atributo);

    if (tabelaDeSimbolos == NULL) {
        tabelaDeSimbolos = el;
    } else {
        Simbolo * aux = tabelaDeSimbolos;
        while (aux->prox != NULL) {
            aux = aux->prox;
        }
        aux->prox = el;
    }

    quantidadeDeSimbolos++;
}

int inserirString(char * valor) {
    String * el = criarString(valor);

    if(tabelaDeStrings == NULL) {
        tabelaDeStrings = el;
        quantidadeDeStrings = 1;
        return quantidadeDeStrings;
    } else {
        String * aux = tabelaDeStrings;
        int posicao = 0;
        while(aux->prox != NULL) {
            posicao++;
            if(strcmp(aux->valor, valor) == 0) {
                return posicao;
            }

            aux = aux->prox;
        }
        aux->prox = el;
        quantidadeDeStrings++;
    }

    return quantidadeDeStrings;
}

int buscarSimbolo(char * id) {
    int contagem = 0;
    Simbolo * aux = tabelaDeSimbolos;
    while (aux != NULL) {
        if (strcmp(id, aux->lexema) == 0) {
            return contagem;
        }
        contagem++;
        aux = aux->prox;
    }
    return -1;
}

int estado = 0;
int partida = 0;
int cont_simb_lidos = 0;
int valor_lexico = 0;
char * code;
char lexema[100];
char lexema_len = 0;
int casas_decimais = 0;
int contadorLinha= 1;
char erro_char;
char str[100];
int stringlen=0;

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

// Verifica se o ID é uma palavra reservada
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
        printf("Caractere %c inesperado na linha %d\n", erro_char, contadorLinha);
        break;
    case 14:
        printf("Erro em numero float. Casas decimais excedidas na linha %d\n", contadorLinha);
        break;
    case 21:
        printf("Erro: string na linha %d ultrapassa o limite de caracteres\n", contadorLinha);
        break;
    case 34:
        printf("Erro no comentario longo na linha %d \n", contadorLinha);
        break;
    default:
        printf("Erro do compilador!");
        break;
    }
    return (partida);
}

Token proximo_token;
int token_cache = 0;

Token lerToken() 
{
    Token token;
    char c;
    int contadorColchetesAninhados=0;

    while (code[cont_simb_lidos] != '\0' || estado != 0)
    {
        switch (estado)
        {
        case 0:
            c = code[cont_simb_lidos];
            if(c == ' '){
                estado = 0;
                cont_simb_lidos++;
            }
            else if(c == '\n'){
                estado = 0;
                contadorLinha++;
                cont_simb_lidos++;
            }
            else if(c == '<') estado = 1;
            else if(c == '=') estado = 5;
            else if(c == '>') estado = 6;
            else if(isdigit(c)) estado = 13;
            else if(isalpha(c) || c == '_') estado = 10;
            else if(c=='"') estado = 20;
            else if(c == '-') estado = 30;
            else if(c == ';') {
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else if(c == '{') {
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else if(c == '}') {
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else if(c == '+') {
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else if(c == '*') {
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else if(c == '/') {
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            } else if(c == '(') {
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            } else if(c == ')') {
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else {
                erro_char = c;
                estado = falhar();
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
            token.nome_token = RELOP;
            token.atributo = LE;
            estado = 0;
            return token;
            break;
        case 3:
            cont_simb_lidos++;
            token.nome_token = RELOP;
            token.atributo = NE;
            estado = 0;
            return(token);
            break;
        case 4:
            cont_simb_lidos++;
            token.nome_token = RELOP;
            token.atributo = LT;
            estado = 0;
            return(token);
            break;
        case 5:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(c == '=') estado = 9;
            else {
                token.nome_token = code[cont_simb_lidos - 1];
                estado = 0;
                return(token);
            }
            break;
        case 6:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c == '=') estado = 7;
            else estado = 8;
            break;
        case 7:
            cont_simb_lidos++;
            token.nome_token = RELOP;
            token.atributo = GE;
            estado = 0;
            return(token);
            break;
        case 8:
            cont_simb_lidos++;
            token.nome_token = RELOP;
            token.atributo = GT;
            estado = 0;
            return(token);
            break;

        case 9:
            cont_simb_lidos++;
            token.nome_token = RELOP;
            token.atributo = EQ;
            estado = 0;
            return(token);
            break;
        case 10:
            lexema[lexema_len++] = c;
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(isdigit(c) || isalpha(c) || c == '_') estado = 10;
            else estado = 11;
            break;
        case 11:
            //  ID reconhecido
            lexema[lexema_len] = '\0';
            PalavraReservada * verificaPalavraReservada = ehPalavraReservada(lexema);
            
            if(verificaPalavraReservada == NULL) {
                token.nome_token = ID;
                int idExiste = buscarSimbolo(lexema);
                
                if(idExiste >= 0) {
                    token.atributo = idExiste;
                } else {
                    inserirSimbolo(lexema, ID);
                    token.atributo = quantidadeDeSimbolos - 1;
                }
            } else {
                token.nome_token = verificaPalavraReservada->nome_token;
            }   
            estado = 0;
            lexema_len = 0;
            return token;
            break;
        case 13:
            // reconhecimento de numeros
            lexema[lexema_len++] = c;
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(isdigit(c)) estado = 13;
            else if(c == '.') estado = 14;
            else estado = 16;
            break;
        case 14:
            lexema[lexema_len++] = c;
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            casas_decimais++;

            if(casas_decimais > LIMITE_CASAS_DECIMAIS) {
                estado = falhar();
                cont_simb_lidos++;
                lexema_len = 0;
                casas_decimais = 0;
                break;
            } 
            else if(isdigit(c) && casas_decimais <= LIMITE_CASAS_DECIMAIS) estado = 14;
            else estado = 15;
            break;
        case 15:
            lexema[lexema_len] = '\0';
            double valorDecimal = atof(lexema);
            token.nome_token = NUM;
            token.atributo = valorDecimal;
            estado = 0;
            lexema_len = 0;
            casas_decimais = 0;
            return token;
            break;
        case 16:
            // numero inteiro reconhecido
            lexema[lexema_len] = '\0';
            int valor = atoi(lexema);
            token.nome_token = NUM;
            token.atributo = valor;
            estado = 0;
            lexema_len = 0;
            return token;
            break;
        case 20:    //reconhecimento de string
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c=='"'){  // string vazia
                int valor_string = inserirString(""); // aponta para a tabela de strings
                token.nome_token = STRING_VALOR;
                token.atributo = valor_string;
                cont_simb_lidos++;
                estado = 0;
                return token;
            }
            else{
                stringlen = 0; // tamanho atual da string
                estado = 21;
            }
            break;
        case 21: //adicionando em str os chars e verificando ocorrencia de "\\"
            c = code[cont_simb_lidos];

            if(stringlen >= 100) {
                estado = falhar();
                
                cont_simb_lidos++;
                char char_atual = code[cont_simb_lidos];
                
                while (char_atual != '"' && char_atual != '\0') {
                    cont_simb_lidos++;
                    char_atual = code[cont_simb_lidos];
                }
                
                if (char_atual == '"') {
                    cont_simb_lidos++;
                }
                estado = 0; 
                stringlen = 0;
                break;
            }

            if(c=='\\'){
                estado = 22;
                cont_simb_lidos++;
            }
            else if(c=='"'){
                str[stringlen] = '\0';
                int valor_string = inserirString(str); // aponta para a tabela de strings
                token.nome_token = STRING_VALOR;
                token.atributo = valor_string;
                cont_simb_lidos++;
                estado = 0;
                stringlen = 0;
                return token;
            }
            else{
                estado = 21;
                str[stringlen] = c;
                stringlen++;
                cont_simb_lidos++;
            }
            break;
        case 22:    // \a, \b, \f, \n, \r, \t, \v, \\, \”
        c = code[cont_simb_lidos];
        char char_scape;

            switch (c)
            {
            case 'a':
                char_scape = '\a';
                break;
            case 'b':
                char_scape = '\b';
                break;
            case 'f':
            char_scape = '\f';
            break;
            case 'n':
                char_scape = '\n';
                break;
            case 'r':
                char_scape = '\r';
                break;
            case 't':
                char_scape = '\t';
                break;
            case 'v':
                char_scape = '\v';
                break;
            case '\\':
                char_scape = '\\';
                break;
                case '"':
                char_scape = '"';
                break;
                default:
                estado = falhar();
                cont_simb_lidos++;
                stringlen = 0;
                break;
            }
            
            estado = 21;
            str[stringlen] = char_scape;
            stringlen++;
            cont_simb_lidos++;
            break;
        case 30:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c == '-') estado = 31;
            else {
                token.nome_token = '-';
                estado = 0;
                return token;
            }
            break;
        case 31: //estado de comentário inicial (--)
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c == '\n' || c == '\0') {
                estado = 0;
            }
            else if(c=='['){
                estado=33;
            }
            else estado = 31;
            break;
        case 32: //comentário curto (-- e não possue [ imeditamente depois)
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c == '\n' || c == '\0') {
                estado = 0;
            }
            else estado = 32;
            break;
        case 33: //comentário longo inicial (--[)
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c=='['){
                estado=34;
            }
            else if(c == '\n' || c == '\0') {
                estado = 0;
            }
            else estado=32;
            break;
        case 34: // comentário longo (--[[)
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(c == '\n') contadorLinha++;
            else if(c=='['){
                estado=36;
            }
            else if(c == ']') {
                estado = 35;
            }
            else if(c=='\0'){  
                estado = falhar();
                cont_simb_lidos++;
            }
            else estado=34;
            break;
        case 35: //comentário longo (--[[])
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c==']'){
                if(contadorColchetesAninhados==0){
                    estado=0;
                    cont_simb_lidos++;
                }
                else{
                    --contadorColchetesAninhados;
                    estado=34;
                }
            }
            else estado=34;
            
            break;
        case 36: //comentário longo contando se existe [[ interno
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c=='['){
                ++contadorColchetesAninhados;
                estado=34;
            }
            else estado=34;
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


// Análise sintática
Token olharToken()
{
    if(!token_cache) {
        proximo_token = lerToken();
        token_cache = 1;
    }
    return proximo_token;
}

Token obterToken() {
    if(token_cache) {
        token_cache = 0;
        return proximo_token;
    }
    return lerToken();
}

Token token;

int simbolosSincronizacao[] = {';', IF, WHILE, ')', '(', INT, FLOAT, STRING, ID, READ, PRINT, '{', '}', NUM, '-'};
int tamanhoSimbolosSincronizacao = sizeof(simbolosSincronizacao) / sizeof(simbolosSincronizacao[0]);

void modoPanico(int *lista, int tamanho){

    int achou = 0;
    while(achou==0){

        token = obterToken();
        Token t = olharToken();
        if (token.nome_token == EOF) {
            return;
        }

        for(int i = 0; i < tamanho; i++){
            if(lista[i] == t.nome_token){
                achou = 1;
                break;
            }
        }

        for(int i = 0; i < tamanhoSimbolosSincronizacao; i++){
            if(simbolosSincronizacao[i] == t.nome_token){
                achou = 1;
                break;
            }
        }
    }
    return;
}

/* arcii code for  {=123 }=125 ,=44 )=41 (=40 ;=59 +=43 -=45 *=42 /=47
int Fdecls[8]= {ID, READ, PRINT, IF, WHILE, 123, FIM, 125};
int Fdecl[11]= {INT,FLOAT,STRING, ID, READ, PRINT, IF, WHILE, 123, FIM, 125};
int Fid[1]= {ID};
int Fcomandos[2]= {FIM, 125};
int Fcomando[9]= {ID, READ, PRINT, IF, WHILE, 123, FIM, 125, ELSE};
int Fcomando_id[1]= {59};
int Fcomando_id_linha[1]= {59};
int Fargs[1]= {41};
int Fexpr_list[1]= {41};
int Fexpr_list_linha[1]= {41};
int Fentrada[1]= {59};
int Fsaida[1]= {59};
int Fif_stmt[9]= {ID, READ, PRINT, IF, WHILE, 123, FIM, 125, ELSE};
int Felse_opt[9]= {ID, READ, PRINT, IF, WHILE, 123, FIM, 125, ELSE};
int Fwhile_stmt[9]= {ID, READ, PRINT, IF, WHILE, 123, FIM, 125, ELSE};
int Fbloco[9]= {ID, READ, PRINT, IF, WHILE, 123, FIM, 125, ELSE};
int Fexpr[2]= {44,41};
int Fexpr_linha[2]= {44,41}; 
int Fterm[]= {43, 45, 44, 41}
int Fterm_linha[]= {43, 45, 44, 41}
int Ffactor[]= {42, 47, 43, 45, 44 , 41}
*/

void expr_list();
void comando();
void comandos();
void tipo();
void decl();
void expr();

void tipo()
{
    token = obterToken();
    if(token.nome_token != STRING && 
        token.nome_token != FLOAT && 
        token.nome_token != INT) {
        printf("Erro em tipo: esperado string, float ou int na linha %d\n", contadorLinha);
        int follow[1] = {ID};
        modoPanico(follow, 1);
        return;
    }
}

void decl()
{
    int follow[11] = {INT, FLOAT, STRING, ID, READ, PRINT, IF, WHILE, '{', FIM, '}'};
    tipo();

    token = obterToken();
    if(token.nome_token != ID) {
        printf("Erro em decl: esperado id na linha %d\n", contadorLinha);
        modoPanico(follow, 11);
        return;
    }

    token = obterToken();
    if(token.nome_token != ';') {
        printf("Erro em decl: esperado ; na linha %d\n", contadorLinha);
        modoPanico(follow, 11);
        return;
    }
}

void decls()
{
    while (olharToken().nome_token == INT 
        || olharToken().nome_token == FLOAT 
        || olharToken().nome_token == STRING)
    {
        decl();
    }
}

void factor()
{
    int follow[7]= {'*', '/', '+', '-', ',', ')'};
    token = olharToken();  

    if(token.nome_token == NUM ||
       token.nome_token == ID  ||
       token.nome_token == STRING) {
            obterToken();
           return;
       }

    if(token.nome_token == '(') {
        obterToken();
        expr();
        token = obterToken(); 
        if(token.nome_token != ')') {
            printf("Erro em factor: esperado ) na linha %d\n", contadorLinha);
            modoPanico(follow, 7);
        }
        return;
    }

    if(token.nome_token == '-') {
        obterToken();
        factor();
        return;
    }

    printf("erro em factor: token inesperado na linha %d\n", contadorLinha);
    modoPanico(follow, 7);
}

void term_linha()
{
    if(olharToken().nome_token == '*' || olharToken().nome_token == '/') {
        token = obterToken();
        factor();
    }
}

void term()
{
    factor();
    term_linha();
}


void expr_linha()
{

    if(olharToken().nome_token == '+' || olharToken().nome_token == '-') {
        token = obterToken();
        term();
    }

}

void expr()
{
    term();
    expr_linha();
}

void expr_list_linha()
{
    
    if(olharToken().nome_token == ',') {
        token = obterToken();
        expr_list();
    }
}

void expr_list()
{
    expr();
    expr_list_linha();
}


void args()
{
    expr_list();
}

void comando_id_linha()
{
    int follow[] = {';'};
    token = olharToken();
    
    if(token.nome_token == '=') {  
        obterToken();  
        expr();
        return;
    }

    if(token.nome_token == '(') {
        obterToken();  
        args();
        
        token = obterToken(); 
        if(token.nome_token != ')') {
            printf("Erro em comando_id_linha: esperado ) na linha %d\n", contadorLinha);
            modoPanico(follow, 1);
        }
        return;
    }

    printf("erro em comando_id_linha: esperado = ou (\n");
    modoPanico(follow, 1);
}

void comando_id()
{
    comando_id_linha();
}


void entrada()
{
    int follow[1] = {';'};
    token = obterToken();

    if(token.nome_token != '(') {
        printf("Erro em entrada: esperado ( na linha %d\n", contadorLinha);
        modoPanico(follow, 1);
        return;
    }

    token = obterToken();

    if(token.nome_token != ID) {
        printf("Erro em entrada: esperado id na linha %d\n", contadorLinha);
        modoPanico(follow, 1);
        return;
    }

    token = obterToken();

    if(token.nome_token != ')') {
        printf("Erro em entrada: esperado ) na linha %d\n", contadorLinha);
        modoPanico(follow, 1);
        return;
    }
}

void saida()
{
    int follow[1] = {';'};
    token = obterToken();

    if(olharToken().nome_token != '(') {
        printf("Erro em saida: esperado ( na linha %d\n", contadorLinha);
        modoPanico(follow, 1);
        return;
    }

    token = obterToken();
    expr();

    token = obterToken();

    if(token.nome_token != ')') {
        printf("Erro em saida: esperado ) na linha %d\n", contadorLinha);
        modoPanico(follow, 1);
        return;
    }    
}

void else_opt()
{
    if(olharToken().nome_token == ELSE) {
        token = obterToken();
        comando();
    }
}

void if_stmt()
{
    int follow[9] = {ID, READ, PRINT, IF, WHILE, '{', FIM, '}', ELSE};
    token = obterToken();

    if(olharToken().nome_token != '(') {
        printf("Erro em if_stmt: esperado ( na linha %d\n", contadorLinha);
        modoPanico(follow, 9);
        return;
    }

    token = obterToken();
    expr();

    token = obterToken();

    if(token.nome_token != ')') {
        printf("Erro em if_stmt: esperado ) na linha %d\n", contadorLinha);
        modoPanico(follow, 9);
        return;
    }

    comando();
    else_opt();
}


void while_stmt()
{
    int follow[9] = {ID, READ, PRINT, IF, WHILE, '{', FIM, '}', ELSE};
    token = obterToken();

    if(olharToken().nome_token != '(') {
        printf("Erro em while_stmt: esperado ( na linha %d\n", contadorLinha);
        modoPanico(follow, 9);
        return;
    }
    token = obterToken();

    expr();

    token = obterToken();

    if(token.nome_token != ')') {
        printf("Erro em while_stmt: esperado ) na linha %d\n", contadorLinha);
        modoPanico(follow, 9);
        return;
    }

    comando();
}

void bloco()
{
    comandos();

    int follow[9] = {ID, READ, PRINT, IF, WHILE, '{', FIM, '}', ELSE};
    token = obterToken();

    if(token.nome_token != '}') {
        printf("Erro em bloco: esperado } na linha %d\n", contadorLinha);
        modoPanico(follow, 9);
        return;
    }
}

void comando()
{
    int follow[9] = {ID, READ, PRINT, IF, WHILE, '{', FIM, '}', ELSE};
    token = olharToken();

    if(token.nome_token == ID) {
        obterToken();
        comando_id();
        token = obterToken();

        if(token.nome_token != ';') {
            printf("Erro em comando: esperado ; na linha %d\n", contadorLinha);
            modoPanico(follow, 9);
            return;
        }

    } else if(token.nome_token == READ) {
        obterToken();
        entrada();
        token = obterToken();

        if(token.nome_token != ';') {
            printf("Erro em comando: esperado ; na linha %d\n", contadorLinha);
            modoPanico(follow, 9);
            return;
        }
    } else if(token.nome_token == PRINT) {
        saida();
        token = obterToken();

        if(token.nome_token != ';') {
            printf("Erro em comando: esperado ; na linha %d\n", contadorLinha);
            modoPanico(follow, 9);
            return;
        }
    } else if(token.nome_token == IF) {
        if_stmt();
    } else if(token.nome_token == WHILE) {
        while_stmt();
    } else if(token.nome_token == '{') {
        obterToken();
        bloco();
    } else {
        printf("Token inesperado na linha %d\n", contadorLinha);
        modoPanico(follow, 9);
    }
}

void comandos()
{
    while (olharToken().nome_token == ID
        || olharToken().nome_token == READ
        || olharToken().nome_token == PRINT
        || olharToken().nome_token == IF
        || olharToken().nome_token == WHILE
        || olharToken().nome_token == '{')
    {
        comando();
    }
}

void programa() 
{
    token = obterToken();

    if(token.nome_token != INICIO) {
        printf("Erro em programa: esperado inicio na linha %d\n", contadorLinha);
        return;
    }

    decls();
    comandos();

    token = obterToken();
    if(token.nome_token != FIM) {
        printf("Erro em programa: esperado fim na linha %d\n", contadorLinha);
        return;
    }

    token = obterToken();
    if(token.nome_token != EOF) {
        printf("Erro em programa: esperado fim de arquivo\n");
        return;
    }

    printf("fim do programa\n");
    
    return;
}

int main() 
{
    Token token;
    code = readFile("programa.txt");
    programa();

    free(code);
    return 0;
}
