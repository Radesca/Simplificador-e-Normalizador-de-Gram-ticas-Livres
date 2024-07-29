#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIMBOLOS 256
#define MAX_PRODUCOES 256

typedef struct {
    char lado_esquerdo;
    char *lado_direito[MAX_PRODUCOES];
    int contagem_producoes;
} Regra;

typedef struct {
    Regra regras[MAX_SIMBOLOS];
    int contagem_regras;
} Gramatica;

int eh_terminal(char c) {
    return c >= 'a' && c <= 'z';
}

int eh_nao_terminal(char c) {
    return c >= 'A' && c <= 'Z';
}

void adicionar_regra(Gramatica *gramatica, char lado_esquerdo, const char *lado_direito) {
    for (int i = 0; i < gramatica->contagem_regras; i++) {
        if (gramatica->regras[i].lado_esquerdo == lado_esquerdo) {
            gramatica->regras[i].lado_direito[gramatica->regras[i].contagem_producoes] = strdup(lado_direito);
            gramatica->regras[i].contagem_producoes++;
            return;
        }
    }
    gramatica->regras[gramatica->contagem_regras].lado_esquerdo = lado_esquerdo;
    gramatica->regras[gramatica->contagem_regras].lado_direito[0] = strdup(lado_direito);
    gramatica->regras[gramatica->contagem_regras].contagem_producoes = 1;
    gramatica->contagem_regras++;
}

void imprimir_gramatica(Gramatica *gramatica) {
    for (int i = 0; i < gramatica->contagem_regras; i++) {
        printf("%c ->", gramatica->regras[i].lado_esquerdo);
        for (int j = 0; j < gramatica->regras[i].contagem_producoes; j++) {
            printf(" %s", gramatica->regras[i].lado_direito[j]);
            if (j < gramatica->regras[i].contagem_producoes - 1) {
                printf(" |");
            }
        }
        printf("\n");
    }
}

void remover_simbolos_inatingiveis(Gramatica *gramatica) {
    int inatingivel[MAX_SIMBOLOS] = {0};
    inatingivel['S'] = 1;

    int alterado = 1;
    while (alterado) {
        alterado = 0;
        for (int i = 0; i < gramatica->contagem_regras; i++) {
            if (inatingivel[gramatica->regras[i].lado_esquerdo]) {
                for (int j = 0; j < gramatica->regras[i].contagem_producoes; j++) {
                    for (char *p = gramatica->regras[i].lado_direito[j]; *p; p++) {
                        if (eh_nao_terminal(*p) && !inatingivel[*p]) {
                            inatingivel[*p] = 1;
                            alterado = 1;
                        }
                    }
                }
            }
        }
    }

    Gramatica nova_gramatica = {0};
    for (int i = 0; i < gramatica->contagem_regras; i++) {
        if (inatingivel[gramatica->regras[i].lado_esquerdo]) {
            for (int j = 0; j < gramatica->regras[i].contagem_producoes; j++) {
                int valido = 1;
                for (char *p = gramatica->regras[i].lado_direito[j]; *p; p++) {
                    if (eh_nao_terminal(*p) && !inatingivel[*p]) {
                        valido = 0;
                        break;
                    }
                }
                if (valido) {
                    adicionar_regra(&nova_gramatica, gramatica->regras[i].lado_esquerdo, gramatica->regras[i].lado_direito[j]);
                }
            }
        }
    }

    *gramatica = nova_gramatica;
}

