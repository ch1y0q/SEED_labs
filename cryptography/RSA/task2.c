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
  BN_CTX *ctx = BN_CTX_new();
  BIGNUM *n = BN_new();
  BIGNUM *e = BN_new();
  BIGNUM *M = BN_new();
  BIGNUM *C = BN_new();

  BN_hex2bn(&n, "DCBFFE3E51F62E09CE7032E2677A78946A849DC4CDDE3A4D0CB81629242FB1A5");
  BN_hex2bn(&e, "010001");
  BN_hex2bn(&M, "4120746f702073656372657421"); // "A top secret!"

  // C = M^e mod n
  BN_mod_exp(C, M, e, n, ctx);
  printBN("Encryption result:", C);

  // clear sensitive data
  BN_clear_free(n);
  BN_clear_free(e);
  BN_clear_free(M);
  BN_clear_free(C);

  return 0;
}
