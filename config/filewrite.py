import difflib
import hashlib
import itertools
import operator
import os
import json

from . import makefile
from . import instantiation_file
from . import constants_file
from . import trace_generator_file
from . import modules
from . import util

constants_file_name = 'champsim_constants.h'
trace_generator_file_name = 'trace_gen.h'
instantiation_file_name = 'core_inst.inc'
core_modules_file_name = 'ooo_cpu_modules.inc'
cache_modules_file_name = 'cache_modules.inc'
makefile_file_name = '_configuration.mk'

cxx_generated_warning = ('/***', ' * THIS FILE IS AUTOMATICALLY GENERATED', ' * Do not edit this file. It will be overwritten when the configure script is run.', ' ***/', '')
make_generated_warning = ('###', '# THIS FILE IS AUTOMATICALLY GENERATED', '# Do not edit this file. It will be overwritten when the configure script is run.', '###', '')

def write_if_different(fname, new_file_string):
    ratio = 0
    if os.path.exists(fname):
        with open(fname, 'rt') as rfp:
            f = list(l.strip() for l in rfp)
        new_file_lines = list(l.strip() for l in new_file_string.splitlines())
        ratio = difflib.SequenceMatcher(a=f, b=new_file_lines).ratio()

    if ratio < 1:
        with open(fname, 'wt') as wfp:
            wfp.write(new_file_string)

def get_map_lines(fname_map):
    yield from ('#define {} {}'.format(*x) for x in fname_map.items())

def get_files(executable, elements, module_info, config_file, env, bindir_name, srcdir_names, objdir_name):
    build_id = hashlib.shake_128(json.dumps((executable, elements, module_info, config_file, env)).encode('utf-8')).hexdigest(4)

    inc_dir = os.path.normpath(os.path.join(objdir_name, build_id, 'inc'))

    yield os.path.join(inc_dir, instantiation_file_name), instantiation_file.get_instantiation_lines(**elements) # Instantiation file
    yield os.path.join(inc_dir, core_modules_file_name), modules.get_branch_lines(module_info['branch']) # Core modules file
    yield os.path.join(inc_dir, core_modules_file_name), modules.get_btb_lines(module_info['btb'])
    yield os.path.join(inc_dir, cache_modules_file_name), modules.get_repl_lines(module_info['repl']) # Cache modules file
    yield os.path.join(inc_dir, cache_modules_file_name), modules.get_pref_lines(module_info['pref'])
    yield os.path.join(inc_dir, constants_file_name), constants_file.get_constants_file(config_file, elements['pmem']) # Constants header
    yield os.path.join(inc_dir, trace_generator_file_name), trace_generator_file.get_trace_generator_file(config_file, elements["trace_generator"]) # Constants header

    joined_module_info = util.chain(*module_info.values()) # remove module type tag
    yield from ((os.path.join(inc_dir, m['name'] + '.inc'), get_map_lines(m['func_map'])) for m in joined_module_info.values())
    yield makefile_file_name, makefile.get_makefile_lines(objdir_name, build_id, os.path.normpath(os.path.join(bindir_name, executable)), srcdir_names, joined_module_info, env)

def write_files(iterable):
    fileparts = itertools.chain(*itertools.starmap(get_files, iterable))
    for fname, fcontents in itertools.groupby(sorted(fileparts, key=operator.itemgetter(0)), key=operator.itemgetter(0)):
        os.makedirs(os.path.abspath(os.path.dirname(fname)), exist_ok=True)
        if os.path.splitext(fname)[1] in ('.cc', '.h', '.inc'):
            contents_with_header = itertools.chain(cxx_generated_warning, *(f[1] for f in fcontents))
        elif os.path.splitext(fname)[1] in ('.mk',):
            contents_with_header = itertools.chain(make_generated_warning, *(f[1] for f in fcontents))
        else:
            contents_with_header = itertools.chain.from_iterable(f[1] for f in fcontents) # no header

        write_if_different(fname, '\n'.join(contents_with_header))

