#ifndef MENSAGEM_H
#define MENSAGEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

typedef struct {
    unsigned char crc8;                         // Detecção de erros
    unsigned char recebido: 1;                  // bit indicando se mensagem chegou no destino
    unsigned char sucesso: 1;                   // bit indicando se mensagem chegou íntegra
    unsigned char marcadorInicio;               // 0111 1110
    unsigned char destino: 2;                   // 2 bits para representar os 4 jogadores
    unsigned char origem: 2;                    // 2 bits para representar os 4 jogadores
    unsigned char tamanho: 4;                   // 3 bits para representar os 5 bytes
    unsigned char tipo: 3;                      // 3 bits para representar os 6 tipos
    unsigned char dados [10];             
    

} mensagem_t;

int verifica_mensagem(mensagem_t *enviado, int tipo_desejado);

// Monta a mensagem baseada na estrutura do enunciado e nos parâmetros passados
mensagem_t monta_mensagem (unsigned char dest, unsigned char orig, unsigned char tam, unsigned char tipo, unsigned char* dados, int crc_flag);

// Envia a mensagem e aguarda o retorno
int envia_mensagem (int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len, mensagem_t *mensagem, mensagem_t *mensagem_resp);

// Imprime o mensagem na tela
void imprime_mensagem (mensagem_t *mensagem);
void divulgar_mensagem(unsigned char tipo_mensagem, unsigned char *dados, unsigned char tamanho_dados, unsigned char id_jogador, int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len);

#endif // MENSAGEM_H