void remover_simbolos_improdutivos(Gramatica *gramatica) {
    int produtivo[MAX_SIMBOLOS] = {0};

    int alterado = 1;
    while (alterado) {
        alterado = 0;
        for (int i = 0; i < gramatica->contagem_regras; i++) {
            for (int j = 0; j < gramatica->regras[i].contagem_producoes; j++) {
                int todos_produtivos = 1;
                for (char *p = gramatica->regras[i].lado_direito[j]; *p; p++) {
                    if (eh_nao_terminal(*p) && !produtivo[*p]) {
                        todos_produtivos = 0;
                        break;
                    }
                }
                if (todos_produtivos && !produtivo[gramatica->regras[i].lado_esquerdo]) {
                    produtivo[gramatica->regras[i].lado_esquerdo] = 1;
                    alterado = 1;
                }
            }
        }
    }

    Gramatica nova_gramatica = {0};
    for (int i = 0; i < gramatica->contagem_regras; i++) {
        if (produtivo[gramatica->regras[i].lado_esquerdo]) {
            for (int j = 0; j < gramatica->regras[i].contagem_producoes; j++) {
                int valido = 1;
                for (char *p = gramatica->regras[i].lado_direito[j]; *p; p++) {
                    if (eh_nao_terminal(*p) && !produtivo[*p]) {
                        valido = 0;
                        break;
                    }
                }
                if (valido) {
                    adicionar_regra(&nova_gramatica, gramatica->regras[i].lado_esquerdo, gramatica->regras[i].lado_direito[j]);
                }
            }
        }
    }

    *gramatica = nova_gramatica;
}

void remover_producoes_vazias(Gramatica *gramatica) {
    int anulavel[MAX_SIMBOLOS] = {0};

    int alterado = 1;
    while (alterado) {
        alterado = 0;
        for (int i = 0; i < gramatica->contagem_regras; i++) {
            for (int j = 0; j < gramatica->regras[i].contagem_producoes; j++) {
                if (strlen(gramatica->regras[i].lado_direito[j]) == 0) {
                    if (!anulavel[gramatica->regras[i].lado_esquerdo]) {
                        anulavel[gramatica->regras[i].lado_esquerdo] = 1;
                        alterado = 1;
                    }
                } else {
                    int todos_anulaveis = 1;
                    for (char *p = gramatica->regras[i].lado_direito[j]; *p; p++) {
                        if (!anulavel[*p]) {
                            todos_anulaveis = 0;
                            break;
                        }
                    }
                    if (todos_anulaveis && !anulavel[gramatica->regras[i].lado_esquerdo]) {
                        anulavel[gramatica->regras[i].lado_esquerdo] = 1;
                        alterado = 1;
                    }
                }
            }
        }
    }

    Gramatica nova_gramatica = {0};
    for (int i = 0; i < gramatica->contagem_regras; i++) {
        for (int j = 0; j < gramatica->regras[i].contagem_producoes; j++) {
            int tamanho = strlen(gramatica->regras[i].lado_direito[j]);
            if (tamanho > 0) {
                adicionar_regra(&nova_gramatica, gramatica->regras[i].lado_esquerdo, gramatica->regras[i].lado_direito[j]);
            }

            for (int mascara = 1; mascara < (1 << tamanho); mascara++) {
                char nova_producao[MAX_SIMBOLOS];
                int novo_tamanho = 0;
                for (int k = 0; k < tamanho; k++) {
                    if (mascara & (1 << k)) {
                        nova_producao[novo_tamanho++] = gramatica->regras[i].lado_direito[j][k];
                    }
                }
                nova_producao[novo_tamanho] = '\0';
                if (novo_tamanho > 0) {
                    adicionar_regra(&nova_gramatica, gramatica->regras[i].lado_esquerdo, nova_producao);
                }
            }
        }
    }

    *gramatica = nova_gramatica;
}

int main() {
    Gramatica gramatica = {0};

    adicionar_regra(&gramatica, 'S', "aAa");
    adicionar_regra(&gramatica, 'S', "bBv");
    adicionar_regra(&gramatica, 'A', "a");
    adicionar_regra(&gramatica, 'A', "aA");

    printf("Gramática Original:\n");
    imprimir_gramatica(&gramatica);

    remover_simbolos_inatingiveis(&gramatica);
    printf("\nApós remoção de símbolos inatingíveis:\n");
    imprimir_gramatica(&gramatica);

    remover_simbolos_improdutivos(&gramatica);
    printf("\nApós remoção de símbolos improdutivos:\n");
    imprimir_gramatica(&gramatica);

    remover_producoes_vazias(&gramatica);
    printf("\nApós remoção de produções vazias:\n");
    imprimir_gramatica(&gramatica);

    return 0;
}
