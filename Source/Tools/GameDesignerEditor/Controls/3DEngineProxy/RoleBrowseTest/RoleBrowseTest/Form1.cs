using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
//using atlaxLib;
using AtlKG3DEngineProxyLib;
using System.IO;

namespace RoleBrowseTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private string m_PartDirPath;
        private int m_oldX, m_oldY;

        //private Test3DEngine m_3DEngie; 
        private AtlOld3DEngine m_3DEngie;
        private ArrayList m_MdlFile;
        private ArrayList m_MdlFilePath;
        private ArrayList m_HeadFilePath;
        private ArrayList m_HandFilePath;
        private ArrayList m_LegFilePath;
        private ArrayList m_BodyFilePath;
        private ArrayList m_BeltFilePath;
        private ArrayList m_FaceFilePath;
        private ArrayList m_AniFilePath;
        private ArrayList m_ItemFilePath;
        private ArrayList m_PartFilePath;

        static String[] m_strSockets;
        static string[] m_strMdlDefaultPath;
        static string[] m_strMdlDefaultAnimationPathSuffix;
        static string[] m_strMdlDefaultFacePathSuffix;
       
        //private ArrayList test_PartFilePath;
        enum PartType { HEAD, BODY, LEG, HAND, BELT,FACE };
        enum MdlType { MT_PLAYER, MT_NPC, MT_DODEAD, MT_OTHER, MT_COUNT };
        private void Form1_Load(object sender, EventArgs e)
        {
            //ArrayList[] a = { new ArrayList(), new ArrayList(), new ArrayList(), new ArrayList(), new ArrayList(), new ArrayList() };
            //ArrayList elem = (ArrayList)a[0];
            //elem.Add("aaa");
            m_strSockets = new string[9];
            m_strSockets[0] = "s_hat"; m_strSockets[1] = "s_spine2"; m_strSockets[2] = "s_spine";
            m_strSockets[3] = "s_rp";  m_strSockets[4] = "s_lp";     m_strSockets[5] = "s_rc";
            m_strSockets[6] = "s_lc";  m_strSockets[7] = "s_rh";     m_strSockets[8] = "s_lh";
            
            m_strMdlDefaultPath = new string[(int)MdlType.MT_COUNT];
            m_strMdlDefaultPath[(int)MdlType.MT_PLAYER] = "data\\source\\player\\";
            m_strMdlDefaultPath[(int)MdlType.MT_NPC] = "data\\source\\npc\\";
            m_strMdlDefaultPath[(int)MdlType.MT_DODEAD] = "data\\source\\doodad\\";
            m_strMdlDefaultPath[(int)MdlType.MT_OTHER] = "";

            m_strMdlDefaultAnimationPathSuffix = new string[(int)MdlType.MT_COUNT];
            m_strMdlDefaultAnimationPathSuffix[(int)MdlType.MT_PLAYER] = "动作";
            m_strMdlDefaultAnimationPathSuffix[(int)MdlType.MT_NPC] = "";
            m_strMdlDefaultAnimationPathSuffix[(int)MdlType.MT_DODEAD] = "";
            m_strMdlDefaultAnimationPathSuffix[(int)MdlType.MT_OTHER] = "";

            m_strMdlDefaultFacePathSuffix = new string[(int)MdlType.MT_COUNT];
            m_strMdlDefaultFacePathSuffix[(int)MdlType.MT_PLAYER] = "部件";
            m_strMdlDefaultFacePathSuffix[(int)MdlType.MT_NPC] = "";
            m_strMdlDefaultFacePathSuffix[(int)MdlType.MT_DODEAD] = "";
            m_strMdlDefaultFacePathSuffix[(int)MdlType.MT_OTHER] = "";

            m_HeadFilePath = new ArrayList();
            m_HandFilePath = new ArrayList();
            m_LegFilePath = new ArrayList();
            m_BodyFilePath = new ArrayList();
            m_BeltFilePath = new ArrayList();
            m_FaceFilePath = new ArrayList();
            m_AniFilePath = new ArrayList();
            m_ItemFilePath = new ArrayList();
            m_MdlFile = new ArrayList();
            m_MdlFilePath = new ArrayList();
            m_3DEngie = new AtlOld3DEngine();

            StringBuilder sb = new StringBuilder();
            FileFolderHelper.ReadIniFile("General", "3DEnginePath", "", sb, 255, Application.StartupPath + "/GameDesingerTools.ini");
            string EngineDir = sb.ToString();
            m_3DEngie.Init((int)this.pictureBox1.Handle, EngineDir, Application.StartupPath);

            comboBox_Type.SelectedIndex = 0;
            RefreshModel();
            ItemsRefresh();
            this.Player_comboBox.SelectedIndex = 0;
            if (Ani_listBox.Items.Count > 0)
            {
                this.Ani_listBox.SelectedIndex = 0;
            }
                     
        }
        private string GetAnimationPath()
        {
            int nType = comboBox_Type.SelectedIndex;
            switch ((MdlType)nType)
            {
                case MdlType.MT_PLAYER:
                case MdlType.MT_NPC:
                    {
                        string strMdlFileName = m_MdlFile[Player_comboBox.SelectedIndex].ToString();
                        int nPos = strMdlFileName.LastIndexOf('.');
                        string strMdlDirName = strMdlFileName.Substring(0, nPos);
                        return Application.StartupPath + "\\" + m_strMdlDefaultPath[nType] + strMdlDirName + "\\" + m_strMdlDefaultAnimationPathSuffix[nType];
                    }
                case MdlType.MT_DODEAD:
                    {
                        char[] Separators = {'\\', '/'};
                        string strMdlFileName = m_MdlFilePath[Player_comboBox.SelectedIndex].ToString();
                        int nPos = strMdlFileName.LastIndexOfAny(Separators);
                        return strMdlFileName.Substring(0, nPos);
                    }
            }
            return null;
        }

        private void RefreshFaceModel()
        {
            int nType = comboBox_Type.SelectedIndex;
            m_FaceFilePath.Clear();
            comboBox_Face.Items.Clear();
            string strMdlFileName = m_MdlFile[Player_comboBox.SelectedIndex].ToString();
            int nPos = strMdlFileName.LastIndexOf('.');
            string strMdlDirName = strMdlFileName.Substring(0, nPos);
            string strFaceStartPath = Application.StartupPath + "\\" + m_strMdlDefaultPath[nType] + strMdlDirName + "\\" + m_strMdlDefaultFacePathSuffix[nType];
            string[] strFaces = FindModelInDirectory(strFaceStartPath, "mesh", "_face");
            if (strFaces != null)
            {
                for (int i = 0; i < strFaces.Length; i++)
                {
                    char[] Separators = { '\\', '/' };
                    nPos = strFaces[i].LastIndexOfAny(Separators);
                    int nPosEnd = strFaces[i].LastIndexOf('.');
                    if (nPos == -1)
                    {
                        comboBox_Face.Items.Add(strFaces[i].Substring(0, nPosEnd));
                    }
                    else
                    {
                        comboBox_Face.Items.Add(strFaces[i].Substring(nPos + 1, nPosEnd - nPos - 1));
                    }
                    m_FaceFilePath.Add(strFaces[i]);
                }
                if (comboBox_Face.Items.Count > 0)
                {
                    comboBox_Face.SelectedIndex = 0;
                }
            }
        }
        private void RefreshModel()
        {
            string[] strSuffix = { "mdl", "mdl", "mesh", "mesh" };
            string[] strModelStartPart = {
                "data\\public\\PlayerCheckTool", 
                "data\\source\\npc\\",
                "data\\source\\doodad\\",
                ""};
            int nType = comboBox_Type.SelectedIndex;
            m_PartDirPath = PlayerPathText.Text;
            m_MdlFile.Clear();
            m_MdlFilePath.Clear();
            Player_comboBox.Items.Clear();
            string[] strMdlFiles = FindModelInDirectory(Application.StartupPath + "\\" + strModelStartPart[nType], strSuffix[nType], "");
            if (strMdlFiles == null)
                return;
            for (int i = 0; i < strMdlFiles.Length; i++)
            {
                char[] separators = { '\\', '/' };
                int pos = strMdlFiles[i].LastIndexOfAny(separators);

                if (pos == -1)
                {
                    if (String.Compare(strMdlFiles[i], ".svn", true) != 0)
                        m_MdlFile.Add(strMdlFiles[i]);
                }
                else
                {
                    if (String.Compare(strMdlFiles[i].Substring(pos + 1), ".svn", true) != 0)
                    {
                        m_MdlFile.Add(strMdlFiles[i].Substring(pos + 1));
                        m_MdlFilePath.Add(strMdlFiles[i]);
                    }
                }
            }            
            for (int i = 0; i < m_MdlFile.Count; i++)
            {
                Player_comboBox.Items.Add(m_MdlFile[i]);
            }
            if (Player_comboBox.Items.Count > 0)
            {
                Player_comboBox.SelectedIndex = 0;
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            m_3DEngie.FrameMove();
            m_3DEngie.Render();
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            int xoff, yoff;
            if (m_3DEngie == null)
                return;
            if(e.Button == MouseButtons.Left)
            {
                xoff = m_oldX - e.X;
                yoff = m_oldY - e.Y;

                m_3DEngie.ZoomView(yoff);   
            }
            m_oldX = e.X;
            m_oldY = e.Y;
            
        }

        public string[] FindModelInDirectory(string strStartPath, string strSuffix, string strKeyword)
        {
            if (!System.IO.Directory.Exists(strStartPath))
                return null;
            return System.IO.Directory.GetFiles(strStartPath, "*" + strKeyword + "." + strSuffix, System.IO.SearchOption.AllDirectories);
        }

    

        public void GetAllFiles(string parentDir)
        {
           // char[] separators = { '\\', '/' };
            int pos;
            string tempfile;
                           
            parentDir = parentDir + Player_comboBox.Text;

            if (!System.IO.Directory.Exists(parentDir))
                return;
            string[] file = System.IO.Directory.GetFiles(parentDir, "*.*", System.IO.SearchOption.AllDirectories);
            for (int i = 0; i < file.Length; i++)
            {
                
               // startpos = file[i].LastIndexOfAny(separators)+1;

                pos = file[i].LastIndexOf('_');
                tempfile = file[i].Substring(pos + 1);

                if(String.Compare(tempfile ,"head.mesh",true) == 0)
                    m_HeadFilePath.Add(file[i]);
                if(String.Compare(tempfile ,"body.mesh",true) == 0)
                    m_BodyFilePath.Add(file[i]);
                if(String.Compare(tempfile ,"leg.mesh",true) == 0)
                    m_LegFilePath.Add(file[i]);
                if(String.Compare(tempfile ,"hand.mesh",true) == 0)
                    m_HandFilePath.Add(file[i]);
                if(String.Compare(tempfile ,"belt.mesh",true) == 0)
                    m_BeltFilePath.Add(file[i]);
                if (String.Compare(tempfile, "face.mesh", true) == 0)
                    m_FaceFilePath.Add(file[i]);
                
                pos = file[i].LastIndexOf('.');
                tempfile = file[i].Substring(pos);
                if (String.Compare(tempfile, ".ani", true) == 0)
                    m_AniFilePath.Add(file[i]);
                  
            }
            
        }

        private void comboBox_Player_SelectedIndexChanged(object sender, EventArgs e)
        {
            AllClear();
            GetAllFiles(m_PartDirPath);
            comboBox_Part_SelectedIndexChanged(sender, e);
            ChangePlayer();
            if (Ani_listBox.Items.Count > 0)
            {
                Ani_listBox.SelectedIndex = 0;
                Ani_listBox_SelectedIndexChanged(sender, e);
            }
            string strAnimationPath = GetAnimationPath();
            RefreshAnimation(strAnimationPath);
            RefreshFaceModel();
        }

        private void comboBox_Part_SelectedIndexChanged(object sender, EventArgs e)
        {
            
           //
            ChangeRefresh();
        }
        private void AllClear()
        {
            
            m_HandFilePath.Clear();
            m_HeadFilePath.Clear();
            m_FaceFilePath.Clear();
            m_BeltFilePath.Clear();
            m_BodyFilePath.Clear();
            m_LegFilePath.Clear();
            m_AniFilePath.Clear();
        }
       
        private void RefreshAnimation(string strStartDirectory)
        {
            Ani_listBox.Items.Clear();
            m_AniFilePath.Clear();
            string[] strAnimations = System.IO.Directory.GetFiles(strStartDirectory, "*.ani", System.IO.SearchOption.AllDirectories);
            for (int i = 0; i < strAnimations.Length; i++)
            {
                char[] Separators = { '\\', '/'}; 
                int nPosStart = strAnimations[i].LastIndexOfAny(Separators);
                int nPosEnd = strAnimations[i].LastIndexOf('.');
                Ani_listBox.Items.Add(strAnimations[i].Substring(nPosStart + 1, nPosEnd - nPosStart - 1));
                m_AniFilePath.Add(strAnimations[i]);
            }
            if (Ani_listBox.Items.Count > 0)
            {
                Ani_listBox.SelectedIndex = 0;
            }
        }
        private void ChangePlayer()
        {
            /*if(m_HeadFilePath.Count > 0)
                m_3DEngie.ChangeModelPart(m_HeadFilePath[0].ToString(), 0);
            if (m_BodyFilePath.Count > 0)
                m_3DEngie.ChangeModelPart(m_BodyFilePath[0].ToString(), 1);
            if (m_LegFilePath.Count > 0)
                m_3DEngie.ChangeModelPart(m_LegFilePath[0].ToString(), 2);
            if (m_HandFilePath.Count > 0)
                m_3DEngie.ChangeModelPart(m_HandFilePath[0].ToString(), 3);
            if (m_BeltFilePath.Count > 0)
                m_3DEngie.ChangeModelPart(m_BeltFilePath[0].ToString(), 4);
            if (m_FaceFilePath.Count > 0)
                m_3DEngie.LoadFace(m_FaceFilePath[0].ToString());*/
            if (m_MdlFile.Count > 0)
                m_3DEngie.LoadPlayerModel(m_MdlFilePath[Player_comboBox.SelectedIndex].ToString());

        }
        private void ChangeRefresh()
        {
            PartlistBox.Items.Clear();
            Ani_listBox.Items.Clear();
            char[] separators = { '\\', '/' };
            int startpos;
            string tempStr;
            switch ((PartType)Part_comboBox.SelectedIndex)
            {
                case PartType.HEAD:
                    m_PartFilePath = m_HeadFilePath;
                    for (int i = 0; i < m_HeadFilePath.Count; i++)
                    {
                        tempStr = m_HeadFilePath[i].ToString();
                        startpos = tempStr.LastIndexOfAny(separators) + 1;
                        if (startpos == -1)
                            startpos = 0;
                        this.PartlistBox.Items.Add(tempStr.Substring(startpos));
                    }
                    break;
                case PartType.BODY:
                    m_PartFilePath = m_BodyFilePath;
                    for (int i = 0; i < m_BodyFilePath.Count; i++)
                    {
                        tempStr = m_BodyFilePath[i].ToString();
                        startpos = tempStr.LastIndexOfAny(separators) + 1;
                        if (startpos == -1)
                            startpos = 0;
                        this.PartlistBox.Items.Add(tempStr.Substring(startpos));
                    }
                    break;
                case PartType.LEG:
                    m_PartFilePath = m_LegFilePath;
                    for (int i = 0; i < m_LegFilePath.Count; i++)
                    {
                        tempStr = m_LegFilePath[i].ToString();
                        startpos = tempStr.LastIndexOfAny(separators) + 1;
                        if (startpos == -1)
                            startpos = 0;
                        this.PartlistBox.Items.Add(tempStr.Substring(startpos));
                    }
                    break;
                case PartType.HAND:
                    m_PartFilePath = m_HandFilePath;
                    for (int i = 0; i < m_HandFilePath.Count; i++)
                    {
                        tempStr = m_HandFilePath[i].ToString();
                        startpos = tempStr.LastIndexOfAny(separators) + 1;
                        if (startpos == -1)
                            startpos = 0;
                        this.PartlistBox.Items.Add(tempStr.Substring(startpos));
                    }
                    break;
                case PartType.BELT:
                    m_PartFilePath = m_BeltFilePath;
                    for (int i = 0; i < m_BeltFilePath.Count; i++)
                    {
                        tempStr = m_BeltFilePath[i].ToString();
                        startpos = tempStr.LastIndexOfAny(separators) + 1;
                        if (startpos == -1)
                            startpos = 0;
                        this.PartlistBox.Items.Add(tempStr.Substring(startpos));
                    }
                    break;
                case PartType.FACE:
                    m_PartFilePath = m_FaceFilePath;
                    for (int i = 0; i < m_FaceFilePath.Count; i++)
                    {
                        tempStr = m_FaceFilePath[i].ToString();
                        startpos = tempStr.LastIndexOfAny(separators) + 1;
                        if (startpos == -1)
                            startpos = 0;
                        this.PartlistBox.Items.Add(tempStr.Substring(startpos));
                    }
                    break;
            }
            
            for (int i = 0; i < m_AniFilePath.Count; i++)
            {
                tempStr = m_AniFilePath[i].ToString();
                startpos = tempStr.LastIndexOfAny(separators) + 1;
                if (startpos == -1)
                    startpos = 0;
                this.Ani_listBox.Items.Add(tempStr.Substring(startpos));
            }

        }
        
        private void PartlistBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            int parttype = PartlistBox.SelectedIndex;
            if (parttype < 0)
                return; 
            if(Part_comboBox.SelectedIndex == 5 )
                m_3DEngie.LoadFace(m_PartFilePath[PartlistBox.SelectedIndex].ToString());
            else
                m_3DEngie.ChangeModelPart(m_PartFilePath[PartlistBox.SelectedIndex].ToString(), Part_comboBox.SelectedIndex);
        }

        private void button1_Click(object sender, EventArgs e)
        {

            string tempPath = m_PartDirPath;
            m_PartDirPath = PlayerPathText.Text;
            char[] separators = { '\\', '/' };
            int pos;
            pos = m_PartDirPath.LastIndexOfAny(separators);
            if (pos != m_PartDirPath.Length-1)
            {
                m_PartDirPath = m_PartDirPath + "\\";
                PlayerPathText.Text = m_PartDirPath;
            }
            if(!System.IO.Directory.Exists(m_PartDirPath))
            {
                string str = "<" + m_PartDirPath +">" + "目录不存在!" ;
                MessageBox.Show(str);
                m_PartDirPath = tempPath;
                PlayerPathText.Text = m_PartDirPath;
                return;
            }
            m_MdlFile.Clear();
            this.Player_comboBox.Items.Clear();
            //FindModelInDirectory(m_PartDirPath);
            for (int i = m_MdlFile.Count - 1; i >= 0; i--)
                this.Player_comboBox.Items.Add(m_MdlFile[i]);
            this.Player_comboBox.SelectedIndex = 0;
            ItemsRefresh();
        }

        private void Ani_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            
        }

        private void ItemsRefresh()
        {
            if (comboBox_Type.SelectedIndex == (int)MdlType.MT_DODEAD)
                return;
            if (!System.IO.Directory.Exists(Item_textBox.Text))
            {
                string str = "<" + Item_textBox.Text + ">" + "目录不存在!";
                MessageBox.Show(str);
                
                return;
            }
            char[] separators = { '\\', '/' };
            int pos;
            Item_listBox.Items.Clear();
            string[] file = System.IO.Directory.GetFiles(Item_textBox.Text, "*.Mesh", System.IO.SearchOption.AllDirectories);
            for(int i=0; i < file.Length; i++)
            {
                m_ItemFilePath.Add(file[i]);
                pos = file[i].LastIndexOfAny(separators)+1;
                if (pos == -1)
                    pos = 0;
                Item_listBox.Items.Add(file[i].Substring(pos));
            }
        }

        private void Item_listBox_SelectedIndexChanged(object sender, EventArgs e)
        {  
            int ItemID = Item_listBox.SelectedIndex;
            if (Socket_comboBox.SelectedIndex < 0)
            {
                MessageBox.Show("请先选择插槽!");
                return;
            }
            if(ItemID < 0)
                return ;
            if(SingleItems_radioButton.Checked)
                m_3DEngie.ItemsClear();
            m_3DEngie.LoadPluginModel(m_ItemFilePath[ItemID].ToString(), m_strSockets[Socket_comboBox.SelectedIndex]);
        }

        private void ItemsClearbutton_Click(object sender, EventArgs e)
        {
            //m_3DEngie.ItemsClear();
            m_3DEngie.LoadMaterial("E:\\work\\Sword3\\Sword3-Products\\trunk\\client\\data\\source\\doodad\\稻香村\\GM_凝血草001.Mtl");
        }

        private void SingleItems_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            m_3DEngie.ItemsClear();
        }

        private void Ani_listBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (Ani_listBox.SelectedIndex < 0)
                return;
            m_3DEngie.LoadAni(m_AniFilePath[Ani_listBox.SelectedIndex].ToString(), 1);
        }

        private void Save_button_Click(object sender, EventArgs e)
        {
            if(this.saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (saveFileDialog1.FileName.Trim() != "")
                {
                    string filename = this.saveFileDialog1.FileName;
                    m_3DEngie.SaveMDLToFile(filename);
                }
            }
        }

        private void comboBox_Face_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_3DEngie.LoadPluginModel(m_FaceFilePath[comboBox_Face.SelectedIndex].ToString(), "s_face");
        }

        private void comboBox_Type_SelectedIndexChanged(object sender, EventArgs e)
        {
            Boolean[] EnableControlState = {false, false, false, true};
            PlayerPathText.Enabled = EnableControlState[comboBox_Type.SelectedIndex];
            Part_comboBox.Enabled = !EnableControlState[comboBox_Type.SelectedIndex];
            PartlistBox.Enabled = !EnableControlState[comboBox_Type.SelectedIndex];
            RefreshModel();
        }
        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (m_3DEngie != null)
            {
                m_3DEngie.UnInit();
                m_3DEngie = null;
            }
        }
    }
}