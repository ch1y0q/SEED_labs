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

  BN_hex2bn(&n, "F05BCB1FCCD738FC0CAC1E5911D694B5FBA95D58F7003208B7DB9A6F16679A738955E111E20043066B99FF9CEF6AD88A2EACC4D39970D37AEF4978E25A303019265059BC73E2A508C46C4EA6FCA9B16E6D6EF7272A43C6F6FB36B63FAEF8A48A082300EFDD9CE2BAE85CE0478E540797E113FB19D67379F1A1BC3BC8BD6F4E111C0640AC2E2BB8044E5E11333312996E6798554F7D442124E453E2F6AA854FAAF976BC22CA99BE22289607A46226D6A5464607F9E24C5F85197DAC4BB8936AD0794689878AF974ACF5CB44C50702C800CE27C0655CEB51F94BCEF832DD59C398B69319D1CA630D839145B82910148AE352516F943A2132804D46F6EDCC74001D");
  BN_dec2bn(&e, "65537");
  BN_hex2bn(&M, "0001FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF003031300D060960864801650304020105000420f2be0b2a7cb25f785b9c6291c6aca1f5c1f4237b0183b7a26979bb78a7af2da6");
  BN_hex2bn(&S, "006da439825c8900be606d2439ace5dc886d1b51cb17ba179bb2a5b12e61c5c811008b8913cd0cbf2703cb3c1f65683bb30983045f0ef14c26901bad616d1ab05a432f7a05aa182e72725240219275f5d624143adbb43f969f6c65eb71392708c973f066430eb106603cc81c7d2c6917a89793b14c5a0ff45d2c98b484e963d1d3c9381b638868a869e1d52e76bf05ae42808bbeb802d3406b5e35bd83d680c385df66a900af7b8014fbb5bbe1b1d434b2dc5fa94c8c59d2ce6264d20e7dc5a521a724378eaa8b9346a38b455ebe0d16248e52a3cefdfd36e5aaf2377aca10b2a6ab1af33febe1767a630d8b78c46301ac201bbfafa79c0eb3010336ac6a4668");

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
