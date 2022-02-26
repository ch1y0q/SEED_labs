#include <stdio.h>
#include <openssl/bn.h>

void printBN(char *msg, BIGNUM *a)
{
  char *number_str_a = BN_bn2hex(a);
  printf("%s %s\n", msg, number_str_a);
  OPENSSL_free(number_str_a);
}

int main()
{
  // init
  BN_CTX *ctx = BN_CTX_new();
  BIGNUM *n = BN_new();
  BIGNUM *e = BN_new();
  BIGNUM *M = BN_new();
  BIGNUM *C = BN_new();
  BIGNUM *S = BN_new();

  BN_hex2bn(&n, "AE1CD4DC432798D933779FBD46C6E1247F0CF1233595113AA51B450F18116115");
  BN_dec2bn(&e, "65537");
  BN_hex2bn(&M, "4c61756e63682061206d697373696c652e"); // "Launch a missile."
  BN_hex2bn(&S, "643D6F34902D9C7EC90CB0B2BCA36C47FA37165C0005CAB026C0542CBDB6802F");
  // BN_hex2bn(&S, "643D6F34902D9C7EC90CB0B2BCA36C47FA37165C0005CAB026C0542CBDB6803F");

  // C = S^e mod n
  BN_mod_exp(C, S, e, n, ctx);

  if (BN_cmp(C, M) == 0)
  {
    printf("Valid Signature! \n");
  }
  else
  {
    printf("Invalid! \n");
  }

  BN_clear_free(n);
  BN_clear_free(e);
  BN_clear_free(M);
  BN_clear_free(C);
  BN_clear_free(S);

  return 0;
}
