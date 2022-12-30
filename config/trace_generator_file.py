
def get_trace_generator_file(env, tracegen):
    yield from (
        '#ifndef CHAMPSIM_TRACE_GEN_H',
        '#define CHAMPSIM_TRACE_GEN_H',
        '#include <cstdlib>',
        '#include "util.h"',
        'constexpr uint64_t SEED = {seed};'.format(**tracegen),
        'constexpr uint64_t PERCENT_MEMORY = {percent_memory};'.format(**tracegen),
        'constexpr uint64_t PERCENT_LOADS = {percent_loads};'.format(**tracegen),
        'constexpr uint64_t SEQUENTIAL_IP_INCREMENT = {sequential_ip_increment};'.format(**tracegen),
        'constexpr uint8_t AVG_BASIC_BLOCK = {average_basic_block};'.format(**tracegen),
        'constexpr uint32_t UNIQUE_INSTR = {unique_instructions};'.format(**tracegen),
        '#endif')

