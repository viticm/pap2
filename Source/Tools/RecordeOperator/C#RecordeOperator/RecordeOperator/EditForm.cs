using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using AtlKGRecordeOperatorLib;

namespace RecordeOperator
{
    public partial class EditForm : Form
    {
        string m_strType = null;

        private int[] nCount = new int[18];

        Label[] aLabel = new Label[6];
        TextBox[] aTextBox = new TextBox[6];
        Button Button1 = new Button();

        string m_strValue = null;
        string m_strName  = null;
        int    m_nNum     = -1;
        
        public EditForm()
        {
            InitializeComponent();
        }

        ~EditForm()
        {

        }

        public void Init(string strType)
        {
            m_strType = strType;

            if (strType == "Model" || strType == "ModelST" || strType == "SFX")
            {
                ListBox.Items.Add("对象加载属性");
                ListBox.Items.Add("设置位置属性");
                ListBox.Items.Add("设置旋转属性");
                ListBox.Items.Add("设置旋转中心属性");
                ListBox.Items.Add("设置缩放属性");
                ListBox.Items.Add("设置缩放中心属性");
                ListBox.Items.Add("设置缩放旋转属性");
                ListBox.Items.Add("重置位置缩放旋转");
                ListBox.Items.Add("绑定关节属性");
                ListBox.Items.Add("绑定骨骼属性");
                ListBox.Items.Add("绑定对象属性");
                ListBox.Items.Add("分离对象属性");
                ListBox.Items.Add("播放动画属性");
                ListBox.Items.Add("播放SPLIT动画属性");
                ListBox.Items.Add("加载材质选项");
                ListBox.Items.Add("设置对象透明属性");
                ListBox.Items.Add("设置对象回调函数");
                ListBox.Items.Add("对象析构");

                InitCount();
            }
            else if (strType == "Camera")
            {
                ListBox.Items.Add("设置镜头位置");
                ListBox.Items.Add("设置镜头目标位置");
                ListBox.Items.Add("设置镜头上方向");
                //FT_KG3DCAMERA_SETTRACKINFO
                //FT_KG3DCAMERA_UPDATECAMERA
                //FT_KG3DCAMERA_SETGLOBALPERSPECTIVE
                //FT_KG3DCAMERA_SETGLOBALORTHOGONAL

                InitCount();
            }
            else if (strType == "SceneEditor")
            {
                ListBox.Items.Add("场景加载属性");
                ListBox.Items.Add("场景析构");
                ListBox.Items.Add("加入渲染对象");
                ListBox.Items.Add("删除渲染对象");
                //FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW

                InitCount();
            }

            aLabel[0] = new Label();
            aLabel[1] = new Label();
            aLabel[2] = new Label();
            aLabel[3] = new Label();
            aLabel[4] = new Label();
            aLabel[5] = new Label();

            aTextBox[0] = new TextBox();
            aTextBox[1] = new TextBox();
            aTextBox[2] = new TextBox();
            aTextBox[3] = new TextBox();
            aTextBox[4] = new TextBox();
            aTextBox[5] = new TextBox();
        }

        private void InitCount()
        {
            if (m_strType == "Model" || m_strType == "ModelST" || m_strType == "SFX")
            {
                nCount[7] = nCount[16] = nCount[17] = 0;
                nCount[10] = nCount[11] = 1;
                nCount[8] = nCount[9] = nCount[14] = nCount[15] = 2;
                nCount[0] = nCount[1] = nCount[3] = nCount[4] = nCount[5] = 3;
                nCount[2] = nCount[6] = nCount[12] = 4;
                nCount[13] = 6;
            }
            else if (m_strType == "Camera")
            {
                nCount[0] = nCount[1] = nCount[2] = 3;
            }
            else if (m_strType == "SceneEditor")
            {
                nCount[0] = nCount[1] = nCount[2] = 3;
            }
            return;
        }

        private void ListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            int nIndex = ListBox.SelectedIndex;
            PanelShow(nIndex);

