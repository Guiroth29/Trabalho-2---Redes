#ifndef JOGO_H
#define JOGO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include "baralho.h"
#include "mensagem.h"

#define PLAYER1 26583
#define PLAYER2 26584
#define PLAYER3 26585
#define PLAYER4 26586

void configura_endereços(int sockfd, struct sockaddr_in *my_addr, struct sockaddr_in *next_machine_addr, int porta);
void valores_jogadores(int porta, unsigned char *bastao, unsigned char *id_jogador);
void coleta_e_divulga_palpites(unsigned char id_jogador, unsigned char num_cartas, unsigned char *palpites_rodada, int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len, mensagem_t *recebido, unsigned char *destino_msg);
void coleta_e_divulga_jogadas(unsigned char id_jogador, int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len, mensagem_t *recebido, unsigned char *jogadas_rodada, unsigned char *dados, mensagem_t *enviado);

#endif 