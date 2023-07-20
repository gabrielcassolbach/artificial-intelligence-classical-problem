#include <stdlib.h>
#include <stdio.h>
#include "decisor.h"
#include "pqueue.c"

#define RIGHT 4
#define LEFT 3
#define UP 1
#define DOWN 2

/*------------------------------------------------------------------------------------------------------*/
void alocaEstruturas (Decisor* d, int altura, int largura);
void defineValoresIniciais(Decisor* d, int altura, int largura);

int movimentosIniciais(int movimento);
int procuraAgua(Decisor* d,  Coordenada pos, int agua);
int retornaNave(Decisor* d, Coordenada pos, int agua, int n_lava);
int proxMovProgramado(Decisor* d);

int pontoValido(Decisor* d, int y, int x);
void resetarMatrizCusto(Decisor* d);
void atualizaMapa(Decisor* d, Coordenada pos, int agua, int n_lava);
int dentroMapa(Decisor* d, int y, int x);
void preencheMatrizCusto(Decisor* d, Coordenada pos, int retorna); 
void observaPontosAdjacentes(Decisor* d, Queue* q, int y, int x, int k);
int calculaCaminho(Decisor* d, Coordenada pos);
ponto retornaMelhorAlternativa(Decisor* d, int y, int x, int i);

int bombasAoRedor(Decisor* d, int y, int x);
void calcularProbabilidade(Decisor* d);
void atualizarProb(Decisor* d, int y, int x);
void probabilidadeLavaAdjacencias(Decisor* d, int y, int x);
void aoRedorLivre(Decisor* d, int y, int x);
int olhaAoRedor(Decisor* d, int y, int x);
void melhorProbabilidade(Decisor* d);
int probFixa(Decisor* d, int y, int x);
/*------------------------------------------------------------------------------------------------------*/

Decisor* criaDecisor (int altura, int largura)
{
    Decisor* d = ((Decisor*) malloc (sizeof (Decisor)));
    alocaEstruturas(d, altura, largura);
    defineValoresIniciais(d, altura, largura);
    return d;
}

void defineValoresIniciais(Decisor* d, int altura, int largura)
{
    d->achou_agua = 0; 
    d->primeiros_movimentos = 3;
    d->movimentos_programados = 0;
    d->movimentos_programados_realizados = 0;

    d->melhor_movimento.y = -1; 
    d->melhor_movimento.x = -1;
    
    d->altura_caverna = altura; 
    d->largura_caverna = largura;    
    
    for(int i = 0; i < altura; i++)
        for(int j = 0; j < largura; j++)
            d->matriz_controle[i][j] = d->mapa[i][j] = d->mapa_prob[i][j] = d->matriz_custo[i][j] = -1;

    d->mapa[0][0] = d->mapa_prob[0][0] = 0;
}

void alocaEstruturas (Decisor* d, int altura, int largura)
{
    d->mapa = (int **) malloc (sizeof (int*) * altura);
    for(int i = 0; i < altura; i++)
        {d->mapa[i] = (int *) malloc (sizeof (int) * largura);}

    d->mapa_prob = (int **) malloc (sizeof (int*) * altura);
      for(int i = 0; i < altura; i++)
        {d->mapa_prob[i] = (int *) malloc (sizeof (int) * largura);}

    d->matriz_custo = (int **) malloc (sizeof (int*) * altura);
      for(int i = 0; i < altura; i++)
        {d->matriz_custo[i] = (int *) malloc (sizeof (int) * largura);}

    d->matriz_controle = (int **) malloc (sizeof (int*) * altura);
      for(int i = 0; i < altura; i++)
        {d->matriz_controle[i] = (int *) malloc (sizeof (int) * largura);}

    d->caminho = (int*) malloc (sizeof(int)*(altura * largura + 1)); 
}


void resetarMatrizCusto(Decisor* d)
{
    for(int i = 0; i < d->altura_caverna; i++)
        for(int j = 0; j < d->largura_caverna; j++)
            d->matriz_controle[i][j] = d->matriz_custo[i][j] = -1;
}

void destroiDecisor (Decisor* d)
{
    for(int i = 0; i < d->altura_caverna; i++)
        {free(d->mapa[i]); free(d->matriz_custo[i]); free(d->matriz_controle[i]); free(d->mapa_prob[i]);}
    
    free(d->mapa); free(d->matriz_custo); free(d->matriz_controle); free(d->mapa_prob);

    free(d->caminho);
    
    free (d);
}

