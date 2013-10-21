# -*- coding: cp936 -*-

import os, os.path, re, codecs, shelve, sys, time, shutil
from luaiparser import LuaiParser
from strAll import strAll
from fnmatch import fnmatch
import HtmlWriter

SRC_DIR = ur'.\src'
DST_DIR_UTF8 = ur'.\html_utf8'
DST_DIR_GB2312 = ur'.\html_gb2312'
TEMP_DIR = ur'.\temp'
SEE_DB_FILE = u'see.db'
SEE_DB = None
GET_DB_FILE = u'get.db'
GET_DB = None
TOC_DB_FILE = u'toc.db'
TOC_DB = None
CHM_IDX_DB_FILE = u'chmidx.db'
CHM_IDX_DB = None
TRAN_DICT = {u'':u'', u'function':u'Function', u'property':u'Property'}
HTML_STYLE = ur'''<!--
    body    {margin-left:5%; margin-right:5%}
    .catalogIndexTable  {border:0px; border-collapse:collapse; width:80%}
    .catalogIndexTableHead {border:0px; font-weight:bold; color:blue;
        width:30%}
    .catalogIndexTableBodyTd   {border-bottom:outset, 1px; width:25%}
    .catalogBlockTable  {border:1; border-collapse:collapse; width:80%}
    .catalogBlockTableHead  {border:1; font-weight:bold; color:blue;
        width:30%}
    .catalogBlockTableBody  {border-top:inset 2; padding:2px}
    .blockTable {border-collapse:collapse; width:95%}
    .blockHead  {font-weight:normal; color:blue; }
    .blockPrototype {font-weight:bold; color:blue; font-size:large}
    .blockTableHead {width:50%}
    .blockTableHeadTd2  {text-align:right}
    .blockTitle {font-weight:bold}
    .blockBody  {}
    .getTable   {border:ridge black 1px; border-collapse:collapse}
    .getTableHead   {border:solid black 1px; border-collapse:collapse;
            background-color:#9595CA; padding:3px}
    .getTableColTitle   {border:solid black 1px; border-collapse:collapse;
        color:green; padding:3px}
    .getTableBody {border:solid black 1px; border-collapse:collapse;
        padding:3px}
    .contentTable {border:ridge black 1px; border-collapse:collapse}
    .contentTableHead   {border:solid black 1px; border-collapse:collapse;
            background-color:#9595CA; padding:3px}
    .contentTableColTitle   {border:solid black 1px; border-collapse:collapse;
        font-weight:bold; background-color:#9595CA; padding:3px}
    .contentTableBody {border:solid black 1px; border-collapse:collapse;
        padding:3px}
    .refLinks   {font-size:small; text-align:right}
-->'''

def open_utf8(filename, mode='r'):
    f = codecs.open(filename, mode, encoding='utf-8')
    if f.read(1) != unicode(codecs.BOM_UTF8, 'utf-8'):
        f.seek(0)
    return f


def make_fileName(fileBaseName=u'', obj=u'', subType=u'', fileType=u''):
    if obj:
        f = obj
    else:
        f = fileBaseName
    if subType and obj:
        f = u'%s-%s' % (f, subType)
    if fileType:
        f = u'%s-%s' % (f, fileType)
    return f + u'.html'

def make_linkBaseName(name, subType):
    return u'%s-%s' % (subType, name)


def make_linkFullName(fileBaseName, name, obj=u'', subType=u'',
                      fileType=u''):
    return u'%s#%s' %(make_fileName(fileBaseName, obj, subType, fileType), \
                      make_linkBaseName(name, subType))
