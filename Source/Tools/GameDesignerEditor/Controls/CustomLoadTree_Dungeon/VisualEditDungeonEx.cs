using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DataTableRemotingClient;
using System.Collections;

namespace CustomLoadTree_Dungeon
{
    public partial class VisualEditDungeonEx : Form
    {
        public VisualEditDungeonEx(SqlConnection Conn, Class1 parentClass, string RootDir, bool bImageSupport, int nFont)
        {
            InitializeComponent();

            m_bImageSupport = bImageSupport;
            m_Root = RootDir;
            m_bHasChanged = false;
            m_Parent = parentClass;
            m_strTabName = "tbl_Guide_Dungeon";
            m_Conn = Conn;
            m_LastTags = new Object[2];
			m_nDefFont = nFont;
            font = new FontHelper(m_Root + FontFile, m_Root + FontListFile, m_Root + ColorFile, m_Root);

            string[] FontNames = font.GetAllFontName();

            
            foreach (string strName in FontNames)
            {
                FontBox.Items.Add(strName);
            }
            FontBox.Items.Add("默认(" + m_nDefFont.ToString() + ")");

            m_nLastLength = 0;
            MarkArray = new ArrayList();
            ImageArray = new ArrayList();
            LinkArray = new ArrayList();

            defFont = font.GetFontByID(m_nDefFont);
            defColor = font.GetColorByID(m_nDefFont);
            textBox.SelectionColor = defColor;
            textBox.SelectionFont = defFont;

            refreshText();
        }

        public string Code
        {
            set 
            { 
                richTextBox1.Text = value;
            }

            get 
            {
                return richTextBox1.Text; 
            }
        }

        public string LinkCode
        {
            set
            {
                richTextBox4.Text = value;
            }

            get
            {
                return richTextBox4.Text;
            }
        }

        public RichTextBox textBox
        {
            get { return richTextBox2; }
        }

        public RichTextBox linkBox
        {
            get { return richTextBox3; }
        }

        public ComboBox FontBox
        {
            get { return comboBox1; }
        }

        public TabControl Table
        {
            get { return tabControl1; }
        }

        FontHelper font;
        string m_Root;
        string FontFile = "\\ui\\Scheme\\Elem\\Font.ini";
        string FontListFile = "\\ui\\Scheme\\Elem\\FontList.ini";
        string ColorFile = "\\ui\\Scheme\\Elem\\Color.txt";

        string textMark = "<text>text=\"";
        string fontMark = "\" font=";
        string fontEndMark = "</text>";

        string imageMark = "<image>path=\"";
        string frameMark = "\" frame=";
        string widthMark = " w=";
        string heightMark = " h=";
        string frameEndMark = "</image>";

        string linkMark = " link=\"";
        string linkEndMark = "\"</text>";

        SqlConnection m_Conn;
        Class1 m_Parent;
        Font defFont;
        Color defColor;
        public ArrayList LinkArray;
        ArrayList MarkArray;
        ArrayList ImageArray;
        int m_nLastLength;
        const int ImageMark = - 500; 
        bool m_bImageSupport;
        bool m_bHasChanged;
        string m_strTabName;
        Object[] m_LastTags;
		int m_nDefFont;

        private void button1_Click(object sender, EventArgs e)
        {
            refreshCode();
            refreshText();
            DialogResult = DialogResult.OK;
            if (MarkArray.Contains(-1 as Object))
            {
                MessageBox.Show("有未加标签的字符，请确认所有字符已经选择了字体。");
            }

            this.Hide();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Hide();
        }

        private void richTextBox2_TextChanged(object sender, EventArgs e)
        {
            // refreshCode();
            if (m_nLastLength < textBox.Text.Length)    //add
            {
                int nPos = textBox.SelectionStart - (textBox.Text.Length - m_nLastLength);
                int nIndex = nPos;

                for (int i = 0; i < textBox.Text.Length - m_nLastLength; i++)
                {
                    MarkArray.Insert(nPos++, m_nDefFont as Object);
                }

                textBox.Select(nIndex, textBox.Text.Length - m_nLastLength);
                textBox.SelectionFont = defFont;
                textBox.SelectionColor = defColor;
                textBox.Select(nPos, 0);
             }
            else        //del
            {
                int nPos = textBox.SelectionStart;

                for (int i = 0; i < m_nLastLength - textBox.Text.Length; i++)
                {
                    MarkArray.RemoveAt(nPos);
                }
            }

            m_bHasChanged = true;
            m_nLastLength = textBox.Text.Length;
        }

