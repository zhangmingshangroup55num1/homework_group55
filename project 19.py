import random

def Isprime(a, b):
    while a != 0:
        a, b = b % a, a
    if b != 1 and b != -1:
        return 1
    return 0
#根据矩阵计算最大公因子
def gcd(a, m):
    if Isprime(a, m):
        return None
    u1, u2, u3 = 1, 0, a
    v1, v2, v3 = 0, 1, m
    while v3 != 0:
        q = u3 // v3
        v1, v2, v3, u1, u2, u3 = (u1 - q * v1), (u2 - q * v2), (u3 - q * v3), v1, v2, v3
    if u1 > 0:
        return u1 % m
    else:
        return (u1 + m) % m
#椭圆曲线加法
def ECadd(P, Q, a, p):
    if (P == 0):
        return Q
    if (Q == 0):
        return P
    if P == Q:
        ss = (3*pow(P[0],2) + a)
        zz = gcd(2*P[1], p)
        k = (ss*zz) % p
    else:
        ss = (P[1]-Q[1])
        zz = (P[0]-Q[0])
        k = (ss*gcd(zz, p)) % p

    Rx = (pow(k,2)-P[0] - Q[0]) % p
    Ry = (k*(P[0]-Rx) - P[1]) % p
    R = [Rx, Ry]
    return R
#椭圆曲线乘法
def ECmul(n, l, a, p):
    if n == 0:
        return 0
    if n == 1:
        return l
    t = l
    while (n >= 2):
        t = ECadd(t, l, a, p)
        n = n - 1
    return t
#签名，用到哈希函数
def Sign(message, a, b, p, n, d, G):
    k = random.randint(1, n-1)
    R = ECmul(k, G, a, p)
    r = R[0] % n
    e = hash(message)
    s = (gcd(k, n) * (e + d * r)) % n
    return r, s
#验证
def Vy(r, s, message, a, b, p, n, G, P):
    e = hash(message)
    w = gcd(s, n)
    S = ECadd(ECmul((e * w) % n, G, a, p), ECmul((r * w) % n, P, a, p), a, p)
    if(S != 0):
        if(S[0] % n == r):
            return True
        else: return False
    return False

def Ecd(e, r, s, a, b, p, n, G, P):
    w = gcd(s, n)
    S = ECadd(ECmul((e * w) % n, G, a, p), ECmul((r * w) % n, P, a, p), a, p)
    if(S != 0):
        if(S[0] % n == r):
            return True
        else: return False
    return False
#伪造
def Pretend(r, s, a, b, p, n, G, P):
    u = random.randint(1, n-1)
    v = random.randint(1, n-1)
    R1 = ECadd(ECmul(u, G, a, p), ECmul(v, P, a, p), a, p)
    r1 = R1[0] % n
    e1 = (r1 * u * gcd(v, n)) % n
    s1 = (r1 * gcd(v, n)) % n
    print('pretend mess：', e1,'pretend sign：', r1, s1)
    if(Ecd(e1, r1, s1, a, b, p, n, G, P)):
        print('pass')
    else: print('no pass')

a,b,p,d,n = 2,2,17,7,19
G = [5, 1]
message = 'Satoshi'
e = hash(message)
P = ECmul(d, G, a, p)

r, s = Sign(message, a, b, p, n, d, G)
print("before sign:",r,s)
if(Vy(r,s, message, a, b, p, n, G, P)):
    print('right')
else: print('wrong')
Pretend(r,s,a, b, p, n, G, P)
