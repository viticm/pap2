--require("CLRPackage")

Forms = CLRPackage("System.Windows.Forms", "System.Windows.Forms")
Drawing = CLRPackage("System.Drawing", "System.Drawing")
LuaInterface = CLRPackage("LuaInterface", "LuaInterface")
IO = CLRPackage("System.IO", "System.IO")
System = CLRPackage("System", "System")

Form=Forms.Form
TextBox=Forms.TextBox
Label=Forms.Label
ListBox=Forms.ListBox
Button=Forms.Button
Point=Drawing.Point
Size=Drawing.Size
Lua=LuaInterface.Lua
OpenFileDialog=Forms.OpenFileDialog
File=IO.File
StreamReader=IO.StreamReader
FileMode=IO.FileMode
ScrollBars=Forms.ScrollBars
FormBorderStyle=Forms.FormBorderStyle
FormStartPosition=Forms.FormStartPosition
trace = Forms.MessageBox.Show
--Forms.MessageBox.Show("中华人民共和国")

function onaftervaluechanged(prop, e)
    trace(prop.Category.."  "..e.value, "我是ＬＵＡ对话框")
end
