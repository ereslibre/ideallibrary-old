#! /usr/bin/env python
#
# This file is part of the Ideal libraries
# Copyright (C) 2009 Rafael Fernández López <ereslibre@ereslibre.es>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.
#

import UnitTest, Utils, Options

LIBVERSION = '0.0.8'

VERSION    = 'v' + LIBVERSION
APPNAME    = 'ideallibrary'
srcdir     = '.'
blddir     = 'build'

subdirs_d  = 'src/core src/gui src/modules src/tests'
subdirs_r  = 'src/core src/gui src/modules'

checkCompilerFeatures = '''struct A {}; struct B {};
                           template <typename... Type>
                           struct C : public Type... {
                               C(const Type&... obj) : Type(obj)... {}
                           };
                           int main(int argc, char **argv)
                           {
                               A a; B b;
                               C<A, B> *c = new C<A, B>(a, b);
                               delete c;
                               return 0;
                           }'''

def init():
    pass

def set_options(opt):
    opt.tool_options('compiler_cxx')
    opt.add_option('--release', action = 'store_true', default = False,
                   help = 'Do not build unit tests. Compile without debug information')

def configure(conf):
    # check for basics
    conf.find_program('pkg-config')
    conf.check_tool('compiler_cxx')
    if conf.env['COMPILER_CXX'] == []:
        conf.fatal('A C++ compiler is needed. Please, install it and try again')
    conf.env['CXXFLAGS_CONFTESTS'] = ['-std=c++0x'];
    conf.check(fragment = checkCompilerFeatures,
               msg = 'Checking whether ' + conf.env['COMPILER_CXX'] + ' supports C++0x',
               uselib = 'CONFTESTS',
               mandatory = 1)
    if Options.options.release:
        conf.sub_config(subdirs_r)
    else:
        conf.sub_config(subdirs_d)
    # set environment
    conf.env['RELEASE'] = Options.options.release
    conf.env['CXXFLAGS'] += ['-std=c++0x', '-fvisibility=hidden']
    # write config file
    conf.define('IDEALLIBRARY_PREFIX', conf.env['PREFIX'])
    conf.define('IDEALLIBRARY_VERSION', VERSION)
    if Options.options.release:
        conf.define('NDEBUG', 1)
    else:
        conf.undefine('NDEBUG')
    # uselib stuff
    conf.env['RPATH_IDEAL'] = conf.env['PREFIX'] + '/lib'
    if Options.options.release:
        print '*** Going to compile in RELEASE mode'
        conf.env['CXXFLAGS'] += ['-O2', '-w']
    else:
        print '*** Going to compile in DEBUG mode (default)'
        print '*** To compile in RELEASE mode call "waf configure --release [more-options]"'
        conf.env['CXXFLAGS'] += ['-O', '-g', '-Wall', '-Werror']
    conf.write_config_header('src/ideal_conf.h')

def build(bld):
    bld.env['LIBVERSION'] = LIBVERSION
    bld.install_files('${PREFIX}/include/ideal', 'src/ideal_conf.h')
    bld.install_files('${PREFIX}/include/ideal', 'src/ideal_export.h')
    if bld.env['RELEASE']:
        bld.add_subdirs(subdirs_r)
    else:
        bld.add_subdirs(subdirs_d)

def check(context):
    ut = UnitTest.unit_test()
    ut.run()
    ut.print_results()

def shutdown():
    if Options.commands['install']:
        print ''
        Utils.pprint('RED', '*** Running unit tests')
        print ''
        ut = UnitTest.unit_test()
        ut.run_if_waf_does = 'install'
        ut.run()
        ut.print_results()
