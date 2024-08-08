#include "baralho.h"

void imprime_carta(carta_t carta) {
    switch (carta.tipo) {
        case 0:
            printf("OURO ");
        break;
        case 1:
            printf("ESPADA ");
        break;
        case 2:
            printf("COPA ");
        break;
        case 3:
            printf("PAUS ");
        break;
        
        default:
        break;
    }

    switch (carta.numero) {
        case 0:
            printf("4 ");
        break;
        case 1:
            printf("5 ");
        break;
        case 2:
            printf("6 ");
        break;
        case 3:
            printf("7 ");
        break;
        case 4:
            printf("Q ");
        break;
        case 5:
            printf("J ");
        break;
        case 6:
            printf("K ");
        break;
        case 7:
            printf("A ");
        break;
        case 8:
            printf("2 ");
        break;
        case 9:
            printf("3 ");
        break;
        
        default:
        break;
    }
}

unsigned char *escolhe_cartas(unsigned char numero_cartas, unsigned char *cartas_escolhidas) {
    unsigned char *cartas, valor;
    cartas = (unsigned char*)malloc(numero_cartas * sizeof(unsigned char));

    unsigned char i = 0;
    while (i < numero_cartas) {
        do {
            valor = rand() % 40;
        } while (cartas_escolhidas[valor] != 0);

        cartas_escolhidas[valor] = 1;
        cartas[i] = valor;

        i++;
    }

    return cartas;
}

void distribui_cartas(unsigned char num_cartas, unsigned char flag_crescente, carta_t* cartas_representacao, unsigned char id_jogador, int sockfd, struct sockaddr_in next_machine_addr, socklen_t addr_len, mensagem_t* recebido, unsigned char** cartas_valor) {
    srand(time(NULL));
    unsigned char* cartas_escolhidas = (unsigned char*)calloc(40, sizeof(unsigned char));

    if (num_cartas == 0) {
        num_cartas += 2;
        flag_crescente = 1;
    } else if (num_cartas == 11) {
        num_cartas -= 2;
        flag_crescente = 0;
    }

    cartas_representacao = (carta_t*)malloc(num_cartas * sizeof(carta_t));

    for (unsigned char i = 1; i < 4; i++) {
        *cartas_valor = escolhe_cartas(num_cartas, cartas_escolhidas);
        unsigned char destino_msg = (id_jogador + i) % 4;
        mensagem_t enviado = monta_mensagem(destino_msg, id_jogador, num_cartas, 0X00, *cartas_valor, 1);

        if (!envia_mensagem(sockfd, next_machine_addr, addr_len, &enviado, recebido)) {
            // Handle failure if needed
            return;
        }

        free(*cartas_valor);
    }

    *cartas_valor = escolhe_cartas(num_cartas, cartas_escolhidas);
}

// Function to print round cards
void imprime_cartas_round(unsigned char num_cartas, carta_t* cartas_representacao, unsigned char* cartas_valor) {
    printf("CARTAS: \n");
    for (unsigned char i = 0; i < num_cartas; i++) {
        cartas_representacao[i].tipo = cartas_valor[i] / 10;
        cartas_representacao[i].numero = cartas_valor[i] % 10;
        printf("%d = ", (int)(i+1));
        imprime_carta(cartas_representacao[i]);
        printf("\n");
    }
    printf("\n");
}