        public void refreshCode()
        {
            Code = string.Empty;
            if (MarkArray.Count != textBox.Text.Length)
            {
                MessageBox.Show("调试信息,标记位出错，联系作者更正");
                return;
            }
            int nLastFontID = m_nDefFont;
            int nLastFontStart = 0;
            for (int i = 0; i < MarkArray.Count; i++)
            {
                if (Int32.Parse(MarkArray[i].ToString()) != nLastFontID)
                {
                    AddToCode(nLastFontStart, i - nLastFontStart, nLastFontID);
                    nLastFontID = Int32.Parse(MarkArray[i].ToString());
                    nLastFontStart = i;
                }
                if (Int32.Parse(MarkArray[i].ToString()) < -1)
                {
                    int nIndex = Int32.Parse(MarkArray[i].ToString());
                    AddToCode(ImageArray[nIndex - ImageMark].ToString());
                }
            }
            AddToCode(nLastFontStart, MarkArray.Count - nLastFontStart, nLastFontID);
        }

        private void AddToCode(string strAddToCode)
        {
            Code = Code + strAddToCode;
        }

        private void AddToCode(int nStart, int nLength, int nFontID)
        {
            if (nFontID < -1 || nLength == 0)
            {
                return;
            }
            string text = textBox.Text.Substring(nStart, nLength).Replace("\n", "\\\\\\n");

            if (nFontID != -1)
            {
                text = textMark + text + fontMark + nFontID.ToString() + fontEndMark;
            }
            else
            {
                MessageBox.Show("出错信息：nFontID == -1");
            }

            Code = Code + text;
        }

        private string CheckImageMark(int _nIndexS, int _nIndexImageMarkS)
        {
            bool bRetString = false;
            if (_nIndexImageMarkS < -1)
                return string.Empty;
            if (_nIndexS > -1 && _nIndexImageMarkS > -1)
            {
                if (_nIndexS > _nIndexImageMarkS)
                {
                    bRetString = true;
                }
            }
            else if (_nIndexS <= -1 && _nIndexImageMarkS > -1)
            {
                bRetString = true;
            }
            
            string strCode = Code;
            int nIndexP = _nIndexImageMarkS;
            
            if (nIndexP != -1)
            {
                int nIndexE = strCode.IndexOf(frameEndMark, nIndexP);
                strCode = strCode.Substring(nIndexP, nIndexE + frameEndMark.Length - nIndexP);
            }
            else 
            {
                strCode = string.Empty;
            }
            return bRetString ? strCode : string.Empty;
        }

        public void refreshText()
        {
            MarkArray.Clear();
            ImageArray.Clear();
            textBox.TextChanged -= new System.EventHandler(this.richTextBox2_TextChanged);
            
            textBox.Text = string.Empty;
            string strCode = Code;
            int nIndexP = -1;
            int nIndexE = -1;
            int nIndexMarkP = -1;
            int nIndexMarkE = -1;
            int nStart = 0;

            bool bEnd = false;
            bool bImageEnd = false;

            while (true)
            {
                string strToAdd = string.Empty;
                nIndexP = strCode.IndexOf(textMark, nIndexP + 1);
                if (!bImageEnd)
                {
                    nIndexMarkP = strCode.IndexOf(imageMark, nIndexMarkP + 1);
                    if (nIndexMarkP == -1)
                    {
                        bImageEnd = true;
                    }
                }
                
                string strImageMark = CheckImageMark(nIndexP, nIndexMarkP);

                if (nIndexP == -1 && nIndexMarkP < 0)
                {
                    strToAdd = strToAdd + strCode.Substring(nStart, strCode.Length - nStart);
                    bEnd = true;
                }
                else
                {
                    if (strImageMark == string.Empty)
                    {
                        strToAdd = strToAdd + strCode.Substring(nStart, nIndexP - nStart);
                    }
                    else
                    {
                        strToAdd = strToAdd + strCode.Substring(nStart, nIndexMarkP - nStart);
                    }
                }

                int nIndex = 0;
                if (strToAdd != string.Empty)
                {
                    // 默认字体
                    strToAdd = strToAdd.Replace("\\\\\\n", "\n");
                    nIndex = textBox.Text.Length;

                    textBox.AppendText(strToAdd);

                    textBox.Select(nIndex, strToAdd.Length);
                    textBox.SelectionFont = defFont;
                    textBox.SelectionColor = defColor;

                    for (int i = 0; i < strToAdd.Length; i++)
                    {
                        MarkArray.Insert(nIndex++, m_nDefFont as Object);
                    }
                }
                
                if (bEnd)
                    break;

                if (strImageMark != string.Empty)
                {
                    MarkArray.Add(ImageArray.Count + ImageMark);
                    ImageArray.Add(strImageMark);

                    int nCurPos = strImageMark.IndexOf(frameMark);

                    string filename = strImageMark.Substring(imageMark.Length, nCurPos - imageMark.Length);
                    filename = m_Root + filename;

                    int nWidthMarkPos = strImageMark.IndexOf(widthMark, nCurPos);
                    int nFrame = Int32.Parse(strImageMark.Substring(nCurPos + frameMark.Length, nWidthMarkPos - nCurPos - frameMark.Length));
                    //int nWidth = 0;
                    //int nHeight = 0;
                   //////////////////////////////////////////////////////////////////////////
                    //这个功能暂未实现，标签有，无预览效果
                    //////////////////////////////////////////////////////////////////////////
                    
                    string retRTF = RTFHelper.GetRTFFromTgaFile(filename, textBox, nFrame);
                    textBox.Select(textBox.Text.Length, 0);
                    textBox.SelectedRtf = retRTF;

                    nIndexMarkE = strCode.IndexOf(frameEndMark, nIndexMarkP);
                    nStart = nIndexMarkE + frameEndMark.Length;
                    nIndexP = nStart - 1;
                    continue;
                }

                // 特定字体
                strToAdd = string.Empty;
                nIndexP = nIndexP + textMark.Length;
                nIndexE = strCode.IndexOf(fontMark, nIndexP);

                strToAdd = strToAdd + strCode.Substring(nIndexP, nIndexE - nIndexP);

                nIndexP = nIndexP + strToAdd.Length + fontMark.Length;
                nIndexE = strCode.IndexOf(fontEndMark, nIndexP);

                string strFontID = strCode.Substring(nIndexP, nIndexE - nIndexP);
                int nFontID = Int32.Parse(strFontID);
                nStart = nIndexE + fontEndMark.Length;

                Color CurColor = font.GetColorByID(nFontID);
                Font CurFont = font.GetFontByID(nFontID);

                strToAdd = strToAdd.Replace("\\\\\\n", "\n");

                nIndex = textBox.Text.Length;
                textBox.AppendText(strToAdd);

                textBox.Select(nIndex, strToAdd.Length);
                textBox.SelectionColor = CurColor;
                textBox.SelectionFont = CurFont;


                for (int i = 0; i < strToAdd.Length; i++)
                {
                    MarkArray.Insert(nIndex++, nFontID as Object);
                }

                nIndexMarkP--;
            }
            textBox.TextChanged += new System.EventHandler(this.richTextBox2_TextChanged);
            m_nLastLength = textBox.Text.Length;
        }

