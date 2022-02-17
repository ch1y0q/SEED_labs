import binascii
from base64 import b64encode, b64decode
from binascii import unhexlify

from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad

cipher_text = "764aa26b55a4da654df6b19e4bce00f4ed05e09346fb0e762583cb7da2ac93a2"
iv = "aabbccddeeff00998877665544332211"
msg = "This is a top secret."
assert(len(msg) == 21)
iv = unhexlify(iv)
# Pad to AES Block Size
msg = pad(msg.encode(), AES.block_size)

with open('words.txt', 'r') as f:
    for word in f:
        word = word.strip("\n")
        if len(word) > 16:
            continue
        word += ('#' * (16 - len(word)))
        password = word.encode('latin-1')
        print(f"word: {word}, len: {len(word)}")
        print(f"key: {password}, len: {len(password)}")
        print(f"IV: {iv.hex()}")
        # print(f"MSG: {msg}")

        # Encipher Text
        cipher = AES.new(key=password, mode=AES.MODE_CBC, iv=iv)
        my_cipher_text = cipher.encrypt(msg)
        print(f"OUT: {my_cipher_text.hex()}")

        if my_cipher_text.hex() == cipher_text:
            print(f"Found! key: {word.strip('#')}")
            break
