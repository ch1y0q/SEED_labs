# RSA Public-Key Encryption and Signature

## Task 1: Deriving the Private Key

```
p = F7E75FDC469067FFDC4E847C51F452DF
q = E85CED54AF57E53E092113E62F436F4F
e = 0D88C3
```

```c
#include <stdio.h>
#include <openssl/bn.h>

#define NBITS 128
void printBN(char *msg, BIGNUM *a, BIGNUM *b)
{
  char *a_str = BN_bn2hex(a);
  char *b_str = BN_bn2hex(b);
  printf("%s (%s,%s)\n", msg, a_str, b_str);
  OPENSSL_free(a_str);
  OPENSSL_free(b_str);
}

int main()
{
  BN_CTX *ctx = BN_CTX_new();
  BIGNUM *p = BN_new();
  BIGNUM *q = BN_new();
  BIGNUM *e = BN_new();
  BIGNUM *n = BN_new();
  BIGNUM *res = BN_new();

  BIGNUM *phi = BN_new();
  BIGNUM *d = BN_new();
  BIGNUM *p_minus_1 = BN_new();
  BIGNUM *q_minus_1 = BN_new();

  BN_hex2bn(&p, "F7E75FDC469067FFDC4E847C51F452DF");
  BN_hex2bn(&q, "E85CED54AF57E53E092113E62F436F4F");
  BN_hex2bn(&e, "0D88C3");

  // n = pq
  BN_mul(n, p, q, ctx);
  printBN("public key", e, n);

  // phi(n) = (p-1)*(q-1)
  BN_sub(p_minus_1, p, BN_value_one());
  BN_sub(q_minus_1, q, BN_value_one());
  BN_mul(phi, p_minus_1, q_minus_1, ctx);

  // check if e and phi(n) coprime
  BN_gcd(res, phi, e, ctx);
  if (!BN_is_one(res))
  {
    printf("Error: e and phi(n) is not coprime \n ");
    return -1;
  }

  BN_mod_inverse(d, e, phi, ctx);
  printBN("private key", d, n);

  BN_clear_free(p);
  BN_clear_free(q);
  BN_clear_free(n);
  BN_clear_free(res);
  BN_clear_free(phi);
  BN_clear_free(e);
  BN_clear_free(d);
  BN_clear_free(p_minus_1);
  BN_clear_free(q_minus_1);

  return 0;
}
```



```

public key (0D88C3,E103ABD94892E3E74AFD724BF28E78366D9676BCCC70118BD0AA1968DBB143D1)
private key (3587A24598E5F2A21DB007D89D18CC50ABA5075BA19A33890FE7C28A9B496AEB,E103ABD94892E3E74AFD724BF28E78366D9676BCCC70118BD0AA1968DBB143D1)

```

## Task 2: Encrypting a Message

```shell
$ python -c 'print("A top secret!".encode("hex"))'
4120746f702073656372657421
```

```c
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

```



```

Encryption result: 6FB078DA550B2650832661E14F4F8D2CFAEF475A0DF3A75CACDC5DE5CFC5FADC
```



## Task 3: Decrypting a Message

```c
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
  BIGNUM *d = BN_new();
  BIGNUM *C = BN_new();
  BIGNUM *M = BN_new();

  BN_hex2bn(&n, "DCBFFE3E51F62E09CE7032E2677A78946A849DC4CDDE3A4D0CB81629242FB1A5");
  BN_hex2bn(&d, "74D806F9F3A62BAE331FFE3F0A68AFE35B3D2E4794148AACBC26AA381CD7D30D");
  BN_hex2bn(&C, "8C0F971DF2F3672B28811407E2DABBE1DA0FEBBBDFC7DCB67396567EA1E2493F");

  // M = C^d mod n
  BN_mod_exp(M, C, d, n, ctx);
  printBN("Decryption result:", M);

  // clear sensitive data
  BN_clear_free(n);
  BN_clear_free(d);
  BN_clear_free(M);
  BN_clear_free(C);

  return 0;
}

```

