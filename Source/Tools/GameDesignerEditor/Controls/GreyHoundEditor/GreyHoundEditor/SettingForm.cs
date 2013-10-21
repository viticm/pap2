using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GreyHoundEditor
{
    public partial class SettingForm : Form
    {
        private List<string> itemList; // 下拉框内容链表
        private string areaName; // 区域名称
        private string backgroundMusic; // 背景音乐

        public SettingForm(string id, List<string> itemList):this(id, "", itemList)
        {

        }

        public SettingForm(string id, string name, List<string> itemList):this(id, name, "", itemList)
        {

        }

        public SettingForm(string id, string name, string music, List<string> itemList)
        {
            this.itemList = itemList;
            InitializeComponent();
            InitData();
            this.Text = "灰度图选项 ID:" + id;
            textBoxX1.Text = name;
            comboBoxEx1.Text = music;        
        }

        /// <summary>
        /// 下拉框内容链表属性
        /// </summary>
        public List<string> ItemList
        {
            set
            {
                itemList = value;
            }
        }

        /// <summary>
        /// 区域名称属性
        /// </summary>
        public string AreaName
        {
            get
            {
                return areaName;
            }
        }

        /// <summary>
        /// 背景音乐属性
        /// </summary>
        public string BackgroundMusic
        {
            get
            {
                return backgroundMusic;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            foreach(string s in itemList)
            {
                comboBoxEx1.Items.Add(s);
            }
        }

        /// <summary>
        /// 浏览文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            string fileName; // 文件名
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.InitialDirectory = Environment.SpecialFolder.MyComputer.ToString();
            openFileDialog.Filter = "音乐文件(*.mp3)|*.mp3|所有文件(*.*)|*.*"; // 定义文件过滤
            openFileDialog.RestoreDirectory = true;
            openFileDialog.FilterIndex = 0;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                fileName = openFileDialog.FileName;
                if ((fileName != null) && (fileName != ""))
                {
                    if(!itemList.Contains(fileName)) // 新的音乐文件，添加入下拉框内容
                    {
                        comboBoxEx1.Items.Add(fileName);
                        itemList.Add(fileName);    
                    }
                    comboBoxEx1.Text = fileName;
                }
            }
        }

        /// <summary>
        /// 确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            areaName = textBoxX1.Text;
            backgroundMusic = comboBoxEx1.Text;

            if(areaName == "")
            {
                MessageBox.Show("区域名称没有设置!", "输入参数检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if(backgroundMusic == "")
            {
                MessageBox.Show("背景音乐没有设置!", "输入参数检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }
    }
}