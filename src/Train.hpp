#pragma once

#include "BPT.hpp"
#include "MemoryRiver.hpp"
#include "map/src/map.hpp"
#include "my_sort.hpp"
#include "utils.hpp"
#include "vector/src/vector.hpp"
#include <cstring>
#include <iostream>

struct Train {
  char trainID[21] = {};
  int stationNum;
  char stations[100][41] = {};
  int seatNum;
  int seats[100][100] = {};
  int prices[100] = {};
  int startTime;
  int travelTimes[100] = {};
  int stopoverTimes[100] = {};
  int saleDate_begin, saleDate_end;
  char type;
  int is_released = 0;
  Train() = default;
  Train(const std::string &id, int sta_num, int seat_num,
        const std::string &station, const std::string &price,
        const std::string &st_time, const std::string &travel_time,
        const std::string &stop_time, const std::string &sale_date, char ty) {
    strncpy(trainID, id.c_str(), 20);
    stationNum = sta_num;
    seatNum = seat_num;
    startTime = TimeToInt(st_time);
    type = ty;
    sjtu::vector<std::string> station_ = split_string(station);
    sjtu::vector<std::string> price_ = split_string(price);
    sjtu::vector<std::string> travel_time_ = split_string(travel_time);
    sjtu::vector<std::string> stop_time_ = split_string(stop_time);
    sjtu::vector<std::string> sale_date_ = split_string(sale_date);
    saleDate_begin = DateToInt(sale_date_[0]);
    saleDate_end = DateToInt(sale_date_[1]);
    for (int i = 0; i < stationNum; i++) {
      strncpy(stations[i], station_[i].c_str(), 40);
    }
    for (int i = 0; i < stationNum - 1; i++) {
      prices[i] = StringToInt(price_[i]);
      travelTimes[i] = StringToInt(travel_time_[i]);
    }
    for (int i = 1; i < stationNum - 1; i++) {
      stopoverTimes[i] = StringToInt(stop_time_[i - 1]);
    }
    for (int i = saleDate_begin; i <= saleDate_end; i++) {
      for (int j = 0; j < stationNum - 1; j++) {
        seats[i][j] = seatNum;
      }
    }
  }
  int calc_arrival_time(const std::string &sta) {
    if (sta == stations[0]) {
      return 0;
    }
    int time = 0;
    for (int i = 1; i < stationNum; i++) {
      time += travelTimes[i - 1];
      if (sta == stations[i]) {
        return time;
      }
      time += stopoverTimes[i];
    }
    return -1;
  }
  int calc_departure_time(const std::string &sta) {
    if (sta == stations[0]) {
      return 0;
    }
    int time = 0;
    for (int i = 1; i < stationNum; i++) {
      time += travelTimes[i - 1];
      time += stopoverTimes[i];
      if (sta == stations[i]) {
        return time;
      }
    }
    return -1;
  }
  int calc_cost(const std::string &st, const std::string &ed) {
    int pos = 0, ans = 0;
    while (pos < stationNum && stations[pos] != st) {
      pos++;
    }
    while (pos < stationNum && stations[pos] != ed) {
      ans += prices[pos++];
    }
    return ans;
  }
  int calc_seat(const std::string &st, const std::string &ed, int start_date) {
    int pos = 0, ans = seatNum;
    while (pos < stationNum && stations[pos] != st) {
      pos++;
    }
    while (pos < stationNum && stations[pos] != ed) {
      ans = std::min(ans, seats[start_date][pos++]);
    }
    return ans;
  }
  void debug() {
    puts("**********debug train**********");
    std::cout << "ID = " << trainID << '\n';
    std::cout << "stationNum = " << stationNum << '\n';
    std::cout << "seatNum = " << seatNum << '\n';
    std::cout << "startTime = " << startTime << '\n';
    std::cout << "saleDate_begin = " << saleDate_begin << '\n';
    std::cout << "saleDate_end = " << saleDate_end << '\n';
    std::cout << "type = " << type << '\n';
    std::cout << "is_released = " << is_released << '\n';
    for (int i = 0; i < stationNum; i++) {
      std::cout << stations[i] << ", travaltime = " << travelTimes[i] << ", prices = " << prices[i] << ", stoptime = " << stopoverTimes[i] << '\n';
    }
    puts("********end debug train********");
  }
  void debug_seat(int date = -1) {
    if (date != -1) {
      std::cout << "date = " << date << ':';
      for (int j = 0; j < stationNum; j++) {
        std::cout << seats[date][j] << ' ';
      }
      puts("");
      return ;
    }
    for (int i = saleDate_begin; i <= saleDate_end; i++) {
      std::cout << "date = " << i << ' ';
      for (int j = 0; j < stationNum; j++) {
        std::cout << seats[i][j] << ' ';
      }
      puts("");
    }
  }
};

