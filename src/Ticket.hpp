#pragma once
#include "BPT.hpp"
#include "MemoryRiver.hpp"
#include "Train.hpp"
#include "User.hpp"
#include "map/src/map.hpp"
#include "utils.hpp"
#include "vector/src/vector.hpp"
#include "my_sort.hpp"
#include <climits>
#include <cstring>
#include <string>

enum class TicketStatus { Success, Pending, Refunded };
struct Ticket {
  char username[21] = {};
  char trainID[21] = {};
  char st_station[41] = {}, ed_station[41] = {};
  int amount, price;
  int start_date, arrival_time, leaving_time;
  int ticket_num;
  TicketStatus status;
  Ticket() = default;
  Ticket(const std::string &user_str, const std::string &id,
         const std::string &st, const std::string &ed, int date, int num,
         int leaving, int arrival, int cost, TicketStatus statu, int nu) {
    strncpy(username, user_str.c_str(), 20);
    strncpy(trainID, id.c_str(), 20);
    strncpy(st_station, st.c_str(), 40);
    strncpy(ed_station, ed.c_str(), 40);
    amount = num;
    start_date = date;
    arrival_time = arrival;
    leaving_time = leaving;
    status = statu;
    price = cost;
    ticket_num = nu;
  }
  void debug() {
    std::cout << "username = " << username << '\n';
    std::cout << "trainID = " << trainID << '\n';
    std::cout << "st_station = " << st_station << '\n';
    std::cout << "ed_station = " << ed_station << '\n';
    std::cout << "amount = " << amount << '\n';
    std::cout << "price = " << price << '\n';
    std::cout << "start_date = " << start_date << '\n';
    std::cout << "arrival_time = " << arrival_time << '\n';
    std::cout << "leaving_time = " << leaving_time << '\n';
    std::cout << "ticket_num = " << ticket_num << '\n';
  }
};