            if (m_strType == "Model" || m_strType == "ModelST" || m_strType == "SFX")
            {
                switch (nIndex)
                {
                    case 0:
                        aLabel[0].Text = "加载文件路径";
                        aLabel[1].Text = "加载文件哈希";
                        aLabel[2].Text = "加载文件选项";
                        Button1.Text = "...";
                        panel1.Controls.Add(Button1);
                        aLabel[0].Size = new System.Drawing.Size(77, 12);
                        aLabel[1].Size = new System.Drawing.Size(77, 12);
                        aLabel[2].Size = new System.Drawing.Size(77, 12);
                        aTextBox[0].Location = new System.Drawing.Point(110, 30);
                        aTextBox[1].Location = new System.Drawing.Point(110, 80);
                        aTextBox[2].Location = new System.Drawing.Point(110, 130);
                        Button1.Location = new System.Drawing.Point(216, 27);
                        Button1.Size = new System.Drawing.Size(40, 25);
                        break;
                    case 1:
                        aLabel[0].Text = "位置x分量";
                        aLabel[1].Text = "位置y分量";
                        aLabel[2].Text = "位置z分量";
                        break;
                    case 2:
                        aLabel[0].Text = "旋转x分量";
                        aLabel[1].Text = "旋转y分量";
                        aLabel[2].Text = "旋转z分量";
                        aLabel[3].Text = "旋转w分量";
                        break;
                    case 3:
                        aLabel[0].Text = "旋转中心x分量";
                        aLabel[1].Text = "旋转中心y分量";
                        aLabel[2].Text = "旋转中心z分量";
                        break;
                    case 4:
                        aLabel[0].Text = "缩放x分量";
                        aLabel[1].Text = "缩放y分量";
                        aLabel[2].Text = "缩放z分量";
                        break;
                    case 5:
                        aLabel[0].Text = "缩放中心x分量";
                        aLabel[1].Text = "缩放中心y分量";
                        aLabel[2].Text = "缩放中心z分量";
                        break;
                    case 6:
                        aLabel[0].Text = "缩放旋转x分量";
                        aLabel[1].Text = "缩放旋转y分量";
                        aLabel[2].Text = "缩放旋转z分量";
                        aLabel[3].Text = "旋转旋转w分量";
                        break;
                    case 7:
                        break;
                    case 8:
                        aLabel[0].Text = "绑定到对象ID";
                        aLabel[1].Text = "绑定到关节名称";
                        break;
                    case 9:
                        aLabel[0].Text = "绑定到对象ID";
                        aLabel[1].Text = "绑定到骨骼名称";
                        break;
                    case 10:
                        aLabel[0].Text = "绑定到对象ID";
                        break;
                    case 11:
                        aLabel[0].Text = "分离对象ID";
                        break;
                    case 12:
                        aLabel[0].Text = "播放类型";
                        aLabel[1].Text = "动画路径";
                        aLabel[2].Text = "播放速度";
                        aLabel[3].Text = "开始时间";
                        break;
                    case 13:
                        aLabel[0].Text = "动画路径";
                        aLabel[1].Text = "SPLIT类型";
                        aLabel[2].Text = "附加信息";
                        aLabel[3].Text = "播放类型";
                        aLabel[4].Text = "播放速度";
                        aLabel[5].Text = "开始时间";
                        break;
                    case 14:
                        aLabel[0].Text = "材质路径";
                        aLabel[1].Text = "材质选项";
                        break;
                    case 15:
                        aLabel[0].Text = "设置透明度";
                        aLabel[1].Text = "是否使用特殊模式";
                        break;
                    case 16:
                    case 17:
                        break;
                    default:
                        break;
                }
            }
            else if (m_strType == "Camera")
            {
                switch (nIndex)
                {
                    case 0:
                        aLabel[0].Text = "镜头位置x分量";
                        aLabel[1].Text = "镜头位置y分量";
                        aLabel[2].Text = "镜头位置z分量";
                        break;
                    case 1:
                        aLabel[0].Text = "镜头目标位置x分量";
                        aLabel[1].Text = "镜头目标位置y分量";
                        aLabel[2].Text = "镜头目标位置z分量";
                        break;
                    case 2:
                        aLabel[0].Text = "镜头上方向x分量";
                        aLabel[1].Text = "镜头上方向y分量";
                        aLabel[2].Text = "镜头上方向z分量";
                        break;
                    default:
                        break;
                }
            }
            else if (m_strType == "SceneEditor")
            {
                switch (nIndex)
                {
                    case 0:
                        aLabel[0].Text = "加载文件路径";
                        aLabel[1].Text = "加载文件哈希";
                        aLabel[2].Text = "加载文件选项";
                        Button1.Text = "...";
                        panel1.Controls.Add(Button1);
                        aLabel[0].Size = new System.Drawing.Size(77, 12);
                        aLabel[1].Size = new System.Drawing.Size(77, 12);
                        aLabel[2].Size = new System.Drawing.Size(77, 12);
                        aTextBox[0].Location = new System.Drawing.Point(110, 30);
                        aTextBox[1].Location = new System.Drawing.Point(110, 80);
                        aTextBox[2].Location = new System.Drawing.Point(110, 130);
                        Button1.Location = new System.Drawing.Point(216, 27);
                        Button1.Size = new System.Drawing.Size(40, 25);
                        break;
                    case 1:
                        break;
                    case 2:
                    case 3:
                        aLabel[0].Text = "对象ID";
                        aLabel[1].Text = "类别";
                        aLabel[2].Text = "是否可选择";
                        break;
                    default:
                        break;
                }
            }
        }

