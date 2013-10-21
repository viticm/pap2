# -*- coding: utf-8 -*-

'''
    Author: LI Qing
    Last Update:    2008-1-8
    Description:    本脚本从lua脚本接口文档的源文件（.luai）中提取提示信息，策划会在他们的编辑器中
        使用，这样编辑时可以有提示信息。
'''

import codecs
import os, os.path
import sys
import fnmatch
import getopt
from luaiparser import LuaiParser

LUAI_OBJ_2_CLASS = {
    u"KNpc"   : u"npc",
    u"KPlayer"   : u"player",
    u"KDoodad"   : u"doodad",
    u"KShop"     : u"shop",
    u"KScene"    : u"scene",
    u"KItem"     : u"item",
    u"KSkill"    : u"skill",
    u"KCell"     : u"cell",
    }

DATA_TYPES = (u"b", u"n", u"dw", u"f", u"sz")
OUTPUT_ENCODING = u"GB18030"

def begin_extract(luaiDir, outDir):
    '''开始提取智能提示信息
    luaiDir:    luai文件所在的目录，不会读取子目录，只读取根目录下的文件
    outDir:     输出文件所在的目录
    '''
    luaiList = os.listdir(luaiDir)
    luaiList = filter(lambda luai: os.path.isfile(os.path.join(luaiDir, luai)), luaiList)
    luaiList = fnmatch.filter(luaiList, "*.luai")
    for luai in luaiList:
        fullName = os.path.join(luaiDir, luai)
        convert_luai(fullName, outDir, OUTPUT_ENCODING)

        
def write_function(obj, funcTable, outfo):
    '''向文件内写入一个函数的信息
    obj:    函数所属的对象，如果是全局函数，则obj为空字符串
    funcTable:  LuaiParser所返回的一个列表，包含函数信息
    outfo:      file object, 写入的文件
    '''
    bFirstLine = True
    for func in funcTable[u'value']:
        if bFirstLine:
            bFirstLine = False
            outfo.write(LUAI_OBJ_2_CLASS.get(obj, u''))
            outfo.write(u'\t')      #class col
            outfo.write(u'\t')      #return value, unable to provide now
            outfo.write(u'\t')      #empf col, unknown
        else:
            outfo.write(u'\t\t\t')
        outfo.write(funcTable[u'value'][0])     
        outfo.write(u'\t')  #function signature
        outfo.write(funcTable[u'name'])
        outfo.write(u'\t')      #function name
        outfo.write(funcTable[u'catalog'])
        outfo.write(u'\t')      #function catalog
        outfo.write(u'\t')      #type col
        outfo.write(funcTable[u'summary'])
        outfo.write(u'\t')      #summary
        outfo.write(u'\n')
        
def write_property(obj, ptyTable, outfo):
    '''向文件内写入一个属性的信息
    obj:    属性所属的对象，如果是全局属性，则obj为空字符串
    ptyTable:  LuaiParser所返回的一个列表，包含属性信息
    outfo:      file object, 写入的文件
    '''
    line = [LUAI_OBJ_2_CLASS.get(obj, u''), u'', u'', 
            ptyTable[u'name'], ptyTable[u'name'], ptyTable[u'catalog'], u'',
            ptyTable[u'summary']
           ]
    line = u'\t'.join(line)
    outfo.write(line)
    outfo.write(u'\n')

def write_title(outfo):
    '''向输出文件内写入表格的标题行
    '''
    titles = (u'class', u'returnclass', u'empf', u'name', u'word', u'suffix', u'type', u'info', u'regex')
    line = u'\t'.join(titles)
    outfo.write(line)
    outfo.write(u'\n')
        
def convert_luai(luai, outDir, outEncoding):
    '''提取一个Luai文件的内容
    luai:   luai文件的路径
    outDir: 输出的目录
    outEncoding:    字符串，输出文件的编码
    '''
    print 'converting: %s' % (luai, )
    luaiBaseName = os.path.basename(luai)
    luaiBaseName = os.path.splitext(luaiBaseName)[0];
    fo = codecs.open(luai, u"r", u"utf8")
    parser = LuaiParser(fo)
    header = parser.get_headerBlock()
    outName = make_outfile_name(header["obj"], luaiBaseName)
    outName = os.path.join(outDir, outName)
    outfo = codecs.open(outName, u'w', outEncoding)
    write_title(outfo)
    obj = header[u'obj']
    blk = parser.get_bodyBlock()
    while blk != None:
        if blk[u'type'] == u'function':
            write_function(obj, blk, outfo)
        elif blk[u'type'] == u'property':
            write_property(obj, blk, outfo)
        blk = parser.get_bodyBlock()
    fo.close()
    outfo.close()
            
        
    
    
def make_outfile_name(obj, defaultName):
    return LUAI_OBJ_2_CLASS.get(obj, defaultName) + u'.tab'
    
def usage():
    print u'''Usage: code_list.py srcDir outputDir
    '''
if __name__ == '__main__':
    if len(sys.argv) != 3:
        usage()
        sys.exit(1)
    begin_extract(sys.argv[1], sys.argv[2])
    