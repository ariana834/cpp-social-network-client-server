#include "CommandFactory.h"
#include <sstream>
#include "LoginCommand.h"
#include "RegisterCommand.h"
#include "LogoutCommand.h"
#include "SetProfileVisibilityCommand.h"
#include "SetFriendTypeCommand.h"
#include "ListFriendsCommand.h"
#include "AcceptFriendCommand.h"
#include "ChatCommand.h"
#include "ChatsCommand.h"
#include "FriendRequestCommand.h"
#include "ListFriendRequestsCommand.h"
#include "CreatePostCommand.h"
#include "ListPostsCommand.h"
#include "OpenCommand.h"
#include "SendCommand.h"
#include "ShowCommand.h"

#include <vector>

#include "AddCommand.h"
#include "GroupCommand.h"

Command* CommandFactory::create(const std::string& input) {
    std::stringstream ss(input);
    std::string cmd;
    ss >> cmd;
    if (cmd== "login") {
        std::string user, pass;
        ss >> user >> pass;
        return new LoginCommand(user, pass);
    }

    if (cmd == "register") {
        std::string user, pass, email, role, visibility;
        ss >> user >> pass >> email >> role >> visibility;
        return new RegisterCommand(user, pass, email, role, visibility);
    }

    if (cmd == "logout") {
        return new LogoutCommand();
    }

    if (cmd == "set_visibility") {
        std::string vis;
        ss >> vis;
        return new SetProfileVisibilityCommand(vis);
    }
    if (cmd == "add_friend") {
        std::string user;
        ss >> user;
        return new FriendRequestCommand(user);
    }

    if (cmd == "accept_friend") {
        std::string user;
        ss >> user;
        return new AcceptFriendCommand(user);
    }

    if (cmd == "list_friends") {
        return new ListFriendsCommand();
    }

    if (cmd == "set_friend_type") {
        std::string user;
        int type;
        ss >> user >> type;
        return new SetFriendTypeCommand(user, type);
    }

    if (cmd == "list_friend_requests") {
        return new ListFriendRequestsCommand();
    }

    if (cmd == "create_post") {
        int vis;
        ss >> vis;
        std::string content;
        std::getline(ss, content);
        if (!content.empty() && content[0] == ' ') content.erase(0, 1);
        return new CreatePostCommand(vis, content);
    }

    if (cmd == "list_posts") {
        return new ListPostsCommand();
    }

    if (cmd == "chats") {
        return new ChatsCommand();
    }

    if (cmd == "chat") {
        std::string otherUser;
        ss >> otherUser;
        if (otherUser.empty())
            return nullptr;
        return new ChatCommand(otherUser);
    }

    if (cmd == "open") {
        int conv_id;
        ss >> conv_id;
        if (!ss)
            return nullptr;
        return new OpenCommand(conv_id);
    }

    if (cmd == "send") {
        std::string content;
        std::getline(ss, content);
        if (!content.empty() && content[0] == ' ')
            content.erase(0, 1);
        if (content.empty())
            return nullptr;
        return new SendCommand(content);
    }
    if (cmd == "show") {
        return new ShowCommand();
    }
    if (cmd == "group") {
        std::vector<std::string> users;
        std::string u;
        while (ss >> u)
            users.push_back(u);
        if (users.empty())
            return nullptr;
        return new GroupCommand(users);
    }
    if (cmd == "add") {
        std::string user;
        ss >> user;
        if (user.empty())
            return nullptr;
        return new AddCommand(user);
    }
    return nullptr;
}