```
Decryption result: 50617373776F72642069732064656573
```

```shell
$ python
>>> s = "50617373776F72642069732064656573"
>>> print(bytes.fromhex(s).decode("utf-8"))
Password is dees

```

## Task 4: Signing a Message

```
"I owe you $2000.": 49206F776520796F752024323030302E
"I owe you $3000.": 49206F776520796F752024333030302E

```

```c
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
    BIGNUM *d = BN_new();
    BIGNUM *M1 = BN_new();
    BIGNUM *M2 = BN_new();
    BIGNUM *C1 = BN_new();
    BIGNUM *C2 = BN_new();

    BN_hex2bn(&n, "DCBFFE3E51F62E09CE7032E2677A78946A849DC4CDDE3A4D0CB81629242FB1A5");
    BN_hex2bn(&d, "74D806F9F3A62BAE331FFE3F0A68AFE35B3D2E4794148AACBC26AA381CD7D30D");
    BN_hex2bn(&M1, "49206F776520796F752024323030302E"); // "I owe you $2000."
    BN_hex2bn(&M2, "49206F776520796F752024333030302E"); // "I owe you $3000."

    // C = M^d mod n
    BN_mod_exp(C1, M1, d, n, ctx);
    BN_mod_exp(C2, M2, d, n, ctx);
    printBN("Signature of M1:", C1);
    printBN("Signature of M2:", C2);

    BN_clear_free(n);
    BN_clear_free(d);
    BN_clear_free(M1);
    BN_clear_free(M2);
    BN_clear_free(C1);
    BN_clear_free(C2);

    return 0;
}
```



```

Signature of M1: 55A4E7F17F04CCFE2766E1EB32ADDBA890BBE92A6FBE2D785ED6E73CCB35E4CB
Signature of M2: BCC20FB7568E5D48E434C387C06A6025E90D29D848AF9C3EBAC0135D99305822
```

消息极小的改动都会造成签名完全不同。

## Task 5: Verifying a Signature

```
"Launch a missile.": 4C61756E63682061206D697373696C652E
修改后：4C61756E63682061206D697373696C653F
```

```c
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
```



修改前运行程序，提示“Valid Signature! ”

修改后，提示“Invalid!”

## Task 6: Manually Verifying an X.509 Certificate

### Step 1: Download a certificate from a real web server.

