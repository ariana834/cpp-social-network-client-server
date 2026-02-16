#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <optional>
#include <mutex>
#include <vector>

struct User {
    int user_id;
    std::string username;
    std::string password;
    std::string user_type;
    std::string profile_visibility;
};
struct Post {
    std::string author;
    std::string content;
    int visibility;
    std::string created_at;
};
struct Message {
    int message_id;
    int conversation_id;
    std::string sender;
    std::string content;
    std::string sent_at;
};


class Database {
private:
    sqlite3* db;
    std::string db_path;
    std::mutex dbMutex;

    std::string Hashpassword(const std::string& password);

public:
    Database(const std::string& db_name = "social_network.db");
    ~Database();

    static Database& getInstance();
    bool initDatabase();

    //Functii user
    bool registerUser(const std::string& username,const std::string& password,const std::string& email,const std::string& user_type = "normal");
    std::optional<User> authenticateUser(const std::string& username,const std::string& password);
    bool updateProfileVisibility(int user_id,const std::string& visibility);
    std::optional<User> getUserByUsername(const std::string& username);

    //Functii prieteni
    bool sendFriendRequest(const std::string& from,const std::string& to);
    bool acceptFriendRequest(const std::string& to,const std::string& from);
    bool areFriends(const std::string& username,const std::string& friendUsername);
    bool setFriendType(const std::string& user,const std::string& friendUser,int type);
    std::vector<std::pair<std::string, int>>getFriends(const std::string& username);
    std::vector<std::string> getFriendRequests(const std::string& username);

    //Functii postare
    bool createPost(const std::string& author,const std::string& content,int visibility);
    std::vector<Post> getVisiblePosts(const std::string& username);
    std::vector<Post> getPublicPosts();

    //Functii mesagerie
    int createConversation(const std::string& user1,const std::string& user2);
    int createGroupConversation(const std::vector<std::string>& users);
    std::optional<int> getPrivateConversation(const std::string& user1,const std::string& user2);
    std::vector<int> getUserConversations(const std::string& username);
    bool isParticipant(int conversation_id,const std::string& username);
    bool addParticipant(int conversation_id,const std::string& username);
    bool sendMessage(int conversation_id,const std::string& sender,const std::string& content);
    std::vector<Message> getMessages(int conversation_id);
    std::vector<std::string> getConversationParticipants(int conversation_id);
};

#endif
