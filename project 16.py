import socket
from Cryptodome.Cipher import AES
from Cryptodome.Util.Padding import pad, unpad
from Cryptodome.PublicKey import ECC
from Cryptodome.Signature import DSS
from Cryptodome.Hash import SHA256

# SM2 2P解密函数
def sm2_2p_decrypt(private_key, ciphertext):
    # 解密密文
    plaintext = private_key.decrypt(ciphertext)
    return plaintext

# 创建Socket连接
def create_socket_connection(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    return sock

# 发送数据
def send_data(sock, data):
    sock.sendall(data.encode())

# 接收数据
def receive_data(sock):
    data = sock.recv(1024)
    return data.decode()

# 主函数
def main():
    # 生成SM2密钥对
    private_key = ECC.generate(curve='sm2p256v1')
    public_key = private_key.public_key()

    # 将公钥发送给对方
    sock = create_socket_connection('127.0.0.1', 12345)
    send_data(sock, public_key.export_key(format='PEM'))
    sock.close()

    # 接收对方的密文
    sock = create_socket_connection('127.0.0.1', 12345)
    ciphertext = receive_data(sock)
    sock.close()

    # 使用私钥进行解密
    plaintext = sm2_2p_decrypt(private_key, ciphertext)
    print("解密结果:", plaintext)

if __name__ == "__main__":
    main()
