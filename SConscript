
import sys

SConscript(['galerautils/SConscript',
        'gcache/SConscript',
        'gcomm/SConscript',
        'gcs/SConscript',
        'galera/SConscript',
        'garb/SConscript'])

Import('env', 'sysname')

libmmgalera_objs = env['LIBGALERA_OBJS']
libmmgalera_objs.extend(env['LIBMMGALERA_OBJS'])

if sysname == 'windows':
    env.SharedLibrary('galera_smm-2.23.7', libmmgalera_objs, SHLIBSUFFIX='.dll',WIN32DEFPREFIX="galera/src/galera_smm.def")
elif sysname == 'darwin':
    env.SharedLibrary('galera_smm', libmmgalera_objs, SHLIBSUFFIX='.so')
else:
    env.SharedLibrary('galera_smm', libmmgalera_objs)
