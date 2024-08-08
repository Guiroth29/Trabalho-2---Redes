# include "crc.h"

unsigned char laco_crc (unsigned char crc, unsigned char n, unsigned char dados [n]) {
    for  (unsigned int i = 0; i < n; i++) {
        crc ^= dados [i];

        for  (unsigned int j = 0; j < 8; j++) {
            if  (crc & 0x80) {
                crc =  (crc << 1) ^ 0x07;
            }
            else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

unsigned char calcula_crc (mensagem_t *enviado) {
    unsigned char crc = 0x00; // inicialmente é 0;

    // Combina os campos destino, origem, tamanho e tipo em um array temporário
    unsigned char temp [4];

    // Garante que apenas os bits menos significativos sejam levados em consideração
    temp [0] =  (enviado->destino & 0x03);
    temp [1] =  (enviado->origem & 0x03);
    temp [2] =  (enviado->tamanho & 0x0F);
    temp [3] =  (enviado->tipo & 0x07);

    crc = laco_crc (crc, 4, temp);
    crc = laco_crc (crc, enviado->tamanho, enviado->dados);

    return crc;
}

int verifica_crc (mensagem_t *enviado) {
    if  (enviado->crc8 == calcula_crc (enviado))
        return 1;
    
    return 0;
}