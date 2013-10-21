using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using System.Data.SqlClient;

namespace StandardNpcCreate
{
    public partial class WeaponSelectForm : Form
    {
        public enum WeaponType { LeftHand, RightHand };
        private WeaponType weaponType;
        private string sourceID;
        private string weaponFilePath;
        private string currentBoneNumber;
        private string folderPath;
        private SqlConnection conn;

        private DataTable boneToActionTable;
        private DataTable itemForInsertTable;
        private DataTable modelTable;
        private Hashtable boneNumberTable = new Hashtable();

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">数据库连接</param>
        /// <param name="boneToActionTable">数据表</param>
        /// <param name="itemForInsertTable">数据表</param>
        /// <param name="weaponType">武器类型</param>
        /// <param name="currentBoneNumber">BoneNumber</param>
        /// <param name="itemForce">武器势力</param>
        public WeaponSelectForm(SqlConnection conn, DataTable boneToActionTable, DataTable itemForInsertTable, DataTable modelTable, WeaponType weaponType, string currentBoneNumber, string sourceID)
        {
            this.conn = conn;
            this.boneToActionTable = boneToActionTable;
            this.itemForInsertTable = itemForInsertTable;
            this.modelTable = modelTable;
            this.weaponType = weaponType;
            this.sourceID = sourceID;
            this.currentBoneNumber = currentBoneNumber;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 武器文件路径
        /// </summary>
        public string WeaponFilePath
        {
            get
            {
                return weaponFilePath;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化数据列
            DataGridViewTextBoxColumn itemNumberColumn = new DataGridViewTextBoxColumn();
            itemNumberColumn.Name = "ItemNumber";
            itemNumberColumn.HeaderText = "ItemNumber";

            DataGridViewTextBoxColumn itemTypeColumn = new DataGridViewTextBoxColumn();
            itemTypeColumn.Name = "ItemType";
            itemTypeColumn.HeaderText = "武器类型";

            dataGridViewX1.Columns.Add(itemNumberColumn);
            dataGridViewX1.Columns.Add(itemTypeColumn);

            // 初始化分类
            foreach (DataRow dataRow in boneToActionTable.Rows)
            {
                string boneNumber = dataRow["BoneNumber"] as string;
                string detailType = dataRow["DetailType"] as string;
                string weaponType = dataRow["WeaponType"] as string;

                if (!string.IsNullOrEmpty(boneNumber) && !string.IsNullOrEmpty(detailType))
                {
                    if (boneNumber == currentBoneNumber || string.IsNullOrEmpty(currentBoneNumber))
                    {
                        string displayValue = string.Format("{0} [{1}]    {2}", boneNumber, weaponType, detailType);
                        listBox1.Items.Add(displayValue);
                    }                    
                }
            }

            // 初始化图片文件夹路径
            folderPath = Path.Combine(Application.StartupPath, @"Design\WeaponImage");
        }

        /// <summary>
        /// 获取显示信息
        /// </summary>
        /// <param name="displayValue">显示字符串</param>
        /// <returns>显示信息数组</returns>
        private string[] GetDisplayInfo(string displayValue)
        {
            string[] displayInfo = new string[3];

            int index = displayValue.IndexOf("[");
            displayInfo[0] = displayValue.Substring(0, index - 1);
            string tempString = displayValue.Substring(index + 1, displayValue.Length - index - 1);

            index = tempString.IndexOf("]");
            displayInfo[1] = tempString.Substring(0, index);
            tempString = tempString.Substring(index + 1, tempString.Length - index - 1);

            displayInfo[2] = tempString.Trim();

            return displayInfo;
        }

        /// <summary>
        /// 获取武器文件路径
        /// </summary>
        /// <param name="itemNumber">ItemNumber</param>
        /// <returns>武器文件路径</returns>
        private string GetWeaponFilePath(string itemNumber)
        {
            string filePath = null;
            string relrevatePath; // 相对路径

            if (itemNumber.StartsWith("RH_"))
            {
                string tempString = itemNumber.Replace("RH_", "");
                int tempIndex = tempString.IndexOf("_");
                tempString = tempString.Substring(0, tempIndex);
                relrevatePath = string.Format(@"data\source\item\weapon\{0}", tempString);
            }
            else if (itemNumber.StartsWith("LH_"))
            {
                string tempString = itemNumber.Replace("LH_", "");
                int tempIndex = tempString.IndexOf("_");
                tempString = tempString.Substring(0, tempIndex);
                relrevatePath = string.Format(@"data\source\item\weapon\{0}", tempString);
            }
            else
            {
                relrevatePath = @"data\source\item\prop";
            }

            filePath = string.Format(@"{0}\{1}.Mesh", relrevatePath, itemNumber);

            return filePath;
        }

        /// <summary>
        /// 获取势力ID
        /// </summary>
        /// <returns>势力ID</returns>
        private string GetForceID()
        {
            string forceID = "";

            DataRow[] rows = modelTable.Select(string.Format("SourceID = '{0}'", sourceID));
            if (rows.Length > 0)
            {
                string forceName = rows[0]["Force"] as string;

                switch(forceName)
                {
                    case "少林":
                        {
                            forceID = "1";
                            break;
                        }
                    case "天竺":
                        {
                            forceID = "2";
                            break;
                        }
                    case "纯阳":
                        {
                            forceID = "3";
                            break;
                        }
                    case "唐门":
                        {
                            forceID = "4";
                            break;
                        }
                    case "天策":
                        {
                            forceID = "5";
                            break;
                        }
                    case "五毒":
                        {
                            forceID = "6";
                            break;
                        }
                    case "明教":
                        {
                            forceID = "7";
                            break;
                        }
                    case "万花":
                        {
                            forceID = "8";
                            break;
                        }
                    case "藏剑":
                        {
                            forceID = "9";
                            break;
                        }
                    case "七秀":
                        {
                            forceID = "10";
                            break;
                        }
                    case "霸刀":
                        {
                            forceID = "11";
                            break;
                        }
                    case "镖局":
                        {
                            forceID = "12";
                            break;
                        }
                    case "红衣教":
                        {
                            forceID = "13";
                            break;
                        }
                    case "僵尸教":
                        {
                            forceID = "14";
                            break;
                        }
                    case "神策军":
                        {
                            forceID = "15";
                            break;
                        }
                    case "十二连环坞":
                        {
                            forceID = "16";
                            break;
                        }
                    case "天一教":
                        {
                            forceID = "17";
                            break;
                        }
                    case "昆仑":
                        {
                            forceID = "18";
                            break;
                        }
                    case "狼牙":
                        {
                            forceID = "19";
                            break;
                        }
                    case "叛军":
                        {
                            forceID = "20";
                            break;
                        }
                    case "恶人谷":
                        {
                            forceID = "21";
                            break;
                        }
                    case "浩气盟":
                        {
                            forceID = "22";
                            break;
                        }
                    case "一刀流":
                        {
                            forceID = "23";
                            break;
                        }
                    case "长歌门":
                        {
                            forceID = "24";
                            break;
                        }
                    case "隐元会":
                        {
                            forceID = "25";
                            break;
                        }
                    case "丐帮":
                        {
                            forceID = "26";
                            break;
                        }
                    case "地鼠门":
                        {
                            forceID = "28";
                            break;
                        }
                    case "倭寇":
                        {
                            forceID = "29";
                            break;
                        }
                    case "侠客岛":
                        {
                            forceID = "30";
                            break;
                        }
                    case "商会":
                        {
                            forceID = "31";
                            break;
                        }
                    case "铜钱会":
                        {
                            forceID = "32";
                            break;
                        }
                    case "世界":
                        {
                            forceID = "33";
                            break;
                        }
                    case "城市":
                        {
                            forceID = "34";
                            break;
                        }
                    case "城镇":
                        {
                            forceID = "35";
                            break;
                        }
                    case "乡村":
                        {
                            forceID = "36";
                            break;
                        }
                    case "凌霄阁":
                        {
                            forceID = "37";
                            break;
                        }
                    case "动物":
                        {
                            forceID = "38";
                            break;
                        }
                    case "机甲":
                        {
                            forceID = "39";
                            break;
                        }
                }
            }

            return forceID;
        }

        /// <summary>
        /// 比较势力
        /// </summary>
        /// <param name="forceValue">势力字段值</param>
        /// <param name="forceID">势力ID</param>
        /// <returns>势力是否符合条件</returns>
        private bool CompareForce(string forceValue, string forceID)
        {
            bool forceEqual = false;

            if (forceValue == "")
            {
                forceEqual = true;
            }
            else
            {
                string[] tempArray = forceValue.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                foreach(string s in tempArray)
                {
                    if (s == forceID)
                    {
                        forceEqual = true;
                        break;
                    }
                }
            }

            return forceEqual;
        }

        /// <summary>
        /// 获取ItemType
        /// </summary>
        /// <param name="displayWeaponType">武器表现字符串</param>
        /// <returns>ItemType</returns>
        private string GetItemType(string displayWeaponType)
        {
            string itemType = null;

            switch (displayWeaponType)
            {
                case "单兵":
                    {
                        itemType = "1";
                        break;
                    }
                case "长兵":
                    {
                        itemType = "2";
                        break;
                    }
                case "特殊":
                    {
                        itemType = "3";
                        break;
                    }
                case "拳套":
                    {
                        itemType = "4";
                        break;
                    }
                case "笔":
                    {
                        itemType = "5";
                        break;
                    }
                case "拐杖":
                    {
                        itemType = "6";
                        break;
                    }
                case "蛋刀":
                    {
                        itemType = "7";
                        break;
                    }
                default:
                    {
                        itemType = displayWeaponType;
                        break;
                    }
            }

            return itemType;
        }

        /// <summary>
        /// 获取武器的描述文字
        /// </summary>
        /// <param name="itemNumber">ItemNumber</param>
        /// <returns>武器的描述文字</returns>
        private string GetWeaponDescription(string itemNumber)
        {
            string description = "";

            DataRow[] rows = itemForInsertTable.Select(string.Format("ItemNumber = '{0}'", itemNumber));
            if (rows.Length > 0)
            {
                description = rows[0]["requirement"].ToString();
            }

            return description;
        }

        /// <summary>
        /// 获取武器对应的图片
        /// </summary>
        /// <param name="itemNumber">ItemNumber</param>
        /// <returns>武器对应的图片</returns>
        private Image GetWeaponImage(string itemNumber)
        {
            Image image = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT Image FROM WeaponImage WHERE ItemNumber = '{0}'", itemNumber);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                if (executeResult != null)
                {
                    byte[] imageData = (byte[])executeResult;
                    MemoryStream ms = new MemoryStream();
                    ms.Write(imageData, 0, imageData.Length);
                    image = Image.FromStream(ms);
                    ms.Close();
                }                
            }
            catch (Exception ex)
            {
                // MessageBox.Show("在读取武器图片时产生sql异常：" + ex.Message, "选择武器图片", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            // 图片缩放
            if (image != null)
            {
                int desWidth = image.Width;
                int desHeight = image.Height;

                if (desWidth > previewLabel.Width)
                {
                    desWidth = previewLabel.Width;
                }

                if (desHeight > previewLabel.Height)
                {
                    desHeight = previewLabel.Height;
                }

                if (desWidth != image.Width || desHeight != image.Height)
                {
                    Bitmap bmp = new Bitmap(desWidth, desHeight);
                    Graphics g = Graphics.FromImage(bmp);
                    g.DrawImage(image, 0, 0, desWidth, desHeight);
                    image = bmp;
                }
            }

            return image;
        }

        /// <summary>
        /// 获取武器的描述类型
        /// </summary>
        /// <param name="itemType">武器类型</param>
        /// <returns>武器的描述类型</returns>
        private string GetWeaponDisplayType(string itemType)
        {
            string weaponDisplayType = null;

            switch (itemType)
            {
                case "1":
                    {
                        weaponDisplayType = "单兵";
                        break;
                    }
                case "2":
                    {
                        weaponDisplayType = "长兵";
                        break;
                    }
                case "3":
                    {
                        weaponDisplayType = "特殊";
                        break;
                    }
                case "4":
                    {
                        weaponDisplayType = "拳套";
                        break;
                    }
                case "5":
                    {
                        weaponDisplayType = "笔";
                        break;
                    }
                case "6":
                    {
                        weaponDisplayType = "拐杖";
                        break;
                    }
                case "7":
                    {
                        weaponDisplayType = "蛋刀";
                        break;
                    }
                default:
                    {
                        weaponDisplayType = itemType;
                        break;
                    } 
            }

            return weaponDisplayType;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (dataGridViewX1.SelectedCells.Count > 0)
            {
                DataGridViewCell currentCell = dataGridViewX1.SelectedCells[0];

                if (currentCell.RowIndex >= 0)
                {
                    string itemNumber = currentCell.OwningRow.Cells["ItemNumber"].Value.ToString();
                    weaponFilePath = GetWeaponFilePath(itemNumber);
                }
                else
                {
                    weaponFilePath = "";
                }
            }
            else
            {
                weaponFilePath = "";
            }

            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 选择的ItemNumber改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = listBox1.SelectedIndex;

            if (index >= 0)
            {
                dataGridViewX1.Rows.Clear();
                string displayValue = listBox1.Items[index].ToString();
                string[] displayInfo = GetDisplayInfo(displayValue);
                string boneNumber = displayInfo[0];
                string displayWeaponType = displayInfo[1];
                string detailType = displayInfo[2];
                string itemType = GetItemType(displayWeaponType);
                string priorityField = null;

                switch (weaponType)
                {
                    case WeaponType.LeftHand:
                        {
                            priorityField = "S_LH_MeshFile";
                            break;
                        }
                    case WeaponType.RightHand:
                        {
                            priorityField = "S_RH_MeshFile";
                            break;
                        }
                }

                string sqlString = null;
                string itemForce = GetForceID();

                switch (displayWeaponType)
                {
                    case "空手": // 只允许拳套类型
                        {
                            sqlString = sqlString = string.Format("ItemType = '4' AND ([{0}] = '1' OR [{0}] = '2')", priorityField);
                            break;
                        }
                    case "双兵": // 允许为非蛋刀的任何类型
                        {
                            sqlString = string.Format("ItemType <> '7' AND ([{0}] = '1' OR [{0}] = '2')", priorityField);
                            break;
                        }                    
                    default:
                        {
                            sqlString = string.Format("ItemType = '{0}' AND ([{1}] = '1' OR [{1}] = '2')", itemType, priorityField);
                            break;
                        }
                }
                
                DataRow[] rows = itemForInsertTable.Select(sqlString);

                int rowIndex = 0;
                for (int i = 0; i < rows.Length; i++)
                {
                    if (CompareForce(rows[i]["ItemForce"].ToString(), itemForce))
                    {
                        dataGridViewX1.Rows.Add(1);
                        DataGridViewRow newRow = dataGridViewX1.Rows[rowIndex];
                        newRow.Cells["ItemNumber"].Value = rows[i]["ItemNumber"].ToString();
                        newRow.Cells["ItemType"].Value = GetWeaponDisplayType(rows[i]["ItemType"].ToString());
                        rowIndex++;
                    }                                       
                }

                // 清理掉当前的选择项
                dataGridViewX1.ClearSelection();
            }
        }

        /// <summary>
        /// 选择武器
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void dataGridViewX1_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.RowIndex >=0)
            {
                DataGridViewRow currentRow = dataGridViewX1.Rows[e.RowIndex];
                string itemNumber = currentRow.Cells["ItemNumber"].Value.ToString();
                previewLabel.Image = GetWeaponImage(itemNumber);
                textBoxX1.Text = GetWeaponDescription(itemNumber);
            }
        }
    }
}