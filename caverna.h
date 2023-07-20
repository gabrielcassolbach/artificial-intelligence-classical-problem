/*============================================================================*/
/* CAVERNA MARCIANA                                                           */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/** Fun��es para manipula��o da "caverna marciana". A caverna � representada
 * como uma matriz alocada dinamicamente e preenchida aleatoriamente. Os valores
 * de cada posi��o da matriz indicam o conte�do daquela casa no tabuleiro:
 *
 * 0: Casa livre/vazia.
 * -1: Po�o de lava.
 * 1 a 4: Casa contendo "calor" - o n�mero diz quantas casas vizinhas cont�m
 *   po�os de lava.
 * 5: Po�a d'�gua.
 * 6 a 9: Casa contendo po�a d'�gua E calor - o n�mero-5 diz quantas casas
 *   vizinhas cont�m po�os de lava.*/
/*============================================================================*/

#ifndef __CAVERNA_H
#define __CAVERNA_H

/*============================================================================*/

#define POSICAO_INVALIDA -10
#define POSICAO_COM_LAVA -1
#define POSICAO_COM_AGUA 5

/* Um par x,y. */
typedef struct
{
    int x;
    int y;
} Coordenada;

/* Representa a "caverna marciana". */
typedef struct
{
    int altura;
    int largura;
    int** tabuleiro;
} Caverna;

/*============================================================================*/

Caverna* geraCaverna (int altura, int largura, int n_pocos_lava, int random_seed);
void destroiCaverna (Caverna* caverna);
void mostraCaverna (Caverna* caverna);

int posicaoLivre (int valor_casa);
int posicaoComLava (int valor_casa);
int posicaoComCalor (int valor_casa);
int posicaoComAguaECalor (int valor_casa);
int posicaoComAgua (int valor_casa);

/*============================================================================*/
#endif /* __CAVERNA_H */
