# Pseudo Random Number Generation

## Task 1: Generate Encryption Key in a Wrong Way

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define KEYSIZE 16
void main()
{
	int i;
	char key[KEYSIZE];
	printf("%lld\n", (long long) time(NULL));
	srand (time(NULL)); // comment this
	for (i = 0; i < KEYSIZE; i++) {
		key[i] = rand() % 256;
		printf("%.2x", (unsigned char)key[i]);
	}
	printf("\n");
}
```

```shell
$ nano 128b.c
$ gcc -o 128b 128b.c
$ ./128b 
1644818726
98f21f424dbb56005f79f73b3911fd74
$ ./128b 
1644818728
a93118454f4bd60c4092f3b8979a4c12
$ ./128b 
1644818729
fd305134f1063d3cfa4a2e9871e62eeb
$ ./128b 
1644818730
9ad59c38f158261ea3c58da4d3656869
$ ./128b 
1644818730
9ad59c38f158261ea3c58da4d3656869
$ ./128b 
1644818732
27228de326ffcad10621442eef2efed7
$ ./128b 
1644818732
27228de326ffcad10621442eef2efed7
$ ./128b 
1644818735
38cccdba3ea5098e2acd4b21c9a8102f
$ ./128b 
1644818736
18fd55d4f86be2d12d39c04dd851daa2
$ ./128b 
1644818736
18fd55d4f86be2d12d39c04dd851daa2
$ ./128b 
1644818736
18fd55d4f86be2d12d39c04dd851daa2
$ ./128b 
1644818737
d86c46813851c7e6df229dfe9e7957cb
$ ./128b 
1644818738
ce3119fd6c388d29b65af7e0ff678dea

```

`time(NULL)`用于获得UNIX时间戳，`srand()`用于设置随机数种子。如果程序两次调用间隔很短（小于1秒），种子一样，生成的随机数一样。

如果注释掉`srand (time(NULL));`，则生成的随机数永远相同。

## Task 2: Guessing the Key

```shell
pip3 uninstall pycrypto
pip3 install pycryptodome
```



```python
from ctypes import CDLL
from Crypto.Cipher import AES
import base64

libc = CDLL("libc.so.6")

data = bytearray.fromhex('255044462d312e350a25d0d4c5d80a34')
ciphertext = bytearray.fromhex('d06bf9d0dab8e8ef880660d2af65aa82')
iv = bytearray.fromhex('09080706050403020100A2B2C2D2E2F2')

keys = set()
for i in range(1524020929 - 7200, 1524020929):
    key = ''
    libc.srand(i)
    for j in range(0, 16):
        key += "{:02X}".format(libc.rand() % 256)
    print(key)
    keys.add(key)


for k in keys:
    # k = k.rstrip('\n')
    key = bytes(bytearray.fromhex(k))
    cipher = AES.new(key=key, mode=AES.MODE_CBC, iv=iv)
    guess = cipher.encrypt(data)
    if guess == ciphertext:
        print("the key is : ", k)
        break

```



```
the key is : 95FA2030E73ED3F8DA761B4EB805DFD7
```

## Task 3: Measure the Entropy of Kernel

测量内核的熵：

```shell
$ cat /proc/sys/kernel/random/entropy_avail
$ watch -n .1 cat /proc/sys/kernel/random/entropy_avail
```

观察到鼠标移动、点击，键盘使用等等都会增加熵。

## Task 4: Get Pseudo Random Numbers from /dev/random

移动鼠标或使用键盘，会不断产生新的输出，每当产生新的输出，熵的数值会归零；当停止操作，就不会再有新的输出，熵的数值增加很慢。

Question: If a server uses /dev/random to generate the random session key with a client. Please describe how you can launch a Denial-Of-Service (DOS) attack on such a server.

Answer: 如果发动DDoS攻击耗尽服务器的可用熵，服务器将不能生成新的随机数。

## Task 5: Get Random Numbers from /dev/urandom

```shell
$ cat /dev/urandom | hexdump
$ head -c 1M /dev/urandom > urandom-output.bin
$ ent output.bin
```

`/dev/urandom`会源源不断产生新的随机数，且不存在被DDoS攻击而耗尽随机数的问题。

```shell
[02/14/22]seed@VM:~/.../pseudo-random$ ent urandom-output.bin 
Entropy = 7.999806 bits per byte.

Optimum compression would reduce the size
of this 1048576 byte file by 0 percent.

Chi square distribution for 1048576 samples is 282.59, and randomly
would exceed this value 11.32 percent of the times.

Arithmetic mean value of data bytes is 127.4963 (127.5 = random).
Monte Carlo value for Pi is 3.141026081 (error 0.02 percent).
Serial correlation coefficient is 0.001080 (totally uncorrelated = 0.0).

```

可以认为`/dev/urandom`的随机数质量较好。

可以将其作为一个块设备，在程序中使用。

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define KEYSIZE 16
void main()
{
        int i;
        unsigned char key[KEYSIZE];
        FILE* random = fopen("/dev/urandom", "r");
        fread(key, sizeof(unsigned char) * 16, 1, random);
        fclose(random);
        for (i=0; i<KEYSIZE; i++){
                printf("%.2x", key[i]);
        }
        printf("\n");
}

```