```shell
$ openssl s_client -connect www.qq.com:443 -showcerts
CONNECTED(00000003)
depth=2 C = US, O = DigiCert Inc, OU = www.digicert.com, CN = DigiCert Global Root CA
verify return:1
depth=1 C = US, O = DigiCert Inc, CN = DigiCert Secure Site CN CA G3
verify return:1
depth=0 C = CN, ST = Guangdong Province, L = Shenzhen, O = Shenzhen Tencent Computer Systems Company Limited, CN = may21-2021-2.ias.qq.com
verify return:1
---
Certificate chain
 0 s:C = CN, ST = Guangdong Province, L = Shenzhen, O = Shenzhen Tencent Computer Systems Company Limited, CN = may21-2021-2.ias.qq.com
   i:C = US, O = DigiCert Inc, CN = DigiCert Secure Site CN CA G3
-----BEGIN CERTIFICATE-----
MIIJijCCCHKgAwIBAgIQCNdmJSlbmjsh+S/aWMSvlzANBgkqhkiG9w0BAQsFADBM
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMSYwJAYDVQQDEx1E
aWdpQ2VydCBTZWN1cmUgU2l0ZSBDTiBDQSBHMzAeFw0yMTA1MjAwMDAwMDBaFw0y
MjA1MjUyMzU5NTlaMIGbMQswCQYDVQQGEwJDTjEbMBkGA1UECBMSR3Vhbmdkb25n
IFByb3ZpbmNlMREwDwYDVQQHEwhTaGVuemhlbjE6MDgGA1UEChMxU2hlbnpoZW4g
VGVuY2VudCBDb21wdXRlciBTeXN0ZW1zIENvbXBhbnkgTGltaXRlZDEgMB4GA1UE
AxMXbWF5MjEtMjAyMS0yLmlhcy5xcS5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IB
DwAwggEKAoIBAQCUfURPhjVWAQA3qyCHPKTVMVrXLMhsuHAnYzprzKI/5RKISxCx
6k89+TMk8aFiIEvyeHGHZV/aQIeAHBcZ7YHny3eh5kyvV9k3jagVo0ksIN4m68pI
grw+rJIWYVRIPo7wIZsS9tVDylRWh8/N0Rq9ypfXJsP2dAQJE9Lw+BueI23cNJzM
STWER45aTM6s+2y2UghGKzxDQHR401FQx7/gAZQijCMjUcTP3zdZH4ufYjy17+37
IEFjntFGRJn7PD09nW6GZP9njTVPg6//YA0Ge7IlaiewykjLI66QvuSLAIIT8mTw
X2PaDv1Dm+vKzrbBgeXDa5wVQfJgPHY8GbGrAgMBAAGjggYWMIIGEjAfBgNVHSME
GDAWgBRE2chKM47TUo2nkpRhH5rIpbfsyzAdBgNVHQ4EFgQU5flfmqJE+dYB3vC1
ld6cJ8wmOKMwggMPBgNVHREEggMGMIIDAoIXbWF5MjEtMjAyMS0yLmlhcy5xcS5j
b22CCzIwMTAucXEuY29tggsyMDE4LnFxLmNvbYIOM2cudGVjaC5xcS5jb22CDmFy
c2VuYWwucXEuY29tghRhc3Ryby5mYXNoaW9uLnFxLmNvbYIRYXN0cm8ubGFkeS5x
cS5jb22CFWF1c3RyYWxpYW5vcGVuLnFxLmNvbYILYXV0by5xcS5jb22CC2JhYnku
cXEuY29tgg5iaXpuZXh0LnFxLmNvbYINYmouampqLnFxLmNvbYINYmoyMDIyLnFx
LmNvbYIPYnVzaW5lc3MucXEuY29tggljeS5xcS5jb22CC2RpZ2kucXEuY29tggpl
bnQucXEuY29tggtmYWN0LnFxLmNvbYISZm8ucnVzaGlkYW8ucXEuY29tggxmb3h1
ZS5xcS5jb22CDGdhbWVzLnFxLmNvbYIJaGIucXEuY29tggpoZWEucXEuY29tggxo
ZW5hbi5xcS5jb22CDmhpc3RvcnkucXEuY29tggxpbGlrZS5xcS5jb22CDGppYWp1
LnFxLmNvbYIKampqLnFxLmNvbYIKa2lkLnFxLmNvbYIJbHkucXEuY29tghFtLm5i
YWNoaW5hLnFxLmNvbYIMbWVkaWEucXEuY29tghNtaW5pc2l0ZTIwMTIucXEuY29t
ggxtb25leS5xcS5jb22CEG5iYS5zdGF0cy5xcS5jb22CCm5ldy5xcS5jb22CC25l
d3MucXEuY29tggtyYWluLnFxLmNvbYIJcnUucXEuY29tgg5ydWZvZGFvLnFxLmNv
bYIJc2gucXEuY29tggxzcGFjZS5xcS5jb22CF3Rlc3RhcGkuc3VzcGVuc2UucXEu
Y29tggp0aHIucXEuY29tggx2LmVudC5xcS5jb22CFXZpZGVvY2dpLnF6b25lLnFx
LmNvbYIOd2VhdGhlci5xcS5jb22CD3dvcmxkY3VwLnFxLmNvbYIKd3d3LnFxLmNv
bYILeGlhbi5xcS5jb22CC3lzbHAucXEuY29tMA4GA1UdDwEB/wQEAwIFoDAdBgNV
HSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwRAYDVR0fBD0wOzA5oDegNYYzaHR0
cDovL2NybC5kaWdpY2VydC5jbi9EaWdpQ2VydFNlY3VyZVNpdGVDTkNBRzMuY3Js
MD4GA1UdIAQ3MDUwMwYGZ4EMAQICMCkwJwYIKwYBBQUHAgEWG2h0dHA6Ly93d3cu
ZGlnaWNlcnQuY29tL0NQUzB4BggrBgEFBQcBAQRsMGowIwYIKwYBBQUHMAGGF2h0
dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNuMEMGCCsGAQUFBzAChjdodHRwOi8vY2FjZXJ0
cy5kaWdpY2VydC5jbi9EaWdpQ2VydFNlY3VyZVNpdGVDTkNBRzMuY3J0MAwGA1Ud
EwEB/wQCMAAwggF+BgorBgEEAdZ5AgQCBIIBbgSCAWoBaAB2AEalVet1+pEgMLWi
iWn0830RLEF0vv1JuIWr8vxw/m1HAAABeYv0mfwAAAQDAEcwRQIgUbt9lT0KmEmn
MstxKhQ64Jxg9S80l9Vdvj0vTmQeprsCIQC2a13pD4/aXhCr27P9mOGor+AbbaZ7
bCjfuE8pCZgeCgB2ACJFRQdZVSRWlj+hL/H3bYbgIyZjrcBLf13Gg1xu4g8CAAAB
eYv0mcsAAAQDAEcwRQIhAKrjw+3Lzv8yAE7R5ZYqD04meSaFspqK1E3B/gOQ8xmJ
AiBTgu5+ueK6C7PR4GEwm/aT49pO05LhErzr8TcIJw9vQgB2AFGjsPX9AXmcVm24
N3iPDKR6zBsny/eeiEKaDf7UiwXlAAABeYv0mfIAAAQDAEcwRQIhAMJ+AWPA/gmE
k8sy4XoBEbeobHk2BYZngkuXgq+ObiUwAiAOh6wm7egN5vkxMsWuDtBiMbP53qoC
nEVSYSz1CZdR9TANBgkqhkiG9w0BAQsFAAOCAQEAAG2kOYJciQC+YG0kOazl3Iht
G1HLF7oXm7KlsS5hxcgRAIuJE80MvycDyzwfZWg7swmDBF8O8UwmkButYW0asFpD
L3oFqhgucnJSQCGSdfXWJBQ627Q/lp9sZetxOScIyXPwZkMOsQZgPMgcfSxpF6iX
k7FMWg/0XSyYtITpY9HTyTgbY4hoqGnh1S52vwWuQoCLvrgC00BrXjW9g9aAw4Xf
ZqkAr3uAFPu1u+Gx1DSy3F+pTIxZ0s5iZNIOfcWlIackN46qi5NGo4tFXr4NFiSO
UqPO/f025aryN3rKELKmqxrzP+vhdnpjDYt4xGMBrCAbv6+nnA6zAQM2rGpGaA==
-----END CERTIFICATE-----
 1 s:C = US, O = DigiCert Inc, CN = DigiCert Secure Site CN CA G3
   i:C = US, O = DigiCert Inc, OU = www.digicert.com, CN = DigiCert Global Root CA
-----BEGIN CERTIFICATE-----
MIIFCDCCA/CgAwIBAgIQBsk1GubwrG6wBvsMKqcyQTANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0yMDAzMTMxMjAwMDBaFw0zMDAzMTMxMjAwMDBaMEwxCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxJjAkBgNVBAMTHURpZ2lDZXJ0IFNlY3Vy
ZSBTaXRlIENOIENBIEczMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA
8FvLH8zXOPwMrB5ZEdaUtfupXVj3ADIIt9uabxZnmnOJVeER4gBDBmuZ/5zvatiK
LqzE05lw03rvSXjiWjAwGSZQWbxz4qUIxGxOpvypsW5tbvcnKkPG9vs2tj+u+KSK
CCMA792c4rroXOBHjlQHl+ET+xnWc3nxobw7yL1vThEcBkCsLiu4BE5eETMzEplu
Z5hVT31EISTkU+L2qoVPqvl2vCLKmb4iKJYHpGIm1qVGRgf54kxfhRl9rEu4k2rQ
eUaJh4r5dKz1y0TFBwLIAM4nwGVc61H5S874Mt1Zw5i2kxnRymMNg5FFuCkQFIrj
UlFvlDohMoBNRvbtzHQAHQIDAQABo4IBzzCCAcswHQYDVR0OBBYEFETZyEozjtNS
jaeSlGEfmsilt+zLMB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA4G
A1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwEgYD
VR0TAQH/BAgwBgEB/wIBADAzBggrBgEFBQcBAQQnMCUwIwYIKwYBBQUHMAGGF2h0
dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNuMEAGA1UdHwQ5MDcwNaAzoDGGL2h0dHA6Ly9j
cmwuZGlnaWNlcnQuY24vRGlnaUNlcnRHbG9iYWxSb290Q0EuY3JsMIHOBgNVHSAE
gcYwgcMwgcAGBFUdIAAwgbcwKAYIKwYBBQUHAgEWHGh0dHBzOi8vd3d3LmRpZ2lj
ZXJ0LmNvbS9DUFMwgYoGCCsGAQUFBwICMH4MfEFueSB1c2Ugb2YgdGhpcyBDZXJ0
aWZpY2F0ZSBjb25zdGl0dXRlcyBhY2NlcHRhbmNlIG9mIHRoZSBSZWx5aW5nIFBh
cnR5IEFncmVlbWVudCBsb2NhdGVkIGF0IGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNv
bS9ycGEtdWEwDQYJKoZIhvcNAQELBQADggEBAIEbzergTPhY7dFwqeIB+fRlzYgI
fo40x1uqb+sH3imvCC5V57mSVVPXwEe2hDHCpo2+zGGqot60EkVtLF0+bUpCXffc
3mzVx6ti7GX+znUK5Vh41aoNy1AUara/iWQbcUXo32BrRPmXE+kMzt3VWxQE6ybP
r/l4SAcusEOmuwCTzzkTRttvWCa9VLgY5SsZ0jIj+/eRvQJRxv8x24/HrTI95OMo
+cTFs6iaigj+k85GmxtbpYEJJ4M8E6AtrBQ/i6/jhVY5RssEcxn0sUEvl8Lpxnlr
vyedD9l6digYDmB5jATBq8i8aaLiD7gyah3GHeziwuKmebv3CeTy44o+ORM=
-----END CERTIFICATE-----
---
Server certificate
subject=C = CN, ST = Guangdong Province, L = Shenzhen, O = Shenzhen Tencent Computer Systems Company Limited, CN = may21-2021-2.ias.qq.com

issuer=C = US, O = DigiCert Inc, CN = DigiCert Secure Site CN CA G3

---
No client certificate CA names sent
Peer signing digest: SHA256
Peer signature type: RSA-PSS
Server Temp Key: X25519, 253 bits
---
SSL handshake has read 4402 bytes and written 395 bytes
Verification: OK
---
[...]
```