class GetDb:
    def __init__(self, dbName=None):
        if not dbName:
            self.db = {}
        else:
            self.db = shelve.open(dbName)
    def insert(self, block):
        if block[u'type'] == u'enum':
            self.insert_enum(block)
        elif block[u'type'] == u'table':
            self.insert_table(block)
    def insert_enum(self, block):
        name = block[u'name']
        value = block[u'value']
        s = u'''<table class="getTable">
        <tr><td class="getTableHead" colspan="2">%s</td></tr>
        ''' % name
        s += u'''<tr><td class="getTableHead" colspan="2">%s</td></tr>\n''' % \
             block[u'summary']
        s += u'''<tr>\n<td class="getTableBody">Meaning</td>
            <td class="getTableBody">Value</td></tr>\n'''
        for i in value:
            s += u'''<tr>
                <td class="getTableBody">%s</td>
                <td class="getTableBody">%s</td></tr>\n''' % (i[0], i[1])
        s += u'</table>\n'
        self.db[name.encode('utf-8')] = s
    def insert_table(self, block):
        s = u'''<table class="getTable">\n'''
        s += u'''<tr><td class="getTableHead" colspan="%s">%s</td></tr>\n''' % \
             (block[u'colnum'], block[u'name'])
        s += u'''<tr><td class="getTableHead" colspan="%s">%s</td></tr>\n''' % \
             (block[u'colnum'], block[u'summary'])
        if block[u'cols']:
            s += u'''<tr>\n'''
            for col in block[u'cols']:
                s += u'''<td class="getTableColTitle">%s</td>''' % col
            s += u'''</tr>\n'''
        for row in block[u'value']:
            s += u'<tr>\n'
            for item in row:
                s += u'<td class="getTableBody">%s</td>\n' % item
            s += u'</tr>\n'
        s += u'</table>\n'
        self.db[block[u'name'].encode('utf-8')] = s
    def clear(self):
        self.db.clear()
    def close(self):
        self.db.close()
    def __getitem__(self, key):
        return self.db[key]
    

def make_refLinkTag(baseFileName, obj=u'', subTypes=[], fileType=u'index'):
    s = u'<a href="index.html">TOC</a>'
    for subType in subTypes:
        if obj:
            linkDisplayName = obj + u'-' + TRAN_DICT[subType]
        else:
            linkDisplayName = baseFileName + u'-' + TRAN_DICT[subType]
        linkDisplayName = linkDisplayName.strip(u'-')
        s = u'<a href="%s">%s</a>&nbsp;%s' % (make_fileName(baseFileName, \
            obj, subType, fileType), linkDisplayName, s)
    return s

def make_indexFile(fileBaseName, blocks, subType=u''):
    obj = blocks[0][u'obj']
    fileName = make_fileName(fileBaseName, obj, subType=subType, \
                             fileType = u'index')
    hw = HtmlWriter.HtmlWriter(os.path.join(DST_DIR, fileName))
    write_htmlCommon(hw)
    title = blocks[0][u'title'] + u'Index'
    hw.WriteTagPair(u'title', title)
    hw.WriteEndTag()    #end of head
    hw.WriteBeginTag(u'body')   #begin of html body
    # title
    # file detail
    # hr
    hw.WriteTagPair(u'h3', title)
    hw.WriteTagPair(u'p', blocks[0][u'detail'])
    hw.WriteTagPair(u'hr')
    
    hw.WriteAllEndTag()
    hw.Close()
    
