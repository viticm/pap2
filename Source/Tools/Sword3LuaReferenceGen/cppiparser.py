# -*- coding: utf-8 -*-

import re
import os
import os.path
import sys
import codecs
import miscutil

RE_CLASS_BEGIN = ur'DEFINE_LUA_CLASS_BEGIN\((\w+)\)\s*'
RE_CLASS_END = ur'DEFINE_LUA_CLASS_END\((\w+)\)\s*'
RE_REGISTER_PROPERTY = ur'REGISTER_LUA_(INTEGER|DWORD|STRING|ENUM|BOOL)' \
                     ur'(_READONLY)?\(\w+\s*,\s*(\w+)\s*\)\s*'
RE_REGISTER_FUNCTION = ur'REGISTER_LUA_FUNC\(\w+\s*,\s*(\w+)\s*\)\s*'
TYPE_DICT = {u'INTEGER':u'int', u'DWORD':u'DWORD', u'STRING':u'string', \
          u'ENUM':u'enum', u'BOOL':u'bool'}
TYPE_PREFIX_DICT = {u'INTEGER':u'n', u'DWORD':u'dw', u'STRING':u'sz', \
               u'ENUM':u'n', u'BOOL':u'b'}

class CppiParser:
    def __init__(self, fo):
        self.fo = fo
        self.rClassBegin = re.compile(RE_CLASS_BEGIN, re.UNICODE)
        self.rClassEnd = re.compile(RE_CLASS_END, re.UNICODE)
        self.rRegisterProperty = re.compile(RE_REGISTER_PROPERTY, re.UNICODE)
        self.rRegisterFunction = re.compile(RE_REGISTER_FUNCTION, re.UNICODE)
    def get_all(self):
        result = [{u'obj':u'', u'function':[], u'property':[]}]
        curObj = u''
        for line in self.fo:
            s = u''
            line = line.strip()
            c = self.try_class_begin(line)
            if c:
                curObj = c
                result.append({u'obj':c, u'function':[], u'property':[]})
                continue
            c = self.try_register_function(line)
            if c:
                result = self.add_function(curObj, c, result)
                continue
            c = self.try_register_property(line)
            if c:
                result = self.add_property(curObj, c, result)
                continue
            c = self.try_class_end(line)
            if c:
                curObj = u''
        return result
    def add_function(self, curObj, func, content):
        for i in range(len(content)):
            if content[i][u'obj'] == curObj:
                content[i][u'function'].append(func)
                break
        return content
    def add_property(self, curObj, pty, content):
        for c in content:
            if c[u'obj'] == curObj:
                c[u'property'].append(pty)
                break
        return content
        
    def try_class_begin(self, line):
        m = self.rClassBegin.match(line)
        if m:
            return m.group(1)
        return u''
    def try_class_end(self, line):
        m = self.rClassEnd.match(line)
        if m:
            return True
        return False
    def try_register_function(self, line):
        m = self.rRegisterFunction.match(line)
        if m:
            return m.group(1)
        return None
    def try_register_property(self, line):
        m = self.rRegisterProperty.match(line)
        if m:
            ptyType = TYPE_DICT[m.group(1)]

            if m.group(2):
                rw = u"ro"
            else:
                rw = u"rw"
            ptyName = TYPE_PREFIX_DICT[m.group(1)] + m.group(3)
            return (ptyName, ptyType, rw)
        return None
            
def print_all(m):
    print 'Obj: ', m[u'obj']
    for f in m[u'function']:
        print 'Function: ', f
    for p in m[u'property']:
        print 'Property: ', p
    print '=' * 30

def test():
    fo = codecs.open(sys.argv[1], encoding='cp936')
    cp = CppiParser(fo)
    content = cp.get_all()
    for m in content:
        print 'Obj: ', m[u'obj']
        for f in m[u'function']:
            print 'Function: ', f
        for p in m[u'property']:
            print 'Property: ', p
        print '=' * 30

if __name__ == '__main__':
    test()