int proximoMovimento (Decisor* d, Coordenada pos, int agua, int n_lava)
{
    atualizaMapa (d, pos, agua, n_lava);
    calcularProbabilidade(d);
    
    if(d->primeiros_movimentos)
        return movimentosIniciais(d->primeiros_movimentos--);
    
    if(!d->achou_agua)
        return procuraAgua(d, pos, agua);
    
    return retornaNave(d, pos, agua, n_lava);
}

int movimentosIniciais(int movimento)
{
    if(movimento == 3) {return RIGHT;}
    if(movimento == 2) {return DOWN;}
    if(movimento == 1) {return LEFT;}
}

void atualizaMapa (Decisor* d, Coordenada pos, int agua, int n_lava)
{
    d->mapa[pos.y][pos.x] = n_lava; 
}

int procuraAgua(Decisor* d, Coordenada pos, int agua)
{   
    if(agua)
        {d->achou_agua = 1; d->movimentos_programados = 0;  return 0;}

    if(d->movimentos_programados)
        return proxMovProgramado(d);

    preencheMatrizCusto(d, pos, 0);
    d->movimentos_programados = calculaCaminho(d, pos);    
    
    return proxMovProgramado(d);           
}

int proxMovProgramado(Decisor* d)
{
    if(d->movimentos_programados_realizados + 1 == d->movimentos_programados)   
        {d->movimentos_programados = 0;}
    
    return d->caminho[d->movimentos_programados_realizados++];
}

int retornaNave(Decisor* d, Coordenada pos, int agua, int n_lava)
{
    preencheMatrizCusto(d, pos, 1);
    d->movimentos_programados = calculaCaminho(d, pos); 
    return proxMovProgramado(d);
}

int calculaCaminho(Decisor* d, Coordenada pos)
{
    d->movimentos_programados_realizados = 0;
    int i = 0; ponto p;

    p.y = pos.y; p.x = pos.x;

    while(d->matriz_custo[p.y][p.x])
      {p = retornaMelhorAlternativa(d, p.y, p.x, i); i++;}
    return i;
}   

ponto retornaMelhorAlternativa(Decisor* d, int y, int x, int i)
{
    int melhor_movimento = INT_MAX;  int next_move = 0;
    ponto p;
    if(dentroMapa(d, y + 1, x) && d->matriz_custo[y + 1][x] >= 0 && melhor_movimento > d->matriz_custo[y + 1][x])
        {melhor_movimento = d->matriz_custo[y + 1][x]; p.y = y + 1; p.x = x; next_move = DOWN;}
    
    if(dentroMapa(d, y - 1, x) && d->matriz_custo[y - 1][x] >= 0 && melhor_movimento > d->matriz_custo[y - 1][x])
        {melhor_movimento = d->matriz_custo[y - 1][x]; p.y = y - 1; p.x = x; next_move = UP;}
    
    if(dentroMapa(d, y, x + 1) && d->matriz_custo[y][x + 1] >= 0 && melhor_movimento > d->matriz_custo[y][x + 1])
        {melhor_movimento = d->matriz_custo[y][x + 1]; p.y = y; p.x = x + 1; next_move = RIGHT;}

    if(dentroMapa(d, y, x - 1) && d->matriz_custo[y][x - 1] >= 0 && melhor_movimento > d->matriz_custo[y][x - 1])
        {melhor_movimento = d->matriz_custo[y][x - 1]; p.y = y; p.x = x - 1; next_move = LEFT;}

    d->caminho[i] = next_move;

    return p;
}

void preencheMatrizCusto(Decisor* d, Coordenada pos, int retorna)
{
    int k = 0; int y, x;
    Queue* q = create(d->altura_caverna*d->largura_caverna);

    resetarMatrizCusto(d); calcularProbabilidade(d); melhorProbabilidade(d);

    if(retorna) 
        {y = x = 0;}
    else        
        {y = d->melhor_movimento.y; x = d->melhor_movimento.x;}

    d->matriz_custo[y][x] = k;
    observaPontosAdjacentes(d, q, y, x, k + 1);

    while(!empty(q)){
        point p = dequeue(q); 
        d->matriz_custo[p.y][p.x] = p.value;
        observaPontosAdjacentes(d, q, p.y, p.x, p.value + 1);
    }

    destroy(q);
}

void observaPontosAdjacentes(Decisor* d, Queue* q, int y, int x, int k)
{
    if(dentroMapa(d, y + 1, x) && pontoValido(d, y + 1, x)) 
        {enqueue(q, (point){x, y + 1, k}); d->matriz_controle[y + 1][x] = 1;}

    if(dentroMapa(d, y - 1, x) && pontoValido(d, y - 1, x))
        {enqueue(q, (point){x, y - 1, k}); d->matriz_controle[y - 1][x] = 1;}

    if(dentroMapa(d, y, x + 1) && pontoValido(d, y, x + 1)) 
        {enqueue(q, (point){x + 1, y, k}); d->matriz_controle[y][x + 1] = 1;}

    if(dentroMapa(d, y, x - 1) && pontoValido(d, y, x - 1)) 
        {enqueue(q, (point){x - 1, y, k}); d->matriz_controle[y][x - 1] = 1;}
}

