#include "mensagem.h"
#include "baralho.h"
# include "crc.h"
#include "jogo.h"

int main() {
    // Variáveis de Controle
    int sockfd;
    socklen_t tam_addr;
    unsigned char bastao;
    unsigned char destino;
    unsigned char info[10];
    unsigned char mensagem_fim;
    mensagem_t enviado, recebido;
    struct sockaddr_in addr, prox_addr;
    
    // Variáveis de jogo
    unsigned char jogada;
    unsigned char palpite;
    unsigned char vidas[4];
    unsigned char n_cartas;
    unsigned char vencedor;
    unsigned char crescente;
    unsigned char sem_morte;
    unsigned char *escolhas;
    unsigned char jogadas[4];
    unsigned char id_jogador;
    unsigned char palpites[4];
    unsigned char vitorias[4];
    unsigned char sem_perdedor;
    unsigned char *valor_cartas;
    carta_t *cartas_representacao, carta_adversario, maior, atual;

    // Inicializa variáveis necessárias
    sem_perdedor = 1;
    n_cartas = 10;
    crescente = 0;
    sem_morte = 1;
    tam_addr = sizeof(struct sockaddr_in);

    // Criação do socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }

    while (sem_perdedor) {
        for (int porta = PLAYER1; porta <= PLAYER4; porta++) {
            configura_endereços(sockfd, &addr, &prox_addr, porta);

            valores_jogadores(porta, &bastao, &id_jogador);

            for (unsigned char i = 0; i < 4; i++) {
                vidas[i] = 10;
                printf("VIDAS PLAYER%d: %d\n", (int)(i+1), (int)vidas[i]);
            }
            printf("\n");
            
            // Carteador
            if (bastao == 0xFF) {
                distribui_cartas(n_cartas, crescente, cartas_representacao, id_jogador, sockfd, prox_addr, tam_addr, &recebido, &valor_cartas);

                imprime_cartas_round(n_cartas, cartas_representacao, valor_cartas);

                coleta_e_divulga_palpites(id_jogador, n_cartas, palpites, sockfd, prox_addr, tam_addr, &recebido, &destino);

                // Zera as vitórias de cada jogador para essa nova rodada
                for (unsigned char i = 0; i < 4; i++)
                    vitorias[i] = 0;

                // Enquanto os jogadores tiverem carta para jogar...
                for (unsigned char turno = 0; turno < n_cartas; turno++) {
                    coleta_e_divulga_jogadas(id_jogador, sockfd, prox_addr, tam_addr, &recebido, jogadas, info, &enviado); // pass the address of the variable

                    printf("ESCOLHA: ");
                    scanf("%hhu", &jogada);
                    
                    while (jogada < 1 || jogada > n_cartas || escolhas[valor_cartas[jogada-1]] == 2) {
                        printf("CARTA INVÁLIDA! ");
                        scanf("%hhu", &jogada);
                    }

                    escolhas[valor_cartas[jogada-1]] = 2;

                    jogadas[id_jogador] = valor_cartas[jogada-1];

                    info[0] = id_jogador;
                    info[1] = valor_cartas[jogada-1];

                    // divulgar jogada
                    divulgar_mensagem(0x04, info, 1, id_jogador, sockfd, prox_addr, tam_addr);

                    // Contabiliza vencedor
                    vencedor = 0;
                    maior.numero = jogadas[0] % 10;
                    maior.tipo = jogadas[0] / 10;
                    for (unsigned char i = 1; i < 4; i++) {
                        atual.numero = jogadas[i] % 10;
                        atual.tipo = jogadas[i] / 10;
                        if (atual.numero > maior.numero || (atual.numero == maior.numero && atual.tipo > maior.tipo)) {
                            vencedor = i;
                            maior.numero = atual.numero;
                            maior.tipo = atual.tipo;
                        }
                    }

                    vitorias[vencedor]++;

                    info[0] = vencedor;

                    // divulgação da jogada
                    divulgar_mensagem(0x04, info, 1, id_jogador, sockfd, prox_addr, tam_addr);


                    printf("\nPLAYER%d GANHOU!\n", (int)(vencedor+1));

                    printf("CARTAS: \n");
                    for (unsigned char i = 0; i < n_cartas; i++) {
                    
                        cartas_representacao[i].numero = valor_cartas[i] % 10;
                        cartas_representacao[i].tipo = valor_cartas[i] / 10;
                        printf("%d) ", (int)(i+1));
                        imprime_carta(cartas_representacao[i]);

                        if (escolhas[valor_cartas[i]] == 2)
                            printf("out");
                        printf("\n");
                        
                    }
                    printf("\n");

                    for (unsigned char i = 0; i < 4; i++) {
                        if (i == vencedor) {
                            if (vitorias[i] > palpites[i]) {
                                vidas[i]--;
                                if (vidas[i] == 0) {
                                    sem_perdedor = 0;
                                    break;
                                }
                            }
                        }
                        else {
                            if (palpites[i] - vitorias[i] > n_cartas - (turno + 1)) {
                                palpites[i]--;
                                vidas[i]--;
                                if (vidas[i] == 0) {
                                    sem_perdedor = 0;
                                    break;
                                }
                            }
                        }
                    }

                    if(!sem_perdedor)
                        break;
                }

                // divulgar das vidas
               divulgar_mensagem(0x04, vidas, 1, id_jogador, sockfd, prox_addr, tam_addr);

                for (unsigned char i = 0; i < 4; i++) {
                    if (vidas[i] == 0) {
                        printf("PLAYER%d PERDEU!\n", (int)(i+1));
                    }
                    else {
                        printf("PLAYER%d = %d VIDAS\n", (int)(i+1), (int)vidas[i]);
                    }
                }
                printf("\n");

                // Passa o bastão

                if (sem_perdedor) {
                    destino = (id_jogador + 1) % 4;

                    info[0] = bastao;

                    if (crescente) {
                        info[1] = n_cartas + 1;
                    }
                    else {
                        info[1] = n_cartas - 1;
                    }

                    enviado = monta_mensagem(destino, id_jogador, 0x02, 0X07, info, 1);

                    if (!envia_mensagem(sockfd, prox_addr, tam_addr, &enviado, &recebido))
                        return -1;

                    bastao = 0x00;
                    n_cartas = 0;
                }

                free(valor_cartas);
                free(escolhas);
                free(cartas_representacao);
            }
            // Jogador normal
            else {
                mensagem_fim = 0;

                if (recvfrom(sockfd, &enviado, sizeof(enviado), 0, NULL, NULL) < 0) {
                    perror("Erro no recebimento:");
                    return -1;
                }

                if ((enviado.origem != 0 && ((int)enviado.origem - (int)enviado.destino == 1)) || (enviado.origem == 0 && enviado.destino == 3))
                    mensagem_fim = 1;

                // Quando um jogador perde a partida os jogadores seguintes ao carteador percebem esse fato.
                // Contudo, eles tem que continuar na rede até que o último jogador saiba do fim de jogo.
                if (mensagem_fim && !sem_morte)
                    sem_perdedor = 0;

                if (enviado.destino == id_jogador) {
                    enviado.recebido = 1;

                    if (verifica_crc(&enviado)) {
                        enviado.sucesso = 1;

                        // O jogador recebe suas cartas para a rodada
                        if (verifica_mensagem(&enviado, 0x00)) {
                            n_cartas = enviado.tamanho;

                            escolhas = (unsigned char*)calloc(40, sizeof(unsigned char));
                            valor_cartas = (unsigned char*)malloc(n_cartas * sizeof(unsigned char));

                            for (unsigned char i = 0; i < n_cartas; i++)
                                valor_cartas[i] = enviado.dados[i];

                            cartas_representacao = (carta_t*)malloc(n_cartas*sizeof(carta_t));

                            imprime_cartas_round(n_cartas, cartas_representacao, valor_cartas);

                        }

                        // O jogador da seu palpite
                        else if (verifica_mensagem(&enviado, 0x01)) {
                            printf("APOSTA:");
                            scanf("%hhu", &palpite);

                            while (palpite < 0 || palpite > n_cartas) {
                            
                                printf("APOSTA INVÁLIDA ");
                                scanf("%hhu", &palpite);
                            }

                            enviado.dados[0] = palpite;
                            enviado.crc8 = calcula_crc(&enviado);
                        }

                        // O jogador recebe os palpites dos outros jogadores
                        else if (verifica_mensagem(&enviado, 0x02)) {
                            printf("\n");
                            for (unsigned char i = 0; i < 4; i++) {
                                palpites[i] = enviado.dados[i];
                                printf("APOSTA DO PLAYER%d: %d\n", (int)(i+1), (int)enviado.dados[i]);
                            }
                            printf("\n");
                        }

                        // O jogador escolhe uma carta para jogar
                        else if (verifica_mensagem(&enviado, 0x03)) {
                            printf("CARTA: ");
                            scanf("%hhu", &jogada);

                            while (jogada < 1 || jogada > n_cartas || escolhas[valor_cartas[jogada-1]]) {
                                printf("CARTA INVÁLIDA!");
                                scanf("%hhu", &jogada);
                            }

                            enviado.dados[0] = valor_cartas[jogada-1];
                            enviado.crc8 = calcula_crc(&enviado);
                        }

                        // O jogador recebe as jogadas dos outros jogadores
                        else if (verifica_mensagem(&enviado, 0x04)) {
                            jogadas[enviado.dados[0]] = enviado.dados[1];

                            // Imprime a escolha dos outros jogadores apenas
                            if (enviado.dados[0] != id_jogador) {
                                carta_adversario.numero = enviado.dados[1] % 10;
                                carta_adversario.tipo = enviado.dados[1] / 10;
                                printf("PLAYER%d: ", (int)(enviado.dados[0]+1));
                                imprime_carta(carta_adversario);
                                printf("\n");
                            }
                        }

                        // Divulgação de quem ganhou o turno
                        else if (verifica_mensagem(&enviado, 0x05)) {
                            printf("PLAYER%d GANHOU!\n", (int)(enviado.dados[0]+1));

                            escolhas[valor_cartas[jogada-1]] = 1;

                            printf("CARTAS: \n");
                            for (unsigned char i = 0; i < n_cartas; i++) {
                                cartas_representacao[i].numero = valor_cartas[i] % 10;
                                cartas_representacao[i].tipo = valor_cartas[i] / 10;
                                printf("%d) ", (int)(i+1));
                                imprime_carta(cartas_representacao[i]);

                                if (escolhas[valor_cartas[i]])
                                    printf("out");
                                printf("\n");
                                
                            }
                            printf("\n");
                        }

                        // Divulgação da vida de cada jogador após uma rodada
                        else if (verifica_mensagem(&enviado, 0x06)) {
                            for (unsigned char i = 0; i < 4; i++) {
                                if ((vidas[i] = enviado.dados[i]) == 0) {
                                    printf("PLAYER%d PERDEU!\n", (int)(i+1));

                                    if (mensagem_fim)
                                        sem_perdedor = 0;
                                    
                                    sem_morte = 0;
                                }
                                else {
                                    printf("PLAYER%d = %d VIDAS\n", (int)(i+1), (int)vidas[i]);
                                }
                            }
                            printf("\n"); 

                            // Libera pois é o fim da rodada
                            free(valor_cartas);
                            free(escolhas);
                            free(cartas_representacao);
                        }
                        // O jogador vira o carteador
                        else if (verifica_mensagem(&enviado, 0x07)) {
                            bastao = enviado.dados[0];
                            n_cartas = enviado.dados[1];
                        }
                    }
                }

                if (sendto(sockfd, &enviado, sizeof(enviado), 0, (struct sockaddr *)&prox_addr, tam_addr) < 0) {
                    perror("Erro no envio:");
                    return -1;
                }
            }

            if (porta == PLAYER4) // Volta pro primeiro jogador
                porta = PLAYER1;
        }
    }
    close(sockfd);
    return 0;
}    