class TicketSystem {
private:
  int count = 0;
  BPT<long long, int> ticket_data_pos;
  BPT<long long, int> queue_pos;
  MemoryRiver<Ticket> ticket_data;
  UserSystem &user_sys;
  TrainSystem &train_sys;

public:
  TicketSystem() = delete;
  TicketSystem(UserSystem &user_sys, TrainSystem &train_sys)
      : user_sys(user_sys), train_sys(train_sys),
        ticket_data_pos("TicketDataPos"), queue_pos("QueuePos") {
    ticket_data.initialise("TicketData");
    ticket_data.get_info(count, 1);
  }
  ~TicketSystem() { ticket_data.write_info(count, 1); }
  void buy_ticket(const std::string &user, const std::string &id,
                  const std::string &date, const std::string &st,
                  const std::string &ed, int buy_num, bool flag) {
    sjtu::vector<int> tmp = user_sys.user_data_pos.find(Hash(user), INT_MIN);
    if (tmp.empty()) {
      std::cout << "-1\n";
      return;
    } else if (user_sys.login_stack[Hash(user)] == 0) {
      std::cout << "-1\n";
      return;
    }
    tmp = train_sys.train_data_pos.find(Hash(id), INT_MIN);
    if (tmp.empty()) {
      std::cout << "-1\n";
      return;
    }
    int train_pos = tmp[0];
    Train train;
    train_sys.train_data.read(train, train_pos);
    // train.debug();
    if (train.is_released == false) {
      std::cout << "-1\n";
      return;
    }
    int time_first = train.calc_departure_time(st);
    int time_second = train.calc_arrival_time(ed);
    int delta_date = (train.startTime + time_first) / 1440;
    int start_date = DateToInt(date) - delta_date;
    int start_time = (train.startTime + time_first) % 1440;
    int station1 = 0, station2 = 0;
    int cost = train.calc_cost(st, ed);
    if (start_date < train.saleDate_begin || start_date > train.saleDate_end) {
      std::cout << "-1\n";
      return;
    }
    // std::cout << "time_first = " << time_first << '\n';
    // std::cout << "time_second = " << time_second << '\n';
    // std::cout << "delta_date = " << delta_date << '\n';
    // std::cout << "start_date = " << start_date << '\n';
    // std::cout << "start_time = " << start_time << '\n';
    // std::cout << "cost = " << cost << '\n';
    while (station1 < train.stationNum && train.stations[station1] != st) {
      station1++;
    }
    while (station2 < train.stationNum && train.stations[station2] != ed) {
      station2++;
    }
    if (station1 > station2 || station1 >= train.stationNum ||
        station2 >= train.stationNum) {
      std::cout << "-1\n";
      return;
    }
    int time0 = start_date * 1440 + train.startTime;
    int leaving_time = train.calc_departure_time(st) + time0;
    int arrival_time = train.calc_arrival_time(ed) + time0;
    int empty_seat = train.seatNum;
    // std::cout << "time0 = " << time0 << '\n';
    // std::cout << "leaving_time = " << leaving_time << '\n';
    // std::cout << "arrival_time = " << arrival_time << '\n';
    for (int i = station1; i < station2; i++) {
      empty_seat = std::min(empty_seat, train.seats[start_date][i]);
    }
    // std::cout << "empty_seat = " << empty_seat << '\n';
    if (empty_seat < buy_num && flag == false) {
      std::cout << "-1\n";
      return;
    }
    TicketStatus status;
    if (empty_seat >= buy_num) {
      status = TicketStatus::Success;
      for (int i = station1; i < station2; i++) {
        train.seats[start_date][i] -= buy_num;
      }
      train_sys.train_data.update(train, train_pos);
      std::cout << cost * buy_num << '\n';
    } else {
      status = TicketStatus::Pending;
      std::cout << "queue\n";
    }
    Ticket ticket(user, id, st, ed, start_date, buy_num, leaving_time,
                  arrival_time, cost * buy_num, status, ++count);
    int pos = ticket_data.write(ticket);
    ticket_data_pos.insert(Hash(user), pos);
    if (empty_seat < buy_num) {
      queue_pos.insert(Hash(id + "$" + IntToString(start_date)), pos);
    }
  }
  void query_order(const std::string &user) {
    sjtu::vector<int> tmp = user_sys.user_data_pos.find(Hash(user), INT_MIN);
    if (tmp.empty()) {
      std::cout << "-1\n";
      return;
    } else if (user_sys.login_stack[Hash(user)] == 0) {
      std::cout << "-1\n";
      return;
    }
    sjtu::vector<int> ticket_pos = ticket_data_pos.find(Hash(user), INT_MIN);
    // TODO:
    my::sort(ticket_pos, std::greater<>());
    std::cout << ticket_pos.size() << '\n';
    for (int pos : ticket_pos) {
      Ticket ticket;
      ticket_data.read(ticket, pos);
      if (ticket.status == TicketStatus::Success) {
        std::cout << "[success] ";
      } else if (ticket.status == TicketStatus::Pending) {
        std::cout << "[pending] ";
      } else {
        std::cout << "[refunded] ";
      }
      std::cout << ticket.trainID << ' ' << ticket.st_station << ' '
                << FullTimeToString(ticket.leaving_time) << " -> "
                << ticket.ed_station << ' '
                << FullTimeToString(ticket.arrival_time) << ' ' << ticket.price / ticket.amount
                << ' ' << ticket.amount << '\n';
    }
  }
  int refund_ticket(const std::string &user, int refund_num) {
    sjtu::vector<int> tmp = user_sys.user_data_pos.find(Hash(user), INT_MIN);
    if (tmp.empty()) {
      return -1;
    } else if (user_sys.login_stack[Hash(user)] == 0) {
      return -1;
    }    
    tmp = ticket_data_pos.find(Hash(user), INT_MIN);
    // puts("find ticket");
    if (tmp.size() < refund_num) {
      return -1;
    }
    // TODO:
    my::sort(tmp, std::greater<>());    
    int ticket_pos = tmp[refund_num - 1];
    Ticket ticket;
    ticket_data.read(ticket, ticket_pos);
    // ticket.debug();
    if (ticket.status == TicketStatus::Refunded) {
      return -1;
    } else if (ticket.status == TicketStatus::Pending) {
      queue_pos.remove(Hash((std::string)ticket.trainID + "$" +
                            IntToString(ticket.start_date)),
                       ticket_pos);
    }
    ticket.status = TicketStatus::Refunded;
    ticket_data.update(ticket, ticket_pos);
    // std::cout << "hash = " << Hash(ticket.trainID) << '\n';
    Train train;
    int train_pos = train_sys.train_data_pos.find(Hash(ticket.trainID), INT_MIN)[0];
    train_sys.train_data.read(train, train_pos);
    // train.debug();
    int p = 0;
    while (p < train.stationNum && train.stations[p] != ticket.st_station) {
      p++;
    }
    while (p < train.stationNum && train.stations[p] != ticket.ed_station) {
      train.seats[ticket.start_date][p] += ticket.amount;
      p++;
    }
    // train.debug();
    // puts("aaaaaaaaaaaaaaa");
    // std::cout << "ticket.start_date = " << ticket.start_date << '\n';
    // IntToString(ticket.start_date);
    // exit(0);
    // std::string strrrrr = (std::string)ticket.trainID + "$" + IntToString(ticket.start_date);
    // std::cout << "strrrr = " << strrrrr << '\n';
    // exit(0);
    sjtu::vector<int> pending_queue = queue_pos.find(Hash((std::string)ticket.trainID + "$" + IntToString(ticket.start_date)), INT_MIN);
    // puts("find pending queue");
    // exit(0);

    // TODO:
    my::sort(pending_queue);
    for (int ticket_pos : pending_queue) {
      Ticket ticket;
      ticket_data.read(ticket, ticket_pos);
      std::string st = ticket.st_station, ed = ticket.ed_station;
      int time_first = train.calc_departure_time(st);
      int time_second = train.calc_arrival_time(ed);
      int delta_date = (train.startTime + time_first) / 1440;
      int start_date = ticket.start_date;
      int start_time = (train.startTime + time_first) % 1440;
      int station1 = 0, station2 = 0;
      int cost = train.calc_cost(st, ed);
      while (station1 < train.stationNum && train.stations[station1] != st) {
        station1++;
      }
      while (station2 < train.stationNum && train.stations[station2] != ed) {
        station2++;
      }
      int time0 = start_date * 1440 + train.startTime;
      int leaving_time = train.calc_departure_time(st) + time0;
      int arrival_time = train.calc_arrival_time(ed) + time0;
      int empty_seat = train.seatNum;
      for (int i = station1; i < station2; i++) {
        empty_seat = std::min(empty_seat, train.seats[start_date][i]);
      }
      if (empty_seat < ticket.amount) {
        continue;
      }
      ticket.status = TicketStatus::Success;
      for (int i = station1; i < station2; i++) {
        train.seats[start_date][i] -= ticket.amount;
      }
      ticket_data.update(ticket, ticket_pos);
      queue_pos.remove(
          Hash((std::string)ticket.trainID + "$" + IntToString(start_date)),
          ticket_pos);
    }
    train_sys.train_data.update(train, train_pos);
    return 0 ;
  }
  void clean() {
    count = 0;
    ticket_data_pos.clean();
    queue_pos.clean();
    ticket_data.clean();
  }
};