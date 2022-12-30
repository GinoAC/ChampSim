//This file performs trace generation for the tracegenerator class
//the tracegenerator's interface with ChampSim is defined in tracereader.cc

#include "tracereader.h"
#include "trace_gen_constants.h"

#include <algorithm>
#include <cstring>
#include <string>

input_instr tracegenerator::generate_instr(){
  input_instr instr;

  std::tie(instr.ip, instr.is_branch) = get_ip();

  instr.branch_taken = 0;

  add_registers(instr);
  add_memory_accesses(instr);
  
  return instr;
}

std::pair<uint64_t, uint8_t> tracegenerator::get_ip(){
  uint64_t ip = last_ip;
  uint8_t is_br = 0;

  //First IP generated
  if(ip == 0){
    ip = create_bb();
    trace_starting_ip = ip;
  //Create the next BB    
  }else if(current_bb_size == target_bb){

    if(bb_cfg[base_bb_ip].next_bb != 0){
      ip = bb_cfg[base_bb_ip].next_bb;
      base_bb_ip = ip;

      current_bb_size = 0;
      target_bb = bb_cfg[base_bb_ip].basic_block_size;
 
    //Check if the basic block has another BB it points to already
    }else if(bb_cfg[base_bb_ip].next_bb == 0 && num_instr < UNIQUE_INSTR){

      ip = create_bb();

    }else{

      bb_cfg[base_bb_ip].next_bb = trace_starting_ip;
      ip = trace_starting_ip;
      base_bb_ip = ip;
      current_bb_size = 0;
      target_bb = bb_cfg[trace_starting_ip].basic_block_size;

    }

  }else{
    ip += SEQUENTIAL_IP_INCREMENT;
  }

  current_bb_size += 1;
  num_instr += 1;

  //If this is the end of the basic block make it a branch
  is_br = (current_bb_size == (target_bb));

  last_ip = ip;
  return {ip, (uint8_t)is_br};
}

uint64_t tracegenerator::create_bb(){
  
  uint64_t ip = rand();

  if(base_bb_ip != 0){
    bb_cfg[base_bb_ip].next_bb = ip;
  }

  target_bb = rand()/(RAND_MAX/AVG_BASIC_BLOCK) + 1;

  //Add the new basic block to the CFG
  bb_def bb{ip, target_bb, 0};
  bb_cfg[ip] = bb;

  base_bb_ip = ip;

  current_bb_size = 0;

  return ip;
}

void tracegenerator::add_registers(input_instr &instr){
  for(uint64_t a = 0; a < NUM_INSTR_DESTINATIONS; a++){
    instr.destination_registers[a] = 0;
  } 

  for(uint64_t a = 0; a < NUM_INSTR_SOURCES; a++){
    instr.source_registers[a] = 0;
  } 
  if(instr.is_branch){
    instr.destination_registers[0] = champsim::REG_INSTRUCTION_POINTER;
  }
}

void tracegenerator::add_memory_accesses(input_instr &instr){
  float is_memory = static_cast<float>(rand()/(RAND_MAX/100));

  for(uint64_t a = 0; a < NUM_INSTR_DESTINATIONS; a++){
    instr.destination_memory[a] = 0;
  } 

  for(uint64_t a = 0; a < NUM_INSTR_SOURCES; a++){
    instr.source_memory[a] = 0;
  } 
}