        private void refreshLink()
        {
            linkBox.Text = string.Empty;
            LinkArray.Clear();
            string strCode = LinkCode;

            int nIndexP = -1;
            int nIndexE = -1;

            while (true)
            {
                nIndexP = strCode.IndexOf(textMark, nIndexP + 1);
                if (nIndexP == -1)
                    break;
                nIndexP = nIndexP + textMark.Length;
                nIndexE = strCode.IndexOf(fontMark, nIndexP);
                string content = strCode.Substring(nIndexP, nIndexE - nIndexP);

                nIndexP = nIndexE + fontMark.Length;
                nIndexE = strCode.IndexOf(linkMark, nIndexP);
                string strID = strCode.Substring(nIndexP, nIndexE - nIndexP);

                nIndexP = nIndexE + linkMark.Length;
                nIndexE = strCode.IndexOf(linkEndMark, nIndexP);
                string strLink = strCode.Substring(nIndexP, nIndexE - nIndexP);

                LinkArray.Add(new Object[] { content, strID, strLink });
                linkBox.AppendText(content + "\n");

                int nID = Int32.Parse(strID.ToString());
                Font CurFont = font.GetFontByID(nID);
                Color CurColor = font.GetColorByID(nID);

                linkBox.Select(linkBox.Text.Length - content.Length - 1, content.Length);
                linkBox.SelectionFont = CurFont;
                linkBox.SelectionColor = CurColor;
            }
        }

