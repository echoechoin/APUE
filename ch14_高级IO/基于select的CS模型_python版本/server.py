#coding: utf-8
import select
import socket
import os
import Queue
# 回显服务器：
# 服务器：python ./server.py
# 客户端：telnet 127.0.0.1 8888

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
server.bind(("0.0.0.0", 8888))
server.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
server.listen(100)
# 连接进来的客户端
connections = {}

# 等待处理的action
wait_for_input     = [server]
wait_for_output    = []
wait_for_exception = []

# 消息队列，类似于管道
message_queues = {}

while True:
    # 某个连接断开后，select 表明可读，但是读取到的字节数为0。
    readable, wirteable, exception = select.select(wait_for_input, wait_for_output, wait_for_exception, 10)
    # 读队列
    for file_io in readable:
        if file_io == server:
            client, client_address = server.accept()
            connections[client] = client_address
            print(">>> new connection:", client_address)
            wait_for_input.append(client)
            wait_for_exception.append(client)
        else:
            data = file_io.recv(1024)
            if data != "":
                if file_io not in message_queues.keys():
                    message_queues[file_io] = Queue.Queue()
                message_queues[file_io].put(data)
                wait_for_output.append(file_io)
            else:
                print(">>> connection closed:", connections[file_io])
                del connections[file_io]
                if file_io in wait_for_input:
                    wait_for_input.remove(file_io)
                if file_io in wait_for_output:
                    wait_for_output.remove(file_io)
                if file_io in wait_for_exception:
                    wait_for_exception.remove(file_io)
                if file_io in message_queues.keys():
                    del message_queues[file_io]
                file_io.close()
    # 写队列
    for file_io in wirteable:
        data = ""
        if file_io not in message_queues:
            pass
        else:
            data = message_queues[file_io].get_nowait()
        file_io.send("server return: " + data)  
        wait_for_output.remove(file_io)

    # exception队列
    pass

    # 处理老旧的连接
    for file_io in connections:
        pass
