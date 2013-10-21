import re
import codecs
import ConfigParser
import miscutil
import luaiparser
import cppiparser

class LoadConfig:
    def __init__(self, fname):
        self.fo = miscutil.open_utf8(fname)
        self.cp = ConfigParser.SafeConfigParser()
        self.cp.readfp(self.fo)
    def load(self):
        r = re.compile(u'(check_\d+)_cpp', re.U | re.I)
        result = []
        for opt in self.cp.options(u'check'):
            m = r.match(opt)
            if m:
                cpp = self.cp.get(u'check', opt)
                luai = self.cp.get(u'check', m.group(1) + u'_luai')
                result.append((luai, cpp))
        self.close()
        return result
    def close(self):
        try:
            self.fo.close()
        except:
            pass

class Log:
    def __init__(self):
       self.fo = miscutil.open_utf8(u'errlog.log', 'w', True)
    def writeLine(self, msg):
        if not msg.endswith(u'\r\n'):
            msg += u'\r\n'
        self.fo.write(msg)
    def close(self):
        self.fo.close()

def find_luai_in_cpp(luaiItem, cppContent):
    idx = 0
    curType = luaiItem[u'type']
    for cppItem in cppContent[curType]:
        if curType == u'property' and cppItem[0] == luaiItem[u'name']:
            return True, idx
        elif curType == u'function' and cppItem == luaiItem[u'name']:
            return True, idx
        idx += 1
    return False, -1
    
def find_cpp_function_in_luai(cppItem, luaiContent):
    idx = 0
    for luaiItem in luaiContent:
        if luaiItem[u'name'] == cppItem:
            return True, idx
        idx += 1
    return False, -1

def find_cpp_property_in_luai(cppItem, luaiContent):
    idx = 0
    for luaiItem in luaiContent:
        if luaiItem[u'name'] == cppItem[0] and luaiItem[u'type'] == u'property':
            return True, idx
        idx += 1
    return False, -1

def compare_luai_cpp(luaiContent, cppContent, log):
    luaiObj = luaiContent[0][u'obj']
    c = None
    idx = -1
    for i in range(len(cppContent)):
        if cppContent[i][u'obj'] == luaiObj:
            idx = i
            break
    if idx == -1:
        log.writeLine(u'luai and cpp have nothing in common.')
        return
    luaiContent = luaiContent[1:]
    cppContent = cppContent[idx]
    i = 0
    while True:
        if i == len(luaiContent):
            break
        ret, index = find_luai_in_cpp(luaiContent[i], cppContent)
        if ret:
            curType = luaiContent[i][u'type']
            del cppContent[curType][index]
            del luaiContent[i]
            continue
        else:
            log.writeLine(u'%s %s.%s\t>\tcpp' % (luaiContent[i][u'type'], \
                luaiObj, luaiContent[i][u'name']))
        i += 1
    i = 0
    while True:
        if i == len(cppContent[u'function']):
            break
        ret, index = find_cpp_function_in_luai(cppContent[u'function'][i],\
             luaiContent)
        if ret:
            del cppContent[u'function'][index]
            del luaiContent[i]
            continue
        else:
            log.writeLine(u'luai < function %s.%s' % (luaiObj,
                cppContent[u'function'][i]))
        i += 1
    i = 0
    while True:
        if i == len(cppContent[u'property']):
            break
        if cppContent[u'property'][i][0] == u'nStrength':
            print 'property:', i
        ret, index = find_cpp_property_in_luai(cppContent[u'property'][i],\
             luaiContent)
        if ret:
            del cppContent[u'property'][index]
            del luaiContent[i]
            continue
        else:
            log.writeLine(u'luai < property %s.%s' % (luaiObj,
                cppContent[u'property'][i][0]))
        i += 1
            
def main():
    config = LoadConfig(u'check.ini')
    errLog = Log()
    for luai, cpp in config.load():
        print luai
        errLog.writeLine(u'luai: %s' % luai)
        errLog.writeLine(u'cpp: %s' % cpp)
        luaifo = miscutil.open_utf8(luai)
        cppfo = codecs.open(cpp, encoding='cp936')
        luaip = luaiparser.LuaiParser(luaifo)
        cppip = cppiparser.CppiParser(cppfo)
        luaiContent = [luaip.get_headerBlock()]
        block = luaip.get_bodyBlock()
        while block:
            luaiContent.append(block)
            block = luaip.get_bodyBlock()
        cppContent = cppip.get_all()
        compare_luai_cpp(luaiContent, cppContent, errLog)
        
        luaifo.close()
        cppfo.close()
        errLog.writeLine(u'=' * 30)
        errLog.writeLine(u'')

if __name__ == '__main__':
    main()