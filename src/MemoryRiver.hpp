#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <cstring>
#include <fstream>

template <class T, int info_len = 2> class MemoryRiver {
private:
  std::fstream file;
  std::string file_name;
  int sizeofT = sizeof(T);

public:
  MemoryRiver() = default;
  MemoryRiver(const std::string &file_name) : file_name(file_name) {}
  void initialise(std::string FN = "") {
    if (FN != "")
      file_name = FN;
    std::fstream file_(file_name);
    if (file_.good()) {
      return;
    }
    file.open(file_name, std::ios::app);
    int tmp = 0;
    for (int i = 0; i < info_len; ++i)
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }
  void get_info(int &tmp, int n) {
    if (n > info_len)
      return;
    file.open(file_name, std::ios::in);
    file.seekg((n - 1) * sizeof(int));
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }
  void write_info(int tmp, int n) {
    if (n > info_len)
      return;
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp((n - 1) * sizeof(int));
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }
  int write(T &t) {
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(0, std::ios::end);
    int index = file.tellp();
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
    return index;
  }
  void update(T &t, const int index) {
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
  }
  void read(T &t, const int index) {
    file.open(file_name, std::ios::in);
    file.seekg(index);
    file.read(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
  }
  void Delete(int index) {
    T tmp;
    std::memset(&tmp, 0, sizeof(tmp));
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&tmp), sizeofT);
    file.close();
  }
  void clean() {
    file.close();
    file.open(file_name, std::ios::trunc); // 加 trunc 清空
  }
};

#endif // BPT_MEMORYRIVER_HPP