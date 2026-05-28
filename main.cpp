#include "src/Ticket.hpp"
#include "src/Train.hpp"
#include "src/User.hpp"
#include "src/command.hpp"
#include "src/utils.hpp"
#include <cerrno>
#include <exception>

Command command;
UserSystem user_sys;
TrainSystem train_sys;
TicketSystem ticket_sys(user_sys, train_sys);



void add_user() {
  std::string cur_user, user, pw, name, mail;
  int type = -1;
  for (int i = 0; i < 6; i++) {
    std::string op = command.getstr();
    if (op == "-c") {
      cur_user = command.getstr();
    } else if (op == "-u") {
      user = command.getstr();
    } else if (op == "-p") {
      pw = command.getstr();
    } else if (op == "-n") {
      name = command.getstr();
    } else if (op == "-g") {
      type = StringToInt(command.getstr());
    } else if (op == "-m") {
      mail = command.getstr();
    }
  }
  // std::cout << "cur_user = " << cur_user << '\n';
  // std::cout << "user = " << user << '\n';
  // std::cout << "pw = " << pw << '\n';
  // std::cout << "name = " << name << '\n';
  // std::cout << "mail = " << mail << '\n';
  // std::cout << "type = " << type << '\n';
  std::cout << user_sys.add_user(cur_user, user, pw, name, mail, type) << '\n';
}
void login() {
  std::string user, pw;
  for (int i = 0; i < 2; i++) {
    std::string op = command.getstr();
    if (op == "-u") {
      user = command.getstr();
    } else if (op == "-p") {
      pw = command.getstr();
    }
  }
  std::cout << user_sys.login(user, pw) << '\n';
}
void logout() {
  std::string user;
  for (int i = 0; i < 1; i++) {
    std::string op = command.getstr();
    if (op == "-u") {
      user = command.getstr();
    }
  }
  std::cout << user_sys.logout(user) << '\n';
}
void query_profile() {
  std::string cur_user, user;
  for (int i = 0; i < 2; i++) {
    std::string op = command.getstr();
    if (op == "-u") {
      user = command.getstr();
    } else if (op == "-c") {
      cur_user = command.getstr();
    }
  }
  user_sys.query_profile(cur_user, user);
}
void modify_profile() {
  std::string cur_user = "", user = "", pw = "", name = "", mail = "";
  int type = -1;
  while (!command.empty()) {
    std::string op = command.getstr();
    if (op == "-c") {
      cur_user = command.getstr();
    } else if (op == "-u") {
      user = command.getstr();
    } else if (op == "-p") {
      pw = command.getstr();
    } else if (op == "-n") {
      name = command.getstr();
    } else if (op == "-g") {
      type = StringToInt(command.getstr());
    } else if (op == "-m") {
      mail = command.getstr();
    }
  }
  user_sys.modify_profile(cur_user, user, pw, name, mail, type);
}
void add_train() {
  int sta_num = 0, seat_num = 0;
  char ty = ' ';
  std::string id, station, price, st_time, travel_time, stop_time, sale_date;
  for (int i = 0; i < 10; i++) {
    std::string op = command.getstr();
    if (op == "-i") {
      id = command.getstr();
    } else if (op == "-n") {
      sta_num = StringToInt(command.getstr());
    } else if (op == "-m") {
      seat_num = StringToInt(command.getstr());
    } else if (op == "-s") {
      station = command.getstr();
    } else if (op == "-p") {
      price = command.getstr();
    } else if (op == "-x") {
      st_time = command.getstr();
    } else if (op == "-t") {
      travel_time = command.getstr();
    } else if (op == "-o") {
      stop_time = command.getstr();
    } else if (op == "-d") {
      sale_date = command.getstr();
    } else if (op == "-y") {  
      ty = command.getstr()[0];
    }
  }
  // std::cout << "st_time = " << st_time << '\n';
  std::cout << train_sys.add_train(id, sta_num, seat_num, station, price, st_time, travel_time, stop_time, sale_date, ty) << '\n';
}
void delete_train() {
  std::string op = command.getstr();
  std::string id = command.getstr();
  std::cout << train_sys.delete_train(id) << '\n';
}
void release_train() {
  std::string op = command.getstr();
  std::string id = command.getstr();
  std::cout << train_sys.release_train(id) << '\n'; 
}
void query_train() {
  std::string id, date;
  for (int i = 0; i < 2; i++) {
    std::string op = command.getstr();
    if (op == "-i") {
      id = command.getstr();
    } else if (op == "-d") {
      date = command.getstr();
    } 
  }
  train_sys.query_train(id, date);
}
void query_ticket() {
  std::string st, ed, date;
  bool flag = 1;
  while (!command.empty()) {
    std::string op = command.getstr();
    if (op == "-s") {
      st = command.getstr();
    } else if (op == "-t") {
      ed = command.getstr();
    } else if (op == "-d") {
      date = command.getstr();
    } else if (op == "-p") {
      flag = (command.getstr() == "time");
    } 
  }
  train_sys.query_ticket(st, ed, date, flag);
}
void query_transfer() {
  std::string st, ed, date;
  bool flag = 1;
  while (!command.empty()) {
    std::string op = command.getstr();
    if (op == "-s") {
      st = command.getstr();
    } else if (op == "-t") {
      ed = command.getstr();
    } else if (op == "-d") {
      date = command.getstr();
    } else if (op == "-p") {
      flag = (command.getstr() == "time");
    } 
  }
  train_sys.query_transfer(st, ed, date, flag);
}
void buy_ticket() {
  std::string user, id, date, st, ed;
  int buy_num = 0;
  bool flag = false;
  while (!command.empty()) {
    std::string op = command.getstr();
    if (op == "-u") {
      user = command.getstr();
    } else if (op == "-i") {
      id = command.getstr();
    } else if (op == "-d") {
      date = command.getstr();
    } else if (op == "-n") {
      buy_num = StringToInt(command.getstr());
    } else if (op == "-f") {
      st = command.getstr();
    } else if (op == "-t") {
      ed = command.getstr();
    } else if (op == "-q") {
      flag = (command.getstr() == "true");
    }
  }
  // std::cout << "user = " << user << '\n';
  // std::cout << "id = " << id << '\n';
  // std::cout << "date = " << date << '\n';
  // std::cout << "st = " << st << '\n';
  // std::cout << "ed = " << ed << '\n';
  ticket_sys.buy_ticket(user, id, date, st, ed, buy_num, flag);
}
void query_order() {
  std::string op = command.getstr();
  std::string user = command.getstr();
  ticket_sys.query_order(user);
}
void refund_ticket() {
  std::string user;
  int refund_num = 1;
  while (!command.empty()) {
    std::string op = command.getstr();
    if (op == "-u") {
      user = command.getstr();
    } else if (op == "-n") {
      refund_num = StringToInt(command.getstr());
    }
  }
  std::cout << ticket_sys.refund_ticket(user, refund_num) << '\n';
}
void clean() {
  user_sys.clean();
  train_sys.clean();
  ticket_sys.clean();
}
int main() {
  // std::cout << TimeToInt("18:38");return 0;
  while (command.read()) {
    std::string timestamp = command.getstr();
    // if (timestamp == "[26704]") return 0 ;
    std::cout << timestamp << ' ';
    // int time = StringToInt(timestamp);
    // if (timestamp.size() > 6) return 0;
    std::string op = command.getstr();
    // std::cout << "op = " << op << '\n';
    if (op == "add_user") {
      add_user();
    } else if (op == "login") {
      login();
    } else if (op == "logout") {
      logout();
    } else if (op == "query_profile") {
      query_profile();
    } else if (op == "modify_profile") {
      modify_profile();
    } else if (op == "add_train") {
      add_train();
    } else if (op == "delete_train") {
      delete_train();
    } else if (op == "release_train") {
      release_train();
    } else if (op == "query_train") {
      query_train();
    } else if (op == "query_ticket") {
      query_ticket();
    } else if (op == "query_transfer") {
      query_transfer();
    } else if (op == "buy_ticket") {
      buy_ticket();
    } else if (op == "query_order") {
      query_order();
    } else if (op == "refund_ticket") {
      refund_ticket();
    } else if (op == "clean") {
      clean();
    } else if (op == "exit") {
      std::cout << "bye\n";
      return 0;
    }
  }
  return 0;
}