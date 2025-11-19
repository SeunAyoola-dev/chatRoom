cmake_minimum_required(VERSION 3.16)
project(chatRoom C)

set(CMAKE_C_STANDARD 11)

# Shared code
set(SHARED_FILES
        SocketUtil/socket_utils.c
)

# Server executable
add_executable(chatServer
        SocketServer/main.c
        ${SHARED_FILES}
)

# Client executable
add_executable(chatClient
        SocketClient/main.c
        ${SHARED_FILES}
)