class IndexMaker:
    def __init__(self, fileBaseName, blocks):
        self.fileBaseName = fileBaseName
        self.headerBlock = blocks[0]
        self.bodyBlocks = blocks[1:]
        self.obj = blocks[0][u'obj']

        if self.obj:
            self.subTypes = [u'function', u'property']
        else:
            self.subTypes = [u'']
        self.catalogIndex = {}
        self.catalogNum = 0
    def make_index(self, dst, encoding=u'utf-8'):
        for subType in self.subTypes:
            self.make_indexFile(dst, subType, encoding)
    def make_indexFile(self, dst, subType=u'', encoding=u'utf-8'):
        f = make_fileName(self.fileBaseName, self.obj, subType, u'index')
        hw = HtmlWriter.HtmlWriter(os.path.join(dst, f), encoding)
        hw.WriteCommon()
        hw.WriteTagPair(u'style', HTML_STYLE)
        title = self.headerBlock[u'title'] + TRAN_DICT[subType] + u'Index'
        hw.WriteTagPair(u'title', title)
        hw.WriteEndTag()    #end of head
        hw.WriteBeginTag(u'body')   #begin of html body
        # title
        # file detail
        # hr
        hw.WriteTagPair(u'a', u'', u'id="File-Top"')
        hw.WriteTagPair(u'h3', title)
        hw.WriteTagPair(u'p', self.headerBlock[u'detail'])
        refLink = make_refLinkTag(self.fileBaseName, self.obj,
                               self.subTypes)
        hw.WriteTagPair(u'p', refLink)
        hw.WriteTagPair(u'hr')
        hw.Write(self.make_catalogIndex())
        hw.WriteTagPair(u'hr')
        for catalog in self.headerBlock[u'catalogs']:
            if len(catalog) > 1:
                content = self.get_catalogContent(catalog)
                content = sorted(content)
                content.insert(0, catalog)
            hw.WriteLine(self.make_catalogBlock(content, subType))
            hw.WriteTagPair(u'hr')
        catalog = self.make_remainingCatalog()
        hw.WriteLine(self.make_catalogBlock(catalog, subType))
        hw.WriteAllEndTag()
        hw.Close()
    def get_catalogContent(self, catalog):
        result = []
        for block in self.bodyBlocks:
            if block[u'catalog'] == catalog:
                result.append(block['name'])
        return result
    def make_remainingCatalog(self):
        catalog = [u'NoCatalog']
        for block in self.bodyBlocks:
            catalog.append(block[u'name'])
        if len(catalog) > 1:
            catalog = [catalog[0]] + sorted(catalog[1:])
        return catalog
    def make_catalogRefName(self, catalogName):
        if self.catalogIndex.has_key(catalogName):
            return self.catalogIndex[catalogName]
        name = u'Id%d' % self.catalogNum
        self.catalogIndex[catalogName] = name
        self.catalogNum += 1
        return name
    def make_catalogIndex(self):
        refLink = make_refLinkTag(self.fileBaseName, self.obj,
                               self.subTypes)
        s = ur'''<table class="catalogIndexTable">
        <tr><td class="catalogIndexTableHead">Catalogs</td>'''
        s += u'''<tr>'''
        i = 0
        for catalog in self.headerBlock[u'catalogs']:
            s += ur'''<td class="catalogIndexTableBodyTd"><a href="#%s">%s</a>
                    </td>''' % ( \
                        make_linkBaseName(self.make_catalogRefName(catalog), \
                                          u'catalog'), catalog)
            i += 1
            if i >= 4:
                i = 0
                s += u'''</tr>\n<tr>'''
        if i>= 4: s += u'''</tr>\n<tr>'''
        s += ur'''<td class="catalogIndexTableBodyTd"><a href="#%s">NoCatalog</a>
            </td>''' % ( \
            make_linkBaseName(self.make_catalogRefName(u'NoCatalog'), u'catalog'))
        s += u'</tr>\n</table>'
        return s
    def make_catalogBlock(self, catalog, subType):
        refLink = make_refLinkTag(self.fileBaseName, self.obj, self.subTypes)
        refLink = u'''<a href="#File-Top">Top</a>'''
        catalogLinkName = make_linkBaseName(self.make_catalogRefName(catalog[0]),\
                                            u'catalog')
        s = ur'''<table class="catalogBlockTable">
        <tr><td class="catalogBlockTableHead"><a id="%s">%s</a></td>
        <td class="refLinks">%s</td></tr>''' % (catalogLinkName, catalog[0],\
                                                refLink)
        bEmptyBlock = True

        for i in catalog[1:]:
            idx, summary = self.get_itemSummary(i, subType)
            if idx != -1:
                bEmptyBlock = False
                dstLink = make_linkFullName(self.fileBaseName, i, self.obj,\
                                            self.bodyBlocks[idx][u'type'])
                s += ur'''<tr><td class="catalogBlockTableBody">
                <a href="%s">%s</a></td>
                <td class="catalogBlockTableBody">%s</td></tr>''' % \
                     (dstLink, i, summary)
                del self.bodyBlocks[idx]
        if bEmptyBlock:
            s += ur'''<tr><td>No Entry</td></tr>'''
        s += u'</table>'
        return s
    def get_itemSummary(self, name, subType):
        for i in range(len(self.bodyBlocks)):
            if subType and self.bodyBlocks[i][u'name'] == name and  \
               self.bodyBlocks[i][u'type'] == subType:
                s = self.bodyBlocks[i][u'summary']
                return i, replace_see(s)
            elif not subType and self.bodyBlocks[i][u'name'] == name:
                s = self.bodyBlocks[i][u'summary']
                return i, replace_see(s)
                
        return -1, None

