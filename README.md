# cpp-select-chat

一个使用 C++17 + `select()` 实现的简易 TCP 聊天室示例，支持多客户端连接、昵称、广播与空闲连接清理。

## 功能
- 基于 `select()` 的单线程多路复用
- 昵称握手（首次消息作为昵称）
- `/quit` 退出并广播离开通知
- 处理 `EINTR`、`EPIPE` 等常见异常
- 空闲连接超时清理（默认 60 秒）

## 目录结构
- `server.cpp`: 服务器端逻辑
- `client.cpp`: 客户端逻辑
- `AGENTS.md`: 贡献与协作指南

## 编译
```bash
g++ -std=c++17 -O2 -Wall -Wextra -o server server.cpp
g++ -std=c++17 -O2 -Wall -Wextra -o client client.cpp
```

## 运行
1. 启动服务器：
```bash
./server
```
2. 另开终端启动客户端（可以多开几个）：
```bash
./client
```
3. 首次输入作为昵称，之后正常聊天；输入 `/quit` 退出。

## 备注
- 服务器监听端口为 `1234`，客户端连接 `127.0.0.1`。
- 这是教学/练习用途示例，可作为后端网络编程面试讲解样例。
