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
    saleDate_end = DateToInt(sale_date_[0]);
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
      time += travelTimes[i];
      if (sta == stations[i]) {
        return time;
      }
      time += stopoverTimes[i - 1];
    }
    return -1;
  }
  int calc_departure_time(const std::string &sta) {
    if (sta == stations[0]) {
      return 0;
    }
    int time = 0;
    for (int i = 1; i < stationNum; i++) {
      time += travelTimes[i];
      time += stopoverTimes[i - 1];
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
    int pos = 0, ans = 0;
    while (pos < stationNum && stations[pos] != st) {
      pos++;
    }
    while (pos < stationNum && stations[pos] != ed) {
      ans += seats[start_date][pos++];
    }
    return ans;
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
  TrainSystem()
      : train_data_pos("TrainDataPos"), route_pos("RoutePos"),
        station_pos("StationPos") {
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
      station_pos.insert(Hash(train.stations[i]), train_hash);
    }
    for (int i = 0; i < train.stationNum; i++) {
      for (int j = i + 1; j < train.stationNum; j++) {
        std::string str = (std::string)(train.stations[i]) + '$' +
                          (std::string)(train.stations[j]);
        route_pos.insert(Hash(str), train_hash);
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
    std::cout << train.trainID << ' ' << train.type << '\n';
    int date_int = DateToInt(date);
    int time = date_int * 24 * 60 + train.startTime;
    int total_price = 0;
    for (int i = 0; i < train.stationNum; i++) {
      std::string arriving_time = FullTimeToString(time);
      if (i != 0 && i != train.stationNum - 1) {
        time += train.stopoverTimes[i - 1];
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
        std::cout << train.seats[date_int][i];
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
    sjtu::vector<route> ans;
    for (int pos : all_train_pos) {
      Train train;
      train_data.read(train, pos);
      int time_first = train.calc_departure_time(st);
      int time_second = train.calc_arrival_time(ed);
      int delta_date = (train.startTime + time_first) / 1440;
      int start_date = DateToInt(date) - delta_date;
      int start_time = (train.startTime + time_first) % 1440;
      if (start_date < train.saleDate_begin ||
          start_date > train.saleDate_end) {
        continue;
      }
      route tic;
      tic.id = train.trainID;
      tic.arrival_time = start_date * 1440 + start_time + time_second;
      tic.leaving_time = start_date * 1440 + start_time + time_first;
      tic.price = train.calc_cost(st, ed);
      tic.seat = train.calc_seat(st, ed, start_date);
      tic.val = (flag ? time_second - time_first : tic.price);
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
                      const std::string &date, bool flag) {
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
    for (int pos1 : all_train_pos) {
      Train train1;
      train_data.read(train1, pos1);
      int time1_first = train1.calc_departure_time(st);
      int delta_date1 = (train1.startTime + time1_first) / 1440;
      int start_date1 = DateToInt(date) - delta_date1;
      int start_time1 = (train1.startTime + time1_first) % 1440;
      if (start_date1 < train1.saleDate_begin ||
          start_date1 > train1.saleDate_end) {
        continue;
      }
      bool flag = 0;
      for (int i = 0; i < train1.stationNum; i++) {
        if (flag) {
          std::string mid_station = train1.stations[i];
          int time1_second = train1.calc_arrival_time(mid_station);
          std::string str = mid_station + "$" + (std::string)(ed);
          sjtu::vector<int> all_second_train_pos = route_pos.find(Hash(str), INT_MIN);
          for (int pos2 : all_second_train_pos) {
            if (pos1 == pos2) {
              continue;
            }
            Train train2;
            train_data.read(train2, pos2);
            int mid_time = start_date1 * 1440 + start_time1 + time1_first;
            int time2_first = train2.calc_departure_time(mid_station);
            int time2_second = train2.calc_arrival_time(ed);
            int start_date2 = (mid_time - time2_first - train2.startTime) / 1440;
            int start_time2 = (train2.startTime + time2_first) % 1440;
            if (start_date2 < train2.saleDate_begin || start_date2 > train2.saleDate_end) {
              continue;
            }
            transfer res;
            res.mid_station = mid_station;
            res.id1 = train1.trainID;
            res.arrival_time1 = start_date1 * 1440 + start_time1 + time1_second;
            res.leaving_time1 = start_date1 * 1440 + start_time1 + time1_first;
            res.price1 = train1.calc_cost(st, mid_station);
            res.seat1 = train1.calc_seat(st, mid_station, start_date1);
            res.id2 = train2.trainID;
            res.arrival_time2 = start_date2 * 1440 + start_time2 + time2_second;
            res.leaving_time2 = start_date2 * 1440 + start_time2 + time2_first;
            res.price2 = train2.calc_cost(mid_station, ed);
            res.seat2 = train2.calc_seat(mid_station, ed, start_date2);
            res.val1 = res.price1 + res.price2;
            res.val2 = res.arrival_time2 - res.leaving_time1;
            if (flag) {
              std::swap(res.val1, res.val2);
            }
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