class ContentMaker:
    def __init__(self, fileBaseName, blocks):
        self.fileBaseName = fileBaseName
        self.headerBlock = blocks[0]
        self.bodyBlocks = blocks[1:]
        self.obj = self.headerBlock[u'obj']
        
        if self.obj:
            self.subTypes = [u'function', u'property']
        else:
            self.subTypes = [u'']
        self.refLink = make_refLinkTag(fileBaseName, self.obj, self.subTypes)
    def make_content(self, dst, encoding=u'utf_8'):
        for subType in self.subTypes:
            self.make_contentFile(dst, subType, encoding)
    def make_contentFile(self, dst, subType=u'', encoding=u'utf-8'):
        f = make_fileName(self.fileBaseName, self.obj, subType)
        hw = HtmlWriter.HtmlWriter(os.path.join(dst, f), encoding)
        hw.WriteCommon()
        hw.WriteTagPair(u'style', HTML_STYLE)
        hw.WriteTagPair(u'title', self.headerBlock[u'title'] + \
                        TRAN_DICT[subType])
        hw.WriteEndTag()    # end of head
        hw.WriteBeginTag(u'body')
        hw.WriteTagPair(u'a', u'', u'id="File-Top"')
        hw.WriteTagPair(u'h3', self.headerBlock[u'title'] + \
                        TRAN_DICT[subType])
        hw.WriteTagPair(u'p', self.headerBlock[u'detail'])
        hw.WriteTagPair(u'hr')
        for block in self.bodyBlocks:
            if subType and block[u'type'] == subType:
                hw.WriteLine(self.make_block(block))
                hw.WriteTagPair(u'hr')
            elif not subType:
                hw.WriteLine(self.make_block(block))
                hw.WriteTagPair(u'hr')
        hw.WriteAllEndTag()
        hw.Close()
    def make_block(self, block):
        linkName = make_linkBaseName(block[u'name'], block[u'type'])
        if block[u'seealso']:
            block[u'seealso'] += u'<br/>%s' % self.refLink
        else:
            block[u'seealso'] = self.refLink
        s = u'''<table class="blockTable"><tr>
            <td class="blockTableHead"><span class="blockHead">
            <a id="%s">%s</a></span></td>
            <td class="blockTableHeadTd2"><a href="#File-Top">Top</a></td>
            </tr>''' % (linkName, block[u'type'])
        if block[u'type'] == u'table':
            s += u'''<tr><td colspan="2" class="blockBody">''' \
                u'''<span class="blockPrototype">%s</span></td></tr>\n'''  % \
                (block[u'name'], )
            if block[u'summary']:
                summary = self.get_value(u'summary', block)
                summary = replace_see(summary)
                s += u'''<tr><td colspan="2"><br/>%s<br/><br/></td></tr>\n''' % (summary,)
        if block[u'type'] == u'table':
            s += u'''<tr><td colspan="2" class="blockBody">%s</td></tr>\n''' % \
                self.get_blockValue(block)
        else:
            s += u'''<tr><td colspan="2" class="blockBody">''' \
                u'''<span class="blockPrototype">%s</span></td></tr>\n'''  % \
                 self.get_blockValue(block)
        s += u'''<tr><td colspan="2" class="blockBody"><dl>\n'''
        if block[u'type'] == u'table':
            content = [u'params', u'remarks', u'seealso']
        else:
            content = [u'summary', u'params', u'remarks', u'seealso']
        for key in content:
            if block.has_key(key) and block[key]:
                v = self.get_value(key, block)
                v = replace_see(v)
                s += u'''<dt class="blockTitle">%s</dt>
                    <dd class="blockBody">%s</dd>\n''' % (key, v)
        s += u'</dl></td></tr></table>\n'
        return s
    def get_blockValue(self, block):
        if block[u'type'] == u'enum':
            return self.make_enumTable(block)
        elif block[u'type'] == u'table':
            return self.make_table(block)
        elif block[u'type'] == u'function':
            return self.make_function(block)
        else:
            return block[u'value']
    def make_function(self, block):
        s = u""
        for func in block[u'value']:
            if s:
                s += u'<br/>\n%s' % func
            else:
                s = func
        return s
    def get_value(self, key, block):
        if key == u'params':
            return self.make_paramList(block)
        else:
            return block[key]
    def make_paramList(self, block):
        s = u'''<dl>\n'''
        for param in block[u'params']:
            s += u'''<dt>%s</dt><dd>%s</dd>\n''' % (param[0], param[1])
        s += u'</dl>\n'
        return s
    def make_enumTable(self, block):
        s = ur'''<table>
        <tr><td>Value</td><td>Meaning</td></tr>
        '''
        for i in block[u'value']:
            s += u'''<tr><td>%s</td><td>%s</td></tr>\n''' % (i[1], i[0])
        s += u'''</table>\n'''
        return s
    def make_table(self, block):
        s = u'''<table class="contentTable">\n'''
        if block[u'cols']:
            s += u'''<tr>\n'''
            for col in block[u'cols']:
                s += u'''<td class="contentTableColTitle">%s</td>''' % col
            s += u'''</tr>\n'''
        for row in block[u'value']:
            s += u'<tr>\n'
            for item in row:
                s += u'<td class="contentTableBody">%s</td>\n' % item
            s += u'</tr>\n'
        s += u'</table>\n'
        return s