        private void refreshlinkCode()
        {
            LinkCode = string.Empty;
            for (int i = 0; i < LinkArray.Count; i++)
            {
                string content = (LinkArray[i] as Object[])[0].ToString();
                string strID = (LinkArray[i] as Object[])[1].ToString();
                string strLink = (LinkArray[i] as Object[])[2].ToString();
                string strToAdd = string.Format("{0}{1}{2}{3}{4}{5}{6}", textMark, content, fontMark, strID, linkMark, strLink, linkEndMark);
                LinkCode = LinkCode + strToAdd;
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            Color CurColor;
            Font CurFont;

            int nIndex = comboBox1.SelectedIndex;
            if (comboBox1.SelectedIndex == comboBox1.Items.Count - 1)
            {
                CurColor = defColor;
                CurFont = defFont;
                nIndex = m_nDefFont;
            }
            else
            {
                CurColor = font.GetColorByID(comboBox1.SelectedIndex);
                CurFont = font.GetFontByID(comboBox1.SelectedIndex);
            }
 
            textBox.SelectionColor = CurColor;
            textBox.SelectionFont = CurFont;

            int nStart = textBox.SelectionStart;
            int nLength = textBox.SelectionLength;
           
            while (nLength-- != 0)
            {
                MarkArray[nStart + nLength] = nIndex;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (!m_bImageSupport)
            {
                return;
            }
            //textBox.TextChanged -= new System.EventHandler(this.richTextBox2_TextChanged);
            string filename = FileFolderHelper.BrowserFile(new string[] { "tga", "uitex" });
            if (filename == string.Empty)
                return;
            ImageDialog dlg = new ImageDialog(filename);
            int nFrame = 0;
            int nWidth = 0;
            int nHeight = 0;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                if (dlg.strFrame != string.Empty)
                {
                    nFrame = Int32.Parse(dlg.strFrame);
                }
                if (dlg.strWidth != string.Empty)
                {
                    nWidth = Int32.Parse(dlg.strWidth);
                }
                if (dlg.strHeight != string.Empty)
                {
                    nHeight = Int32.Parse(dlg.strHeight);
                }
            }

            string retRTF = RTFHelper.GetRTFFromTgaFile(filename, textBox, nFrame);
            textBox.SelectedRtf = retRTF;

            MarkArray[textBox.SelectionStart - 1] = ImageArray.Count + ImageMark;
            filename = filename.Substring(m_Root.Length, filename.Length - m_Root.Length);

            string strW = string.Empty;
            string strH = string.Empty;

            if (nWidth != 0)
            {
                strW = widthMark + nWidth.ToString();
            }
            if (nHeight != 0)
            {
                strH = heightMark + nHeight.ToString();
            }

            string strImageMark = imageMark + filename + frameMark + nFrame.ToString() + strW + strH + frameEndMark;
            ImageArray.Add(strImageMark);
            //textBox.TextChanged += new System.EventHandler(this.richTextBox2_TextChanged);
        }

        private void TabSelectionChanged(object sender, EventArgs e)
        {
            if (tabControl1.SelectedIndex == 0)
            {
                refreshText();
                refreshLink();
            }
            else 
            {
                refreshCode();
                refreshlinkCode();
            }
        }

        public bool hasChanged()
        {
            return m_bHasChanged;
        }

        public bool saveAll()
        {
            if (m_bHasChanged)
            {
                refreshCode();
                refreshlinkCode();

                string filter = string.Format("ClassID = {0} and MapID = {1}", m_LastTags[0], m_LastTags[1]);

                DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, filter, null, m_Conn);
                if (Int32.Parse(m_LastTags[1].ToString()) == 0)
                {
                    dt.Rows[0]["Content"] = Code;
                }
                else
                {
                    dt.Rows[0]["Introdution"] = Code;
                }
               
                dt.Rows[0]["Link"] = LinkCode;

                string sql = string.Format("select * from {0} where {1}", m_strTabName, filter);
                TableHelper.SaveTable(dt, sql, m_Conn);
                m_bHasChanged = false;
                return true;
            }
            return false;
        }

        public bool reloadAll()
        {
            return true;
        }

        public bool reloadAll(string keyword)
        {
            string[] keywords = keyword.Split(new char[] { ',' });
            string filter = string.Format("ClassID = {0} and MapID = {1}", keywords[0], keywords[1]);
            m_LastTags[0] = keywords[0];
            m_LastTags[1] = keywords[1];
            


            if (keywords[0] == "-1")
            {
                Code = string.Empty;
                LinkCode = string.Empty;

                refreshText();
                refreshLink();
                m_bHasChanged = false;

                return false;
            }

            DataTable dt = TableHelper.GetDataTableProxy(m_strTabName, filter, null, m_Conn);

            if (Int32.Parse(keywords[1].ToString()) == 0)
                Code = dt.Rows[0]["Content"].ToString();
            else
                Code = dt.Rows[0]["Introdution"].ToString();
            
            LinkCode = dt.Rows[0]["Link"].ToString();

            refreshText();
            refreshLink();
            m_bHasChanged = false;

            return true;
        }

        private void richTextBox3_TextChanged(object sender, EventArgs e)
        {
            m_bHasChanged = true;
        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            m_bHasChanged = true;
        }

        private void richTextBox4_TextChanged(object sender, EventArgs e)
        {
            m_bHasChanged = true;
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            InsertLinkForm newForm = new InsertLinkForm(this, m_Conn, m_Root, m_nDefFont);
            if (newForm.ShowDialog() == DialogResult.OK)
            {
                LinkArray = newForm.LinkArray;

                refreshlinkCode();
                refreshLink();
            }
        }
    }
}