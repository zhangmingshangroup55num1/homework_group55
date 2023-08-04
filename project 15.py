import socket
from gmssl import sm2, func

# 创建socket对象
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 绑定IP地址和端口号
server_address = ('localhost', 12345)
server_socket.bind(server_address)

# 监听连接
server_socket.listen(1)

print('等待客户端连接...')

# 接受客户端连接
client_socket, client_address = server_socket.accept()
print('客户端已连接:', client_address)

# 生成SM2密钥对
private_key, public_key = sm2.generate_key_pair()

# 发送公钥给客户端
client_socket.send(public_key)

# 接收客户端发送的数据
data = client_socket.recv(1024)

# 解密数据
cipher_text = func.bytes_to_list(data)
plain_text = sm2.decrypt(private_key, cipher_text)

print('接收到的数据:', plain_text)

# 加密数据
plain_text = 'Hello, client!'
cipher_text = sm2.encrypt(public_key, func.bytes_to_list(plain_text.encode()))

# 发送加密后的数据给客户端
client_socket.send(func.list_to_bytes(cipher_text))

# 关闭连接
client_socket.close()
server_socket.close()
