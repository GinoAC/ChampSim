#include <cstdio>
#include <deque>
#include <memory>
#include <string>
#include <variant>

#if defined(__GNUG__) && !defined(__APPLE__)
#include <ext/stdio_filebuf.h>
#endif

namespace detail
{
void pclose_file(FILE* f);
}

#include "instruction.h"

class tracereader
{
public:
  const std::string trace_string;
  tracereader(uint8_t cpu_idx, std::string _ts) : trace_string(_ts), cpu(cpu_idx) {printf("Define base reader\n");}
  tracereader(){}
  virtual ~tracereader() = default;

  virtual ooo_model_instr operator()() = 0;
  bool eof() const;

protected:
  static FILE* get_fptr(std::string fname);

  std::unique_ptr<FILE, decltype(&detail::pclose_file)> fp{get_fptr(trace_string), &detail::pclose_file};
#if defined(__GNUG__) && !defined(__APPLE__)
  __gnu_cxx::stdio_filebuf<char> filebuf{fp.get(), std::ios::in};
#endif

  uint8_t cpu;
  bool eof_ = false;

  constexpr static std::size_t buffer_size = 128;
  constexpr static std::size_t refresh_thresh = 1;
  std::deque<ooo_model_instr> instr_buffer;

  template <typename T>
  void refresh_buffer();

  template <typename T>
  ooo_model_instr impl_get();
};

class tracegenerator : public tracereader
{
public: 
  tracegenerator(uint8_t cpu_idx) : cpu(cpu_idx){ printf("Define trace gen\n"); printf("fp %ld\n", fp == NULL);}
  ~tracegenerator() = default;
  
  template <typename T>
  void refresh_buffer();
protected:
  uint8_t cpu;

  constexpr static std::size_t buffer_size = 128;
  constexpr static std::size_t refresh_thresh = 1;
  std::deque<ooo_model_instr> instr_buffer;

  template <typename T>
  ooo_model_instr impl_get();
 
  template <typename T>
  ooo_model_instr operator()(){return impl_get<T>(); }
  bool eof(){return false;};

  input_instr generate_instr();
}; 

std::unique_ptr<tracereader> get_tracereader(std::string fname, uint8_t cpu, bool is_cloudsuite, bool is_generated);
