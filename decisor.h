#ifndef __DECISOR_H
#define __DECISOR_H

/*============================================================================*/

#include "game_state.h"

/*============================================================================*/

typedef struct {
  int x;
  int y;
} ponto; 

typedef struct
{
    int** mapa;
    int** mapa_prob;
    int** matriz_controle;
    int** matriz_custo;
    int* caminho;
    int achou_agua;
    int primeiros_movimentos;
    int altura_caverna;
    int largura_caverna;
    int movimentos_programados;
    int movimentos_programados_realizados;
    ponto melhor_movimento;
} Decisor;

/*============================================================================*/

Decisor* criaDecisor (int altura, int largura);
void destroiDecisor (Decisor* d);
int proximoMovimento (Decisor* d, Coordenada pos, int agua, int n_lava);

/*============================================================================*/
#endif 

