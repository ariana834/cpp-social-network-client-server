#include "Database.h"
#include "Logger.h"
#include <functional>
#include <iostream>
#include <unistd.h>
using namespace std;

Database::Database(const string &db_name)
    : db(nullptr),
      db_path("social_network.db") {
  if (!initDatabase()) {
    Logger::getInstance().log(LogLevel::ERROR,
                              "Database initialization failed");
  }
}

Database::~Database() {
  if (db) {
    sqlite3_close(db);
  }
}

bool Database::initDatabase() {
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
    Logger::getInstance().log(LogLevel::ERROR,
                              "Cannot open database: " +
                                  std::string(sqlite3_errmsg(db)));
    return false;
  }
  const char *sql =
      /* USERS */
      "CREATE TABLE IF NOT EXISTS users ("
      "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "username TEXT UNIQUE NOT NULL,"
      "password TEXT NOT NULL,"
      "email TEXT UNIQUE NOT NULL,"
      "user_type TEXT NOT NULL,"
      "profile_visibility TEXT NOT NULL DEFAULT 'public'"
      ");"

      /* FRIENDS */
      "CREATE TABLE IF NOT EXISTS friends ("
      "user_username TEXT NOT NULL,"
      "friend_username TEXT NOT NULL,"
      "friend_type INTEGER NOT NULL DEFAULT 0," /* 0 normal, 1 close */
      "status INTEGER NOT NULL DEFAULT 0,"      /* 0 pending, 1 accepted */
      "PRIMARY KEY (user_username, friend_username),"
      "FOREIGN KEY (user_username) REFERENCES users(username),"
      "FOREIGN KEY (friend_username) REFERENCES users(username)"
      ");"

      /* POSTS */
      "CREATE TABLE IF NOT EXISTS posts ("
      "post_id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "author_username TEXT NOT NULL,"
      "content TEXT NOT NULL,"
      "visibility INTEGER NOT NULL," /* 0 public, 1 friends, 2 close */
      "created_at TEXT DEFAULT CURRENT_TIMESTAMP,"
      "FOREIGN KEY (author_username) REFERENCES users(username)"
      ");"

      /* CONVERSATIONS */
      "CREATE TABLE IF NOT EXISTS conversations ("
      "conversation_id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "created_at TEXT DEFAULT CURRENT_TIMESTAMP"
      ");"

      /* PARTICIPANTS */
      "CREATE TABLE IF NOT EXISTS conversation_participants ("
      "conversation_id INTEGER NOT NULL,"
      "username TEXT NOT NULL,"
      "joined_at TEXT DEFAULT CURRENT_TIMESTAMP,"
      "PRIMARY KEY (conversation_id, username),"
      "FOREIGN KEY (conversation_id) REFERENCES conversations(conversation_id),"
      "FOREIGN KEY (username) REFERENCES users(username)"
      ");"

      /* MESSAGES */
      "CREATE TABLE IF NOT EXISTS conversation_messages ("
      "message_id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "conversation_id INTEGER NOT NULL,"
      "sender_username TEXT NOT NULL,"
      "content TEXT NOT NULL,"
      "sent_at TEXT DEFAULT CURRENT_TIMESTAMP,"
      "FOREIGN KEY (conversation_id) REFERENCES conversations(conversation_id),"
      "FOREIGN KEY (sender_username) REFERENCES users(username)"
      ");";

  char *errMsg = nullptr;
  if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
    Logger::getInstance().log(LogLevel::ERROR,
                              "Database schema creation failed: " +
                                  std::string(errMsg));
    sqlite3_free(errMsg);
    return false;
  }
  Logger::getInstance().log(LogLevel::INFO, "Database is ready");
  return true;
}

// Functii user
std::string Database::Hashpassword(const std::string &password) {
  std::hash<std::string> hasher;
  return std::to_string(hasher(password));
}

bool Database::registerUser(const std::string &username,
                            const std::string &password,
                            const std::string &email,
                            const std::string &user_type) {
  const char *sql = "INSERT INTO users (username, password, email, user_type) "
                    "VALUES (?, ?, ?, ?);";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    Logger::getInstance().log(LogLevel::ERROR,
                              "Prepare failed (registerUser): " +
                                  std::string(sqlite3_errmsg(db)));
    return false;
  }

  std::string hashed = Hashpassword(password);
  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, user_type.c_str(), -1, SQLITE_TRANSIENT);
  bool success = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  if (!success) {
    Logger::getInstance().log(LogLevel::WARNING,
                              "Register failed for user: " + username);
    return false;
  }
  Logger::getInstance().log(LogLevel::INFO,
                            "User registered successfully: " + username);
  return true;
}
Database &Database::getInstance() {
  static Database instance;
  return instance;
} // pentru a avea un singur obiect

