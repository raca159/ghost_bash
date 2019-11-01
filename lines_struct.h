#ifndef PLAYED_H_
#define PLAYED_H_

/* 
Sentinela da lista de linhas, contém os campos:
- ini (TCelulalinha* - aponta para o primeiro membro da lista)
- fim (TCelulalinha* - aponta para o ultimo membro da lista)
*/
typedef struct tipolinhas TListalinha;

/*
Estrutura presente nas listas de linhas, contém os campos:
- process group id
*/
typedef struct tipolinha Tlinha;

/* 
Estrutura genérica interna das listas de linhas, contém os campos:
- linha (Tlinha* - aponta para o item que contem as informações da linha)
- prox (TCelulalinha* - aponta para a proxima célula da lista)
*/
typedef struct tipocelula_linha TCelulalinha;

/*
Busca uma linha em uma lista de linhas
-inputs: process_group_id da linha procurada e lista onde deseja se procurar
-outputs: estrtura Tlinha* referente a linha procurada
-pre-condicao: lista e process_group_id existem e são diferentes de NULL
-pos-condicao: 
*/
Tlinha* busca_linha(TListalinha* lista, int process_group_id);

/*
Inicializa a sentinela de uma lista de linhas
- inputs: nenhum
- output: Sentinela inicializado
- pre-condicao: nenhuma
- pos-condicao: sentinela da lista de retorno existe e os campos primeiro e ultimo apontam para NULL
*/
TListalinha* InicializaLista(int session_id); 

/*
* Insere uma linha na última posição da lista de linhas
* inputs: linha a ser inserido na lista (do tipo Tipolinha) e a lista
* output: nenhum
* pre-condicao: linha e lista não são nulos
* pos-condicao: lista contém a linha inserido na última posição
*/
void Inserelinha (Tlinha* linha, TListalinha* lista);


/*
* Conta numero de comandos
*/
int Contalinha (TListalinha* lista);


/*
*Retira uma linha de process_group_id process_group_id da lista de linhas
* inputs: a lista e o process_group_id da linha a ser retirado da lista
* output: a linha (do tipo Tlinha) retirado da lista ou NULL, se o aluno não se encontrar na lista
* pre-condicao: lista não é nula
* pos-condicao: lista não contém a linha de process_group_id process_group_id
*/
Tlinha* Retiralinha (TListalinha* lista, int process_group_id);

/*
*Libera toda a memória alocada para a linha
* inputs: a linha
* output: NULL
* pre-condicao: linha não é nula
* pos-condicao: memória alocada é liberada
*/
Tlinha* Liberalinha (Tlinha* linha);

/*
*Libera toda a memória alocada para a linha
* inputs: a linha
* output: NULL
* pre-condicao: linha não é nula
* pos-condicao: memória alocada é liberada
*/
TListalinha* Liberalinhas (TListalinha* linhas);

#endif
