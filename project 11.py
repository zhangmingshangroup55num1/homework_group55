import hashlib
import hmac
import random
import time
from Crypto.Util.number import *
from cryptography.hazmat.primitives.asymmetric.utils import encode_dss_signature

# 定义椭圆曲线参数
p = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f  # 素数p
a = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2c  # a系数
b = 0x0000000000000000000000000000000000000000000000000000000000000030  # b系数
n = 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141  # 群G的阶
G_x = int('32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE171BEC59EB1CF27E3', 16)
G_y = int('E0A02A7D03B8B2386506D7E313BA8CB7DAB35819764048EA3FDBCC18FFD97A22', 16)

start=time.time()
# 随机数生成函数
def deterministic_generate_k(msg_digest, private_key):
    v = b'\x01' * 32
    k = b'\x00' * 32
    K = b'\x00' * 32
    private_key = b'\x00' * (32 - len(private_key)) + private_key

    k = hmac.new(k, v + b'\x00' + private_key + msg_digest, hashlib.sha256).digest()
    K = hmac.new(k, v, hashlib.sha256).digest()
    k = hmac.new(k, v + b'\x01' + private_key + msg_digest, hashlib.sha256).digest()
    K = hmac.new(k, v, hashlib.sha256).digest()

    return bytes_to_long(K)

# 生成私钥
private_key = random.randint(1, n-1)
print("Private Key:", hex(private_key))

# 计算公钥
public_key = (long_to_bytes(private_key), pow(G_y, private_key, p))
print("Public Key:", public_key)

# 消息签名
message = b"I love you."
message_hash = int.from_bytes(hashlib.sha256(message).digest(), byteorder='big')
k = deterministic_generate_k(long_to_bytes(message_hash), long_to_bytes(private_key))

r, s = 0, 0
while r == 0 or s == 0:
    x, y = pow(G_x, k, p), pow(r, p - 2, p)
    tmp = ((message_hash + x * private_key) * y) % p
    r = x % n
    s = tmp % n

signature = encode_dss_signature(r, s)
print("Signature:", signature)
end=time.time()
print("所用时间为",end-start)