int pontoValido(Decisor* d, int y, int x)
{
    return (d->mapa_prob[y][x] == 0 && d->matriz_custo[y][x] == -1 && d->matriz_controle[y][x] != 1);
}

void melhorProbabilidade(Decisor* d)
{
    int melhor = 101;

    for(int i = 0; i < d->altura_caverna; i++)
        for(int j = 0; j < d->largura_caverna; j++)
            if(d->mapa[i][j] == -1 && d->mapa_prob[i][j] != -1 && d->mapa_prob[i][j] < melhor)
                {melhor = d->mapa_prob[i][j]; d->melhor_movimento.y = i; d->melhor_movimento.x = j;}
}

void calcularProbabilidade(Decisor* d)
{
    for(int i = 0; i < d->altura_caverna; i++)
        for(int j = 0; j < d->largura_caverna; j++)
            if(d->mapa[i][j] != -1) {atualizarProb(d, i, j);}
}

void atualizarProb(Decisor* d, int y, int x)
{ 
    d->mapa_prob[y][x] = 0;
    probabilidadeLavaAdjacencias(d, y, x);
}

void probabilidadeLavaAdjacencias(Decisor* d, int y, int x)
{ 
    int n_lavas_ponto = d->mapa[y][x] - bombasAoRedor(d, y, x);
    
    if(!n_lavas_ponto) {
        aoRedorLivre(d, y, x);
    }else{
        int prob = (int)((100*n_lavas_ponto)/(olhaAoRedor(d, y, x)));
        if(dentroMapa(d, y + 1, x) && probFixa(d, y + 1, x))  {d->mapa_prob[y + 1][x] = prob;}
        if(dentroMapa(d, y - 1, x) && probFixa(d, y - 1, x))  {d->mapa_prob[y - 1][x] = prob;}
        if(dentroMapa(d, y, x + 1) && probFixa(d, y, x + 1))  {d->mapa_prob[y][x + 1] = prob;}
        if(dentroMapa(d, y, x - 1) && probFixa(d, y, x - 1))  {d->mapa_prob[y][x - 1] = prob;}
    }
}

int bombasAoRedor(Decisor* d, int y, int x)
{
    int bombas = 0;
    if(dentroMapa(d, y + 1, x) && d->mapa_prob[y + 1][x] == 100) {bombas++;}
    if(dentroMapa(d, y - 1, x) && d->mapa_prob[y - 1][x] == 100) {bombas++;}
    if(dentroMapa(d, y, x + 1) && d->mapa_prob[y][x + 1] == 100) {bombas++;}
    if(dentroMapa(d, y, x - 1) && d->mapa_prob[y][x - 1] == 100) {bombas++;}
    return bombas;
}

int probFixa(Decisor* d, int y, int x)
{ 
    return(d->mapa_prob[y][x] != 0 && d->mapa_prob[y][x] != 100);
}

int olhaAoRedor(Decisor* d, int y, int x)
{
    int soma = 0;
    if(dentroMapa(d, y + 1, x) && probFixa(d, y + 1, x)) {soma++;}
    if(dentroMapa(d, y - 1, x) && probFixa(d, y - 1, x)) {soma++;}
    if(dentroMapa(d, y, x + 1) && probFixa(d, y, x + 1)) {soma++;}
    if(dentroMapa(d, y, x - 1) && probFixa(d, y, x - 1)) {soma++;}
    return soma;
}

void aoRedorLivre(Decisor* d, int y, int x)
{
    if(dentroMapa(d, y + 1, x) && d->mapa_prob[y + 1][x] != 100) {d->mapa_prob[y + 1][x] = 0;}
    if(dentroMapa(d, y - 1, x) && d->mapa_prob[y - 1][x] != 100) {d->mapa_prob[y - 1][x] = 0;}
    if(dentroMapa(d, y, x + 1) && d->mapa_prob[y][x + 1] != 100) {d->mapa_prob[y][x + 1] = 0;}
    if(dentroMapa(d, y, x - 1) && d->mapa_prob[y][x - 1] != 100) {d->mapa_prob[y][x - 1] = 0;}
}

int dentroMapa(Decisor* d, int y, int x)
{
    return(y >= 0 && y < d->altura_caverna && x >= 0 && x < d->largura_caverna);
}

/*==============================================================================================*/
