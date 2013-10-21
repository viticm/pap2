using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using System.Runtime.Serialization;
using System.Text.RegularExpressions;

namespace HelperEditor
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            // 初始化数据表
            TabFile.EventData = new TabFileData("Settings\\EventDefine.tab", "EventID");
            TabFile.ConditionData = new TabFileData("Settings\\ConditionDefine.tab", "ConditionID");

            // 初始化选择器
            formEventSelector = new EventSelector();
            formConditionSelector = new ConditionSelector();
            formActionSelector = new ActionSelector();
            formHelpWindow = new HelpWindow();

            // 初始化修改器
            conditionDefineEditor = new ConditionDefineEditor();

            // 初始化 Condition Tree
            groupBox2.Enabled = false;

            // 初始化 Config
            string szConfigFileName = Directory.GetCurrentDirectory() + @"\Config.ini";
            iniConfig = new INIFile(szConfigFileName);
            if (File.Exists(szConfigFileName) == false)
            {
                // 如果没有 Config 文件则创建一个带有默认值的 Config
                iniConfig.SetValue(szSection, "AutoLoadEventTreeWhenStartUp", "1");
                iniConfig.SetValue(szSection, "AutoSaveEventTreeWhenExchangeEventItem", "1");
                iniConfig.SetValue(szSection, "LastSavedFileName", "");
            }

            if (iniConfig.GetValue(szSection, "AutoLoadEventTreeWhenStartUp") == "1")
            {
                checkBox2.Checked = true;
                string szLastSavedFileName = iniConfig.GetValue(szSection, "LastSavedFileName");
                if (File.Exists(szLastSavedFileName) == true)
                    LoadEventTreeByFile(szLastSavedFileName);
            }
            else
                checkBox2.Checked = false;

            if (iniConfig.GetValue(szSection, "AutoSaveEventTreeWhenExchangeEventItem") == "1")
                checkBox1.Checked = true;
            else
                checkBox1.Checked = false;

            // TEST
            //conditionDefineEditor.ShowDialog();
        }

        private EventSelector formEventSelector;
        private ConditionSelector formConditionSelector;
        private ActionSelector formActionSelector;
        private HelpWindow formHelpWindow;

        private ConditionDefineEditor conditionDefineEditor;

        private INIFile iniConfig;
        private string szSection = "HelperEditorConfig";

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // 首先通过当前树的内容, 修改事件选择器的选定内容
            formEventSelector.SetListAllItemChecked(false);
            foreach (TreeNode Node in treeView1.Nodes)
            {
                formEventSelector.SetListItemChecked(Node.Name, true);
            }

            if (formEventSelector.ShowDialog() == DialogResult.OK)
            {
                string[] EventKeyList = formEventSelector.GetSelectedEventList();
                foreach (string szKey in EventKeyList)
                {
                    if (!treeView1.Nodes.ContainsKey(szKey))
                    {
                        string szNodeName = "[" + String.Format("{0:D2}", Convert.ToInt32(TabFile.EventData.GetTabCell(szKey, "EventID"))) + "] " +
                            TabFile.EventData.GetTabCell(szKey, "ShowName");
                        treeView1.Nodes.Add(szKey, szNodeName);
                    }
                }
            }
        }

        private void treeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            treeView1.SelectedNode = e.Node;

            if (e.Node.Level == 0)
            {
                contextMenuStrip1.Items["ToolStripMenuItemAdd"].Enabled = true;
                contextMenuStrip1.Items["ToolStripMenuItemRename"].Enabled = false;
                contextMenuStrip1.Items["ToolStripMenuItemDel"].Enabled = true;
            }
            else
            {
                contextMenuStrip1.Items["ToolStripMenuItemAdd"].Enabled = false;
                contextMenuStrip1.Items["ToolStripMenuItemRename"].Enabled = true;
                contextMenuStrip1.Items["ToolStripMenuItemDel"].Enabled = true;
            }
            e.Node.ContextMenuStrip = contextMenuStrip1;
        }

        private void toolStripTextBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13)
            {
                treeView1.SelectedNode.ContextMenuStrip.Close();
                TreeNode NewNode = treeView1.SelectedNode.Nodes.Add(toolStripTextBox1.Text);
                toolStripTextBox1.Text = "";

                treeView1.SelectedNode = NewNode;
            }
        }

        private void ToolStripMenuItemAdd_Click(object sender, EventArgs e)
        {
            treeView1.SelectedNode.ContextMenuStrip.Close();
            TreeNode NewNode = treeView1.SelectedNode.Nodes.Add("新建节点");
            toolStripTextBox1.Text = "";

            treeView1.SelectedNode = NewNode;
            treeView1.LabelEdit = true;
            NewNode.BeginEdit();
        }

        private void treeView1_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            treeView1.LabelEdit = false;
        }

        private void toolStripMenuItemRename_Click(object sender, EventArgs e)
        {
            if (treeView1.SelectedNode.Level == 1)
            {
                treeView1.LabelEdit = true;
                treeView1.SelectedNode.BeginEdit();
            }
        }

        private void ToolStripMenuItemDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("你确定要删除节点 [ " + treeView1.SelectedNode.Text + " ] 以及其所有子节点么？", "确认删除节点", MessageBoxButtons.OKCancel) == DialogResult.OK)
            {
                treeView1.SelectedNode.Remove();
            }
        }

        private void treeView2_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            treeView2.SelectedNode = e.Node;

            if (e.Node.ForeColor == Color.Black || e.Node.ForeColor == Color.Empty)
            {
                contextMenuStrip2.Items["ToolStripMenuItemAddAND"].Enabled = false;
                contextMenuStrip2.Items["ToolStripMenuItemAddOR"].Enabled = false;
                contextMenuStrip2.Items["ToolStripMenuItemAddCondition"].Enabled = false;
            }
            else
            {
                contextMenuStrip2.Items["ToolStripMenuItemAddAND"].Enabled = true;
                contextMenuStrip2.Items["ToolStripMenuItemAddOR"].Enabled = true;
                contextMenuStrip2.Items["ToolStripMenuItemAddCondition"].Enabled = true;
            }

            if (e.Node.Level == 0 && (e.Node.Name == "ROOT_AND" || e.Node.Name == "ROOT_OR")) 
                contextMenuStrip2.Items["ToolStripMenuItemDelCondition"].Enabled = false;
            else
                contextMenuStrip2.Items["ToolStripMenuItemDelCondition"].Enabled = true;

            if (e.Node.ForeColor == Color.Green)
                contextMenuStrip2.Items["ToolStripMenuItemAddOR"].Enabled = false;
            else if (e.Node.ForeColor == Color.Blue)
                contextMenuStrip2.Items["ToolStripMenuItemAddAND"].Enabled = false;

            // 如果子节点已经包含同类的逻辑容器, 那么不能新建更多的逻辑容器了
            foreach (TreeNode tempNode in e.Node.Nodes)
            {
                if (tempNode.ForeColor == Color.Green)
                    contextMenuStrip2.Items["ToolStripMenuItemAddOR"].Enabled = false;
                else if (tempNode.ForeColor == Color.Blue)
                    contextMenuStrip2.Items["ToolStripMenuItemAddAND"].Enabled = false;
            }

            e.Node.ContextMenuStrip = contextMenuStrip2;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            TreeNode NewNode = treeView2.Nodes.Add("满足所有条件 (AND)");
            NewNode.ForeColor = Color.Blue;
            NewNode.Checked = true;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            TreeNode NewNode = treeView2.Nodes.Add("满足其一条件 (OR)");
            NewNode.ForeColor = Color.Green;
            NewNode.Checked = true;
        }

        private void ToolStripMenuItemAddAND_Click(object sender, EventArgs e)
        {
            TreeNode NewNode = treeView2.SelectedNode.Nodes.Add("满足所有条件 (AND)");
            NewNode.ForeColor = Color.Blue;
            NewNode.Checked = true;
            treeView2.SelectedNode = NewNode;
        }

        private void ToolStripMenuItemAddOR_Click(object sender, EventArgs e)
        {
            TreeNode NewNode = treeView2.SelectedNode.Nodes.Add("满足其一条件 (OR)");
            NewNode.ForeColor = Color.Green;
            NewNode.Checked = true;
            treeView2.SelectedNode = NewNode;
        }

        private void ToolStripMenuItemDelCondition_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("你确定要删除节点 [ " + treeView2.SelectedNode.Text + " ] 以及其所有子节点么？", "确认删除节点", MessageBoxButtons.OKCancel) == DialogResult.OK)
            {
                treeView2.SelectedNode.Remove();
            }
        }

        // 增加条件节点
        private void ToolStripMenuItemAddCondition_Click(object sender, EventArgs e)
        {
            formConditionSelector.Tag = treeView2.SelectedNode;
            if (formConditionSelector.ShowDialog(this) == DialogResult.OK)
            {
                TreeNode NewNode = treeView2.SelectedNode.Nodes.Add(formConditionSelector.GetConditionString());
                NewNode.Checked = true;
                NewNode.ForeColor = Color.Empty;
                treeView2.SelectedNode = NewNode;
                NewNode.Tag = formConditionSelector.GetConditionNodeTag();
            }
        }

        // 修改条件节点
        private void treeView2_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Button != MouseButtons.Left || (e.Node.ForeColor != Color.Empty && e.Node.ForeColor != Color.Black))
                return;

            formConditionSelector.Tag = e.Node;
            if (formConditionSelector.ShowDialog(this) == DialogResult.OK)
            {
                SaveConditionPageData();
            }
        }

        private void button11_Click(object sender, EventArgs e)
        {
            // 这里先清理空的逻辑容器节点(AND OR), 这里做了多次递归循环知道成功
            // 直到没有删除节点了(循环一次后 nClearCount 没有增值), 就结束循环
            do { } while (TreeViewHelper.ClearAllEmptyLogicContents(treeView2.Nodes));
            treeView2.ExpandAll();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            if (treeView1.SelectedNode == null || treeView1.SelectedNode.Level != 1)
                return;
            
            SaveConditionTreeToEventNode(treeView1.SelectedNode);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            if (treeView1.SelectedNode == null || treeView1.SelectedNode.Level != 1)
                return;

            if (MessageBox.Show("你确定要将当前条件树恢复到最后一次保存的数据？", "确认恢复条件树", MessageBoxButtons.OKCancel) == DialogResult.OK)
                LoadConditionTreeByEventNode(treeView1.SelectedNode);
        }
        
        // 选中一个节点后, 可以处理之前选中的节点, 一般这里用来变色和自动储存
        private TreeNode lastSelectedNode;
        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (lastSelectedNode != null)
            {
                lastSelectedNode.ForeColor = Color.Empty;

                // 检查是否需要切换后保存, 而且只有非事件节点才自动保存
                if (checkBox1.Checked == true && lastSelectedNode.Level == 1)
                {
                    SaveConditionTreeToEventNode(lastSelectedNode);
                }
            }

            if (e.Node.Level == 0)
                e.Node.ForeColor = Color.Blue;
            else
                e.Node.ForeColor = Color.Red;

            // 这里处理读取节点的数据生成 Condition Tree
            LoadConditionTreeByEventNode(e.Node);

            // 如果是事件节点则禁用 Condition Tree
            if (e.Node.Level == 0)
                groupBox2.Enabled = false;
            else
                groupBox2.Enabled = true;

            // 更新数据
            lastSelectedNode = e.Node;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            LoadEventTreeByFile();
        }

        private void button8_Click(object sender, EventArgs e)
        {
            if (treeView1.SelectedNode != null && treeView1.SelectedNode.Level > 0)
                SaveConditionTreeToEventNode(treeView1.SelectedNode);
            SaveEventTreeToFile();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (File.Exists(toolStripStatusLabel1.Text) == true)
            {
                if (treeView1.SelectedNode != null && treeView1.SelectedNode.Level > 0)
                    SaveConditionTreeToEventNode(treeView1.SelectedNode);
                SaveEventTreeToFile(toolStripStatusLabel1.Text);
            }
        }

        private void treeView2_ItemDrag(object sender, ItemDragEventArgs e)
        {
            treeView2.SelectedNode = (TreeNode)e.Item;
            DoDragDrop(e.Item, DragDropEffects.Move);
        }

        private void treeView2_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.Move;
        }

        private void treeView2_DragDrop(object sender, DragEventArgs e)
        {
            Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
            TreeNode targetNode = ((TreeView)sender).GetNodeAt(pt);
            TreeNode sourceNode = (TreeNode)e.Data.GetData(typeof(TreeNode));

            // 判断是源和目标是否都存在
            if (targetNode == null || sourceNode == null)
                return;

            // 判断源是否是跟节点, 如果是则跳出
            if (sourceNode.Name == "ROOT_AND" || sourceNode.Name == "ROOT_OR")
                return;

            // 如果节点就是自己, 则跳出
            if (targetNode == sourceNode)
                return;

            // 判断源是否是目标的父节点(如果目标Level小于或者等于源Level那么就肯定不是父节点了)
            TreeNode parentNode = targetNode;
            while (parentNode.Level >= sourceNode.Level)
            {
                if (sourceNode == parentNode)
                    return;
                parentNode = parentNode.Parent;
            }

            // 如果都是同颜色的容器, 则跳出
            if ((targetNode.ForeColor == Color.Green && targetNode.ForeColor == sourceNode.ForeColor) ||
                (targetNode.ForeColor == Color.Blue && targetNode.ForeColor == sourceNode.ForeColor))
                return;

            // 如果自己是容器, 目标也是容器, 并且目标容器包含了和自己相同颜色的容器, 就跳出
            if ((sourceNode.ForeColor == Color.Green || sourceNode.ForeColor == Color.Blue) &&
                (targetNode.ForeColor == Color.Green || targetNode.ForeColor == Color.Blue))
            {
                foreach (TreeNode tempNode in targetNode.Nodes)
                    if (sourceNode.ForeColor == tempNode.ForeColor)
                        return;
            }

            // 下面处理两种拖弋情况。
            if (targetNode.ForeColor == Color.Empty || targetNode.ForeColor == Color.Black)
            {
                // 把任意合法节点拖动到条件节点时(非容器节点), 由于根肯定只有容器, 所以不可能存在走这个分支取不到 Parent 的情况
                targetNode.Parent.Nodes.Insert(targetNode.Index, (TreeNode)sourceNode.Clone());
                sourceNode.Remove();
            }
            else
            {
                // 把任意节点拖动到容器节点时.
                targetNode.Nodes.Add((TreeNode)sourceNode.Clone());
                sourceNode.Remove();
            }

            treeView2.ExpandAll();
        }

        private void helpToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            formHelpWindow.ShowDialog();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
                iniConfig.SetValue(szSection, "AutoSaveEventTreeWhenExchangeEventItem", "1");
            else
                iniConfig.SetValue(szSection, "AutoSaveEventTreeWhenExchangeEventItem", "0");
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox2.Checked == true)
                iniConfig.SetValue(szSection, "AutoLoadEventTreeWhenStartUp", "1");
            else
                iniConfig.SetValue(szSection, "AutoLoadEventTreeWhenStartUp", "0");
        }

        private void button13_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("你确定要将清除当前工作页的所有内容么？\n并且下次开启将不能自动打开当前页存档。", "确认清除当前页", MessageBoxButtons.OKCancel) == DialogResult.OK)
            {
                groupBox2.Enabled = false;
                toolStripStatusLabel1.Text = "";
                iniConfig.SetValue(szSection, "LastSavedFileName", "");     // 保存到 INI 文件
                treeView1.Nodes.Clear();
                treeView2.Nodes.Clear();
                treeView3.Nodes.Clear();
            }
        }

        private void conditionDefineToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            conditionDefineEditor.ChangeSelectedTab(1);
            conditionDefineEditor.ShowDialog();
        }

        private void button12_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        // ********************* 方法 ********************
        public void SaveConditionPageData()
        {
            treeView2.SelectedNode.Text = formConditionSelector.GetConditionString();
            treeView2.SelectedNode.Tag = formConditionSelector.GetConditionNodeTag();
        }

        /// <summary>
        /// 将 Condition Tree 中的数据保存到 Event Tree 当前所选的树节点中。
        /// </summary>
        /// <param name="nodeForSave">用来保存 Condition Tree 的节点。</param>
        public void SaveConditionTreeToEventNode(TreeNode nodeForSave)
        {
            // 下面是实际保存树
            TreeView tempTree = new TreeView();
            foreach (TreeNode tempNode in treeView2.Nodes)
            {
                TreeNode clonedNode = (TreeNode)tempNode.Clone();
                tempTree.Nodes.Add(clonedNode);
            }
            nodeForSave.Tag = tempTree;
            treeView2.ExpandAll();
        }

        /// <summary>
        /// 将保存在 Event Tree 当前所选的树节点中的数据恢复为 Condition Tree。
        /// </summary>
        /// <param name="nodeForLoad">用来读取 Condition Tree 的节点。</param>
        public void LoadConditionTreeByEventNode(TreeNode nodeForLoad)
        {
            treeView2.Nodes.Clear();
            if (nodeForLoad.Level == 1)
                if (nodeForLoad.Tag == null)
                {
                    // 只有为空并且不是事件根节点, 才初始化为两个默认节点
                    TreeNode treeNodeAND = new TreeNode("满足所有条件 [ROOT] (AND)");
                    TreeNode treeNodeOR = new TreeNode("满足其一条件 [ROOT] (OR)");
                    treeNodeAND.Checked = true;
                    treeNodeAND.ForeColor = System.Drawing.Color.Blue;
                    treeNodeAND.Name = "ROOT_AND";
                    treeNodeAND.Text = "满足所有条件 [ROOT] (AND)";
                    treeNodeOR.Checked = true;
                    treeNodeOR.ForeColor = System.Drawing.Color.Green;
                    treeNodeOR.Name = "ROOT_OR";
                    treeNodeOR.Text = "满足其一条件 [ROOT] (OR)";
                    treeView2.Nodes.AddRange(new TreeNode[] { treeNodeAND, treeNodeOR });
                }
                else
                {
                    TreeView tempTree = (TreeView)nodeForLoad.Tag;
                    foreach (TreeNode tempNode in tempTree.Nodes)
                    {
                        TreeNode clonedNode = (TreeNode)tempNode.Clone();
                        treeView2.Nodes.Add(clonedNode);
                    }
                }
            treeView2.ExpandAll();
        }

        public void SaveEventTreeToFile()
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                SaveEventTreeToFile(saveFileDialog1.FileName);
            }
        }

        public void SaveEventTreeToFile(string szFileName)
        {
            SaveEventTreeToFile(szFileName, true);
        }

        public void SaveEventTreeToFile(string szFileName, bool bExportLua)
        {
            // 可以序列化 node , 所以考虑将所有 nodes 放进一个 root node , 然后序列化, 最后再解释就好了
            TreeNode nodeForSaveToFile = new TreeNode("SaveRoot");
            foreach (TreeNode eventNode in treeView1.Nodes)
            {
                foreach (TreeNode conditionNode in eventNode.Nodes)
                {
                    TreeView conditionTree = (TreeView)conditionNode.Tag;
                    if (conditionTree != null)
                    {
                        foreach (TreeNode dataNode in conditionTree.Nodes)
                        {
                            conditionNode.Nodes.Add((TreeNode)dataNode.Clone());
                        }
                    }
                }
                nodeForSaveToFile.Nodes.Add((TreeNode)eventNode.Clone());
            }

            // 下面开始导出 tsf 文件
            IFormatter formatter = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
            Stream stream = new FileStream(szFileName, FileMode.Create, FileAccess.Write, FileShare.None);
            formatter.Serialize(stream, nodeForSaveToFile);
            stream.Close();

            // 下面开始导出 LUA 文件
            if (bExportLua == true)
            {
                ExportLUA(nodeForSaveToFile, Path.ChangeExtension(szFileName, "lua"));
            }

            // 清理 EVENT TREE 中的临时节点
            foreach (TreeNode eventNode in treeView1.Nodes)
            {
                foreach (TreeNode conditionNode in eventNode.Nodes)
                {
                    conditionNode.Nodes.Clear();
                }
            }
            toolStripStatusLabel1.Text = szFileName;
            iniConfig.SetValue(szSection, "LastSavedFileName", szFileName);     // 保存到 INI 文件

            MessageBox.Show("事件树被保存到 [ " + szFileName + " ]。\n导出的 LUA 文件被保存到 [ " + Path.ChangeExtension(szFileName, "lua") + " ]。", "保存完成", MessageBoxButtons.OK);
        }

        /// <summary>
        /// 递归创建条件节点逻辑表达式。
        /// </summary>
        /// <param name="compareNode">数据树。</param>
        /// <returns></returns>
        private string CtrateCompareExpression(TreeNode compareNode)
        {
            string szCompareExpression = "";        // 组织好的表达式字符串

            string szCompareOperator = "or";
            if (compareNode.ForeColor == Color.Blue)
                szCompareOperator = "and";

            string szCompareEmptyResult = "false";
            if (compareNode.Parent.Level < 2 || compareNode.Parent.ForeColor == Color.Blue)
                szCompareEmptyResult = "true";

            if (compareNode.Checked == false)
            {
                szCompareExpression = szCompareEmptyResult;
            }
            else
            {
                // 如果节点不是容器且没有子节点, 就计算表达式
                if (compareNode.ForeColor != Color.Green && compareNode.ForeColor != Color.Blue && compareNode.Nodes.Count == 0)
                {
                    // 实际条件节点内容
                    //szCompareExpression = compareNode.Text;
                    ConditionNodeTagData conditionNodeTag = (ConditionNodeTagData)compareNode.Tag;

                    // 如果是一个函数条件
                    if (conditionNodeTag.CompareOperator.Value == "()")
                        szCompareExpression = string.Format(conditionNodeTag.CompareCode, conditionNodeTag.CompareTarget.Value);
                    else
                        szCompareExpression = conditionNodeTag.CompareCode + " " + conditionNodeTag.CompareOperator.Value + " " + conditionNodeTag.CompareTarget.Value;
                }
                else if ((compareNode.ForeColor == Color.Green || compareNode.ForeColor == Color.Blue))
                {
                    // 处理容器
                    if (compareNode.Nodes.Count > 0)
                    {
                        szCompareExpression += "(";
                        foreach (TreeNode subNode in compareNode.Nodes)
                        {
                            szCompareExpression = szCompareExpression + CtrateCompareExpression(subNode) + " " + szCompareOperator + " ";
                        }
                        // 这里清理掉最后一个 AND 或者 OR
                        szCompareExpression = Regex.Replace(szCompareExpression, " " + szCompareOperator + " $", "");
                        szCompareExpression += ")";
                    }
                    else
                        szCompareExpression = szCompareEmptyResult;
                }
            }
            return szCompareExpression;
        }

        public void ExportLUA(TreeNode dataNode, string szFileName)
        {
            string szHelperFileName = szFileName;
            StreamWriter stFile = new StreamWriter(szHelperFileName, false, Encoding.Default);
            StringBuilder szbLUAContent = new StringBuilder();

            foreach (TreeNode eventNode in dataNode.Nodes)
            {
                // 生成函数定义
                szbLUAContent.AppendLine("-- " + TabFile.EventData.GetTabCell(eventNode.Name, "ShowName"));
                szbLUAContent.Append("Helper." + TabFile.EventData.GetTabCell(eventNode.Name, "CodeName") + " = function(");
                for (int i = 1; i <= 4; i++)
                {
                    string szParm = TabFile.EventData.GetTabCell(eventNode.Name, "Parm" + i.ToString());
                    if (i > 1 && szParm != "")
                        szParm = ", " + szParm;
                    szbLUAContent.Append(szParm);                    
                }
                szbLUAContent.AppendLine(")");

                // 开始生成条件节点
                foreach (TreeNode conditionNode in eventNode.Nodes)
                {
                    szbLUAContent.AppendLine("\t-- " + conditionNode.Text);
                    // 定义这个跟节点是 AND 节点
                    conditionNode.ForeColor = Color.Blue;
                    conditionNode.Checked = true;
                    string szCompareExpression = CtrateCompareExpression(conditionNode);
                    //szCompareExpression = Regex.Replace(szCompareExpression, @" and \)$", ")");
                    //szCompareExpression = Regex.Replace(szCompareExpression, @"\(\)$", "(false)");
                    szbLUAContent.AppendLine("\tif " + szCompareExpression + " then");

                    // 开始生成行为节点
                    szbLUAContent.AppendLine("\t\t-- Actions");

                    szbLUAContent.AppendLine("\tend;");
                }

                // 结束函数
                szbLUAContent.AppendLine("end;");
                szbLUAContent.AppendLine();
            }
            stFile.Write(szbLUAContent.ToString());
            stFile.Close();
        }

        public void LoadEventTreeByFile()
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                LoadEventTreeByFile(openFileDialog1.FileName);
            }
        }

        public void LoadEventTreeByFile(string szFileName)
        {
            IFormatter formatter = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
            Stream stream = new FileStream(szFileName, FileMode.Open, FileAccess.Read, FileShare.Read);
            TreeNode loadedNode = (TreeNode)formatter.Deserialize(stream);
            stream.Close();

            // 对新加载的 TREE 进行初始化
            treeView1.Nodes.Clear();
            // 先恢复所有树节点
            foreach (TreeNode eventNode in loadedNode.Nodes)
            {
                treeView1.Nodes.Add((TreeNode)eventNode.Clone());
            }
            // 然后将条件节点的子节点打包
            foreach (TreeNode eventNode in treeView1.Nodes)
            {
                eventNode.ForeColor = Color.Empty;
                foreach (TreeNode conditionNode in eventNode.Nodes)
                {
                    conditionNode.ForeColor = Color.Empty;
                    treeView2.Nodes.Clear();
                    foreach (TreeNode dataNode in conditionNode.Nodes)
                    {
                        treeView2.Nodes.Add((TreeNode)dataNode.Clone());
                    }
                    conditionNode.Nodes.Clear();
                    SaveConditionTreeToEventNode(conditionNode);
                }
            }
            treeView1.ExpandAll();
            toolStripStatusLabel1.Text = szFileName;
            iniConfig.SetValue(szSection, "LastSavedFileName", szFileName);     // 保存到 INI 文件
        }

        public TreeNode GetSelectedEventNode()
        {
            return treeView1.SelectedNode;
        }



        #region 行为的处理

        /// <summary>
        /// 将树跟快捷菜单绑定起来
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void treeView3_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            treeView3.SelectedNode = e.Node;
            contextMenuStrip3.Items["ToolStripMenuItemAddAction"].Enabled = true;
            contextMenuStrip3.Items["ToolStripMenuItemRenameAction"].Enabled = true;
            contextMenuStrip3.Items["ToolStripMenuItemDeleteAction"].Enabled = true;

            e.Node.ContextMenuStrip = contextMenuStrip3;
        }

        /// <summary>
        /// 新增行为节点
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button10_Click(object sender, EventArgs e)
        {
            //formActionSelector.Tag = null;
            formActionSelector.Tag = treeView3.Focused;
            if (formActionSelector.ShowDialog(this) == DialogResult.OK)
            {
                //TreeNode NewNode = treeView3.SelectedNode.Nodes.Add(formActionSelector.GetActionString());
                TreeNode NewNode = treeView3.Nodes.Add(formActionSelector.GetActionString());
                NewNode.Checked = true;
                NewNode.ForeColor = Color.Empty;
                treeView3.SelectedNode = NewNode;
                NewNode.Tag = formActionSelector.GetActionString();
            }
        }


        /// <summary>
        /// 针对行为节点新增具体的行为
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ToolStripMenuItemAddAction_Click(object sender, EventArgs e)
        {
            formActionSelector.Tag = treeView3.SelectedNode;
            if (formActionSelector.ShowDialog(this) == DialogResult.OK)
            {
                //TreeNode NewNode = treeView3.SelectedNode.Nodes.Add(formActionSelector.GetActionString());
                TreeNode NewNode = treeView3.Nodes.Add(formActionSelector.GetActionString());
                NewNode.Checked = true;
                NewNode.ForeColor = Color.Empty;
                treeView3.SelectedNode = NewNode;
                NewNode.Tag = formActionSelector.GetActionString();
            }
        }
        /// <summary>
        /// 重命名行为
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ToolStripMenuItemRenameAction_Click(object sender, EventArgs e)
        {
            if (treeView3.SelectedNode.Level == 0)
            {
                treeView3.LabelEdit = true;
                treeView3.SelectedNode.BeginEdit();
            }
        }
        /// <summary>
        /// 删除行为
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ToolStripMenuItemDeleteAction_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("你确定要删除节点[" + treeView3.SelectedNode.Text + "]么？", "确认删除节点", MessageBoxButtons.OKCancel) == DialogResult.OK)
            {
                treeView3.SelectedNode.Remove();
            }
       }

        /// <summary>
        /// 获取当前选定的行为事件
        /// </summary>
        /// <returns></returns>
        public TreeNode GetSelectedActionNode()
        {
            return treeView3.SelectedNode;
        }
        #endregion 
    }
}
