using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace AddNpcActionNode
{
    public partial class ConfigForm : Form
    {
        private SqlConnection conn; // sql连接
        private string animationID; // 动作id
        //private string priority; // 优先级
        private string kindid; // 动作序列类型
        private string loop; // 是否循环
        private string filePath; // 文件路径
        private string representID; // 表现id
        private string modelName; // 模块名称
        private DataTable tbl_npc_ani_relation;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        public ConfigForm(SqlConnection conn)
        {
            this.conn = conn;
            InitializeComponent();
            InitData();
        }

        public void InitFileInfo(string representID)
        {
            this.representID = representID;
            GetFilePath();
        }

        private void InitData()
        {
            string sql = "SELECT * FROM animation_npc_relation ORDER BY 动作序列";
            tbl_npc_ani_relation = Helper.GetDataTable(sql, conn);
            foreach (DataRow row in tbl_npc_ani_relation.Rows)
            {
                comboBoxEx1.Items.Add(row["状态名"].ToString().Trim());
            }
        }

        /// <summary>
        /// 模块名称属性
        /// </summary>
        public string ModelName
        {
            get
            {
                return modelName;
            }
        }

        /// <summary>
        /// 文件路径属性
        /// </summary>
        public string FilePath
        {
            get
            {
                return filePath;
            }
        }

        /// <summary>
        /// 动作id属性
        /// </summary>
        public String AnimationID
        {
            get
            {
                return animationID;
            }
        }

//         /// <summary>
//         /// 优先级属性
//         /// </summary>
//         public String Priority
//         {
//             get
//             {
//                 return priority;
//             }
//         }

        /// <summary>
        /// 序列类型
        /// </summary>
        public String KindID
        {
            get
            {
                return kindid;
            }
        }

        /// <summary>
        /// 是否循环
        /// </summary>
        public String Loop
        {
            get
            {
                return loop;
            }
        }

        /// <summary>
        /// 填充分类的ID
        /// </summary>
        /// <param name="index">分类序号</param>
        private void FillCatifyID(/*int index*/)
        {
            comboBoxEx2.Items.Clear();
            string stateName = comboBoxEx1.Text;
            if (stateName.Length != 0)
            {
                DataRow rrr = tbl_npc_ani_relation.Rows.Find(stateName);
                if (rrr == null)
                    rrr = tbl_npc_ani_relation.Rows.Find(stateName + "\r\n");

                string strID = rrr["ID"].ToString().Trim();
                string[] strIDs = strID.Split(new char[] { '-' });
                if (strIDs.Length == 2)
                {
                    int iStart = Convert.ToInt32(strIDs[0]);
                    int iEnd = Convert.ToInt32(strIDs[1]);
                    for (int i = iStart; i <= iEnd; i++)
                    {
                        comboBoxEx2.Items.Add(i.ToString());
                    }
                }
                else if (strIDs.Length == 1)
                {
                    comboBoxEx2.Items.Add(strIDs[0]);
                }
            }

            comboBoxEx2.SelectedIndex = 0; // 默认选中第一个选项
        }

        /// <summary>
        /// 获取文件路径
        /// </summary>
        private void GetFilePath()
        {
            string sqlString = string.Format("SELECT MainModelFile FROM {0} WHERE RepresentID = '{1}'", "npc", representID);
            DataTable table = Helper.GetDataTable(sqlString, conn);
            if(table.Rows.Count > 0)
            {
                filePath = table.Rows[0][0].ToString();
            }
        }

        /// <summary>
        /// 填充优先级
        /// </summary>
        /// <param name="index">分类序号</param>
        private void FillPropertyID(/*int index*/)
        {
            string stateName = comboBoxEx1.Text;
            if (stateName.Length != 0)
            {
                DataRow rrr = tbl_npc_ani_relation.Rows.Find(stateName);
                if (rrr == null)
                    rrr = tbl_npc_ani_relation.Rows.Find(stateName + "\r\n");

                textBoxX2.Text = rrr["动作序列"].ToString().Trim();
                loop = rrr["默认循环"].ToString().Trim();
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            animationID = comboBoxEx2.Text;
            //priority = textBoxX1.Text;
            kindid = textBoxX2.Text;

            if(animationID == "")
            {
                MessageBox.Show("动作id不能为空!");
                return;
            }

//             if(priority == "")
//             {
//                 MessageBox.Show("动作优先级不能为空!");
//                 return;
//             }

            if(kindid == "")
            {
                MessageBox.Show("动作序列类型不能为空!");
                return;
            }

            string[] data = filePath.Split(new char[] { '\\' });
            filePath = "";
            modelName = data[3];
            for (int i = 0; i < 4; i++)
            {
                filePath += data[i] + "\\";
            }
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 用户选择新的动作分类
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = comboBoxEx1.SelectedIndex; // 选中的序号
            FillPropertyID(/*index*/);
            FillCatifyID(/*index*/);
        }
    }
}