### Step 2: Extract the public key (e, n) from the issuer’s certificate.

```shell
$ openssl x509 -in c1.pem -noout -modulus
Modulus=F05BCB1FCCD738FC0CAC1E5911D694B5FBA95D58F7003208B7DB9A6F16679A738955E111E20043066B99FF9CEF6AD88A2EACC4D39970D37AEF4978E25A303019265059BC73E2A508C46C4EA6FCA9B16E6D6EF7272A43C6F6FB36B63FAEF8A48A082300EFDD9CE2BAE85CE0478E540797E113FB19D67379F1A1BC3BC8BD6F4E111C0640AC2E2BB8044E5E11333312996E6798554F7D442124E453E2F6AA854FAAF976BC22CA99BE22289607A46226D6A5464607F9E24C5F85197DAC4BB8936AD0794689878AF974ACF5CB44C50702C800CE27C0655CEB51F94BCEF832DD59C398B69319D1CA630D839145B82910148AE352516F943A2132804D46F6EDCC74001D

$ openssl x509 -in c1.pem -text -noout
[...]
Exponent: 65537 (0x10001)
[...]
```

### Step 3: Extract the signature from the server’s certificate

```shell
$ openssl x509 -in c0.pem -text -noout
[...]
    Signature Algorithm: sha256WithRSAEncryption
         00:6d:a4:39:82:5c:89:00:be:60:6d:24:39:ac:e5:dc:88:6d:
         1b:51:cb:17:ba:17:9b:b2:a5:b1:2e:61:c5:c8:11:00:8b:89:
         13:cd:0c:bf:27:03:cb:3c:1f:65:68:3b:b3:09:83:04:5f:0e:
         f1:4c:26:90:1b:ad:61:6d:1a:b0:5a:43:2f:7a:05:aa:18:2e:
         72:72:52:40:21:92:75:f5:d6:24:14:3a:db:b4:3f:96:9f:6c:
         65:eb:71:39:27:08:c9:73:f0:66:43:0e:b1:06:60:3c:c8:1c:
         7d:2c:69:17:a8:97:93:b1:4c:5a:0f:f4:5d:2c:98:b4:84:e9:
         63:d1:d3:c9:38:1b:63:88:68:a8:69:e1:d5:2e:76:bf:05:ae:
         42:80:8b:be:b8:02:d3:40:6b:5e:35:bd:83:d6:80:c3:85:df:
         66:a9:00:af:7b:80:14:fb:b5:bb:e1:b1:d4:34:b2:dc:5f:a9:
         4c:8c:59:d2:ce:62:64:d2:0e:7d:c5:a5:21:a7:24:37:8e:aa:
         8b:93:46:a3:8b:45:5e:be:0d:16:24:8e:52:a3:ce:fd:fd:36:
         e5:aa:f2:37:7a:ca:10:b2:a6:ab:1a:f3:3f:eb:e1:76:7a:63:
         0d:8b:78:c4:63:01:ac:20:1b:bf:af:a7:9c:0e:b3:01:03:36:
         ac:6a:46:68

```