class SectionResolver:
    def resolve(self, name, value):
        if name == u'params':
            return self.resolve_params(value)
        elif name == u'seealso':
            return self.resolve_seealso(value)
        else:
            return self.resolve_entry(value)
    def resolve_seealso(self, seealso):
        result = []
        for i in seealso:
            result.append(self.resolve_entry(i))
        return result
    def resolve_entry(self, entry):
        s = entry.replace(u'<', u'&lt;')
        s = s.replace(u'>', u'&gt;')
        s = s.replace(u'\n', u'<br/>')
        s = replace_see(s)
        s = replace_get(s)
        return s
    def resolve_params(self, params):
        result = []
        for param in params:
            s = self.resolve_entry(param[1])
            result.append((param[0], s))
        return result

def replace_get(s):
    reGet = ur'\[\[get\s*(\w+)\s*\]\]'
    r = re.compile(reGet, re.UNICODE)
    p = 0
    m = r.search(s, p)
    while m:
        key = m.group(1)
        try:
            value = GET_DB[key.encode('utf-8')]
            value = u'<blockquote>%s</blockquote>' % value
            s = s[:m.start()] + value + s[m.end():]
        except KeyError:
            p = m.end()
        m = r.search(s, p)
    return s

def replace_see(s):
    resee = ur"\[\[see([\w\|\s.]+)\s*\]\]"
    r = re.compile(resee, re.UNICODE)
    p = 0
    m = r.search(s, p)
    
    while m:
        link = make_seeTag(m.group(1))
        if not link:
            p = m.end()
        else:
            s = s[:m.start()] + link + s[m.end():]
        m = r.search(s, p)
    
    return s

def make_seeTag(s):
    global SEE_DB
    r = s.split(u'|', 2)
    obj = r[0].strip()
    if len(r) < 2:
        content = obj
    else:
        content = obj
        obj = r[1].strip()
    try:
        return u'''<a href="%s">%s</a>''' % (SEE_DB[obj.encode('utf-8')], \
                                         content)
    except KeyError:
        return None
    
class BlockResolver:
    def __init__(self):
        self.sectionResolver = SectionResolver()
    def resolve(self, block):
        for k, v in block.iteritems():
            if k in [u'summary', u'remarks', u'seealso', u'params', u'detail']:
                block[k] = self.sectionResolver.resolve(k, block[k])
        if block.get(u'name', u'') == u'ATTRIBUTE_TYPE':
            print 'after resolve'
        if block.get(u'type', u'') == u'table':
            block[u'value'] = [self._resolve_table_row(row) for row in block[u'value']]
                    
        return block
    def _resolve_table_row(self, row):
        return [self._resolve_string(v) for v in row]
    def _resolve_string(self, s):
        if type(s) == type(u''):
            return self.sectionResolver.resolve_entry(s)
        return s

def preprocess_luai(filename):
    global SEE_DB
    global GET_DB
    fileBaseName = os.path.basename(filename)
    fileBaseName = fileBaseName.split(u'.')[0]
    fo = open_utf8(filename)
    print fileBaseName
    luaip = LuaiParser(fo)
    blocks = []
    blocks.append(luaip.get_headerBlock())
    obj = blocks[0][u'obj']
    # file link
    if obj:
        SEE_DB[obj.encode(u'utf-8')] = u'index.html'
        chmIdxKey = obj
        chmIdx = [[], []]
    else:
        SEE_DB[fileBaseName.encode(u'utf-8')] = make_fileName(fileBaseName,\
                                    fileType=u'index')
        chmIdxKey = fileBaseName
        chmIdx = []
    
    block = luaip.get_bodyBlock()
    
    while block:
        blocks.append(block)
        link = make_linkFullName(fileBaseName, block[u'name'], \
                                 obj, block[u'type'])
        if obj:
            key = u'%s.%s' % (obj, block[u'name'])
            shortKey = block[u'name']
            if block[u'type'] == u'property':
                chmIdx[0].append((shortKey, link))
            else:
                chmIdx[1].append((shortKey, link))
        else:
            key = block[u'name']
            chmIdx.append((key, link))
        # print block
        SEE_DB[key.encode('utf-8')] = link
        
        
        # print CHM_IDX_DB[chmIdxKey.encode('utf-8')]
        if block[u'type'] in [u'enum', u'table']:
            GET_DB.insert(block)
        block = luaip.get_bodyBlock()
    CHM_IDX_DB[chmIdxKey.encode('utf-8')] = chmIdx
    fo.close()


