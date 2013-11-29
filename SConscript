
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
    libmmgalera_objs.append("/def:galera/src/galera_smm.def")
    env.SharedLibrary('galera_smm', libmmgalera_objs, SHLIBSUFFIX='.dll')
elif sysname == 'darwin':
    env.SharedLibrary('galera_smm', libmmgalera_objs, SHLIBSUFFIX='.so')
else:
    env.SharedLibrary('galera_smm', libmmgalera_objs)