```
006da439825c8900be606d2439ace5dc886d1b51cb17ba179bb2a5b12e61c5c811008b8913cd0cbf2703cb3c1f65683bb30983045f0ef14c26901bad616d1ab05a432f7a05aa182e72725240219275f5d624143adbb43f969f6c65eb71392708c973f066430eb106603cc81c7d2c6917a89793b14c5a0ff45d2c98b484e963d1d3c9381b638868a869e1d52e76bf05ae42808bbeb802d3406b5e35bd83d680c385df66a900af7b8014fbb5bbe1b1d434b2dc5fa94c8c59d2ce6264d20e7dc5a521a724378eaa8b9346a38b455ebe0d16248e52a3cefdfd36e5aaf2377aca10b2a6ab1af33febe1767a630d8b78c46301ac201bbfafa79c0eb3010336ac6a4668
```



### Step 4: Extract the body of the server’s certificate.

```shell
$ openssl asn1parse -i -in c0.pem -strparse 4 -out c0_body.bin -noout
$ sha256sum c0_body.bin
f2be0b2a7cb25f785b9c6291c6aca1f5c1f4237b0183b7a26979bb78a7af2da6  c0_body.bin
$ python
>>> hash = "f2be0b2a7cb25f785b9c6291c6aca1f5c1f4237b0183b7a26979bb78a7af2da6"
>>> A = "3031300D060960864801650304020105000420"
>>> pad_len = 256 - 1 - (len(A)+ 4 +len(hash))//2
>>> "0001" + "FF" * pad_len + "00" + A + hash
'0001FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF003031300D060960864801650304020105000420f2be0b2a7cb25f785b9c6291c6aca1f5c1f4237b0183b7a26979bb78a7af2da6'
```

```c
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

```



```shell

./a.out 
Valid Signature! 

```