def add_toc(fileBaseName, headerBlock):
    global TOC_DB
    obj = headerBlock[u'obj']
    if obj:
        key = obj
        value = u'obj'
    else:
        key = fileBaseName
        value = u'normal'
    
    TOC_DB[key.encode('utf-8')] = value
    
def make_toc(dst, encoding=u'utf_8'):
    global TOC_DB    
    hw = HtmlWriter.HtmlWriter(os.path.join(dst, u'index.html'), encoding)

    hw.WriteCommon()
    hw.WriteTagPair(u'title', u'jx3 lua documentation')
    hw.WriteEndTag()    # end of head
    hw.WriteBeginTag(u'body', \
                     attr=u'style="margin-left:10%; margin-right:10%"')
    hw.WriteLine(u'<div style="text-align:center">' \
                 u'<a href="../index.html">&nbsp;Up&nbsp;</a></div>')
    hw.WriteLine(u'<hr />')
    hw.WriteLine(u'<div style="text-align:center">\n')
    hw.WriteTagPair(u'h3', u'jx3 lua')
    updateTime = time.localtime()
    updateTime = u'%d-%d-%d %d:%02d:%02d' % updateTime[:6]
    hw.WriteTagPair(u'p', u'Last Update: ' + updateTime)
    hw.WriteLine(u'</div>')
    hw.WriteTagPair(u'hr')
    s = u'<table style="border:ridge black 1px; border-collapse:collapse;' \
        u'width:50%">\n'
    keys = TOC_DB.keys()
    keys.sort(key=lambda k:k.lower())
    bgcolor = "#ECF3F6"
    ifBgColor = False
    for key in keys:
        ifBgColor = not ifBgColor
        if ifBgColor:
            s += u'<tr style="background-color:%s">' % (bgcolor,)
        else:
            s += u'<tr>'
        fileBaseName = unicode(key, u'utf-8')
        if TOC_DB[key] == u'obj':
            s += u'<td style="border:solid black 1px; ' \
                    u'border-collapse:collapse; padding:3px; width:25%%">' \
                    u'<a id="%s">%s</a></td>' % ( \
                make_linkBaseName(fileBaseName, u'file'), fileBaseName)
            
            for subType in [u'property', u'function']:
                s += u'<td style="border:solid black 1px; ' \
                    u'border-collapse:collapse; padding:3px; width:15%">'
                s += u'''<a href="%s">%s</a>&nbsp;&nbsp&nbsp''' % (\
                    make_fileName(fileBaseName, fileBaseName, subType,\
                        fileType=u'index'),
                    TRAN_DICT[subType])
                s += u'</td>\n'
        else:
            s += u'<td style="border:solid black 1px; ' \
                    u'border-collapse:collapse; padding:3px">' \
                    u'<a href="%s">%s</a><a id="%s"></a></td>\n' \
					u'<td style="border:solid black 1px; ' \
					u'border-collapse:collapse; padding:3px;" colspan="2">' \
					u'<a href="%s">Detail</a></td>' % ( \
                make_fileName(fileBaseName, u'', fileType=u'index'), \
                fileBaseName,
                make_linkBaseName(fileBaseName, u'File'),
				make_fileName(fileBaseName, u'', fileType=u'index'))
        s += u'</tr>'
    s += u'</table>'
    hw.WriteLine(s)
    s = u'<hr/>\n<p><a href="history.html">Change History</a></p>'
    hw.WriteLine(s)
    hw.WriteAllEndTag()
    hw.Close()
    
    
def process_luai(filename):
    fileBaseName = os.path.basename(filename)
    fileBaseName = fileBaseName.split(u'.')[0]
    
    fo = open_utf8(filename)
    br = BlockResolver()
    luaip = LuaiParser(fo)
    blocks = []
    blocks.append(luaip.get_headerBlock())
    blocks[0] = br.resolve(blocks[0])
    obj = blocks[0][u'obj']
    block = luaip.get_bodyBlock()
    while block:
        block = br.resolve(block)
        blocks.append(block)
        block = luaip.get_bodyBlock()
    add_toc(fileBaseName, blocks[0])
    indexMaker = IndexMaker(fileBaseName, blocks)
    indexMaker.make_index(DST_DIR_GB2312, u'gb18030')
    cm = ContentMaker(fileBaseName, blocks)
    cm.make_content(DST_DIR_GB2312, u'gb18030')
    fo.close()
    
    fo = open_utf8(filename)
    br = BlockResolver()
    luaip = LuaiParser(fo)
    blocks = []
    blocks.append(luaip.get_headerBlock())
    blocks[0] = br.resolve(blocks[0])
    obj = blocks[0][u'obj']
    block = luaip.get_bodyBlock()
    while block:
        block = br.resolve(block)
        blocks.append(block)
        block = luaip.get_bodyBlock()
    indexMaker = IndexMaker(fileBaseName, blocks)
    indexMaker.make_index(DST_DIR_UTF8, u'utf_8')
    cm = ContentMaker(fileBaseName, blocks)
    cm.make_content(DST_DIR_UTF8, u'utf_8')
    fo.close()
    
    