optional<User> Database::authenticateUser(const std::string &username,
                                          const std::string &password) {
  Logger::getInstance().log(LogLevel::INFO,
                            "Login attempt for user: " + username);

  const char *sql =
      "SELECT user_id, username, password, user_type, profile_visibility "
      "FROM users WHERE username = ?;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    Logger::getInstance().log(LogLevel::ERROR,
                              "Prepare failed (authenticateUser): " +
                                  std::string(sqlite3_errmsg(db)));
    return std::nullopt;
  }

  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_ROW) {
    Logger::getInstance().log(
        LogLevel::WARNING, "Login failed: user not found (" + username + ")");
    sqlite3_finalize(stmt);
    return std::nullopt;
  }

  std::string storedHash =
      reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
  if (storedHash != Hashpassword(password)) {
    Logger::getInstance().log(
        LogLevel::WARNING, "Login failed: wrong password for user " + username);
    sqlite3_finalize(stmt);
    return std::nullopt;
  }

  User user;
  user.user_id = sqlite3_column_int(stmt, 0);
  user.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
  user.password = storedHash;
  user.user_type = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
  user.profile_visibility =
      reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
  sqlite3_finalize(stmt);

  return user;
}

bool Database::updateProfileVisibility(int user_id,
                                       const std::string &visibility) {
  Logger::getInstance().log(LogLevel::INFO,
                            "Updating profile visibility for user_id: " +
                                std::to_string(user_id));
  const char *sql =
      "UPDATE users SET profile_visibility = ? WHERE user_id = ?;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    Logger::getInstance().log(LogLevel::ERROR,
                              "Prepare failed (updateProfileVisibility)");
    return false;
  }
  sqlite3_bind_text(stmt, 1, visibility.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, user_id);
  bool success = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);

  if (!success) {
    Logger::getInstance().log(
        LogLevel::WARNING, "Failed to update profile visibility for user_id: " +
                               std::to_string(user_id));
    return false;
  }
  return success;
}
std::optional<User> Database::getUserByUsername(const std::string &username) {
  const char *sql =
      "SELECT user_id, username, password, user_type, profile_visibility "
      "FROM users WHERE username = ?;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    return std::nullopt;
  }
  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
  if (sqlite3_step(stmt) != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    return std::nullopt;
  }

  User user;
  user.user_id = sqlite3_column_int(stmt, 0);
  user.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
  user.password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
  user.user_type = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
  user.profile_visibility =
      reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
  sqlite3_finalize(stmt);
  return user;
}
/*
 Functii prieteni
 friend_type:
   0 = prieten normal
   1 = prieten apropiat
   2 = cunostinta
 status:
   0 = cerere trimisă (pending)
   1 = prietenie acceptată
*/
bool Database::sendFriendRequest(const std::string &from,
                                 const std::string &to) {
  std::lock_guard<std::mutex> lock(dbMutex);
  const char *sql = "INSERT INTO friends (user_username, friend_username) "
                    "VALUES (?, ?);";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, from.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, to.c_str(), -1, SQLITE_TRANSIENT);
  bool success = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  return success;
}

