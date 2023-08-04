import hashlib
import random

# SM2算法参数
p = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF
a = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC
b = 0x28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93
n = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123
Gx = int('32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE171BEC59EB1CF27E3', 16)
Gy = int('E0A02A7D03B8B2386506D7E313BA8CB7DAB35819764048EA3FDBCC18FFD97A22', 16)
# 生成随机数k
def generate_k():
    return random.randint(1, n-1)

# 生成公钥
def generate_public_key(private_key):
    return point_multiplication(private_key, (Gx, Gy))

# 生成签名
def generate_signature(private_key, message):
    k = generate_k()
    x1, y1 = point_multiplication(k, (Gx, Gy))
    e = int(hashlib.sha256(message.encode()).hexdigest(), 16)
    r = (e + x1) % n
    s = (mod_inverse(1 + private_key, n) * (k - r * private_key)) % n
    return r, s

# 验证签名
def verify_signature(public_key, message, signature):
    r, s = signature
    e = int(hashlib.sha256(message.encode()).hexdigest(), 16)
    t = (r + s) % n
    x1, y1 = point_multiplication(s, (Gx, Gy))
    x2, y2 = point_multiplication(t, public_key)
    x, y = point_addition((x1, y1), (x2, y2))
    R = (e + x) % n
    return R == r

# 点加法
def point_addition(p1, p2):
    x1, y1 = p1
    x2, y2 = p2
    if p1 == (0, 0):
        return p2
    if p2 == (0, 0):
        return p1
    if x1 == x2 and y1 == y2:
        lam = ((3 * x1 * x1 + a) * mod_inverse(2 * y1, p)) % p
    else:
        lam = ((y2 - y1) * mod_inverse(x2 - x1, p)) % p
    x3 = (lam * lam - x1 - x2) % p
    y3 = (lam * (x1 - x3) - y1) % p
    return x3, y3

# 点倍乘
def point_multiplication(k, p):
    if k == 0 or p == (0, 0):
        return (0, 0)
    q = (0, 0)
    while k > 0:
        if k % 2 == 1:
            q = point_addition(q, p)
        p = point_addition(p, p)
        k = k // 2
    return q

# 求模逆
def mod_inverse(a, m):
    if a == 0:
        return 0
    lm, hm = 1, 0
    low, high = a % m, m
    while low > 1:
        ratio = high // low
        nm, new = hm - lm * ratio, high - low * ratio
        lm, low, hm, high = nm, new, lm, low
    return lm % m

# 测试代码
private_key = random.randint(1, n-1)
public_key = generate_public_key(private_key)
#message = "Hello, world!"
print("请输入您要加密的文字:")
message=input()
signature = generate_signature(private_key, message)
print(signature)
valid = verify_signature(public_key, message, signature)
print("Valid signature:", valid)


