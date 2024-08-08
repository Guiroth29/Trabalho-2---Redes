#ifndef CRC_H
#define CRC_H

#include "mensagem.h"

unsigned char laco_crc (unsigned char crc, unsigned char n, unsigned char dados [n]);
unsigned char calcula_crc (mensagem_t *enviado);
int verifica_crc (mensagem_t *enviado);

#endif