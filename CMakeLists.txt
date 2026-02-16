cmake_minimum_required(VERSION 3.16)
project(VirtualSoc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(SQLite3 REQUIRED)


find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(client
        client/client.cpp
)
target_link_libraries(client PRIVATE SQLite::SQLite3)

add_executable(qt_client
        qt_client/main.cpp
        qt_client/LoginWindow.cpp
        qt_client/LoginWindow.h
        qt_client/NetworkClient.cpp
        qt_client/NetworkClient.h
        qt_client/RegisterWindow.cpp
        qt_client/RegisterWindow.h
        qt_client/MainWindow.cpp
        qt_client/MainWindow.h
        qt_client/AddFriend.cpp
        qt_client/AddFriend.h
        qt_client/FriendRequests.cpp
        qt_client/FriendRequests.h
        qt_client/FriendType.cpp
        qt_client/FriendType.h
        qt_client/CreatePost.cpp
        qt_client/CreatePost.h
        qt_client/MessagesWindow.cpp
        qt_client/MessagesWindow.h
        qt_client/GuestPostsWindow.cpp
        qt_client/GuestPostsWindow.h
)
target_link_libraries(qt_client PRIVATE Qt5::Widgets)

file(GLOB_RECURSE SERVER_COMMANDS_SOURCES
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/server/commands/*.cpp
)

add_executable(server
        server/server.cpp
        server/CommandFactory.cpp
        server/CommandFactory.h
        server/Command.h
        server/Database.cpp
        server/Database.h
        server/Logger.cpp
        server/Logger.h
        server/SessionManager.cpp
        server/SessionManager.h

        # comenzi (explicite dacă ai și în server/ direct)
        server/commands/LoginCommand.cpp
        server/commands/RegisterCommand.cpp
        server/commands/AcceptFriendCommand.cpp
        server/commands/CreatePostCommand.cpp

        # plus tot ce mai apare în commands/
        ${SERVER_COMMANDS_SOURCES}
)

target_include_directories(server PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/server
        ${CMAKE_CURRENT_SOURCE_DIR}/server/commands
)
target_link_libraries(server PRIVATE SQLite::SQLite3)
