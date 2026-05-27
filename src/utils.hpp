#pragma once

#include "vector/src/vector.hpp"
#include <cstring>

const int mod1 = 114514123;
const int base1 = 93;
const int mod2 = 998244353;
const int base2 = 97;

long long Hash(const char* ch,int len) {
  long long val1 = 0, val2 = 0;
  for (int i = 0; i < len && ch[i]; i++) {
    val1 = (val1 * base1 + ch[i]    ) % mod1;
    val2 = (val2 * base2 + ch[i]) % mod2;
  }
  val1 = (val1 + mod1) % mod1;
  val2 = (val2 + mod2) % mod2;
  return (val1 * (1ll << 32) + val2);
}
long long Hash(const std::string ch) {
  int len = ch.size();
  long long val1 = 0, val2 = 0;
  for (int i = 0; i < len; i++) {
    val1 = (val1 * base1 + ch[i]) % mod1;
    val2 = (val2 * base2 + ch[i]) % mod2;
  }
  val1 = (val1 + mod1) % mod1;
  val2 = (val2 + mod2) % mod2;
  return (val1 * (1ll << 32) + val2);
}

int StringToInt(const std::string &str) {
  int pos = 0, ans = 0, flag = 1;
  if (str[0] == '-') {
    pos++;
    flag = -1;
  }
  while (pos < str.size()) {
    ans = ans * 10 + str[pos] - '0';
    pos++;
  }
  return ans * flag;
}
std::string IntToString(int x) {
  std::string ans = "";
  while (x) {
    ans += ((x % 10) + 'a');
    x /= 10;
  }
  for (int i = 0, j = ans.size() - 1; i <= j; i++, j--) {
    std::swap(ans[i], ans[j]);
  }
  return ans;
}
int TimeToInt(const std::string &str) {
  int ans = (str[0] - '0') * 10 + (str[1] - '0');
  ans *= 60;
  ans += (str[3] - '0') * 10 + (str[4] - '0');
  return ans;
}
int DateToInt(const std::string &str) {
  int ans = (str[3] - '0') * 10 + (str[4] - '0');
  if (str[1] == '7') {
    ans += 30;
  } else if (str[1] == '8') {
    ans += 30 + 31;
  } else if (str[2] == '9') {
    ans += 30 + 31 + 31;
  }
  return ans;
}
std::string IntToDate(int date) {
  if (date <= 30) {
    return "06-" + IntToString(date);
  } else if (date <= 61) {
    return "07-" + IntToString(date - 30);
  } else if (date <= 92) {
    return "08-" + IntToString(date - 61);
  } else {
    return "09-" + IntToString(date - 92);
  }
}
std::string IntToTime(int time) {
  return IntToString(time / 60) + ":" + IntToString(time % 60);
}
std::string FullTimeToString(int full_time) {
  int time = full_time % (60 * 24), date = time / 60 / 24;
  return IntToDate(date) + ' ' + IntToTime(time);
}
sjtu::vector <std::string> split_string(const std::string &str) {
  sjtu::vector <std::string> ans;
  std::string res = "";
  for (int i = 0; i < str.size(); i++) {
    if (str[i] == '|' || i + 1 == str.size()) {
      ans.push_back(res);
      res = "";
    } else {
      res += str[i];
    }
  }
  return ans;
}
