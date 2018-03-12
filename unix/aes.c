#include "aes.h"

#define Nb 4
#define Nk 4
#define Nr 10

typedef uint8_t state_t[4][4];

static const uint8_t sbox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const uint8_t Rcon[11] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

static void KeyExpansion(uint8_t* RoundKey, const uint8_t* Key)
{
  uint8_t tempa[4];
  for (unsigned i = 0; i < Nk; ++i) {
    RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
    RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
    RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
    RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
  }

  for (unsigned i = Nk; i < Nb * (Nr + 1); ++i) {
    unsigned k = (i - 1) * 4;
    tempa[0]=RoundKey[k + 0];
    tempa[1]=RoundKey[k + 1];
    tempa[2]=RoundKey[k + 2];
    tempa[3]=RoundKey[k + 3];

    if (i % Nk == 0) {
      k = tempa[0];
      tempa[0] = tempa[1];
      tempa[1] = tempa[2];
      tempa[2] = tempa[3];
      tempa[3] = k;
      tempa[0] = sbox[tempa[0]];
      tempa[1] = sbox[tempa[1]];
      tempa[2] = sbox[tempa[2]];
      tempa[3] = sbox[tempa[3]];
      tempa[0] = tempa[0] ^ Rcon[i/Nk];
    }

    unsigned j = i * 4; k=(i - Nk) * 4;
    RoundKey[j + 0] = RoundKey[k + 0] ^ tempa[0];
    RoundKey[j + 1] = RoundKey[k + 1] ^ tempa[1];
    RoundKey[j + 2] = RoundKey[k + 2] ^ tempa[2];
    RoundKey[j + 3] = RoundKey[k + 3] ^ tempa[3];
  }
}

static void AddRoundKey(uint8_t round,state_t* state,uint8_t* RoundKey)
{
  for (uint8_t i = 0; i < 4; ++i) {
    for (uint8_t j = 0; j < 4; ++j) {
      (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
    }
  }
}

static void SubBytes(state_t* state)
{
  for (uint8_t i = 0; i < 4; ++i) {
    for (uint8_t j = 0; j < 4; ++j) {
      (*state)[j][i] = sbox[(*state)[j][i]];
    }
  }
}

static void ShiftRows(state_t* state)
{
  uint8_t temp;
  temp           = (*state)[0][1];
  (*state)[0][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[3][1];
  (*state)[3][1] = temp;
  temp           = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;
  temp           = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;
  temp           = (*state)[0][3];
  (*state)[0][3] = (*state)[3][3];
  (*state)[3][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[1][3];
  (*state)[1][3] = temp;
}

static uint8_t xtime(uint8_t x)
{
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

static void MixColumns(state_t* state)
{
  uint8_t Tmp, Tm, t;
  for (uint8_t i = 0; i < 4; ++i) {
    t   = (*state)[i][0];
    Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3] ;
    Tm  = (*state)[i][0] ^ (*state)[i][1] ; Tm = xtime(Tm);  (*state)[i][0] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][1] ^ (*state)[i][2] ; Tm = xtime(Tm);  (*state)[i][1] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][2] ^ (*state)[i][3] ; Tm = xtime(Tm);  (*state)[i][2] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][3] ^ t ;              Tm = xtime(Tm);  (*state)[i][3] ^= Tm ^ Tmp ;
  }
}

static void Cipher(state_t* state, uint8_t* RoundKey)
{
  AddRoundKey(0, state, RoundKey);
  for (uint8_t round = 1; round < Nr; ++round) {
    SubBytes(state);
    ShiftRows(state);
    MixColumns(state);
    AddRoundKey(round, state, RoundKey);
  }
  SubBytes(state);
  ShiftRows(state);
  AddRoundKey(Nr, state, RoundKey);
}

static void XorWithIv(uint8_t* buf, uint8_t* Iv)
{
  for (uint8_t i = 0; i < AES_BLOCKLEN; ++i) {
    buf[i] ^= Iv[i];
  }
}

static void memcpy(uint8_t *dst, const uint8_t *src, uint32_t length)
{
  for (uint32_t i = 0; i < length; ++i) {
    dst[i] = src[i];
  }
}

void AES_init_ctx_iv(AESContext* ctx, const uint8_t* key, const uint8_t* iv)
{
  KeyExpansion(ctx->RoundKey, key);
  memcpy(ctx->Iv, iv, AES_BLOCKLEN);
}

void AES_CBC_encrypt_buffer(AESContext *ctx,uint8_t* buf, uint32_t length)
/* Encrypt a buffer using the CBC (Cipher Block Chaining) mode of encryption.
   Input: AES Context (IV and Key), buffer of plaintext, and length of plaintext.
   Output: None, but the buffer which was originally plaintext gets filled with ciphertext.
*/
{
  uint8_t *Iv = ctx->Iv;
  for (uint32_t i = 0; i < length; i += AES_BLOCKLEN) {
    XorWithIv(buf, Iv);
    Cipher((state_t*)buf, ctx->RoundKey);
    Iv = buf;
    buf += AES_BLOCKLEN;
  }
  memcpy(ctx->Iv, Iv, AES_BLOCKLEN);
}

int AES_parse_key(const char *src, uint8_t *key)
{
  for (unsigned i = 0; i < 32; ++i) {
    const char chr = src[i];
    int digit = 0;
    if ('0' <= chr && chr <= '9') {
      digit = chr - '0';
    } else if ('A' <= chr && chr <= 'F') {
      digit = chr - 'A' + 10;
    } else if ('a' <= chr && chr <= 'f') {
      digit = chr - 'a' + 10;
    } else {
      return 0;
    }

    if ((i & 1) == 0) {
      key[i >> 1] = (key[i >> 1] & 0x0F) | (digit << 4);
    } else {
      key[i >> 1] = (key[i >> 1] & 0xF0) | (digit << 0);
    }
  }

  return 1;
}