class ChmMaker:
    def __init__(self, idxDb, dest=DST_DIR_GB2312):
        self.dest = dest
        self.idxDb = idxDb
    def make_chm(self):
        self.make_chm_toc()
        self.make_chm_index()
        self.make_project()
    def make_chm_index(self):
        f = codecs.open(os.path.join(self.dest, \
            u'Index.hhk'), u'w', encoding=u'cp936')
        idxDb = shelve.open(self.idxDb, flag='r')
        f.write(u'%s\r\n' % \
            '''<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">''')
        f.write(u'%s\r\n' % '''<HTML>''')
        f.write(u'%s\r\n' % '''<HEAD>''')
        f.write(u'%s\r\n' % '''<meta name="GENERATOR"''' \
             '''content="Microsoft&reg; HTML Help Workshop 4.1">''')
        f.write(u'%s\r\n' % '''<!-- Sitemap 1.0 -->''')
        f.write(u'%s\r\n' % '''</HEAD><BODY>''')
        f.write(u'%s\r\n' % '''<OBJECT type="text/site properties">\r\n''' \
	'''<param name="ImageType" value="Folder">\r\n''')
        f.write(u'%s\r\n' % '''</OBJECT>''')
        f.write(u'<UL>\r\n')
        for head in idxDb.keys():
            indexes = idxDb[head]
            catalog = unicode(head, u'utf-8')
            # self.write_heading(f, catalog)
            if len(indexes) == 2:
                # all = indexes[0][:]
                # all.extend(indexes[1][:])
                all = indexes[0]
                all.extend(indexes[1])
            else:
                # all = indexes[:]
                all = indexes
            if all:
                all.sort()
                for item in all:
                    self.write_single_index_item(f, item[0] + u'-' + catalog, \
                                                    item[0], item[1])
        f.write(u'</UL>\r\n')
        f.write(u'</BODY></HTML>')
        idxDb.close()
        f.close()
    def make_chm_toc(self):
        f = codecs.open(os.path.join(self.dest, \
            u'Table of Contents.hhc'), u'w', encoding=u'cp936')
        idxDb = shelve.open(self.idxDb, flag='r')
        f.write(u'%s\r\n' % \
            '''<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">''')
        f.write(u'%s\r\n' % u'''<HTML>''')
        f.write(u'%s\r\n' % u'''<HEAD>''')
        f.write(u'%s\r\n' % u'''<meta name="GENERATOR"''' \
             u'''content="Microsoft&reg; HTML Help Workshop 4.1">''')
        f.write(u'%s\r\n' % u'''<!-- Sitemap 1.0 -->''')
        f.write(u'%s\r\n' % u'''</HEAD><BODY>''')
        f.write(u'%s\r\n' % u'''<OBJECT type="text/site properties">\r\n''' \
	u'''<param name="ImageType" value="Folder">''')
        f.write(u'%s\r\n' % u'''</OBJECT>''')
        f.write(u'<UL>\r\n')
        f.write(u'''<LI> <OBJECT type="text/sitemap">\r\n''' \
			u'''<param name="Name" value="%s">\r\n''' \
			u'''<param name="Local" value="%s">\r\n''' \
			u'''</OBJECT>\r\n''' % (u'TOC', u'index.html'))
        for head in sorted(idxDb.keys()):
            indexes = idxDb[head]
            catalog = unicode(head, u'utf-8')

            # print indexes
            if len(indexes) == 2:
                self.write_heading(f, catalog)
                f.write(u'<UL>\r\n')
                properties = indexes[0][:]
                properties.sort()
                if properties:
                    self.write_heading(f, u'property', make_fileName( \
                        obj=catalog, subType=u'property', fileType=u'index'))
                    self.write_group(f, properties)
                functions = indexes[1][:]
                functions.sort()
                if functions:
                    self.write_heading(f, u'function', make_fileName( \
                        obj=catalog, subType=u'function', fileType=u'index'))
                    self.write_group(f, functions)
                f.write(u'</UL>\r\n')
            else:
                self.write_heading(f, catalog, make_fileName( \
                    catalog, fileType=u'index'))
                all = indexes[:]
                all.sort()
                self.write_group(f, all)
            
        self.write_single_item(f, u'History', u'history.html')
        f.write(u'</UL>\r\n')
        f.write(u'</BODY></HTML>')
        idxDb.close()
        f.close()
    def make_project(self):
        fo = codecs.open(os.path.join(self.dest, \
            u'jx3_lua_doc.hhp'), u'w', encoding=u'gb18030')
        fo.write(u'''[OPTIONS]\r\n''' \
            u'''Compatibility=1.1 or later\r\n''' \
            u'''Compiled file=jx3_lua_doc.chm\r\n''' \
            u'''Contents file=Table of Contents.hhc\r\n''' \
            u'''Default Window=aa\r\n''' \
            u'''Default topic=index.html\r\n''' \
            u'''Display compile progress=No\r\n''' \
            u'''Full-text search=Yes\r\n''' \
            u'''Index file=Index.hhk\r\n''' \
            u'''Language=0x804 中文(中国)\r\n''' \
            u'''\r\n''' \
            u'''[WINDOWS]\r\n''' \
            u'''aa="JX3 Lua Document","Table of Contents.hhc","Index.hhk","index.html",,,,,,0x22520,,0x10307e,,0x10bb0000,,,,,,0\r\n''' \
            u'''\r\n\r\n''' \
            u'''[FILES]\r\n''')
        entries = os.listdir(DST_DIR_GB2312)
        entries.sort()
        for i in entries:
            if fnmatch(i, u"*.html"):
                fo.write(u'%s\r\n' % i)
        fo.write(u'\r\n[INFOTYPES]\r\n')
        fo.close()
    def write_heading(self, fo, heading, link=u''):
        fo.write(u'''<LI> <OBJECT type="text/sitemap">\r\n''' \
		      u'''<param name="Name" value="%s">\r\n''' % heading)
        if link:
            fo.write(u'''<param name="Local" value="%s">\r\n''' % link)
        fo.write(u'''</OBJECT>\r\n''')
    def write_single_item(self, fo, key, link):
        fo.write(u'''<LI> <OBJECT type="text/sitemap">\r\n''' \
			u'''<param name="Name" value="%s">\r\n''' \
			u'''<param name="Local" value="%s">\r\n''' \
			u'''</OBJECT>\r\n''' % (key, link))
    def write_single_index_item(self, fo, displayName, detailName, link):
        fo.write(u'''<LI> <OBJECT type="text/sitemap">\r\n'''
			u'''<param name="Name" value="%s">\r\n''' \
			u'''<param name="Name" value="%s">\r\n''' \
			u'''<param name="Local" value="%s">\r\n''' \
			u'''</OBJECT>\r\n''' % (displayName, detailName, link))
    def write_group(self, fo, group):
        if not group:
            return
        fo.write(u'<UL>\r\n')
        for item in group:
            fo.write(u'''<LI> <OBJECT type="text/sitemap">\r\n''' \
			'''<param name="Name" value="%s">\r\n''' \
			'''<param name="Local" value="%s">\r\n''' \
			'''</OBJECT>\r\n''' % (item[0], item[1]))
            # print item[0], item[1]
        fo.write(u'</UL>\r\n')

