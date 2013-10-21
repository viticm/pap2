using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using LuaInterface;

namespace Compiler_Sword3
{
    public class LuaEx : LuaInterface.Lua
    {
        public LuaEx()
            : base()
        {
            Init();
        }
#if DEBUG
        public void Init()
#else
        void Init()
#endif
        {
            try
            {
                //this["MainForm"] = Program.MainForm;// (MainForm)ParentForm;

                string sCode = @"
                --将内容以 <%xxx%> 为分隔符, 分在不同的table元素中
Compiler = {}
function Compiler.SplitContent(content)
 local t = {}
 local iStart = 0
    local findstart, findend
    local len = string.len(content)
  while(iStart <= len) do
  findstart, findend = string.find(content, '<%%.-%%>', iStart)
  if findstart then
   --将 <%...%> 之前的内容变为一个单元
   table.insert(t, string.sub(content, iStart, findstart - 1)) --直接进入最终内容的代码
   --将 <%...%> 中的内容变为另一个单元
   table.insert(t, string.sub(content, findstart, findend))
   iStart = findend + 1
  else
   table.insert(t, string.sub(content, iStart))
   break
  end
  end
  return t
end
--将表处理为 out:print 的形式
function Compiler.ExportToTemplate(tContent)
 local ncount = table.getn(tContent)
 local i, str
 for i = 1, ncount do
  str = tContent[i]
  if (string.sub(str, 1, 2) == '<%') then --是脚本代码
   local findassign, findend = string.find(str, '<%%%s-=', 1) --查找是否 <%= 形式
   if findassign then  -- 是 <%= 形式
    tContent[i] = string.format('out:print(tostring(%s))', string.sub(str, findend + 1, -3))
   else
    tContent[i] = string.sub(str, 3, -3)
   end
  else
   str = string.gsub(str, '[(%[)]', '\%1')
   str = string.gsub(str, '[(%])]', '\%1')   
   tContent[i] = string.format('out:print([[%s]])', str)
  end
 end
 local s
 s = table.concat(tContent, '\n')
 return s
end
function Compiler.RemoveAllComment(content)
    return string.gsub(content, '<!.-!>', '')
end
--将某模板转为可执行模板
function Compiler.TransTemplate(template)
 local t = Compiler.RemoveAllComment(template)
 t = Compiler.SplitContent(t) --转为表格
 
 return Compiler.ExportToTemplate(t) --处理为可执行模板
end
--将指定文件名的模板转为可执行模板
function Compiler.main(filename)--, desfilename)
 local file = assert(io.open(filename, 'r'))
 local s = file:read('*all')
 file:close()

 local stempfilecontent = Compiler.TransTemplate(s)
 return stempfilecontent

--[[
 local desfile = assert(io.open(desfilename, 'w'))
 desfile:write(desfile)
 desfile:close() --]]
end
";
                this.DoString(sCode);
            }
            catch (Exception ex)
            {
                System.Windows.Forms.MessageBox.Show(ex.Message);
            }
        }
    };
}
