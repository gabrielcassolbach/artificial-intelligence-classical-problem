/*============================================================================*/
/* GAME STATE PARA O JOGUINHO DA CAVERNA MARCIANA                             */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/** M�dulo b�sico que mant�m o estado de um jogo. */
/*============================================================================*/

#ifndef __GAME_STATE_H
#define __GAME_STATE_H

/*============================================================================*/

#include "caverna.h"

/*============================================================================*/
/** Dire��es para o movimento. */
#define DIR_MOVIMENTO_CIMA 1
#define DIR_MOVIMENTO_BAIXO 2
#define DIR_MOVIMENTO_ESQUERDA 3
#define DIR_MOVIMENTO_DIREITA 4

/*============================================================================*/
/** Estrutura que mant�m o estado do jogo. */

typedef struct
{
    Caverna* caverna; /* Apontador para a caverna do jogo. */
    Coordenada pos_atual; /* Posi��o atual. */
    int n_rodadas; /* N�mero de rodadas j� jogadas. */
    int** posicoes_visitadas; /* Mapa que indica quais posi��es da caverna j� foram visitadas. */
    int achou_agua; /* Flag que indica se o rob� j� coletou a amostra de �gua. */
    int morreu; /* Flag que indica se o rob� caiu em um po�o de lava. */
} GameState;

/*============================================================================*/

GameState* criaGameState (int altura, int largura, int n_pocos_lava, int random_seed);
void destroiGameState (GameState* g);
int jogoTerminado (GameState* g);
int posicaoAtualComAgua (GameState* g);
int nVizinhosComLava (GameState* g);
void mostraMapaAtual (GameState* g);
int move (GameState* g, int direcao_movimento);
int moveCima (GameState* g);
int moveBaixo (GameState* g);
int moveEsquerda (GameState* g);
int moveDireita (GameState* g);

/*============================================================================*/
#endif /* __GAME_STATE_H */
