#pragma once

#include <cstring>
#include <iostream>

class Invalid {
private:
  const char *message;

public:
  Invalid() : message("Invalid"){};
  const char *what() const { return message; }
};

class Command {
private:
  int sz, pos;
  std::string str;

public:
  bool read() {
    bool res = (bool)std::getline(std::cin, str);
    sz = str.size();
    pos = 0;
    return res;
  }
  std::string getstr() {
    std::string ans = "";
    while (pos < sz && str[pos] == ' ') {
      pos++;
    }
    while (pos < sz && str[pos] != ' ') {
      ans += str[pos++];
    }
    return ans;
  }
  void moveback() {
    while (pos > 0 && str[pos - 1] != ' ') {
      pos--;
    }
  }
  bool empty() {
    while (pos < sz && str[pos] == ' ') {
      pos++;
    }
    return (pos == sz);
  }
};
