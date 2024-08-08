#ifndef BARALHO_H
#define BARALHO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include "mensagem.h"

// Estrutura para representar uma carta
typedef struct {
    unsigned char tipo;
    unsigned char numero;

} carta_t;

// Imprime a carta de uma maneira inteligível
void imprime_carta(carta_t carta);

// Escolhe aleatoriamente cartas para distribuir entre os jogadores
unsigned char *escolhe_cartas(unsigned char numero_cartas, unsigned char *cartas_escolhidas);

void imprime_cartas_round(unsigned char num_cartas, carta_t* cartas_representacao, unsigned char* cartas_valor);

void distribui_cartas(unsigned char num_cartas, unsigned char flag_crescente, carta_t* cartas_representacao, unsigned char id_jogador, int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len, mensagem_t* recebido, unsigned char** cartas_valor);

#endif