        private void PanelShow(int nIndex)
        {
            panel1.Controls.Clear();
            switch (nCount[nIndex])
            {
                case 0:
                    break;
                case 1:
                    panel1.Controls.Add(aLabel[0]);
                    panel1.Controls.Add(aTextBox[0]);
                    aLabel[0].Location = new System.Drawing.Point(17, 33);
                    aLabel[0].Size = new System.Drawing.Size(107, 12);
                    aTextBox[0].Location = new System.Drawing.Point(140, 30);
                    break;
                case 2:
                    panel1.Controls.Add(aLabel[0]);
                    panel1.Controls.Add(aLabel[1]);
                    panel1.Controls.Add(aTextBox[0]);
                    panel1.Controls.Add(aTextBox[1]);
                    aLabel[0].Location = new System.Drawing.Point(17, 41);
                    aLabel[0].Size = new System.Drawing.Size(107, 12);
                    aLabel[1].Location = new System.Drawing.Point(17, 111);
                    aLabel[1].Size = new System.Drawing.Size(107, 12);
                    aTextBox[0].Location = new System.Drawing.Point(140, 38);
                    aTextBox[1].Location = new System.Drawing.Point(140, 108);
                    break;
                case 3:
                    panel1.Controls.Add(aLabel[0]);
                    panel1.Controls.Add(aLabel[1]);
                    panel1.Controls.Add(aLabel[2]);
                    panel1.Controls.Add(aTextBox[0]);
                    panel1.Controls.Add(aTextBox[1]);
                    panel1.Controls.Add(aTextBox[2]);
                    aLabel[0].Location = new System.Drawing.Point(17, 33);
                    aLabel[0].Size = new System.Drawing.Size(107, 12);
                    aLabel[1].Location = new System.Drawing.Point(17, 83);
                    aLabel[1].Size = new System.Drawing.Size(107, 12);
                    aLabel[2].Location = new System.Drawing.Point(17, 133);
                    aLabel[2].Size = new System.Drawing.Size(107, 12);
                    aTextBox[0].Location = new System.Drawing.Point(140, 30);
                    aTextBox[1].Location = new System.Drawing.Point(140, 80);
                    aTextBox[2].Location = new System.Drawing.Point(140, 130);
                    break;
                case 4:
                    panel1.Controls.Add(aLabel[0]);
                    panel1.Controls.Add(aLabel[1]);
                    panel1.Controls.Add(aLabel[2]);
                    panel1.Controls.Add(aLabel[3]);
                    panel1.Controls.Add(aTextBox[0]);
                    panel1.Controls.Add(aTextBox[1]);
                    panel1.Controls.Add(aTextBox[2]);
                    panel1.Controls.Add(aTextBox[3]);
                    aLabel[0].Location = new System.Drawing.Point(17, 30);
                    aLabel[0].Size = new System.Drawing.Size(107, 12);
                    aLabel[1].Location = new System.Drawing.Point(17, 70);
                    aLabel[1].Size = new System.Drawing.Size(107, 12);
                    aLabel[2].Location = new System.Drawing.Point(17, 110);
                    aLabel[2].Size = new System.Drawing.Size(107, 12);
                    aLabel[3].Location = new System.Drawing.Point(17, 150);
                    aLabel[3].Size = new System.Drawing.Size(107, 12);
                    aTextBox[0].Location = new System.Drawing.Point(140, 27);
                    aTextBox[1].Location = new System.Drawing.Point(140, 67);
                    aTextBox[2].Location = new System.Drawing.Point(140, 107);
                    aTextBox[3].Location = new System.Drawing.Point(140, 147);
                    break;
                case 6:
                    panel1.Controls.Add(aLabel[0]);
                    panel1.Controls.Add(aLabel[1]);
                    panel1.Controls.Add(aLabel[2]);
                    panel1.Controls.Add(aLabel[3]);
                    panel1.Controls.Add(aLabel[4]);
                    panel1.Controls.Add(aLabel[5]);
                    panel1.Controls.Add(aTextBox[0]);
                    panel1.Controls.Add(aTextBox[1]);
                    panel1.Controls.Add(aTextBox[2]);
                    panel1.Controls.Add(aTextBox[3]);
                    panel1.Controls.Add(aTextBox[4]);
                    panel1.Controls.Add(aTextBox[5]);
                    aLabel[0].Location = new System.Drawing.Point(17, 15);
                    aLabel[0].Size = new System.Drawing.Size(107, 12);
                    aLabel[1].Location = new System.Drawing.Point(17, 46);
                    aLabel[1].Size = new System.Drawing.Size(107, 12);
                    aLabel[2].Location = new System.Drawing.Point(17, 77);
                    aLabel[2].Size = new System.Drawing.Size(107, 12);
                    aLabel[3].Location = new System.Drawing.Point(17, 108);
                    aLabel[3].Size = new System.Drawing.Size(107, 12);
                    aLabel[4].Location = new System.Drawing.Point(17, 139);
                    aLabel[4].Size = new System.Drawing.Size(107, 12);
                    aLabel[5].Location = new System.Drawing.Point(17, 170);
                    aLabel[5].Size = new System.Drawing.Size(107, 12);
                    aTextBox[0].Location = new System.Drawing.Point(140, 12);
                    aTextBox[1].Location = new System.Drawing.Point(140, 43);
                    aTextBox[2].Location = new System.Drawing.Point(140, 74);
                    aTextBox[3].Location = new System.Drawing.Point(140, 105);
                    aTextBox[4].Location = new System.Drawing.Point(140, 136);
                    aTextBox[5].Location = new System.Drawing.Point(140, 167);
                    break;
                default:
                    break;
            }
        }

        private void EventOK_Click(object sender, EventArgs e)
        {
            int nIndex = ListBox.SelectedIndex;
            m_strValue = null;
            m_strName  = null;
            m_nNum     = -1;
            if (nIndex < 0)
            {
                this.Close();
                return;
            }

            m_strName = ListBox.Items[nIndex].ToString();
            m_nNum = nCount[nIndex];
            for (int i = 0; i < m_nNum; i++)
            {
                m_strValue += aTextBox[i].Text;
                m_strValue += "\n";
            }
            this.Close();
        }

        private void Cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        public int GetNum()
        {
            return m_nNum;
        }

        public string GetName()
        {
            return m_strName;
        }

        public string GetValue()
        {
            return m_strValue;
        }
    }
}