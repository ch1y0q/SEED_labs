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
