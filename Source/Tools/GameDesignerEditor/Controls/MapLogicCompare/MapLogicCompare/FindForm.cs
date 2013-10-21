using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DevComponents.DotNetBar;

namespace MapLogicalCompare
{
    public partial class FindForm : Office2007Form
    {
        private string sectionName;
        private string keyIndex; 

        private int aiSetCount = 0;
        private int wayPointSetCount = 0;
        private int npcCount = 0;
        private int doodadCount = 0;
        private int aiGroupCount = 0;
        private int logicalPolyCount = 0;
        private int trafficPointCount = 0;
        private int trafficPointSetCount = 0;
        private int npcReviveGroupsCount = 0;
        private int doodadReviveGroupsCount = 0;
        private int npcRandomGroupsCount = 0;

        public FindForm()
        {
            InitializeComponent();
        }

        public string SectionName
        {
            get
            {
                return sectionName;
            }
        }

        public string KeyIndex
        {
            get
            {
                return keyIndex;
            }
        }

        public int AISetCount
        {
            set
            {
                aiSetCount = value;
            }
        }

        public int WayPointSetCount
        {
            set
            {
                wayPointSetCount = value;
            }
        }

        public int NpcCount
        {
            set
            {
                npcCount = value;
            }
        }

        public int DoodadCount
        {
            set
            {
                doodadCount = value;
            }
        }

        public int AIGroupCount
        {
            set
            {
                aiGroupCount = value;
            }
        }

        public int LogicalPolyCount
        {
            set
            {
                logicalPolyCount = value;
            }
        }

        public int TrafficPointCount
        {
            set
            {
                trafficPointCount = value;
            }
        }

        public int TrafficPointSetCount
        {
            set
            {
                trafficPointSetCount = value;
            }
        }

        public int NpcReviveGroupsCount
        {
            set
            {
                npcReviveGroupsCount = value;
            }
        }

        public int DoodadReviveGroupsCount
        {
            set
            {
                doodadReviveGroupsCount = value;
            }
        }

        public int NpcRandomGroupsCount
        {
            set
            {
                npcRandomGroupsCount = value;
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (comboBoxEx1.SelectedItem != null)
            {
                sectionName = comboBoxEx1.SelectedItem.ToString();
            }                        
            keyIndex = comboBoxEx2.Text;

            if (!string.IsNullOrEmpty(sectionName) && !string.IsNullOrEmpty(keyIndex))
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("查找条件不完整！", "查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
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
        /// 选择Section
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = comboBoxEx1.SelectedIndex;

            if (index >= 0)
            {
                comboBoxEx2.Items.Clear();

                switch (index)
                {
                    case 0:
                        {
                            for (int i = 0; i < aiSetCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 1:
                        {
                            for (int i = 0; i < wayPointSetCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 2:
                        {
                            for (int i = 0; i < npcCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 3:
                        {
                            for (int i = 0; i < doodadCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 4:
                        {
                            for (int i = 0; i < aiGroupCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 5:
                        {
                            for (int i = 0; i < logicalPolyCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 6:
                        {
                            for (int i = 0; i < trafficPointCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 7:
                        {
                            for (int i = 0; i < trafficPointSetCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 8:
                        {
                            for (int i = 0; i < npcReviveGroupsCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 9:
                        {
                            for (int i = 0; i < doodadReviveGroupsCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }
                    case 10:
                        {
                            for (int i = 0; i < npcRandomGroupsCount; i++)
                            {
                                comboBoxEx2.Items.Add(i.ToString());
                            }

                            break;
                        }                        
                }
            }
        }
    }
}