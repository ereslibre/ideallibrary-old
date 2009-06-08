#! /usr/bin/env python
# Rafael Fernández López, 2009

import UnitTest, Utils, Options

VERSION    = 'v0.0.3'
LIBVERSION = '0.0.3'
APPNAME    = 'ideallibrary'
srcdir     = '.'
blddir     = 'build'

subdirs_d  = 'core gui modules tests'
subdirs_r  = 'core gui modules'

checkCompilerFeatures = '''#include <mutex>
                           struct A {}; struct B {};
                           template <typename... Type>
                           struct C : public Type... {
                               C(const Type&... obj) : Type(obj)... {}
                           };
                           int main(int argc, char **argv)
                           {
                               std::mutex myMutex;
                               std::lock_guard<std::mutex> lk(myMutex);
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
    if Options.options.release:
        print '*** Going to compile in RELEASE mode'
        conf.env['CXXFLAGS'] += ['-O2', '-w']
    else:
        print '*** Going to compile in DEBUG mode (default)'
        print '*** To compile in RELEASE mode call "waf configure --release [more-options]"'
        conf.env['CXXFLAGS'] += ['-O', '-g', '-Wall', '-Werror']
    # write config file
    conf.define('IDEALLIBRARY_PREFIX', conf.env['PREFIX'])
    conf.define('IDEALLIBRARY_VERSION', VERSION)
    if Options.options.release:
        conf.define('NDEBUG', 1)
    else:
        conf.undefine('NDEBUG')
    conf.write_config_header('ideal_conf.h')
    # uselib stuff
    conf.env['RPATH_IDEAL'] = conf.env['PREFIX'] + '/lib'

def build(bld):
    bld.env['LIBVERSION'] = LIBVERSION
    if bld.env['RELEASE']:
        bld.add_subdirs(subdirs_r)
    else:
        bld.add_subdirs(subdirs_d)
    bld.install_files('${PREFIX}/include/ideal', 'ideal_conf.h')
    bld.install_files('${PREFIX}/include/ideal', 'ideal_export.h')

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
