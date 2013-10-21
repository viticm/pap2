using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DBManager;
using DevComponents.AdvTree;
using System.IO;
using System.Runtime.InteropServices;

namespace ConstSelector
{
    public partial class TrapForm : DevComponents.DotNetBar.Office2007Form
    {
        private string m_SelectedKey = "";
        public TrapForm()
        {
            InitializeComponent();
            this.DialogResult = DialogResult.Cancel;
        }

        public string show(string oldVarIndex, string mapname, string ClientPath)
        {
            this.advTree1.Nodes.Clear();
            Node node_map = new Node();
            node_map.Text = mapname;
            node_map.Image = imageList1.Images[0];
            advTree1.Nodes.Add(node_map);

            string iniFileName = Path.Combine(ClientPath,
                @"data\source\maps\" + mapname + "\\" + mapname + ".Logic.ini");


            for (int i = 0; ; i++)
            {
                string strName =  IniIO.IniReadValue("Section" + i.ToString(), "名称", iniFileName);
                string strScript = IniIO.IniReadValue("Section" + i.ToString(), "脚本", iniFileName);

                if (strName == "") break;

                if(strScript != "")
                {
                    Node node = new Node();
                    node.Text = strName;
                    node.Image = imageList1.Images[1];
                    node.Tag = mapname + "," + strName;
                    node.Cells.Add(new Cell(strScript));
                    node.Cells.Add(new Cell());
                    node_map.Nodes.Add(node);
                    if (node.Tag.ToString() == oldVarIndex)
                    {
                        advTree1.SelectedNode = node;
                    }
                }
            }

            if(this.ShowDialog() == DialogResult.OK)
            {
                return m_SelectedKey;
            }
            return oldVarIndex;
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (this.advTree1.SelectedNode == null || this.advTree1.SelectedNode.Parent == null) return;
            this.DialogResult = DialogResult.OK;
            m_SelectedKey = this.advTree1.SelectedNode.Tag.ToString();
            return;
        }
    }

    public class IniIO
    {
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        public static void IniWriteValue(string Section, string Key, string Value, string filepath)     //对ini文件进行写操作的函数
        {
            long i = 0;
            i = WritePrivateProfileString(Section, Key, Value, filepath);
        }

        public static string IniReadValue(string Section, string Key, string filepath)                  //对ini文件进行读操作的函数
        {
            StringBuilder temp = new StringBuilder(255);
            int i = GetPrivateProfileString(Section, Key, "", temp, 255, filepath);
            return temp.ToString();
        }
    }

}