bool Database::acceptFriendRequest(const std::string &to,
                                   const std::string &from) {
  std::lock_guard<std::mutex> lock(dbMutex);

  // verificam daca avem cerere
  const char *checkSql =
      "SELECT 1 FROM friends "
      "WHERE user_username = ? AND friend_username = ? AND status = 0;";
  sqlite3_stmt *checkStmt;
  if (sqlite3_prepare_v2(db, checkSql, -1, &checkStmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(checkStmt, 1, from.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(checkStmt, 2, to.c_str(), -1, SQLITE_TRANSIENT);
  if (sqlite3_step(checkStmt) != SQLITE_ROW) {
    sqlite3_finalize(checkStmt);
    return false;
  }
  sqlite3_finalize(checkStmt);

  // acceptam cererea
  const char *updateSql = "UPDATE friends SET status = 1 "
                          "WHERE user_username = ? AND friend_username = ?;";

  sqlite3_stmt *updateStmt;
  sqlite3_prepare_v2(db, updateSql, -1, &updateStmt, nullptr);
  sqlite3_bind_text(updateStmt, 1, from.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(updateStmt, 2, to.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_step(updateStmt);
  sqlite3_finalize(updateStmt);

  // schimbam si pe partea cealalta
  const char *insertReverse =
      "INSERT INTO friends (user_username, friend_username, status) "
      "VALUES (?, ?, 1);";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, insertReverse, -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, to.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, from.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return true;
}

bool Database::areFriends(const std::string &a, const std::string &b) {
  std::lock_guard<std::mutex> lock(dbMutex);
  const char *sql =
      "SELECT 1 FROM friends "
      "WHERE user_username = ? AND friend_username = ? AND status = 1;";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, a.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, b.c_str(), -1, SQLITE_TRANSIENT);
  bool result = (sqlite3_step(stmt) == SQLITE_ROW);
  sqlite3_finalize(stmt);
  return result;
}

bool Database::setFriendType(const std::string &user,
                             const std::string &friendUser, int type) {
  if (type < 0 || type > 2)
    return false;
  if (!areFriends(user, friendUser))
    return false;
  std::lock_guard<std::mutex> lock(dbMutex);
  const char *sql =
      "UPDATE friends SET friend_type = ? "
      "WHERE user_username = ? AND friend_username = ? AND status = 1;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, type);
  sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, friendUser.c_str(), -1, SQLITE_TRANSIENT);
  bool success = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  return success;
}

std::vector<std::pair<std::string, int>>
Database::getFriends(const std::string &username) {
  std::lock_guard<std::mutex> lock(dbMutex);
  std::vector<std::pair<std::string, int>> friends;
  const char *sql = "SELECT friend_username, friend_type "
                    "FROM friends "
                    "WHERE user_username = ? AND status = 1;";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    std::string friendName =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    int type = sqlite3_column_int(stmt, 1);
    friends.push_back({friendName, type});
  }
  sqlite3_finalize(stmt);
  return friends;
}

std::vector<std::string>
Database::getFriendRequests(const std::string &username) {
  std::lock_guard<std::mutex> lock(dbMutex);
  std::vector<std::string> requests;
  const char *sql = "SELECT user_username FROM friends "
                    "WHERE friend_username = ? AND status = 0;";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return requests;

  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    std::string from =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    requests.push_back(from);
  }
  sqlite3_finalize(stmt);
  return requests;
}

// Functii postari
bool Database::createPost(const std::string &author, const std::string &content,
                          int visibility) {
  if (visibility < 0 || visibility > 2)
    return false;

  std::lock_guard<std::mutex> lock(dbMutex);

  const char *sql = "INSERT INTO posts (author_username, content, visibility) "
                    "VALUES (?, ?, ?);";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;

  sqlite3_bind_text(stmt, 1, author.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 3, visibility);

  bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  return ok;
}

std::vector<Post> Database::getVisiblePosts(const std::string &username) {
  std::lock_guard<std::mutex> lock(dbMutex);

  std::vector<Post> posts;

  const char *sql = "SELECT p.post_id, p.author_username, p.content, "
                    "p.visibility, p.created_at "
                    "FROM posts p "
                    "WHERE ( "
                    "   p.author_username = ? "
                    "   OR p.visibility = 0 "
                    "   OR ( "
                    "       p.visibility = 1 AND EXISTS ( "
                    "           SELECT 1 FROM friends f "
                    "           WHERE f.user_username = p.author_username "
                    "           AND f.friend_username = ? "
                    "           AND f.status = 1 "
                    "       ) "
                    "   ) "
                    "   OR ( "
                    "       p.visibility = 2 AND EXISTS ( "
                    "           SELECT 1 FROM friends f "
                    "           WHERE f.user_username = p.author_username "
                    "           AND f.friend_username = ? "
                    "           AND f.status = 1 "
                    "           AND f.friend_type = 1 "
                    "       ) "
                    "   ) "
                    ") "
                    "ORDER BY p.created_at DESC;";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    Logger::getInstance().log(LogLevel::ERROR,
                              "Failed to prepare getVisiblePosts: " +
                                  std::string(sqlite3_errmsg(db)));
    return posts;
  }

  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Post p;
    p.author = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    p.content = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    p.visibility = sqlite3_column_int(stmt, 3);
    p.created_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
    posts.push_back(p);
  }

  sqlite3_finalize(stmt);
  return posts;
}

std::vector<Post> Database::getPublicPosts() {
  std::lock_guard<std::mutex> lock(dbMutex);

  std::vector<Post> posts;

  const char *sql = "SELECT author_username, content, visibility, created_at "
                    "FROM posts "
                    "WHERE visibility = 0 "
                    "ORDER BY created_at DESC;";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return posts;

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Post p;
    p.author = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    p.content = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    p.visibility = sqlite3_column_int(stmt, 2);
    p.created_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    posts.push_back(p);
  }

  sqlite3_finalize(stmt);
  return posts;
}

