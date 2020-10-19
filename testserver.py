#coding=utf-8
import socket
def readfile(file):
    fd = open(file, "rb")
    content = fd.read()
    fd.close()
    return content

#hello是一个可执行ELF
content = readfile('./hello')
s = socket.socket()         # 创建 socket 对象
host = '0.0.0.0' # 获取本地主机名
port = 56789                # 设置端口
s.bind((host, port))
s.listen(5)
while True:
    c,addr = s.accept()
    print '连接地址：', addr
    c.send(p32(len(content)))
    c.send(content)
    c.close()