class TrainSystem {
friend class TicketSystem;
private:
  int count = 0;
  BPT<long long, int> train_data_pos;
  BPT<long long, int> route_pos;
  BPT<long long, int> station_pos;
  MemoryRiver<Train> train_data;

public:
  TrainSystem() : train_data_pos("TrainDataPos"), 
                  route_pos("RoutePos"), station_pos("StationPos") {
    train_data.initialise("TrainData");
    train_data.get_info(count, 1);
  }
  ~TrainSystem() { train_data.write_info(count, 1); }
  int add_train(const std::string &id, int sta_num, int seat_num,
                const std::string &station, const std::string &price,
                const std::string &st_time, const std::string &travel_time,
                const std::string &stop_time, const std::string &sale_date,
                char ty) {
    long long new_train = Hash(id);
    sjtu::vector<int> tmp = train_data_pos.find(new_train, INT_MIN);
    if (!tmp.empty()) {
      return -1;
    }
    count++;
    Train train(id, sta_num, seat_num, station, price, st_time, travel_time,
                stop_time, sale_date, ty);
    int new_train_pos = train_data.write(train);
    train_data_pos.insert(new_train, new_train_pos);
    return 0;
  }
  int delete_train(const std::string &id) {
    long long train_hash = Hash(id);
    sjtu::vector<int> tmp = train_data_pos.find(train_hash, INT_MIN);
    if (tmp.empty()) {
      return -1;
    }
    int train_pos = tmp[0];
    Train train;
    train_data.read(train, train_pos);
    if (train.is_released) {
      return -1;
    }
    train_data_pos.remove(train_hash, train_pos);
    train_data.Delete(train_pos);
    count--;
    return 0;
  }
  int release_train(const std::string &id) {
    long long train_hash = Hash(id);
    sjtu::vector<int> tmp = train_data_pos.find(train_hash, INT_MIN);
    if (tmp.empty()) {
      return -1;
    }
    int train_pos = tmp[0];
    Train train;
    train_data.read(train, train_pos);
    if (train.is_released) {
      return -1;
    }
    train.is_released = 1;
    train_data.update(train, train_pos);
    for (int i = 0; i < train.stationNum; i++) {
      station_pos.insert(Hash(train.stations[i]), train_pos);
    }
    for (int i = 0; i < train.stationNum; i++) {
      for (int j = i + 1; j < train.stationNum; j++) {
        std::string str = (std::string)(train.stations[i]) + '$' +
                          (std::string)(train.stations[j]);
        route_pos.insert(Hash(str), train_pos);
      }
    }
    return 0;
  }
  void query_train(const std::string &id, const std::string &date) {
    long long train_hash = Hash(id);
    sjtu::vector<int> tmp = train_data_pos.find(train_hash, INT_MIN);
    if (tmp.empty()) {
      std::cout << "-1" << '\n';
      return;
    }
    int train_pos = tmp[0];
    Train train;
    train_data.read(train, train_pos);
    int date_int = DateToInt(date);
    if (date_int < train.saleDate_begin || date_int > train.saleDate_end) {
      std::cout << "-1" << '\n';
      return;      
    }
    std::cout << train.trainID << ' ' << train.type << '\n';
    int time = date_int * 24 * 60 + train.startTime;
    int total_price = 0;
    for (int i = 0; i < train.stationNum; i++) {
      std::string arriving_time = FullTimeToString(time);
      if (i != 0 && i != train.stationNum - 1) {
        time += train.stopoverTimes[i];
      }
      std::string leaving_time = FullTimeToString(time);
      if (i == 0) {
        arriving_time = "xx-xx xx:xx";
      } else if (i == train.stationNum - 1) {
        leaving_time = "xx-xx xx:xx";
      }
      std::cout << train.stations[i] << ' ' << arriving_time << " -> "
                << leaving_time << ' ' << total_price << ' ';
      if (i == train.stationNum - 1) {
        std::cout << "x\n";
      } else {
        std::cout << train.seats[date_int][i] << '\n';
      }
      if (i != train.stationNum - 1) {
        time += train.travelTimes[i];
        total_price += train.prices[i];
      }
    }
  }
  void query_ticket(const std::string &st, const std::string &ed,
                    const std::string &date, bool flag) {
    std::string str = (std::string)(st) + "$" + (std::string)(ed);
    // std::cout << "query " << str << '\n';
    sjtu::vector<int> all_train_pos = route_pos.find(Hash(str), INT_MIN);
    struct route {
      std::string id;
      int leaving_time, arrival_time;
      int price, seat;
      int val;
      bool operator<(const route &b) const {
        return (val == b.val ? id < b.id : val < b.val);
      }
    };
    // std::cout << "st = " << st << '\n';
    // std::cout << "ed = " << ed << '\n';
    sjtu::vector<route> ans;
    for (int pos : all_train_pos) {
      // std::cout << "train_pos = " << pos << '\n';
      Train train;
      train_data.read(train, pos);
      // train.debug();
      // train.debug_seat();
      int time_first = train.calc_departure_time(st);
      int time_second = train.calc_arrival_time(ed);
      int delta_date = (train.startTime + time_first) / 1440;
      int start_date = DateToInt(date) - delta_date;
      int start_time = (train.startTime + time_first) % 1440;
      // std::cout << "time_first = " << time_first << '\n';
      // std::cout << "time_second = " << time_second << '\n';
      // std::cout << "delta_date = " << delta_date << '\n';
      // std::cout << "start_date = " << start_date << '\n';
      // std::cout << "start_time = " << start_time << '\n';
      if (start_date < train.saleDate_begin ||
          start_date > train.saleDate_end) {
        continue;
      }
      route tic;
      tic.id = train.trainID;
      tic.arrival_time = start_date * 1440 + train.startTime + time_second;
      tic.leaving_time = start_date * 1440 + train.startTime + time_first;
      tic.price = train.calc_cost(st, ed);
      tic.seat = train.calc_seat(st, ed, start_date);
      tic.val = (flag ? time_second - time_first : tic.price);

      // std::cout << "id = " << tic.id << '\n';
      // std::cout << "arrival_time = " << tic.arrival_time << '\n';
      // std::cout << "leaving_time = " << tic.leaving_time << '\n';
      // std::cout << "price = " << tic.price << '\n';
      // std::cout << "seat = " << tic.seat << '\n';
      // std::cout << "val = " << tic.val << '\n';

      ans.push_back(tic);
    }
    my::sort(ans);
    std::cout << ans.size() << '\n';
    for (route tic : ans) {
      std::cout << tic.id << ' ' << st << ' '
                << FullTimeToString(tic.leaving_time) << " -> " << ed << ' '
                << FullTimeToString(tic.arrival_time) << ' ' << tic.price << ' '
                << tic.seat << '\n';
    }
  }
  void query_transfer(const std::string &st, const std::string &ed,
                      const std::string &date, bool flag_time) {
    sjtu::vector<int> all_train_pos = station_pos.find(Hash(st), INT_MIN);
    struct transfer {
      std::string mid_station, id1, id2;
      int leaving_time1, arrival_time1, leaving_time2, arrival_time2;
      int price1, seat1, price2, seat2;
      int val1, val2;
      bool operator<(const transfer &b) const  {
        if (val1 != b.val1) {
          return val1 < b.val1;
        } else if (val2 != b.val2) {
          return val2 < b.val2;
        } else if (id1 != b.id1) {
          return id1 < b.id1;
        } else {
          return id2 < b.id2;
        }
      }
    } ans;
    ans.val1 = 2e9;
    // std::cout << "all_train_pos size = " << all_train_pos.size() << '\n';
    for (int pos1 : all_train_pos) {
      Train train1;
      train_data.read(train1, pos1);
      int time1_first = train1.calc_departure_time(st);
      int delta_date1 = (train1.startTime + time1_first) / 1440;
      int start_date1 = DateToInt(date) - delta_date1;
      int start_time1 = (train1.startTime + time1_first) % 1440;
      
      if (start_date1 > train1.saleDate_end) {
        continue;
      } else if (start_date1 < train1.saleDate_begin) {
        continue;
        // start_date1 = train1.saleDate_begin;
      }
      // train1.debug();
      bool flag = 0;
      for (int i = 0; i < train1.stationNum; i++) {
        // std::cout << "i = " << i << " flag = " << flag << '\n';
        if (flag) {
          std::string mid_station = train1.stations[i];
          int time1_second = train1.calc_arrival_time(mid_station);
          std::string str = mid_station + "$" + (std::string)(ed);
          sjtu::vector<int> all_second_train_pos = route_pos.find(Hash(str), INT_MIN);
          // std::cout << "all_second_train_pos size = " << all_second_train_pos.size() << '\n';
          for (int pos2 : all_second_train_pos) {
            if (pos1 == pos2) {
              continue;
            }
            Train train2;
            train_data.read(train2, pos2);
            // train2.debug();
            int mid_time = start_date1 * 1440 + train1.startTime + time1_second;
            int time2_first = train2.calc_departure_time(mid_station);
            int time2_second = train2.calc_arrival_time(ed);
            int start_date2 = (mid_time - time2_first - train2.startTime + 1439) / 1440;
            int start_time2 = (train2.startTime + time2_first) % 1440;

            if (start_date2 > train2.saleDate_end) {
              continue;
            } else if (start_date2 < train2.saleDate_begin) {
              start_date2 = train2.saleDate_begin;
            }
            // train2.debug();
            transfer res;
            res.mid_station = mid_station;
            res.id1 = train1.trainID;
            res.arrival_time1 = start_date1 * 1440 + train1.startTime + time1_second;
            res.leaving_time1 = start_date1 * 1440 + train1.startTime + time1_first;
            res.price1 = train1.calc_cost(st, mid_station);
            res.seat1 = train1.calc_seat(st, mid_station, start_date1);
            res.id2 = train2.trainID;
            res.arrival_time2 = start_date2 * 1440 + train2.startTime + time2_second;
            res.leaving_time2 = start_date2 * 1440 + train2.startTime + time2_first;
            res.price2 = train2.calc_cost(mid_station, ed);
            res.seat2 = train2.calc_seat(mid_station, ed, start_date2);
            res.val1 = res.price1 + res.price2;
            res.val2 = res.arrival_time2 - res.leaving_time1;
            // if (res.arrival_time1 > res.leaving_time2) {
            //   std::cout << "mid_time = " << mid_time << '\n';
            //   std::cout << "time2_first = " << time2_first << '\n';
            //   std::cout << "train2.startTime = " << train2.startTime << '\n';
            //   std::cout << "start_date2 = " << start_date2 << '\n';
            //   std::cout << "start_time2 = " << start_time2 << '\n';
              // std::cout << "time2_first = " << time2_first << '\n';
            // }
            // if (start_date1 == 33) {
            //   std::cout << "start_date1 = " << start_date1 << '\n';
            //   std::cout << ""
            // }
            if (flag_time) {
              std::swap(res.val1, res.val2);
            }
      //       if (res.mid_station == "北京市" && st == "内蒙乌兰浩特市" && ed == "辽宁省大连市") {
      //               std::cout << res.id1 << ' ' << st << ' '
      //           << FullTimeToString(res.leaving_time1) << " -> "
      //           << res.mid_station << ' ' << FullTimeToString(res.arrival_time1)
      //           << ' ' << res.price1 << ' ' << res.seat1 << '\n';
      // std::cout << res.id2 << ' ' << res.mid_station << ' '
      //           << FullTimeToString(ans.leaving_time2) << " -> " << ed << ' '
      //           << FullTimeToString(ans.arrival_time2) << ' ' << res.price2
      //           << ' ' << res.seat2 << '\n';
      //         std::cout << res.val1 << '\n';
      //       }
            if (res < ans) {
              ans = res;
            }
          }
        } else if (train1.stations[i] == st) {
          flag = 1;
        }
      }
    }
    if (ans.val1 == 2e9) {
      std::cout << "0\n";
    } else {
      std::cout << ans.id1 << ' ' << st << ' '
                << FullTimeToString(ans.leaving_time1) << " -> "
                << ans.mid_station << ' ' << FullTimeToString(ans.arrival_time1)
                << ' ' << ans.price1 << ' ' << ans.seat1 << '\n';
      std::cout << ans.id2 << ' ' << ans.mid_station << ' '
                << FullTimeToString(ans.leaving_time2) << " -> " << ed << ' '
                << FullTimeToString(ans.arrival_time2) << ' ' << ans.price2
                << ' ' << ans.seat2 << '\n';
    }
  }
  void clean() {
    count = 0;
    train_data_pos.clean();
    route_pos.clean();
    station_pos.clean();
    train_data.clean();
  }
};