// Functii mesagerie
bool Database::isParticipant(int conversation_id, const std::string &username) {
  const char *sql = "SELECT 1 FROM conversation_participants "
                    "WHERE conversation_id = ? AND username = ?;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, conversation_id);
  sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
  bool ok = (sqlite3_step(stmt) == SQLITE_ROW);
  sqlite3_finalize(stmt);
  return ok;
}

bool Database::addParticipant(int conversation_id,
                              const std::string &username) {
  std::lock_guard<std::mutex> lock(dbMutex);

  if (isParticipant(conversation_id, username))
    return true;
  const char *sql =
      "INSERT INTO conversation_participants (conversation_id, username) "
      "VALUES (?, ?);";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;

  sqlite3_bind_int(stmt, 1, conversation_id);
  sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);

  bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  return ok;
}
bool Database::sendMessage(int conversation_id, const std::string &sender,
                           const std::string &content) {
  std::lock_guard<std::mutex> lock(dbMutex);

  if (!isParticipant(conversation_id, sender))
    return false;

  const char *sql = "INSERT INTO conversation_messages (conversation_id, "
                    "sender_username, content) "
                    "VALUES (?, ?, ?);";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;

  sqlite3_bind_int(stmt, 1, conversation_id);
  sqlite3_bind_text(stmt, 2, sender.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, content.c_str(), -1, SQLITE_TRANSIENT);

  bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  return ok;
}
std::vector<Message> Database::getMessages(int conversation_id) {
  std::lock_guard<std::mutex> lock(dbMutex);
  std::vector<Message> messages;

  const char *sql = "SELECT message_id, sender_username, content, sent_at "
                    "FROM conversation_messages "
                    "WHERE conversation_id = ? "
                    "ORDER BY sent_at;";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return messages;

  sqlite3_bind_int(stmt, 1, conversation_id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Message m;
    m.message_id = sqlite3_column_int(stmt, 0);
    m.conversation_id = conversation_id;
    m.sender = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    m.content = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    m.sent_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    messages.push_back(m);
  }

  sqlite3_finalize(stmt);
  return messages;
}

std::vector<std::string>
Database::getConversationParticipants(int conversation_id) {
  std::lock_guard<std::mutex> lock(dbMutex);
  std::vector<std::string> users;
  const char *sql = "SELECT username FROM conversation_participants "
                    "WHERE conversation_id = ?;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return users;
  sqlite3_bind_int(stmt, 1, conversation_id);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    std::string user =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    users.push_back(user);
  }
  sqlite3_finalize(stmt);
  return users;
}

std::optional<int> Database::getPrivateConversation(const std::string &user1,
                                                    const std::string &user2) {

  std::lock_guard<std::mutex> lock(dbMutex);

  const char *sql = "SELECT cp1.conversation_id "
                    "FROM conversation_participants cp1 "
                    "JOIN conversation_participants cp2 "
                    "ON cp1.conversation_id = cp2.conversation_id "
                    "WHERE cp1.username = ? AND cp2.username = ? "
                    "GROUP BY cp1.conversation_id "
                    "HAVING COUNT(*) = 2;";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return std::nullopt;

  sqlite3_bind_text(stmt, 1, user1.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, user2.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return id;
  }

  sqlite3_finalize(stmt);
  return std::nullopt;
}
int Database::createConversation(const std::string &user1,
                                 const std::string &user2) {
  const char *sql = "INSERT INTO conversations DEFAULT VALUES;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return -1;
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    return -1;
  }
  sqlite3_finalize(stmt);
  int conversation_id = sqlite3_last_insert_rowid(db);
  addParticipant(conversation_id, user1);
  addParticipant(conversation_id, user2);
  return conversation_id;
}

int Database::createGroupConversation(const std::vector<std::string> &users) {
  if (users.size() < 2)
    return -1;
  const char *sql = "INSERT INTO conversations DEFAULT VALUES;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return -1;
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    return -1;
  }
  sqlite3_finalize(stmt);
  int conversation_id = sqlite3_last_insert_rowid(db);
  for (const auto &user : users)
    addParticipant(conversation_id, user);
  return conversation_id;
}
std::vector<int> Database::getUserConversations(const std::string &username) {

  std::lock_guard<std::mutex> lock(dbMutex);

  std::vector<int> conversations;

  const char *sql =
      "SELECT cp.conversation_id FROM conversation_participants cp JOIN "
      "conversations c ON cp.conversation_id = c.conversation_id WHERE "
      "cp.username = ? ORDER BY c.created_at DESC;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return conversations;

  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW)
    conversations.push_back(sqlite3_column_int(stmt, 0));

  sqlite3_finalize(stmt);
  return conversations;
}
