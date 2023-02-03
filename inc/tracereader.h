/*
 *    Copyright 2023 The ChampSim Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRACEREADER_H
#define TRACEREADER_H

#include <cstdio>
#include <deque>
#include <memory>
#include <string>
#include <variant>
#include <stdlib.h>
#include <map>

#if defined(__GNUG__) && !defined(__APPLE__)
#include <ext/stdio_filebuf.h>
#endif

namespace detail
{
void pclose_file(FILE* f);
}

#include "instruction.h"
#include "trace_gen_constants.h"

class tracereader
{
  static uint64_t instr_unique_id;

public:
  const std::string trace_string;
  tracereader(uint8_t cpu_idx, std::string _ts) : trace_string(_ts), cpu(cpu_idx) {}
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
  tracegenerator(uint8_t cpu_idx) : cpu(cpu_idx){srand(SEED);}
  ~tracegenerator() = default;
  
  template <typename T>
  void refresh_buffer();
protected:

  uint8_t cpu;

  //Define the basic blocks previously seen to autogenerate
  //a repeating instruction flow
  struct bb_def{
    uint64_t starting_ip;
    uint64_t basic_block_size;
    uint64_t next_bb;
  };

  std::map<uint64_t, bb_def> bb_cfg;

  uint8_t current_bb_size = 0;
  uint64_t num_instr, base_bb_ip, target_bb = 0;
  uint64_t last_ip = 0;
  uint64_t trace_starting_ip = 0;

  constexpr static std::size_t buffer_size = 128;
  constexpr static std::size_t refresh_thresh = 1;
  std::deque<ooo_model_instr> instr_buffer;

  template <typename T>
  ooo_model_instr impl_get();
 
  template <typename T>
  ooo_model_instr operator()(){return impl_get<T>(); }
  bool eof(){return false;};

  input_instr generate_instr();
  uint64_t create_bb();
  std::pair<uint64_t, uint8_t> get_ip();

  void add_memory_accesses(input_instr &instr);
  void add_registers(input_instr &instr);
}; 

std::unique_ptr<tracereader> get_tracereader(std::string fname, uint8_t cpu, bool is_cloudsuite, bool is_generated);

#endif
