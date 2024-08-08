#include "mensagem.h"
#include "crc.h"

#include "mensagem.h"

int verifica_mensagem(mensagem_t *enviado, int tipo_desejado) {
    if (enviado->marcadorInicio != 0x7E || enviado->tipo != tipo_desejado) {
        return 0;
    }
    return 1;
}

// Função interna para imprimir os bits de cada byte na tela
void imprime_bits (unsigned char byte, int num_bits) {
    for  (int i = num_bits - 1; i >= 0; --i)
        printf ("%d",  (byte >> i) & 1);
}

void imprime_enviado (mensagem_t *enviado) {
    imprime_bits (enviado->marcadorInicio, 8);
    imprime_bits (enviado->destino, 2);
    imprime_bits (enviado->origem, 2);
    imprime_bits (enviado->tamanho, 4);
    imprime_bits (enviado->tipo, 3);

    for  (int i = 0; i < 10; ++i) {
        imprime_bits (enviado->dados [i], 8);
    }

    imprime_bits (enviado->crc8, 8);
    imprime_bits (enviado->recebido, 1);
    imprime_bits (enviado->sucesso, 1);
}

mensagem_t monta_mensagem (unsigned char dest, unsigned char orig, unsigned char tam, unsigned char tipo, unsigned char* dados, int crc_flag) {
    mensagem_t enviado;

    enviado.marcadorInicio = 0x7E; //  (0111 1110)
    enviado.destino = dest;
    enviado.origem = orig;
    enviado.tamanho = tam;
    enviado.tipo = tipo;

    if  (dados != NULL)
        memcpy (enviado.dados, dados, 10);

    // caso seja necessário, calcula o crc
    if  (crc_flag)
        enviado.crc8 = calcula_crc (&enviado);
    else
        enviado.crc8 = 0x00;

    enviado.recebido = 0;
    enviado.sucesso = 0;

    return enviado;
}

int envia_mensagem (int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len, mensagem_t *enviado, mensagem_t *recebido) {
    // Envio da mensagem
    // Segue no while enquanto o destinatário não receber com sucesso
    do {
        if  (sendto (sockfd, enviado, sizeof (*enviado), 0,  (struct sockaddr *)&next_machine_addr, addr_len) < 0)
            return 0;
        if  (recvfrom (sockfd, recebido, sizeof (*recebido), 0, NULL, NULL) < 0)
            return 0;
    } while  (!recebido->recebido || !recebido->sucesso);

    return 1;
}

// Função genérica para divulgação
void divulgar_mensagem(unsigned char tipo_mensagem, unsigned char *dados, unsigned char tamanho_dados, unsigned char id_jogador, int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len) {
    mensagem_t enviado, recebido;
    unsigned char destino_msg;

    for (unsigned char i = 1; i < 4; i++) {
        destino_msg = (id_jogador + i) % 4;

        enviado = monta_mensagem(destino_msg, id_jogador, tipo_mensagem, 0XFF, dados, tamanho_dados);

        if (!envia_mensagem(sockfd, next_machine_addr, addr_len, &enviado, &recebido)) {
            exit(EXIT_FAILURE);
        }
    }
}