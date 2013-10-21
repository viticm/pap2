using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public interface ICommand
    {
        void Execute();
        void UnExecute();
    };

    class Command : ICommand
    {
        public virtual void Execute()
        {
            string s = string.Format("未实现此功能。");
            //System.Windows.Forms.MessageBox.Show(s);
            throw new Exception(s);
        }
        public virtual void UnExecute()
        {
            string s = string.Format("未实现此功能。");
            //System.Windows.Forms.MessageBox.Show(s);
            throw new Exception(s);
        }
        
    };

    class SaveCommand : Command
    {
        public override void Execute()
        {
            if (!RightsSettings.AllowSave)
            {
                throw new Exception("已经被禁止保存功能。");
            }
        }
    }
    
    class SetValueCommand : Command
    {
        public override void Execute()
        {
            if (!RightsSettings.AllowSave)
            {
                throw new Exception("已经被禁止修改值的功能。");
            }
        }
    }
    class NewSubTalbeRecordCommand : Command
    {
        public override void Execute()
        {
            if (!RightsSettings.AllowSave)
            {
                string s = "已经被禁止新建从表记录的功能。";
                MessageBox.Show(s, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                throw new Exception(s);
            }
        }
    }
    class DelSubTalbeRecordCommand : Command
    {
        public override void Execute()
        {
            if (!RightsSettings.AllowSave)
            {
                throw new Exception("已经被禁止删除从表记录的功能。");
            }
        }
    }
    class ModifyTreeCommand : Command
    {
        public ModifyTreeCommand(TreeView tree)
        {
            if (!RightsSettings.AllowSave)
            {
                tree.AllowDrop = false;
                tree.BeforeLabelEdit -= new NodeLabelEditEventHandler(this.baseTree_BeforeLabelEdit);
                tree.BeforeLabelEdit -= new NodeLabelEditEventHandler(this.baseTree_BeforeLabelEdit);
                tree.BeforeLabelEdit += new NodeLabelEditEventHandler(this.baseTree_BeforeLabelEdit);
                tree.NodeMouseClick -= new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_NodeMouseClick);
                tree.NodeMouseClick -= new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_NodeMouseClick);
                tree.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_NodeMouseClick);
            }
        }
        public override void Execute()
        {
            /*if (!RightsSettings.AllowSave)
            {
                throw new Exception("已经被禁止修改树结构的功能。");
            }*/
        }
        private void baseTree_BeforeLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            //MessageBox.Show("已经被禁止修改树结构的功能。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            throw new Exception("已经被禁止修改树结构的功能。");
        }
        private void treeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Button == MouseButtons.Right && e.Node != null)
            {
                //MessageBox.Show("已经被禁止修改树结构的功能。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                throw new Exception("已经被禁止修改树结构的功能。");
            }
        }
    }

    class OpenDBToolsCommand : Command
    {
        public override void Execute()
        {
            if (!RightsSettings.AllowSave)
            {
                throw new Exception("已经被禁止打开数据表编辑器的功能。");
            }
        }
    }
    
    class OpenMetaInfoEditorCommand : Command
    {
        public override void Execute()
        {
            if (!RightsSettings.AllowSave)
            {
                throw new Exception("已经被禁止打开表元编辑器的功能。");
            }
        }
    }
    class ModifyScriptCommand : Command
    {
        public override void Execute()
        {
            if (!RightsSettings.AllowSave)
            {
                throw new Exception("已经被禁止修改脚本的功能。");
            }
        }
    }
}
