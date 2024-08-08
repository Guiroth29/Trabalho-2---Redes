#include "jogo.h"
#include "crc.h"

void configura_endereços(int sockfd, struct sockaddr_in *my_addr, struct sockaddr_in *next_machine_addr, int porta) {
    // Configuração do endereço da máquina atual
    memset(my_addr, 0, sizeof(*my_addr));
    my_addr->sin_family = AF_INET;
    my_addr->sin_addr.s_addr = INADDR_ANY;
    my_addr->sin_port = htons(porta);

    // Vinculação do socket ao endereço da máquina atual
    if (bind(sockfd, (struct sockaddr *)my_addr, sizeof(*my_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(-1);
    }

    // Configuração do endereço da próxima máquina
    memset(next_machine_addr, 0, sizeof(*next_machine_addr));
    next_machine_addr->sin_family = AF_INET;
    next_machine_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
    next_machine_addr->sin_port = htons(porta + 1);
}

void valores_jogadores(int porta, unsigned char *bastao, unsigned char *id_jogador) {
    switch (porta) {
        case PLAYER1:
            *bastao = 0xFF;
            *id_jogador = 0;
            break;
        case PLAYER2:
            *bastao = 0x00;
            *id_jogador = 1;
            break;
        case PLAYER3:
            *bastao = 0x00;
            *id_jogador = 2;
            break;
        case PLAYER4:
            *bastao = 0x00;
            *id_jogador = 3;
            break;
        default:
            printf("ERRO DE PORTA");
            exit(-1);
    }
}

void coleta_e_divulga_palpites(unsigned char id_jogador, unsigned char num_cartas, unsigned char *palpites_rodada, int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len, mensagem_t *recebido, unsigned char *destino_msg) {
    unsigned char palpite;

    for (unsigned char i = 1; i < 4; i++) {
        do {
            *destino_msg = (id_jogador + i) % 4;
            mensagem_t enviado = monta_mensagem(*destino_msg, id_jogador, 0x00, 0X01, 0x00, 1);

            if (!envia_mensagem(sockfd, next_machine_addr, addr_len, &enviado, recebido))
                exit(-1);

        } while (!verifica_crc(recebido));

        palpites_rodada[*destino_msg] = recebido->dados[0];
    }

    printf("APOSTA:");
    scanf("%hhu", &palpite);

    while (palpite < 0 || palpite > num_cartas) {
        printf("APOSTA IMPOSSÍVEL, TENTE NOVAMENTE! ");
        scanf("%hhu", &palpite);
    }

    palpites_rodada[id_jogador] = palpite;

    for (unsigned char i = 1; i < 4; i++) {
        *destino_msg = (id_jogador + i) % 4;
        mensagem_t enviado = monta_mensagem(*destino_msg, id_jogador, 0x04, 0X02, palpites_rodada, 1);

        if (!envia_mensagem(sockfd, next_machine_addr, addr_len, &enviado, recebido))
            exit(-1);
    }

    printf("\n");
    for (unsigned char i = 0; i < 4; i++)
        printf("APOSTA DE PLAYER%d: %d\n", (int)(i+1), (int)palpites_rodada[i]);
    printf("\n");
}

void coleta_e_divulga_jogadas(unsigned char id_jogador, int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len, mensagem_t *recebido, unsigned char *jogadas_rodada, unsigned char *dados, mensagem_t *enviado) {
    unsigned char destino_msg;
    carta_t carta_adversario;

    for (unsigned char i = 1; i < 4; i++) {
        do {
            destino_msg = (id_jogador + i) % 4;
            *enviado = monta_mensagem(destino_msg, id_jogador, 0x00, 0X03, 0x00, 1);

            if (!envia_mensagem(sockfd, next_machine_addr, addr_len, enviado, recebido))
                return;

        } while (!verifica_crc(recebido));

        jogadas_rodada[destino_msg] = recebido->dados[0];
        dados[0] = destino_msg;
        dados[1] = recebido->dados[0];

        for (unsigned char j = 1; j < 4; j++) {
            destino_msg = (id_jogador + j) % 4;
            *enviado = monta_mensagem(destino_msg, id_jogador, 0x02, 0X04, dados, 1);

            if (!envia_mensagem(sockfd, next_machine_addr, addr_len, enviado, recebido))
                return;
        }

        carta_adversario.numero = dados[1] % 10;
        carta_adversario.tipo = dados[1] / 10;
        printf("PLAYER%d: %d - %d\n", (int)(dados[0]+1), carta_adversario.numero, carta_adversario.tipo);
    }
}