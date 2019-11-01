#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lines_struct.h"

struct tipolinha
{
    int process_group_id;
};

struct tipocelula_linha
{
    Tlinha *linha;
    TCelulalinha *prox;
};

struct tipolinhas
{
    TCelulalinha *ini;
    TCelulalinha *fim;
    int session_id;
};


Tlinha* busca_linha(TListalinha* lista, int process_group_id)
{
    TCelulalinha* aux = lista->ini;
    while (aux!=NULL)
    {
        if(aux->linha->process_group_id==process_group_id){
            return aux->linha;
        }
        aux = aux->prox;
    }
    return NULL;
    
}

TListalinha* InicializaLista(int session_id)
{
    TListalinha *lista;
    lista = (TListalinha*)malloc(sizeof(TListalinha));
    // lista->session_id = (int)malloc(sizeof(int));
    lista->session_id = session_id;
    lista->ini = NULL;
    lista->fim = NULL;

    return lista;
}

Tlinha* InicializaTlinha(int process_group_id)
{
    Tlinha *linha = (Tlinha*)malloc(sizeof(Tlinha));
    // linha->process_group_id = (int)malloc(sizeof(int));
    linha->process_group_id =  process_group_id;
    return linha;
}

int Contalinha (TListalinha* lista){
    int counter = 0;
    TCelulalinha* aux = lista->ini;
    while (aux!=NULL)
    {
        counter++;
        aux = aux->prox;
    }
    return counter;
}

void Inserelinha(Tlinha* linha, TListalinha* lista)
{
    if(linha==NULL){
        printf("Erro na estrutura de dados Tlinha recebida");
        return;
    }
    if(lista==NULL){
        printf("Erro na estrutura de dados TListalinha recebida");
        return;
    }

    TCelulalinha* nova = (TCelulalinha*)malloc(sizeof(TCelulalinha));
    nova->linha = linha;
    nova->prox = NULL;
    if (lista->ini == NULL)
    {
        lista->ini = lista->fim = nova;
    } else
    {
    lista->fim->prox = nova;
    lista->fim = nova;
    }
}

Tlinha* Liberalinha (Tlinha* linha)
{
    // free(linha->process_group_id);
    free(linha);
    return NULL;
}

TListalinha* Liberalinhas (TListalinha* lista)
{

    if(lista!=NULL)
    {
        return NULL;
    }
    for(TCelulalinha* p=lista->ini; p!=NULL; p=p->prox)
    {
        Liberalinha(p->linha);
    }
    free(lista);
    return NULL;
}