SEE_DB = shelve.open(os.path.join(TEMP_DIR, SEE_DB_FILE))
GET_DB = GetDb(os.path.join(TEMP_DIR, GET_DB_FILE))
TOC_DB = shelve.open(os.path.join(TEMP_DIR, TOC_DB_FILE))
CHM_IDX_DB = shelve.open(os.path.join(TEMP_DIR, CHM_IDX_DB_FILE))
SEE_DB.clear()
GET_DB.clear()
TOC_DB.clear()
CHM_IDX_DB.clear()

srcFiles = []
for f in os.listdir(SRC_DIR):
    if fnmatch(f, u'*.luai'):
        srcFiles.append(f)

for f in srcFiles:
    preprocess_luai(os.path.join(SRC_DIR, f))

for f in srcFiles:
    process_luai(os.path.join(SRC_DIR, f))

make_toc(DST_DIR_UTF8, u'utf_8')
make_toc(DST_DIR_GB2312, u'gb2312')
SEE_DB.close()
GET_DB.close()
TOC_DB.close()
CHM_IDX_DB.close()
shutil.copy(os.path.join(SRC_DIR, u'history.html'),
    DST_DIR_GB2312)
chm = ChmMaker(os.path.join(TEMP_DIR, CHM_IDX_DB_FILE))
chm.make_chm()
