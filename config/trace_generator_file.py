
def get_trace_generator_file(env, tracegen):
    yield from (
        '#ifndef CHAMPSIM_TRACE_GEN_H',
        '#define CHAMPSIM_TRACE_GEN_H',
        '#include <cstdlib>',
        '#include "util.h"',
        'constexpr uint64_t SEED = {seed};'.format(**tracegen),
        'constexpr uint64_t PERCENT_MEMORY = {percent_memory};'.format(**tracegen),
        'constexpr uint64_t PERCENT_LOADS = {percent_loads};'.format(**tracegen),
        '#endif')

