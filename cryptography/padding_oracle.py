#!/usr/bin/python3
# based on https://seedsecuritylabs.org/Labs_20.04/Crypto/Crypto_Padding_Oracle/, Task 3
import socket
from binascii import hexlify, unhexlify

# XOR two bytearrays


def xor(first, second):
    return bytearray(x ^ y for x, y in zip(first, second))


class PaddingOracle:

    def __init__(self, host, port) -> None:
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((host, port))

        ciphertext = self.s.recv(4096).decode().strip()
        self.ctext = unhexlify(ciphertext)

    def decrypt(self, ctext: bytes) -> None:
        self._send(hexlify(ctext))
        return self._recv()

    def _recv(self):
        resp = self.s.recv(4096).decode().strip()
        return resp

    def _send(self, hexstr: bytes):
        self.s.send(hexstr + b'\n')

    def __del__(self):
        self.s.close()


if __name__ == "__main__":
    oracle = PaddingOracle('10.9.0.80', 6000)

    # Get the IV + Ciphertext from the oracle
    iv_and_ctext = bytearray(oracle.ctext)
    l = len(iv_and_ctext)
    n_blocks = l // 16 - 1
    print(f"n_blocks: {n_blocks}")
    C = [bytearray(16) for _ in range(n_blocks + 1)]
    IV = iv_and_ctext[00:16]
    C[0] = IV
    for _ in range(1, n_blocks + 1):
        C[_] = iv_and_ctext[16*_:16*(_+1)]
        print(f"C{_}: " + C[_].hex())

    plaintext = bytearray()
    for cur_block in range(n_blocks, 0,  -1):
        C1 = C[cur_block-1]  # aliasing

        D2 = bytearray(16)

        D2[0] = C1[0]
        D2[1] = C1[1]
        D2[2] = C1[2]
        D2[3] = C1[3]
        D2[4] = C1[4]
        D2[5] = C1[5]
        D2[6] = C1[6]
        D2[7] = C1[7]
        D2[8] = C1[8]
        D2[9] = C1[9]
        D2[10] = C1[10]
        D2[11] = C1[11]
        D2[12] = C1[12]
        D2[13] = C1[13]
        D2[14] = C1[14]
        D2[15] = C1[15]

        CC1 = bytearray(16)

        CC1[0] = 0x00
        CC1[1] = 0x00
        CC1[2] = 0x00
        CC1[3] = 0x00
        CC1[4] = 0x00
        CC1[5] = 0x00
        CC1[6] = 0x00
        CC1[7] = 0x00
        CC1[8] = 0x00
        CC1[9] = 0x00
        CC1[10] = 0x00
        CC1[11] = 0x00
        CC1[12] = 0x00
        CC1[13] = 0x00
        CC1[14] = 0x00
        CC1[15] = 0x00

        for K in range(1, 16 + 1):
            for i in range(256):
                CC1[16 - K] = i
                # print(CC1.hex())
                barr = bytearray()
                for _ in range(cur_block - 1):
                    barr += C[_]
                barr += CC1
                barr += C[cur_block]
                # print(barr.hex())
                status = oracle.decrypt(barr)
                if status == "Valid":
                    D2[16-K] = i ^ K
                    for j in range(1, K + 1):
                        CC1[16-j] = D2[16-j] ^ (K + 1)
                    print("Valid: i = 0x{:02x}".format(i))
                    print("CC1: " + CC1.hex())
                    break   # break search for i

        # Once you get all the 16 bytes of D2, you can easily get P2
        P = xor(C1, D2)
        print(f"P{cur_block}:  " + P.hex())
        plaintext = P + plaintext
    print(f"Plaintext: " + plaintext.hex())
