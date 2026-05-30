#pragma once
#include "BPT.hpp"
#include "MemoryRiver.hpp"
#include "map/src/map.hpp"
#include "utils.hpp"
#include <cstring>

struct User {
  char username[21] = {};
  char password[31] = {};
  char name[21] = {};
  char mailAddr[31] = {};
  int privilege = -1;
  User() = default;
  User(const char *user_str, const char *pw, const char *str, const char *mail,
       int type) {
    strncpy(username, user_str, 20);
    strncpy(password, pw, 30);
    strncpy(name, str, 20);
    strncpy(mailAddr, mail, 30);
    privilege = type;
  }
  User(const std::string &user_str, const std::string &pw,
       const std::string &str, const std::string &mail, int type) {
    strncpy(username, user_str.c_str(), 20);
    strncpy(password, pw.c_str(), 30);
    strncpy(name, str.c_str(), 20);
    strncpy(mailAddr, mail.c_str(), 30);
    privilege = type;
  }
};

class UserSystem {
  friend class TicketSystem;

private:
  int count = 0;
  MemoryRiver<User> user_data;
  BPT<long long, int> user_data_pos;
  sjtu::map<long long, int> login_stack;

public:
  UserSystem() : user_data_pos("UserDataPos") {
    user_data.initialise("UserData");
    user_data.get_info(count, 1);
  }
  ~UserSystem() { user_data.write_info(count, 1); }
  int add_user(const std::string &cur_user, const std::string &user,
               const std::string &pw, const std::string &name,
               const std::string &mail, int type) {
    if (count == 0) {
      count++;
      User new_user(user, pw, name, mail, 10);
      int new_user_pos = user_data.write(new_user);
      user_data_pos.insert(Hash(user), new_user_pos);
      return 0;
    }
    long long now_user = Hash(cur_user);
    if (login_stack.find(now_user) == login_stack.end() ||
        !login_stack[now_user]) {
      return -1;
    }
    sjtu::vector<int> tmp = user_data_pos.find(now_user, INT_MIN);
    if (tmp.empty()) {
      return -1;
    }
    int now_user_pos = tmp[0];
    User now;
    user_data.read(now, now_user_pos);
    if (now.privilege <= type) {
      return -1;
    }
    count++;
    User new_user(user, pw, name, mail, type);
    int new_user_pos = user_data.write(new_user);
    user_data_pos.insert(Hash(user), new_user_pos);
    return 0;
  }
  int login(const std::string &user, const std::string &pw) {
    long long now_user = Hash(user);
    sjtu::vector<int> tmp = user_data_pos.find(now_user, INT_MIN);
    if (tmp.empty()) {
      return -1;
    }
    int now_user_pos = tmp[0];
    User now;
    user_data.read(now, now_user_pos);
    if (strcmp(now.password, pw.c_str()) || login_stack[now_user]) {
      return -1;
    }
    login_stack[now_user] = 1;
    return 0;
  }
  int logout(const std::string &user) {
    long long now_user = Hash(user);
    if (login_stack.find(now_user) == login_stack.end() ||
        !login_stack[now_user]) {
      return -1;
    }
    login_stack[now_user] = 0;
    return 0;
  }
  void query_profile(const std::string &cur_user, const std::string &user) {
    long long now_user = Hash(cur_user);
    long long target_user = Hash(user);
    if (login_stack.find(now_user) == login_stack.end() ||
        !login_stack[now_user]) {
      std::cout << "-1" << '\n';
      return;
    }
    int now_user_pos = user_data_pos.find(now_user, INT_MIN)[0];
    User now;
    user_data.read(now, now_user_pos);
    sjtu::vector<int> tmp = user_data_pos.find(target_user, INT_MIN);
    if (tmp.empty()) {
      std::cout << "-1" << '\n';
      return;
    }
    int target_user_pos = tmp[0];
    User tar_user;
    user_data.read(tar_user, target_user_pos);
    if (now.privilege == tar_user.privilege && now_user != target_user) {
      std::cout << "-1" << '\n';
    } else if (now.privilege < tar_user.privilege) {
      std::cout << "-1" << '\n';
    } else {
      std::cout << tar_user.username << ' ' << tar_user.name << ' '
                << tar_user.mailAddr << ' ' << tar_user.privilege << '\n';
    }
  }
  void modify_profile(const std::string &cur_user, const std::string &user,
                      const std::string &pw, const std::string &name,
                      const std::string &mail, int type) {
    long long now_user = Hash(cur_user);
    long long target_user = Hash(user);
    if (login_stack.find(now_user) == login_stack.end() ||
        !login_stack[now_user]) {
      std::cout << "-1" << '\n';
      return;
    }
    int now_user_pos = user_data_pos.find(now_user, INT_MIN)[0];
    User now;
    user_data.read(now, now_user_pos);
    sjtu::vector<int> tmp = user_data_pos.find(target_user, INT_MIN);
    if (tmp.empty()) {
      std::cout << "-1" << '\n';
      return;
    }
    int target_user_pos = tmp[0];
    User tar_user;
    user_data.read(tar_user, target_user_pos);
    if (now.privilege == tar_user.privilege && now_user != target_user) {
      std::cout << "-1" << '\n';
    } else if (now.privilege < tar_user.privilege) {
      std::cout << "-1" << '\n';
    } else if (type != -1 && type >= now.privilege) {
      std::cout << "-1" << '\n';
    } else {
      if (pw != "") {
        strncpy(tar_user.password, pw.c_str(), 30);
      }
      if (name != "") {
        strncpy(tar_user.name, name.c_str(), 20);
      }
      if (mail != "") {
        strncpy(tar_user.mailAddr, mail.c_str(), 30);
      }
      if (type != -1) {
        tar_user.privilege = type;
      }
      std::cout << tar_user.username << ' ' << tar_user.name << ' '
                << tar_user.mailAddr << ' ' << tar_user.privilege << '\n';
      user_data.update(tar_user, target_user_pos);
    }
  }
  void clean() {
    count = 0;
    user_data_pos.clean();
    user_data.clean();
    login_stack.clean();
  }
};