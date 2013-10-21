using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.OleDb;
using System.IO;
using System.Collections;

namespace ItemEditor
{
    public partial class Main : Form
    {
        public Main()
        {
            InitializeComponent();
        }

        private void InitItemEntry(ref ItemEntry rhs)
        {
            // show list
            rhs.nLevel = 0;
            rhs.strType = "";
            rhs.strSubType = "";
            rhs.strQuality = "";
            rhs.strSchool = "";
            // show text
            rhs.nMagicNowValue = 0;
            rhs.nMagicTotalValue = 0;
            rhs.nMagicBaseValue = 0;
            rhs.nWGHurtMax = 0;
            rhs.nWGHurtMin = 0;
            rhs.nWGDefendMax = 0;
            rhs.nWGDefendMin = 0;
            // Magic
            rhs.nItemBaseMagicCount = 0;
            rhs.nItemMagicCount = 0;
            rhs.bItemMagicSige = new bool[8];
            rhs.ItemMagicSon = new MagicAttribute[8];
            for (int nIndex = 0; nIndex != 8; ++nIndex)
                rhs.bItemMagicSige[nIndex] = false;
            for (int nIndex = 0; nIndex != 8; ++nIndex)
                InitItemMagicSon(ref rhs.ItemMagicSon[nIndex]);
            // SpeicalMagic son
            rhs.nRemainMagicValue = 0;
            rhs.nMagicSonCount = 0;
            rhs.ItemSpeMagicSon = new MagicAttribute[400];
            for (int nIndex = 0; nIndex != 400; ++nIndex)
                InitItemMagicSon(ref rhs.ItemSpeMagicSon[nIndex]);
        }

        private void GiveItemEntry(ItemEntry right, ref ItemEntry rhs)
        {
            // show list
            rhs.nLevel = right.nLevel;
            rhs.strType = right.strType;
            rhs.strSubType = right.strSubType;
            rhs.strQuality = right.strQuality;
            rhs.strSchool = right.strSchool;
            // show text
            rhs.nMagicNowValue = right.nMagicNowValue;
            rhs.nMagicTotalValue = right.nMagicTotalValue; ;
            rhs.nMagicBaseValue = right.nMagicBaseValue; ;
            rhs.nWGHurtMax = right.nWGHurtMax; ;
            rhs.nWGHurtMin = right.nWGHurtMin; ;
            rhs.nWGDefendMax = right.nWGDefendMax; ;
            rhs.nWGDefendMin = right.nWGDefendMin; ;
            // Magic
            rhs.nItemBaseMagicCount = right.nItemBaseMagicCount;
            rhs.nItemMagicCount = right.nItemMagicCount;
            for (int nIndex = 0; nIndex != 8; ++nIndex)
                rhs.bItemMagicSige[nIndex] = right.bItemMagicSige[nIndex];
            for (int nIndex = 0; nIndex != 8; ++nIndex)
                GiveItemMagicSon(right.ItemMagicSon[nIndex], ref rhs.ItemMagicSon[nIndex]);
            // SpeicalMagic son
            rhs.nRemainMagicValue = right.nRemainMagicValue;
            rhs.nMagicSonCount = right.nMagicSonCount;
            for (int nIndex = 0; nIndex != 400; ++nIndex)
                GiveItemMagicSon(right.ItemSpeMagicSon[nIndex], ref rhs.ItemSpeMagicSon[nIndex]);
        }

        private void InitItemMagicSon(ref MagicAttribute rhs)
        {
            rhs.nMagicId = 0;
            rhs.nMagicMax = 0;
            rhs.nMagicMin = 0;
            rhs.nMagicValue = 0;
            rhs.strMagicString = "";
            rhs.strMagicType = "";
            // sign(Add/Del)
            rhs.bSign = true;
        }

        private void GiveItemMagicSon(MagicAttribute right, ref MagicAttribute rhs)
        {
            rhs.nMagicId = right.nMagicId;
            rhs.nMagicMax = right.nMagicMax;
            rhs.nMagicMin = right.nMagicMin;
            rhs.nMagicValue = right.nMagicValue;
            rhs.strMagicString = right.strMagicString;
            rhs.strMagicType = right.strMagicType;
            // sign(Add/Del)
            rhs.bSign = right.bSign;
        }

        private int MaxNumber(int a, int b)
        {
            return (a >= b)? a : b;
        }

        private int MinNumber(int a, int b)
        {
            return (a <= b) ? a : b;
        }

        private bool NumberBetween(int between, int big, int small)
        {
            return (big >= between) && (between >= small);
        }

        private string CorrectNullStringToNumber(string strIn)
        {
            return (strIn == "") ? "0" : strIn;
        }

        private bool ShowItemTotalValue(bool bKind, int nAddValue, int nTotalValue, int nMaxMagicNumber, ref int nValue)
        {
            if (bKind && nMaxMagicNumber >= 8)
            {
                MessageBox.Show("道具魔法属性条已满,添加新魔法属性失败!");
                return false;
            }
            if (!bKind && nMaxMagicNumber <= 0)
            {
                MessageBox.Show("道具魔法属性条已空,删除新魔法属性失败!");
                return false;
            }
            if (nValue <= 0)                nValue = 0;
            if (bKind && nValue + nAddValue > nTotalValue)
            {
                MessageBox.Show("道具价值量大于总价值量,添加新魔法属性失败!");
                return false;
            }
            if (bKind)
                nValue += nAddValue;
            else
                nValue -= nAddValue;
            this.ItemValueText.Text = nValue.ToString();
            this.ItemTotalValueLab.Text = "Total : " + nTotalValue.ToString();
            this.RemainMagicValue.Text = "Remain : " + (nTotalValue - nValue).ToString();
            return true;
        }

        private string CorrectNullStringToString(string strInA, string strInB)
        {
            if (strInA == "" || strInA == "0")
                return strInB;
            if (strInB == "" || strInB == "0")
                return strInA;
            return strInB;
        }

        private void ChangeStringToInfoCuston()
        {
            for (int nPost = (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN; nPost != (int)MainTypeName.MAIN_ITEM_TYPE_END; ++nPost)
                for (int nIndex = 0; nIndex != m_nCuston_Info_Count[nPost]; ++nIndex)
                    ParseCustonItemString(m_Custon_Info[nPost, nIndex], nPost + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN, ref m_Custon_Item[nPost, nIndex]);
        }

        private void ChangeStringToInfoCH()
        {
            for (int nPost = (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN; nPost != (int)MainTypeName.MAIN_ITEM_TYPE_END; ++nPost)
                for (int nIndex = 0; nIndex != m_nCH_Info_Count[nPost]; ++nIndex)
                    ParseCHItemString(m_CH_Info[nPost, nIndex], nPost + (int)FilePath.CH_ITEM_TXT_BEGIN, ref m_CH_Item[nPost, nIndex]);

        }

        private void SaveItem(int nMainFileId)
        {
            {
                string strFilePath = m_strFile[nMainFileId + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN];
                StreamWriter dout = new StreamWriter(strFilePath);
                dout.WriteLine(m_strFileHead[nMainFileId + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN]);
                for (int nIndex = 0; nIndex != m_nCuston_Info_Count[nMainFileId]; ++nIndex)
                    dout.WriteLine(m_Custon_Info[nMainFileId, nIndex]);
                dout.Close();
            }

            {
                string strFilePath = m_strFile[nMainFileId + (int)FilePath.CH_ITEM_TXT_BEGIN];
                StreamWriter dout = new StreamWriter(strFilePath);
                dout.WriteLine(m_strFileHead[nMainFileId + (int)FilePath.CH_ITEM_TXT_BEGIN]);
                for (int nIndex = 0; nIndex != m_nCH_Info_Count[nMainFileId]; ++nIndex)
                    dout.WriteLine(m_CH_Info[nMainFileId, nIndex]);
                dout.Close();
            }

            MessageBox.Show("保存完毕!");
        }

        private void OpenFile_Click(object sender, EventArgs e)
        {
            bool bRead = true;
            char[] szDelimiter;
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();

            folderBrowserDialog1.ShowDialog();
            m_strFile[(int)FilePath.FILE_INI] = folderBrowserDialog1.SelectedPath + "\\File.ini";
            m_strFile[(int)FilePath.MAIN_INI] = folderBrowserDialog1.SelectedPath + "\\Main.ini";

            //m_strFile[(int)FilePath.FILE_INI] = "D:\\KingSoft\\Game\\Sword3\\Source\\Tools\\ItemEditor\\ItemEditor\\File.ini";
            //m_strFile[(int)FilePath.MAIN_INI] = "D:\\KingSoft\\Game\\Sword3\\Source\\Tools\\ItemEditor\\ItemEditor\\Main.ini";
            //folderBrowserDialog1.SelectedPath = "D:\\KingSoft\\Game\\Sword3\\Source\\Tools\\ItemEditor\\ItemEditor";

            if (folderBrowserDialog1.SelectedPath == "")
            {
                MessageBox.Show("请选择配置文件所在文件夹!");
                return;
            }

            {
                try {
                    StreamReader din = File.OpenText(m_strFile[(int)FilePath.FILE_INI]);
                    for (int nFile = (int)FilePath.FILE_PATH_BEGIN + 2; nFile != (int)FilePath.FILE_PATH_END; ++nFile)
                    {
                        string strInfo = din.ReadLine();
                        strFileInfoName = strInfo.Split(szDelimiter, 2);
                        m_strFile[nFile] = strFileInfoName[0];
                        m_nFileInfoNumber[nFile] = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[1]));
                    }
                    din.Close();
                }
                catch 
                {
                    goto Exit0;
                }                
            }
            {
                try
                {
                    StreamReader din = File.OpenText(m_strFile[(int)FilePath.MAIN_INI]);
                    for (int nType = (int)MainTypeName.MAIN_TYPE_NAME_BEGIN; nType != (int)MainTypeName.MAIN_TYPE_NAME_END; ++nType)
                        m_strTypeName[nType] = din.ReadLine();
                    din.Close();
                }
                catch 
                {
                    goto Exit0;
                }       
            }

            bRead = OnReadItemFileInfo();
            if (!bRead) goto Exit0;
            bRead = OnReadCHFileInfo();
            if (!bRead) goto Exit0;
            bRead = OnReadGZFileInfo();
            if (!bRead) goto Exit0;
            OnSetComBoxInfo();

            MessageBox.Show("Read All File Over!\n");
            return;
        Exit0:
            MessageBox.Show("读取文件失败,请检查文件格式是否正确,或该文件是否存在!\n");
            return;
        }

        private void About_Click(object sender, EventArgs e)
        {
            string strInfo = "";
            Form2 frm = new Form2();
            strInfo  = "Name        :   ItemEditor\n";
            strInfo += "Author      :   林嘉琦(LY)\n";
            strInfo += "Department  :   XSJ-JX3Online\n";
            strInfo += "Edition     :   0.1\n";
            strInfo += "Date        :   2006.10.10\n";
            frm.label1.Text = strInfo;
            
            strInfo  = "说明 : \n";
            strInfo += "1、如果读取的文字为乱码则请用UE转换为UTF-8字型!\n";
            strInfo += "2、请不要随便做非法的操作或点击,可能会导致出错!\n";
            strInfo += "3、如果文件读取失败,则可能是格式不正确,请检测是否多了或少了几个\\t\n";
            strInfo += "4、如有什么问题请与我联系 msn: linyuazh@hotmail.com\n";
            frm.label2.Text = strInfo;

            frm.Show();
        }

        private void MainTypeComB1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string strType = this.MainTypeComB1.Text;
            this.SubTypeComB1.Items.Clear();
            this.SubTypeComB1.Text = "";
            if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_ARMOR])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_END; ++nType)
                    this.SubTypeComB1.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            else if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_TRINKET])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_TRINKET_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_TRINKET_END; ++nType)
                    this.SubTypeComB1.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            else if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_WEAPON])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_END; ++nType)
                    this.SubTypeComB1.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
        }

        private void MainTypeComB2_SelectedIndexChanged(object sender, EventArgs e)
        {
            string strType = this.MainTypeComB2.Text;
            this.SubTypeComb2.Items.Clear();
            this.SubTypeComb2.Text = "";
            if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_ARMOR])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_END; ++nType)
                    this.SubTypeComb2.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            else if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_TRINKET])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_TRINKET_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_TRINKET_END; ++nType)
                    this.SubTypeComb2.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            else if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_WEAPON])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_END; ++nType)
                    this.SubTypeComb2.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
        }

        private void MainTypeComB3_SelectedIndexChanged(object sender, EventArgs e)
        {
            string strType = this.MainTypeComB3.Text;
            this.SubTypeComb3.Items.Clear();
            this.SubTypeComb3.Text = "";
            if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_ARMOR])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_END; ++nType)
                    this.SubTypeComb3.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            else if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_TRINKET])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_TRINKET_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_TRINKET_END; ++nType)
                    this.SubTypeComb3.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            else if (strType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_WEAPON])
            {
                for (int nType = (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN; nType != (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_END; ++nType)
                    this.SubTypeComb3.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
        }

        private void OnSetComBoxInfo()
        {
            this.MainTypeComB1.Items.Clear();
            this.MainTypeComB2.Items.Clear();
            this.MainTypeComB3.Items.Clear();

            this.QualityComb1.Items.Clear();
            this.QualityComb2.Items.Clear();
            this.QualityComb3.Items.Clear();

            this.SchoolComb1.Items.Clear();
            this.SchoolComb2.Items.Clear();

            this.LeashComB1.Items.Clear();
            this.LeashComb2.Items.Clear();

            this.AttributeComb1.Text = "";
            this.AttributeComb2.Text = "";

            // Main Type
            for (int nType = (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN; nType != (int)MainTypeName.MAIN_ITEM_TYPE_END; ++nType)
            {
                this.MainTypeComB1.Items.AddRange(new object[] { m_strTypeName[nType] });
                this.MainTypeComB2.Items.AddRange(new object[] { m_strTypeName[nType] });
                this.MainTypeComB3.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            // Quality
            for (int nType = (int)MainTypeName.QUAKITY_BEGIN; nType != (int)MainTypeName.QUAKITY_END; ++nType)
            {
                this.QualityComb1.Items.AddRange(new object[] { m_strTypeName[nType] });
                this.QualityComb2.Items.AddRange(new object[] { m_strTypeName[nType] });
                this.QualityComb3.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            // School
            for (int nType = (int)MainTypeName.SCHOOL_BEGIN; nType != (int)MainTypeName.SCHOOL_END; ++nType)
            {
                this.SchoolComb1.Items.AddRange(new object[] { m_strTypeName[nType] });
                this.SchoolComb2.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            // Leash
            for (int nType = (int)MainTypeName.LEASH_BEGIN; nType != (int)MainTypeName.LEASH_END; ++nType)
            {
                this.LeashComB1.Items.AddRange(new object[] { m_strTypeName[nType] });
                this.LeashComb2.Items.AddRange(new object[] { m_strTypeName[nType] });
            }
            // Att1
            this.AttributeComb1.Text = m_strTypeName[(int)MainTypeName.BASE_DEMAND1_WG_HURT];
            // Att2
            this.AttributeComb2.Text = m_strTypeName[(int)MainTypeName.BASE_DEMAND1_WG_DEFAND];
        }

        private bool OnReadItemFileInfo()
        {
            // Read Custon Item Information
            for (int nFile = (int)FilePath.CUSTOM_ITEM_TXT_BEGIN; nFile != (int)FilePath.CUSTOM_ITEM_TXT_END; ++nFile)
            {
                try
                {
                    string strInfo;
                    StreamReader din = File.OpenText(m_strFile[nFile]);
                    m_strFileHead[nFile] = din.ReadLine();
                    m_nCuston_Info_Count[nFile - (int)FilePath.CUSTOM_ITEM_TXT_BEGIN] = 0;
                    while ((strInfo = din.ReadLine()) != null)
                    {
                        switch (nFile)
                        {
                            case (int)FilePath.CUSTOM_ARMOR_TXT:
                                m_Custon_Info[0, m_nCuston_Info_Count[0]] = strInfo;
                                m_nCuston_Info_Count[0]++;
                                break;
                            case (int)FilePath.CUSTOM_TRINKET_TXT:
                                m_Custon_Info[1, m_nCuston_Info_Count[1]] = strInfo;
                                m_nCuston_Info_Count[1]++;
                                break;
                            case (int)FilePath.CUSTOM_WEAPON_TXT:
                                m_Custon_Info[2, m_nCuston_Info_Count[2]] = strInfo;
                                m_nCuston_Info_Count[2]++;
                                break;
                        }
                    }
                    din.Close();
                }
                catch
                {
                    return false;
                }
            }
            return true;
        }

        private bool OnReadCHFileInfo()
        {
            // Read CH Item Information
            for (int nFile = (int)FilePath.CH_ITEM_TXT_BEGIN; nFile != (int)FilePath.CH_ITEM_TXT_END; ++nFile)
            {
                try
                {
                    string strInfo;
                    StreamReader din = File.OpenText(m_strFile[nFile]);
                    m_strFileHead[nFile] = din.ReadLine();
                    m_nCH_Info_Count[nFile - (int)FilePath.CH_ITEM_TXT_BEGIN] = 0;
                    while ((strInfo = din.ReadLine()) != null)
                    {
                        switch (nFile)
                        {
                            case (int)FilePath.CH_ARMOR_TXT:
                                m_CH_Info[0, m_nCH_Info_Count[0]] = strInfo;
                                m_nCH_Info_Count[0]++;
                                break;
                            case (int)FilePath.CH_TRINKET_TXT:
                                m_CH_Info[1, m_nCH_Info_Count[1]] = strInfo;
                                m_nCH_Info_Count[1]++;
                                break;
                            case (int)FilePath.CH_WEAPON_TXT:
                                m_CH_Info[2, m_nCH_Info_Count[2]] = strInfo;
                                m_nCH_Info_Count[2]++;
                                break;
                        }
                    }
                    din.Close();
                }
                catch
                {
                    return false;
                }
            }
            return true;
        }

        private bool OnReadGZFileInfo()
        {
            bool bRead = true;

            // Read GZ Information
            // 组合表 * 3
            bRead = OnReadItemCombination();
            if (!bRead) return bRead; 
            // 装备魔法价值量
            bRead = OnReadValue();
            if (!bRead) return bRead;
            // 魔法属性中文对照表
            bRead = OnReadMagicTypeName();
            if (!bRead) return bRead;
            // 魔法属性表
            bRead = OnReadMagicTypeValue();
            if (!bRead) return bRead;
            // 外功伤害
            bRead = OnReadWGHurt();
            if (!bRead) return bRead;
            // 外功防御
            bRead = OnReadWGDefend();
            if (!bRead) return bRead;

            return bRead;
        }

        private bool OnReadItemCombination()
        {
            int nStringNumber = 0;
            char[] szDelimiter;
            string strInfo = "";
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();

            {// Armor
                try
                {
                    StreamReader din = File.OpenText(m_strFile[(int)FilePath.GZ_ARMOR_TXT]);
                    for (int nIndex = 0; nIndex != 14; ++nIndex)
                    {
                        strInfo = din.ReadLine();
                        if (nIndex == 0)
                            m_strFileHead[(int)FilePath.GZ_ARMOR_TXT] = strInfo;
                        strFileInfoName = strInfo.Split(szDelimiter, m_nFileInfoNumber[(int)FilePath.GZ_ARMOR_TXT]);
                        nStringNumber = strFileInfoName.GetLength(0);
                        for (int nPost = 0; nPost != m_nFileInfoNumber[(int)FilePath.GZ_ARMOR_TXT]; ++nPost)
                        {
                            if (nPost >= nStringNumber)
                                m_strItemArmorCom[nIndex, nPost] = "";
                            else
                                m_strItemArmorCom[nIndex, nPost] = strFileInfoName[nPost];
                        }
                    }
                    din.Close();
                }
                catch
                {
                    return false;
                }
            }
            {// Trinket
                try
                {
                    StreamReader din = File.OpenText(m_strFile[(int)FilePath.GZ_TRINKET_TXT]);
                    for (int nIndex = 0; nIndex != 14; ++nIndex)
                    {
                        strInfo = din.ReadLine();
                        if (nIndex == 0)
                            m_strFileHead[(int)FilePath.GZ_TRINKET_TXT] = strInfo;
                        strFileInfoName = strInfo.Split(szDelimiter, m_nFileInfoNumber[(int)FilePath.GZ_TRINKET_TXT]);
                        nStringNumber = strFileInfoName.GetLength(0);
                        for (int nPost = 0; nPost != m_nFileInfoNumber[(int)FilePath.GZ_TRINKET_TXT]; ++nPost)
                        {
                            if (nPost >= nStringNumber)
                                m_strItemTrinketCom[nIndex, nPost] = "";
                            else
                                m_strItemTrinketCom[nIndex, nPost] = strFileInfoName[nPost];
                        }
                    }
                    din.Close();
                }
                catch
                {
                    return false;
                }
            }
            {// Weapon
                try
                {
                    StreamReader din = File.OpenText(m_strFile[(int)FilePath.GZ_WEAPON_TXT]);
                    for (int nIndex = 0; nIndex != 14; ++nIndex)
                    {
                        strInfo = din.ReadLine();
                        if (nIndex == 0)
                            m_strFileHead[(int)FilePath.GZ_WEAPON_TXT] = strInfo;
                        strFileInfoName = strInfo.Split(szDelimiter, m_nFileInfoNumber[(int)FilePath.GZ_WEAPON_TXT]);
                        nStringNumber = strFileInfoName.GetLength(0);
                        for (int nPost = 0; nPost != m_nFileInfoNumber[(int)FilePath.GZ_WEAPON_TXT]; ++nPost)
                        {
                            if (nPost >= nStringNumber)
                                m_strItemWeaponCom[nIndex, nPost] = "";
                            else
                                m_strItemWeaponCom[nIndex, nPost] = strFileInfoName[nPost];
                        }
                    }
                    din.Close();
                }
                catch
                {
                    return false;
                }
            }
            return true;
        }
    
        private bool OnReadValue()
        {
            int nStringNumber = 0;
            char[] szDelimiter;
            string strInfo = "";
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();

            try
            {
                StreamReader din = File.OpenText(m_strFile[(int)FilePath.GZ_VALUE_TXT]);
                m_strFileHead[(int)FilePath.GZ_VALUE_TXT] = din.ReadLine();
                for (int nIndex = 0; nIndex != 100; ++nIndex)
                {
                    strInfo = din.ReadLine();
                    strFileInfoName = strInfo.Split(szDelimiter, m_nFileInfoNumber[(int)FilePath.GZ_VALUE_TXT]);
                    nStringNumber = strFileInfoName.GetLength(0);
                    for (int nPost = 0; nPost != m_nFileInfoNumber[(int)FilePath.GZ_VALUE_TXT]; ++nPost)
                    {
                        if (nPost >= nStringNumber)
                            m_nValue[nIndex, nPost] = 0;
                        else
                            m_nValue[nIndex, nPost] = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nPost]));
                    }
                }
                din.Close();
            }
            catch 
            {
                return false;
            }
            return true;
        }

        private bool OnReadMagicTypeName()
        {
            int nStringNumber = 0;
            char[] szDelimiter;
            string strInfo = "";
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();

            try
            {
                StreamReader din = File.OpenText(m_strFile[(int)FilePath.GZ_MAGIC_TYPE_NAME_TXT]);
                m_strFileHead[(int)FilePath.GZ_MAGIC_TYPE_NAME_TXT] = din.ReadLine();
                for (int nIndex = 0; nIndex != 100; ++nIndex)
                {
                    strInfo = din.ReadLine();
                    strFileInfoName = strInfo.Split(szDelimiter, m_nFileInfoNumber[(int)FilePath.GZ_MAGIC_TYPE_NAME_TXT]);
                    nStringNumber = strFileInfoName.GetLength(0);
                    for (int nPost = 0; nPost != m_nFileInfoNumber[(int)FilePath.GZ_MAGIC_TYPE_NAME_TXT]; ++nPost)
                    {
                        if (nPost >= nStringNumber)
                            m_strMagicTypeName[nIndex, nPost] = "";
                        else
                            m_strMagicTypeName[nIndex, nPost] = strFileInfoName[nPost];
                    }
                }
                din.Close();
            }
            catch
            {
                return false;
            }
            return true;
        }

        private bool OnReadMagicTypeValue()
        {
            int nStringNumber = 0;
            char[] szDelimiter;
            string strInfo = "";
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();

            try
            {
                StreamReader din = File.OpenText(m_strFile[(int)FilePath.GZ_MAGIC_TYPE_VALUE_TXT]);
                m_strFileHead[(int)FilePath.GZ_MAGIC_TYPE_VALUE_TXT] = din.ReadLine();
                for (int nIndex = 0; nIndex != 398; ++nIndex)
                {
                    strInfo = din.ReadLine();
                    strFileInfoName = strInfo.Split(szDelimiter, m_nFileInfoNumber[(int)FilePath.GZ_MAGIC_TYPE_VALUE_TXT]);
                    nStringNumber = strFileInfoName.GetLength(0);
                    for (int nPost = 0; nPost != m_nFileInfoNumber[(int)FilePath.GZ_MAGIC_TYPE_VALUE_TXT]; ++nPost)
                    {
                        if (nPost >= nStringNumber)
                            m_strMagicTypeValue[nIndex, nPost] = "";
                        else
                            m_strMagicTypeValue[nIndex, nPost] = strFileInfoName[nPost];
                    }
                }
                din.Close();
            }
            catch
            {
                return false;
            }
            return true;
        }

        private bool OnReadWGHurt()
        {
            int nStringNumber = 0;
            char[] szDelimiter;
            string strInfo = "";
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();

            try
            {
                StreamReader din = File.OpenText(m_strFile[(int)FilePath.GZ_WGHURT_TXT]);
                m_strFileHead[(int)FilePath.GZ_WGHURT_TXT] = din.ReadLine();
                for (int nIndex = 0; nIndex != 100; ++nIndex)
                {
                    strInfo = din.ReadLine();
                    strFileInfoName = strInfo.Split(szDelimiter, m_nFileInfoNumber[(int)FilePath.GZ_WGHURT_TXT]);
                    nStringNumber = strFileInfoName.GetLength(0);
                    for (int nPost = 0; nPost != m_nFileInfoNumber[(int)FilePath.GZ_WGHURT_TXT]; ++nPost)
                    {
                        if (nPost >= nStringNumber)
                            m_fWGHurt[nIndex, nPost] = 0.0;
                        else
                            m_fWGHurt[nIndex, nPost] = double.Parse(CorrectNullStringToNumber(strFileInfoName[nPost]));
                    }
                }
                din.Close();
            }
            catch
            {
                return false;
            }
            return true;
        }

        private bool OnReadWGDefend()
        {
            int nStringNumber = 0;
            char[] szDelimiter;
            string strInfo = "";
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();

            try
            {
                StreamReader din = File.OpenText(m_strFile[(int)FilePath.GZ_WGDEFEND_TXT]);
                m_strFileHead[(int)FilePath.GZ_WGDEFEND_TXT] = din.ReadLine();
                for (int nIndex = 0; nIndex != 100; ++nIndex)
                {
                    strInfo = din.ReadLine();
                    strFileInfoName = strInfo.Split(szDelimiter, m_nFileInfoNumber[(int)FilePath.GZ_WGDEFEND_TXT]);
                    nStringNumber = strFileInfoName.GetLength(0);
                    for (int nPost = 0; nPost != m_nFileInfoNumber[(int)FilePath.GZ_WGDEFEND_TXT]; ++nPost)
                    {
                        if (nPost >= nStringNumber)
                            m_fWGDefend[nIndex, nPost] = 0.0;
                        else
                            m_fWGDefend[nIndex, nPost] = double.Parse(CorrectNullStringToNumber(strFileInfoName[nPost]));
                    }
                }
                din.Close();
            }
            catch
            {
                return false;
            }
            return true;
        }

        private void ParseCustonItemString(string strInfo, int nKind, ref Custon_ItemInfo Cus_ItemInfo)
        {
            int nStringNumber = 0;
            int nIndex = 0;
            char[] szDelimiter;
            string[] strFileInfoTemp;
            string[] strFileInfoName = new string[m_nFileInfoNumber[nKind]];
            szDelimiter = m_strDelimStr.ToCharArray();
            strFileInfoTemp = strInfo.Split(szDelimiter, m_nFileInfoNumber[nKind]);
            nStringNumber = strFileInfoTemp.GetLength(0);

            for (int nTemp = 0; nTemp != nStringNumber; ++nTemp)
                strFileInfoName[nTemp] = strFileInfoTemp[nTemp];
            for (int nTemp = nStringNumber; nTemp != m_nFileInfoNumber[nKind]; ++nTemp)
                strFileInfoName[nTemp] = "";

            Cus_ItemInfo.strBaseNeedType = new string[6];
            Cus_ItemInfo.nBaseNeedValue = new int[12];
            Cus_ItemInfo.nRequireInfo = new int[12];
            Cus_ItemInfo.nMagicType = new int[8];

            // Info
            Cus_ItemInfo.nID = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.strName = strFileInfoName[nIndex++];
            Cus_ItemInfo.nUiID = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nRepresentID = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nKindType = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nMainType = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nSubType = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nPrice = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nLevel = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nBindType = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nMaxDurability = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nCanTrade = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.nSetID = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            Cus_ItemInfo.strScriptNamePath = strFileInfoName[nIndex++];
            Cus_ItemInfo.nQuality = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));

            for (int nPost = 0; nPost != 6; ++nPost)
            {
                Cus_ItemInfo.strBaseNeedType[nPost] = strFileInfoName[nIndex++];
                Cus_ItemInfo.nBaseNeedValue[nPost * 2] = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
                Cus_ItemInfo.nBaseNeedValue[nPost * 2 + 1] = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            }
            for (int nPost = 0; nPost != 12; ++nPost)
                Cus_ItemInfo.nRequireInfo[nPost] = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            for (int nPost = 0; nPost != 8; ++nPost)
                Cus_ItemInfo.nMagicType[nPost] = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
        }

        private void ParseCHItemString(string strInfo, int nKind, ref CH_ItemInfo CH_ItemInfo)
        {
            int nStringNumber = 0;
            int nIndex = 0;
            char[] szDelimiter;
            string[] strFileInfoTemp;
            string[] strFileInfoName = new string[m_nFileInfoNumber[nKind]];
            szDelimiter = m_strDelimStr.ToCharArray();
            strFileInfoTemp = strInfo.Split(szDelimiter, m_nFileInfoNumber[nKind]);
            nStringNumber = strFileInfoTemp.GetLength(0);

            for (int nTemp = 0; nTemp != nStringNumber; ++nTemp)
                strFileInfoName[nTemp] = strFileInfoTemp[nTemp];
            for (int nTemp = nStringNumber; nTemp != m_nFileInfoNumber[nKind]; ++nTemp)
                strFileInfoName[nTemp] = "";

            CH_ItemInfo.strMagicType = new string[8];

            // Info
            CH_ItemInfo.nID = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            CH_ItemInfo.strName = strFileInfoName[nIndex++];
            CH_ItemInfo.nKindType = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            CH_ItemInfo.nMainType = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            CH_ItemInfo.nSubType = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            CH_ItemInfo.nLevel = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
            CH_ItemInfo.strSchool = strFileInfoName[nIndex++];
            CH_ItemInfo.strQuality = strFileInfoName[nIndex++];
            for (int nPost = 0; nPost != 8; ++nPost)
                CH_ItemInfo.strMagicType[nPost] = strFileInfoName[nIndex++];
            CH_ItemInfo.nTotal_Value = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[nIndex++]));
        }

        private void FindItem_Click(object sender, EventArgs e)
        {
            int nIDMin = 0;
            int nIDMax = 10000;
            int nLevelMin = 0;
            int nLevelMax = 100;
            int nPriceMin = 0;
            int nPriceMax = 10000;
            int nSetID = 0;

            string strTemp;
            int nMainTypeID = -1;
            int nFindItemNumber = 0;
            int nTempItemNumber = 0;
            int[] nCuston_Find = new int[MAX_ITEM_INFO_SIZE];

            strTemp = this.MainTypeComB1.Text;
            if (strTemp == "")
            {
                MessageBox.Show("请选择道具类型Type!");
                return;
            }

            ChangeStringToInfoCuston();
            ChangeStringToInfoCH();

            if (strTemp == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_ARMOR])
            {
                nMainTypeID = 0;
                for (int nIndex = 0; nIndex != m_nCuston_Info_Count[0]; ++nIndex)
                    nCuston_Find[nFindItemNumber++] = nIndex;
            }
            else if (strTemp == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_TRINKET])
            {
                nMainTypeID = 1;
                for (int nIndex = 0; nIndex != m_nCuston_Info_Count[1]; ++nIndex)
                    nCuston_Find[nFindItemNumber++] = nIndex;
            }
            else if (strTemp == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_WEAPON])
            {
                nMainTypeID = 2;
                for (int nIndex = 0; nIndex != m_nCuston_Info_Count[2]; ++nIndex)
                    nCuston_Find[nFindItemNumber++] = nIndex;
            }

            strTemp = this.SubTypeComB1.Text;
            if (strTemp != "")
            {
                nTempItemNumber = 0;
                for (int nIndex = 0; nIndex != nFindItemNumber; ++nIndex)
                {
                    string strName = "";
                    if (nMainTypeID == 0)
                        strName = m_strSubTypeByPrgm[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nMainType];
                    else if (nMainTypeID == 1)
                        strName = m_strSubTypeByPrgm[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nMainType];
                    else if (nMainTypeID == 2)
                        strName = m_strTypeName[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nSubType + (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN];
                    if (strTemp == strName)
                        nCuston_Find[nTempItemNumber++] = nCuston_Find[nIndex];
                }
                nFindItemNumber = nTempItemNumber;
            }

            strTemp = this.QualityComb1.Text;
            if (strTemp != "")
            {
                nTempItemNumber = 0;
                for (int nIndex = 0; nIndex != nFindItemNumber; ++nIndex)
                {
                    if (strTemp == m_strTypeName[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nQuality + (int)MainTypeName.QUAKITY_BEGIN])
                        nCuston_Find[nTempItemNumber++] = nCuston_Find[nIndex];
                }
                nFindItemNumber = nTempItemNumber;
            }

            strTemp = this.LeashComB1.Text;
            if (strTemp != "")
            {
                nTempItemNumber = 0;
                for (int nIndex = 0; nIndex != nFindItemNumber; ++nIndex)
                    if (strTemp == m_strTypeName[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nBindType + (int)MainTypeName.LEASH_BEGIN])
                        nCuston_Find[nTempItemNumber++] = nCuston_Find[nIndex];
                nFindItemNumber = nTempItemNumber;
            }

            nIDMax = MaxNumber(nIDMax, Int32.Parse(CorrectNullStringToNumber(this.IDMoreMaxText.Text)));
            nIDMin = MinNumber(nIDMin, Int32.Parse(CorrectNullStringToNumber(this.IDMoreMinText.Text)));
            nTempItemNumber = 0;
            for (int nIndex = 0; nIndex != nFindItemNumber; ++nIndex)
                if (NumberBetween(m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nID, nIDMax, nIDMin))
                    nCuston_Find[nTempItemNumber++] = nCuston_Find[nIndex];
            nFindItemNumber = nTempItemNumber;

            nLevelMax = MaxNumber(nLevelMax, Int32.Parse(CorrectNullStringToNumber(this.LevelMoreMaxText.Text)));
            nLevelMin = MinNumber(nLevelMin, Int32.Parse(CorrectNullStringToNumber(this.LevelMoreMinText.Text)));
            nTempItemNumber = 0;
            for (int nIndex = 0; nIndex != nFindItemNumber; ++nIndex)
                if (NumberBetween(m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nLevel, nLevelMax, nLevelMin))
                    nCuston_Find[nTempItemNumber++] = nCuston_Find[nIndex];
            nFindItemNumber = nTempItemNumber;

            nPriceMax = MaxNumber(nPriceMax, Int32.Parse(CorrectNullStringToNumber(this.PriceMoreMaxText.Text)));
            nPriceMin = MinNumber(nPriceMin, Int32.Parse(CorrectNullStringToNumber(this.PriceMoreMinText.Text)));
            nTempItemNumber = 0;
            for (int nIndex = 0; nIndex != nFindItemNumber; ++nIndex)
                if (NumberBetween(m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nPrice, nPriceMax, nPriceMin))
                    nCuston_Find[nTempItemNumber++] = nCuston_Find[nIndex];
            nFindItemNumber = nTempItemNumber;

            if (this.SetIDText1.Text != "")
            {
                nSetID = MaxNumber(nSetID, Int32.Parse(CorrectNullStringToNumber(this.SetIDText1.Text)));
                nTempItemNumber = 0;
                for (int nIndex = 0; nIndex != nFindItemNumber; ++nIndex)
                    if (nSetID == m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nSetID)
                        nCuston_Find[nTempItemNumber++] = nCuston_Find[nIndex];
                nFindItemNumber = nTempItemNumber;
            }

            // Show
            this.FindItemResultListB.Items.Clear();
            strTemp = "ID\tName\tType\tSubType\tLevel\tQuality\tBind\tSchool\tSetID\tPrice\tUiID\tRepID";
            this.FindItemResultListB.Items.AddRange(new object [] {strTemp});
            for (int nIndex = 0; nIndex != nFindItemNumber; ++nIndex)
            {
                int nPost = 0;
                if (nMainTypeID == 0)
                    nPost = m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nSubType + (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN;
                else if (nMainTypeID == 1)
                    nPost = m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nSubType + (int)MainTypeName.SUB_ITEM_TYPE_TRINKET_BEGIN;
                else if (nMainTypeID == 2)
                    nPost = m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nSubType + (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN;

                strTemp = "";
                strTemp += m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nID.ToString() + "\t";
                strTemp += m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].strName + "\t";
                strTemp += m_strTypeName[nMainTypeID + (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN] + "\t";
                if (nMainTypeID == 0)
                    strTemp += m_strSubTypeByPrgm[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nMainType] + "\t";
                else if (nMainTypeID == 1)
                    strTemp += m_strSubTypeByPrgm[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nMainType] + "\t";
                else if (nMainTypeID == 2)
                    strTemp += m_strTypeName[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nSubType + nPost] + "\t";
                strTemp += m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nLevel.ToString() + "\t";
                strTemp += m_strTypeName[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nQuality + (int)MainTypeName.QUAKITY_BEGIN] + "\t";
                strTemp += m_strTypeName[m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nBindType + (int)MainTypeName.LEASH_BEGIN] + "\t";
                strTemp += m_CH_Item[nMainTypeID, nCuston_Find[nIndex]].strSchool + "\t";
                strTemp += m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nSetID.ToString() + "\t";
                strTemp += m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nPrice.ToString() + "\t";
                strTemp += m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nUiID.ToString() + "\t";
                strTemp += m_Custon_Item[nMainTypeID, nCuston_Find[nIndex]].nRepresentID.ToString();
                this.FindItemResultListB.Items.AddRange(new object[] {strTemp});
            }

            // Init Show Item Info
            this.IDOnlyText.Text = "";
            this.NameText.Text = "";
            this.MainTypeComB3.Text = "";
            this.SubTypeComb3.Text = "";
            this.LevelOnlyText2.Text = "";
            this.QualityComb3.Text = "";
            this.LeashComb2.Text = "";
            this.SchoolComb2.Text = "";
            this.SetIDText2.Text = "";
            this.PriceOnlyText2.Text = "";
            this.UiIDText.Text = "";
            this.RepIDText.Text = "";
            this.DemValueText1.Text = "";
            this.DemValueText2.Text = "";
            this.DemValueText3.Text = "";
            this.DemValueText4.Text = "";
            this.DemValueText5.Text = "";
            this.DemValueText6.Text = "";
            this.DemValueText7.Text = "";
            this.DemValueText8.Text = "";
            this.AttributeComb1.Text = "";
            this.AttributeComb2.Text = "";
            this.AttributeMinText1.Text = "";
            this.AttributeMaxText1.Text = "";
            this.AttributeMinText2.Text = "";
            this.AttributeMaxText2.Text = "";

        }

        private void AddItem_Click(object sender, EventArgs e)
        {
            int nMainTypeID = 0;
            int nId = Int32.Parse(CorrectNullStringToNumber(this.IDOnlyText.Text));
            string strMainType = this.MainTypeComB3.Text;

            if (nId == -1 || strMainType == "")
            {
                MessageBox.Show("请填写ID号和道具主类型Type!");
                return;
            }

            if (strMainType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_ARMOR])
                nMainTypeID = 0;
            else if (strMainType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_TRINKET])
                nMainTypeID = 1;
            else if (strMainType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_WEAPON])
                nMainTypeID = 2;

            if (CreatItemInfo(nMainTypeID, nId) == false)
            {
                MessageBox.Show("添加失败!");
                return;
            }
            FindItem_Click(sender, e);
        }

        private void DelItem_Click(object sender, EventArgs e)
        {
            int nMainTypeID = 0;
            int nId = Int32.Parse(CorrectNullStringToNumber(this.IDOnlyText.Text));
            string strMainType = this.MainTypeComB3.Text;

            if (nId == -1 || strMainType == "")
            {
                MessageBox.Show("请填写ID号和道具主类型Type!");
                return;
            }

            if (strMainType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_ARMOR])
                nMainTypeID = 0;
            else if (strMainType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_TRINKET])
                nMainTypeID = 1;
            else if (strMainType == m_strTypeName[(int)MainTypeName.MAIN_ITEM_TYPE_WEAPON])
                nMainTypeID = 2;

            if (DelItemInfo(nMainTypeID, nId) == false)
            {
                MessageBox.Show("删除失败!");
                return;
            }
            FindItem_Click(sender, e);
        }

        // Create New Item
        private void CreateItem_Click(object sender, EventArgs e)
        {
            m_strCreateCondition[0] = this.MainTypeComB2.Text;
            m_strCreateCondition[1] = this.SubTypeComb2.Text;
            m_strCreateCondition[2] = this.QualityComb2.Text;
            m_strCreateCondition[3] = this.SchoolComb1.Text;
            m_strCreateCondition[4] = this.LevelMinOnlyText1.Text;
            m_strCreateCondition[5] = this.LevelMaxOnlyText1.Text;

            if (m_strCreateCondition[0] == "" || m_strCreateCondition[1] == ""
             || m_strCreateCondition[2] == "" || m_strCreateCondition[3] == "")
            {
                MessageBox.Show("请填写主类型Type、子类型SubType、装备品质Quality、门派School!");
                return;
            }

            m_nItemEntryCount = 0;
            CreateItemEntry();

            if (m_nItemEntryCount == 0)
            {
                MessageBox.Show("选择的条件不符合生成条件,请重新选择!");
                return;
            }

            ShowCreateList();
        }

        private void CreateItemResultListB_SelectedIndexChanged(object sender, EventArgs e)
        {
            int nIndex = 0;
            string strInfo = "";
            string strText = this.CreateItemResultListB.Text;

            char[] szDelimiter;
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();
            strFileInfoName = strText.Split(szDelimiter, 6);

            if (strText == "" || strFileInfoName[0] == "Level")
                return;

            this.MainTypeComB1.Text = strFileInfoName[1];
            this.SubTypeComB1.Text = strFileInfoName[2];
            this.QualityComb1.Text = strFileInfoName[4];

            for (nIndex = 0; nIndex != m_nItemEntryCount; ++nIndex)
            {
                strInfo = "";
                strInfo += m_ItemEntry[nIndex].nLevel.ToString() + "\t";
                strInfo += m_ItemEntry[nIndex].strType + "\t";
                strInfo += m_ItemEntry[nIndex].strSubType + "\t";
                strInfo += m_ItemEntry[nIndex].strSchool + "\t";
                strInfo += m_ItemEntry[nIndex].strQuality + "\t";
                for (int nSige = 0; nSige != m_ItemEntry[nIndex].nItemBaseMagicCount; ++nSige)
                    strInfo += m_ItemEntry[nIndex].ItemMagicSon[nSige].strMagicString + " ";
                if (strText == strInfo)
                    break;
            }
            if (nIndex < m_nItemEntryCount)
            {
                m_nItemIndex = nIndex;
                ShowMagicList(nIndex);
                this.LevelOnlyText2.Text = strFileInfoName[0];
                this.MainTypeComB3.Text = strFileInfoName[1];
                this.SubTypeComb3.Text = strFileInfoName[2];
                this.SchoolComb2.Text = strFileInfoName[3];
                this.QualityComb3.Text = strFileInfoName[4];
                this.ItemValueText.Text = m_ItemEntry[nIndex].nMagicNowValue.ToString();
                this.ItemTotalValueLab.Text = "Total : " + m_ItemEntry[nIndex].nMagicTotalValue.ToString();
                this.RemainMagicValue.Text = "Remain : " + (m_ItemEntry[nIndex].nMagicTotalValue - m_ItemEntry[nIndex].nMagicNowValue).ToString();

                this.AttributeMinText1.Text = "";
                this.AttributeMaxText1.Text = "";

                this.AttributeMinText2.Text = "";
                this.AttributeMaxText2.Text = "";

                if (!(m_ItemEntry[nIndex].nWGDefendMax == 0 && m_ItemEntry[nIndex].nWGDefendMin == 0))
                {
                    this.AttributeMinText1.Text = m_ItemEntry[nIndex].nWGDefendMin.ToString();
                    this.AttributeMaxText1.Text = m_ItemEntry[nIndex].nWGDefendMax.ToString();
                }
                if (!(m_ItemEntry[nIndex].nWGHurtMax == 0 && m_ItemEntry[nIndex].nWGHurtMin == 0))
                {
                    this.AttributeMinText2.Text = m_ItemEntry[nIndex].nWGHurtMin.ToString();
                    this.AttributeMaxText2.Text = m_ItemEntry[nIndex].nWGHurtMax.ToString();
                }
                ShowItemEntryMagicByCreate(nIndex);
            }
            else
                MessageBox.Show("该条装备不存在!");
        }
        
        private void AddMagicBut_Click(object sender, EventArgs e)
        {
            int nIndex = 0;
            int nPost = 0;
            string strInfo = this.AddMagicListb.Text;

            if (strInfo == "")
            {
                MessageBox.Show("请点选一条待加的魔法属性!");
                return;
            }

            char[] szDelimiter;
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();
            strFileInfoName = strInfo.Split(szDelimiter, 3);

            if (FindItemMagicExplain(strFileInfoName[1], strFileInfoName[2], true, ref nPost))
            {
                ShowMagicToItemInfo(nPost, strFileInfoName[2]);
                nIndex = FindItemSonMagic(strInfo);
                m_ItemEntry[m_nItemIndex].ItemSpeMagicSon[nIndex].bSign = false;

                GiveItemMagicSon(m_ItemEntry[m_nItemIndex].ItemSpeMagicSon[nIndex], ref m_ItemEntry[m_nItemIndex].ItemMagicSon[m_ItemEntry[m_nItemIndex].nItemMagicCount]);
                m_ItemEntry[m_nItemIndex].bItemMagicSige[m_ItemEntry[m_nItemIndex].nItemMagicCount] = true;
                m_ItemEntry[m_nItemIndex].nItemMagicCount++;
            }

            ShowMagicList(m_nItemIndex);
        }

        private void DelMagicBut_Click(object sender, EventArgs e)
        {
            int nPost = 0;
            string strInfo = this.DelMagicListB.Text;

            if (strInfo == "")
            {
                MessageBox.Show("请点选一条待加的魔法属性!");
                return;
            }

            char[] szDelimiter;
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();
            strFileInfoName = strInfo.Split(szDelimiter, 3);

            if (FindItemMagicExplain(strFileInfoName[1], strFileInfoName[2], false, ref nPost))
            {
                ShowMagicToItemInfo(nPost, "");
                m_ItemEntry[m_nItemIndex].ItemSpeMagicSon[FindItemSonMagic(strInfo)].bSign = true;
                m_ItemEntry[m_nItemIndex].bItemMagicSige[nPost] = false;
                m_ItemEntry[m_nItemIndex].nItemMagicCount--;
            }

            ShowMagicList(m_nItemIndex);
        }

        private int FindItemSonMagic(string strText)
        {
            int nIndex = 0;
            string strInfo = "";
            for (nIndex = 0; nIndex != m_ItemEntry[m_nItemIndex].nMagicSonCount; ++nIndex)
            {
                strInfo = "";
                strInfo += m_ItemEntry[m_nItemIndex].ItemSpeMagicSon[nIndex].nMagicId.ToString() + "\t";
                strInfo += m_ItemEntry[m_nItemIndex].ItemSpeMagicSon[nIndex].nMagicValue + "\t";
                strInfo += m_ItemEntry[m_nItemIndex].ItemSpeMagicSon[nIndex].strMagicString;
                if (strInfo == strText)
                    break;
            }
            if (nIndex < m_ItemEntry[m_nItemIndex].nMagicSonCount)
                return nIndex;
            else
                return 0;
        }

        private bool FindItemMagicExplain(string strValue, string strInfo, bool bKind, ref int nPost)
        {
            int nIndex = 0;
            int nAddValue = Int32.Parse(CorrectNullStringToNumber(strValue));
            int nTotalValue = m_ItemEntry[m_nItemIndex].nMagicTotalValue;
            string[] strText = new string[8];
            strText[0] = this.DemValueText1.Text;
            strText[1] = this.DemValueText2.Text;
            strText[2] = this.DemValueText3.Text;
            strText[3] = this.DemValueText4.Text;
            strText[4] = this.DemValueText5.Text;
            strText[5] = this.DemValueText6.Text;
            strText[6] = this.DemValueText7.Text;
            strText[7] = this.DemValueText8.Text;

            for (nIndex = 0; nIndex != 8; ++nIndex)
            {
                if (bKind && strText[nIndex] == "")
                        break;
                if (!bKind && strText[nIndex] == strInfo)
                        break;
            }
            nPost = nIndex;
            if (nPost >= 8)
            {
                MessageBox.Show("该魔法属性未找到,请重新选择!");
                return false;
            }
            return ShowItemTotalValue(bKind, nAddValue, nTotalValue, 
                                        m_ItemEntry[m_nItemIndex].nItemMagicCount, 
                                        ref m_ItemEntry[m_nItemIndex].nMagicNowValue);
        }

        private void ShowMagicToItemInfo(int nPost, string strInfo)
        {
            switch(nPost)
            {
                case 0 :
                    this.DemValueText1.Text = strInfo;
                    break;
                case 1 :
                    this.DemValueText2.Text = strInfo;
                    break;
                case 2 :
                    this.DemValueText3.Text = strInfo;
                    break;
                case 3 :
                    this.DemValueText4.Text = strInfo;
                    break;
                case 4 :
                    this.DemValueText5.Text = strInfo;
                    break;
                case 5 :
                    this.DemValueText6.Text = strInfo;
                    break;
                case 6 :
                    this.DemValueText7.Text = strInfo;
                    break;
                case 7 :
                    this.DemValueText8.Text = strInfo;
                    break;
            }
        }

        private void ShowCreateList()
        {
            string strInfo = "Level\tType\tSubType\tSchool\tQuality\tBaseMagic";
            this.CreateItemResultListB.Items.Clear();
            this.CreateItemResultListB.Items.AddRange(new object[] { strInfo });
            for (int nIndex = 0; nIndex != m_nItemEntryCount; ++nIndex)
            {
                strInfo = "";
                strInfo += m_ItemEntry[nIndex].nLevel.ToString() + "\t";
                strInfo += m_ItemEntry[nIndex].strType + "\t";
                strInfo += m_ItemEntry[nIndex].strSubType + "\t";
                strInfo += m_ItemEntry[nIndex].strSchool + "\t";
                strInfo += m_ItemEntry[nIndex].strQuality + "\t";
                for (int nSige = 0; nSige != m_ItemEntry[nIndex].nItemBaseMagicCount; ++nSige)
                    strInfo += m_ItemEntry[nIndex].ItemMagicSon[nSige].strMagicString + " ";
                this.CreateItemResultListB.Items.AddRange(new object[] { strInfo });
            }
        }

        private void ShowMagicList(int nItenId)
        {
            string strInfo = "MagicID\tMagicValue\tMagicString";
            this.AddMagicListb.Items.Clear();
            this.DelMagicListB.Items.Clear();
            this.AddMagicListb.Items.AddRange(new object[] { strInfo });
            this.DelMagicListB.Items.AddRange(new object[] { strInfo });
            for (int nIndex = 0; nIndex != m_ItemEntry[nItenId].nMagicSonCount; ++nIndex)
            {
                strInfo = "";
                strInfo += m_ItemEntry[nItenId].ItemSpeMagicSon[nIndex].nMagicId.ToString() + "\t";
                strInfo += m_ItemEntry[nItenId].ItemSpeMagicSon[nIndex].nMagicValue + "\t";
                strInfo += m_ItemEntry[nItenId].ItemSpeMagicSon[nIndex].strMagicString;
                if (m_ItemEntry[nItenId].ItemSpeMagicSon[nIndex].bSign == true)
                    this.AddMagicListb.Items.AddRange(new object[] { strInfo });
                else
                    this.DelMagicListB.Items.AddRange(new object[] { strInfo });
            }
        }

        private string FindMagicTypeName(string strMagicString)
        {
            string strMagicTypeName = "";
            for (int nIndex = 0; nIndex != 100; ++nIndex)
                if ("at" + strMagicString == m_strMagicTypeName[nIndex, 2])
                    return m_strMagicTypeName[nIndex, 1];
            return strMagicTypeName;
        }

        private void FindItemResultListB_SelectedIndexChanged(object sender, EventArgs e)
        {
            string strInfo = this.FindItemResultListB.Text;
            char[] szDelimiter;
            string[] strFileInfoName;
            szDelimiter = m_strDelimStr.ToCharArray();
            strFileInfoName = strInfo.Split(szDelimiter, 12);

            if (strInfo == "" || strFileInfoName[0] == "ID")
                return;
            m_nItemIndex = -1;

            this.AddMagicListb.Items.Clear();
            this.DelMagicListB.Items.Clear();

            this.IDOnlyText.Text = strFileInfoName[0];
            this.NameText.Text = strFileInfoName[1];
            this.MainTypeComB3.Text = strFileInfoName[2];
            this.SubTypeComb3.Text = strFileInfoName[3];
            this.LevelOnlyText2.Text = strFileInfoName[4];
            this.QualityComb3.Text = strFileInfoName[5];
            this.LeashComb2.Text = strFileInfoName[6];
            this.SchoolComb2.Text = strFileInfoName[7];
            this.SetIDText2.Text = strFileInfoName[8];
            this.PriceOnlyText2.Text = strFileInfoName[9];
            this.UiIDText.Text = strFileInfoName[10];
            this.RepIDText.Text = strFileInfoName[11];
            ShowItemEntryMagicByFind(Int32.Parse(CorrectNullStringToNumber(strFileInfoName[0])), strFileInfoName[2]);
        }

        private string FindStringSubType(string strInfo)
        {
            for (int nPost = 0; nPost != 12; ++nPost)
                if (strInfo == m_strSubTypeByPrgm[nPost])
                    return nPost.ToString();

            for (int nPost = (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN; nPost != (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_END; ++nPost)
                if (strInfo == m_strTypeName[nPost])
                    return (nPost - (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN).ToString();

            // Leash
            for (int nPost = (int)MainTypeName.LEASH_BEGIN; nPost != (int)MainTypeName.LEASH_END; ++nPost)
                if (strInfo == m_strTypeName[nPost])
                    return (nPost - (int)MainTypeName.LEASH_BEGIN).ToString();
            // Quality
            for (int nPost = (int)MainTypeName.QUAKITY_BEGIN; nPost != (int)MainTypeName.QUAKITY_END; ++nPost)
                if (strInfo == m_strTypeName[nPost])
                    return (nPost - (int)MainTypeName.QUAKITY_BEGIN).ToString();
            return "0";
                
        }

        private void CreateItemCustonAndCH(int nMainTypeID, ref string strCuston, ref string strCH)
        {
            string strId = this.IDOnlyText.Text;
            string strUiId = this.UiIDText.Text;
            string strRepId = this.RepIDText.Text;
            string strSetId = this.SetIDText2.Text;
            string strMainType = this.MainTypeComB3.Text;
            string strSubType = this.SubTypeComb3.Text;
            string strSchool = this.SchoolComb2.Text;
            string strQuality = this.QualityComb3.Text;
            string strLeash = this.LeashComb2.Text;
            string strLevel = this.LevelOnlyText2.Text;
            string strPrice = this.PriceOnlyText2.Text;
            string strName = this.NameText.Text;
            string strItemValue = this.ItemValueText.Text;

            // Custon
            strCuston = "";
            strCuston += strId + "\t";
            strCuston += strName + "\t";
            strCuston += strUiId + "\t";
            strCuston += strRepId + "\t";
            strCuston += "0" + "\t";
            if (nMainTypeID == 0)
            {
                strCuston += FindStringSubType(strSubType) + "\t";
                strCuston += "0" + "\t";
            }
            else if (nMainTypeID == 1)
            {
                strCuston += FindStringSubType(strSubType) + "\t";
                strCuston += "0" + "\t";
            }
            else if (nMainTypeID == 2)
            {
                strCuston += "0" + "\t";
                strCuston += FindStringSubType(strSubType) + "\t";
            }
            strCuston += strPrice + "\t";
            strCuston += strLevel + "\t";
            strCuston += FindStringSubType(strLeash) + "\t";
            strCuston += "0" + "\t";
            strCuston += "0" + "\t";
            strCuston += strSetId + "\t";
            strCuston += "" + "\t";
            strCuston += FindStringSubType(strQuality) + "\t";
            for (int nIndex = 0; nIndex != 18; ++nIndex) // Base string 6 * 3
                strCuston += "" + "\t";
            for (int nIndex = 0; nIndex != 12; ++nIndex) // Require string 6 * 2
                strCuston += "" + "\t";
            for (int nIndex = 0; nIndex != 7; ++nIndex) // magic string 8
            {
                if (m_nItemIndex != -1 && m_ItemEntry[m_nItemIndex].bItemMagicSige[nIndex])
                    strCuston += m_ItemEntry[m_nItemIndex].ItemMagicSon[nIndex].nMagicId.ToString() + "\t";
                else
                    strCuston += "" + "\t";
            }
            if (m_nItemIndex != -1 && m_ItemEntry[m_nItemIndex].bItemMagicSige[7])
                strCuston += m_ItemEntry[m_nItemIndex].ItemMagicSon[7].nMagicId.ToString();
            else
                strCuston += "";

            // CH
            strCH = "";
            strCH += strId + "\t";
            strCH += strName + "\t";
            strCH += "0" + "\t";
            if (nMainTypeID == 0)
            {
                strCH += FindStringSubType(strSubType) + "\t";
                strCH += "0" + "\t";
            }
            else if (nMainTypeID == 1)
            {
                strCH += FindStringSubType(strSubType) + "\t";
                strCH += "0" + "\t";
            }
            else if (nMainTypeID == 2)
            {
                strCH += "0" + "\t";
                strCH += FindStringSubType(strSubType) + "\t";
            }
            strCH += strLevel + "\t";
            strCH += strSchool + "\t";
            strCH += strQuality + "\t";
            for (int nIndex = 0; nIndex != 8; ++nIndex) // magic string 8
            {
                if (m_nItemIndex != -1 && m_ItemEntry[m_nItemIndex].bItemMagicSige[nIndex])
                    strCH += m_ItemEntry[m_nItemIndex].ItemMagicSon[nIndex].strMagicString + "\t";
                else
                    strCH += "" + "\t";
            }
            strCH += strItemValue;
        }

        private bool CreatItemInfo(int nMainTypeID, int nId)
        {
            int nStringNumber = 0;
            bool bSame = false;
            int nIdTemp = 0;
            int nPost = 0;
            char[] szDelimiter;
            string[] strFileInfoName;
            char[] szDelimiterTemp;
            string[] strFileInfoNameTemp;
            string strTemp = "";
            string strCuston = "";
            string strCH = "";

            CreateItemCustonAndCH(nMainTypeID, ref strCuston, ref strCH);

            bSame = false;
            nPost = m_nCuston_Info_Count[nMainTypeID];
            for (int nIndex = 0; nIndex != m_nCuston_Info_Count[nMainTypeID]; ++nIndex)
            {
                szDelimiter = m_strDelimStr.ToCharArray();
                strFileInfoName = m_Custon_Info[nMainTypeID, nIndex].Split(szDelimiter, m_nFileInfoNumber[nMainTypeID + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN]);
                try
                {
                    nIdTemp = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[0]));
                }
                catch
                {
                    MessageBox.Show("文件格式错误,请检查!");
                    return false;
                }
                if (nId < nIdTemp)
                {
                    nPost = nIndex;
                    break;
                }
                if (nId == nIdTemp)
                {
                    bSame = true;
                    nPost = nIndex;
                    break;
                }
            }
            if (nPost < 0)
                return false;
            if (bSame)
            {
                szDelimiter = m_strDelimStr.ToCharArray();
                strFileInfoName = m_Custon_Info[nMainTypeID, nPost].Split(szDelimiter, m_nFileInfoNumber[nMainTypeID + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN]);
                nStringNumber = strFileInfoName.GetLength(0);
                szDelimiterTemp = m_strDelimStr.ToCharArray();
                strFileInfoNameTemp = strCuston.Split(szDelimiterTemp, m_nFileInfoNumber[nMainTypeID + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN]);
                m_Custon_Info[nMainTypeID, nPost] = strFileInfoName[0];
                for (int nIndex = 1; nIndex != m_nFileInfoNumber[(int)FilePath.CUSTOM_ITEM_TXT_BEGIN]; ++nIndex)
                {
                    if (nIndex >= nStringNumber)
                        m_Custon_Info[nMainTypeID, nPost] += "\t" + strFileInfoNameTemp[nIndex];
                    else
                        m_Custon_Info[nMainTypeID, nPost] += "\t" + CorrectNullStringToString(strFileInfoName[nIndex], strFileInfoNameTemp[nIndex]);
                }
            }
            else
            {
                m_nCuston_Info_Count[nMainTypeID]++;
                for (int nIndex = nPost; nIndex != m_nCuston_Info_Count[nMainTypeID]; ++nIndex)
                {
                    strTemp = m_Custon_Info[nMainTypeID, nIndex];
                    m_Custon_Info[nMainTypeID, nIndex] = strCuston;
                    strCuston = strTemp;
                }
            }

            bSame = false;
            nPost = m_nCH_Info_Count[nMainTypeID];
            for (int nIndex = 0; nIndex != m_nCH_Info_Count[nMainTypeID]; ++nIndex)
            {
                szDelimiter = m_strDelimStr.ToCharArray();
                strFileInfoName = m_CH_Info[nMainTypeID, nIndex].Split(szDelimiter, m_nFileInfoNumber[nMainTypeID + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN]);
                try
                {
                    nIdTemp = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[0]));
                }
                catch
                {
                    MessageBox.Show("文件格式错误,请检查!");
                    return false;
                }
                if (nId < nIdTemp)
                {
                    nPost = nIndex;
                    break;
                }
                if (nId == nIdTemp)
                {
                    bSame = true;
                    nPost = nIndex;
                    break;
                }
            }
            if (nPost < 0)
                return false;
            if (bSame)
            {
                szDelimiter = m_strDelimStr.ToCharArray();
                strFileInfoName = m_CH_Info[nMainTypeID, nPost].Split(szDelimiter, m_nFileInfoNumber[nMainTypeID + (int)FilePath.CH_ITEM_TXT_BEGIN]);
                nStringNumber = strFileInfoName.GetLength(0);
                szDelimiterTemp = m_strDelimStr.ToCharArray();
                strFileInfoNameTemp = strCH.Split(szDelimiterTemp, m_nFileInfoNumber[nMainTypeID + (int)FilePath.CH_ITEM_TXT_BEGIN]);
                m_CH_Info[nMainTypeID, nPost] = strFileInfoName[0];
                for (int nIndex = 1; nIndex != m_nFileInfoNumber[(int)FilePath.CH_ITEM_TXT_BEGIN]; ++nIndex)
                {
                    if (nIndex >= nStringNumber)
                        m_CH_Info[nMainTypeID, nPost] += "\t" + strFileInfoNameTemp[nIndex];
                    else
                        m_CH_Info[nMainTypeID, nPost] += "\t" + CorrectNullStringToString(strFileInfoName[nIndex], strFileInfoNameTemp[nIndex]);
                }
            }
            else
            {
                m_nCH_Info_Count[nMainTypeID]++;
                for (int nIndex = nPost; nIndex != m_nCH_Info_Count[nMainTypeID]; ++nIndex)
                {
                    strTemp = m_CH_Info[nMainTypeID, nIndex];
                    m_CH_Info[nMainTypeID, nIndex] = strCH;
                    strCH = strTemp;
                }
            }
            SaveItem(nMainTypeID);
            return true;
        }

        private bool DelItemInfo(int nMainTypeID, int nId)
        {
            int nIdTemp = 0;
            int nPost = 0;
            char[] szDelimiter;
            string[] strFileInfoName;
            
            for (int nIndex = 0; nIndex != m_nCuston_Info_Count[nMainTypeID]; ++nIndex)
            {
                szDelimiter = m_strDelimStr.ToCharArray();
                strFileInfoName = m_Custon_Info[nMainTypeID, nIndex].Split(szDelimiter, m_nFileInfoNumber[nMainTypeID + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN]);
                try
                {
                    nIdTemp = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[0]));
                }
                catch 
                {
                    MessageBox.Show("文件格式错误,请检查!");
                    return false;
                }
                if (nId < nIdTemp)
                    return false;
                if (nId == nIdTemp)
                {
                    nPost = nIndex;
                    break;
                }
            }
            if (nPost >= m_nCuston_Info_Count[nMainTypeID])
                return false;
            if (nPost < m_nCuston_Info_Count[nMainTypeID])
                m_nCuston_Info_Count[nMainTypeID]--;
            for (int nIndex = nPost; nIndex != m_nCuston_Info_Count[nMainTypeID]; ++nIndex)
                m_Custon_Info[nMainTypeID, nIndex] = m_Custon_Info[nMainTypeID, nIndex + 1];

            for (int nIndex = 0; nIndex != m_nCH_Info_Count[nMainTypeID]; ++nIndex)
            {
                szDelimiter = m_strDelimStr.ToCharArray();
                strFileInfoName = m_CH_Info[nMainTypeID, nIndex].Split(szDelimiter, m_nFileInfoNumber[nMainTypeID + (int)FilePath.CUSTOM_ITEM_TXT_BEGIN]);
                try
                {
                    nIdTemp = Int32.Parse(CorrectNullStringToNumber(strFileInfoName[0]));
                }
                catch
                {
                    MessageBox.Show("文件格式错误,请检查!");
                    return false;
                }
                if (nId < nIdTemp)
                    return false;
                if (nId == nIdTemp)
                {
                    nPost = nIndex;
                    break;
                }
            }
            if (nPost >= m_nCH_Info_Count[nMainTypeID])
                return false;
            if (nPost < m_nCH_Info_Count[nMainTypeID])
                m_nCH_Info_Count[nMainTypeID]--;
            for (int nIndex = nPost; nIndex != m_nCH_Info_Count[nMainTypeID]; ++nIndex)
                m_CH_Info[nMainTypeID, nIndex] = m_CH_Info[nMainTypeID, nIndex + 1];

            SaveItem(nMainTypeID);
            return true;
        }

        private void FindSchool(string strSchool)
        {
            m_nSchoolRowOfCom = 0;
            for (m_nSchoolRowOfCom = (int)MainTypeName.SCHOOL_BEGIN; m_nSchoolRowOfCom != (int)MainTypeName.SCHOOL_END; ++m_nSchoolRowOfCom)
                if (strSchool == m_strTypeName[m_nSchoolRowOfCom])
                {
                    m_nSchoolRowOfCom = m_nSchoolRowOfCom - (int)MainTypeName.SCHOOL_BEGIN + 4;
                    return;
                }
        }

        // Create ItemEntry
        private void CreateItemEntry()
        {
            if (m_strCreateCondition[4] == "")
                m_nCreateLevelMin = 1;
            else
                m_nCreateLevelMin = Int32.Parse(CorrectNullStringToNumber(m_strCreateCondition[4]));

            if (m_strCreateCondition[5] == "")
                m_nCreateLevelMax = 100;
            else
                m_nCreateLevelMax = Int32.Parse(CorrectNullStringToNumber(m_strCreateCondition[5]));

            FindSchool(m_strCreateCondition[3]);
            CreateBase();
            for (int nIndex = 0; nIndex != m_nItemEntryCount; ++nIndex)
                if (m_ItemEntry[nIndex].nRemainMagicValue > 0)
                    CreateSpecial(nIndex);
        }

        private void CreateBase()
        {
            for (int nLevel = m_nCreateLevelMin; nLevel <= m_nCreateLevelMax; ++nLevel)
            {
                switch (m_strCreateCondition[0])
                {
                    case "防具":
                        CreateItemBaseArmor(nLevel);
                        break;
                    case "饰品":
                        CreateItemBaseTrinket(nLevel);
                        break;
                    case "武器":
                        CreateItemBaseWeapon(nLevel);
                        break;
                }
            }
        }

        private void CreateItemBaseArmor(int nLevel)
        {
            int nPost = 0;
            double fValue = 0.0;
            double fBaseValue = 0.0;
            double fBaseDefend = 0.0;
            double[] fBaseMagicValue = { 0.0, 0.0, 0.0 };
            double[] fFactBaseMagicValue = { 0.0, 0.0, 0.0 };

            switch (m_strCreateCondition[3])
            {
                case "万花" :
                    fBaseDefend = m_fWGDefend[nLevel - 1, 1];
                    break;
                case "少林":
                    fBaseDefend = m_fWGDefend[nLevel - 1, 2];
                    break;
                case "五毒":
                    fBaseDefend = m_fWGDefend[nLevel - 1, 3];
                    break;
            }

            for (int nIndex = (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN; nIndex != (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_END; ++nIndex)
            {
                if (m_strCreateCondition[1] == m_strTypeName[nIndex])
                {
                    fBaseDefend *= m_fWGDefendBase[nIndex - (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN];
                    break;
                }
            }

            for (int nIndex = (int)MainTypeName.QUAKITY_BEGIN; nIndex != (int)MainTypeName.QUAKITY_END; ++nIndex)
            {
                if (m_strCreateCondition[2] == m_strTypeName[nIndex])
                {
                    fBaseDefend *= m_fQuality[nIndex - (int)MainTypeName.QUAKITY_BEGIN];
                    break;
                }
            }

            if (m_strCreateCondition[2] != "白色")
            {
                for (nPost = (int)MainTypeName.QUAKITY_BEGIN + 1; nPost != (int)MainTypeName.QUAKITY_END; ++nPost)
                    if (m_strCreateCondition[2] == m_strTypeName[nPost])
                        break;
                fValue = (double)m_nValue[nLevel - 1, nPost - (int)MainTypeName.QUAKITY_BEGIN];
                if (fValue <= 0.0)          return;
                for (int nIndex = (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN; nIndex != (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_END; ++nIndex)
                {
                    if (m_strCreateCondition[1] == m_strTypeName[nIndex])
                    {
                        fValue *= m_fMagicBaseAr[nIndex - (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN];
                        break;
                    }
                }
                for (int nIndex = 1; nIndex != 5; ++nIndex)
                {
                    if (m_strItemArmorCom[m_nSchoolRowOfCom, nIndex] == "1")
                    {
                        fBaseValue = 0.0;
                        InitItemEntry(ref m_ItemEntry[m_nItemEntryCount]);
                        m_ItemEntry[m_nItemEntryCount].nLevel = nLevel;
                        m_ItemEntry[m_nItemEntryCount].strType = m_strCreateCondition[0];
                        m_ItemEntry[m_nItemEntryCount].strSubType = m_strCreateCondition[1];
                        m_ItemEntry[m_nItemEntryCount].strQuality = m_strCreateCondition[2];
                        m_ItemEntry[m_nItemEntryCount].strSchool = m_strCreateCondition[3];
                        m_ItemEntry[m_nItemEntryCount].nWGDefendMax = (int)fBaseDefend;
                        m_ItemEntry[m_nItemEntryCount].nWGDefendMin = (int)fBaseDefend;
                        for (int nBase = 0; nBase != 3; ++nBase)
                        {
                            
                            fBaseMagicValue[nBase] = double.Parse(CorrectNullStringToNumber(m_strItemArmorCom[nBase + 1, nIndex])) * fValue;
                            FindBaseValue(nBase, fBaseMagicValue[nBase], ref fFactBaseMagicValue[nBase], 
                                        ref m_ItemEntry[m_nItemEntryCount].ItemMagicSon[m_ItemEntry[m_nItemEntryCount].nItemBaseMagicCount], 
                                        ref m_ItemEntry[m_nItemEntryCount].bItemMagicSige[m_ItemEntry[m_nItemEntryCount].nItemBaseMagicCount]);
                            fBaseValue += fFactBaseMagicValue[nBase];
                        }
                        m_ItemEntry[m_nItemEntryCount].nMagicTotalValue = (int)fValue;
                        m_ItemEntry[m_nItemEntryCount].nMagicBaseValue = (int)fBaseValue;
                        m_ItemEntry[m_nItemEntryCount].nMagicNowValue = (int)fBaseValue;
                        m_ItemEntry[m_nItemEntryCount].nRemainMagicValue = (int)(fValue - fBaseValue);
                        m_nItemEntryCount++;
                    }
                }
            }
            else
            {
                InitItemEntry(ref m_ItemEntry[m_nItemEntryCount]);
                m_ItemEntry[m_nItemEntryCount].nLevel = nLevel;
                m_ItemEntry[m_nItemEntryCount].strType = m_strCreateCondition[0];
                m_ItemEntry[m_nItemEntryCount].strSubType = m_strCreateCondition[1];
                m_ItemEntry[m_nItemEntryCount].strQuality = m_strCreateCondition[2];
                m_ItemEntry[m_nItemEntryCount].strSchool = m_strCreateCondition[3];
                m_ItemEntry[m_nItemEntryCount].nWGDefendMax = (int)fBaseDefend;
                m_ItemEntry[m_nItemEntryCount].nWGDefendMin = (int)fBaseDefend;
                m_nItemEntryCount++;
            }
        }

        private void CreateItemBaseTrinket(int nLevel)
        {
            int nPost = 0;
            double fValue = 0.0;
            double fBaseValue = 0.0;
            double[] fBaseMagicValue = { 0.0, 0.0, 0.0 };
            double[] fFactBaseMagicValue = { 0.0, 0.0, 0.0 };

            if (m_strCreateCondition[2] != "白色")
            {
                for (nPost = (int)MainTypeName.QUAKITY_BEGIN + 1; nPost != (int)MainTypeName.QUAKITY_END; ++nPost)
                    if (m_strCreateCondition[2] == m_strTypeName[nPost])
                        break;
                fValue = (double)m_nValue[nLevel - 1, nPost - (int)MainTypeName.QUAKITY_BEGIN];
                if (fValue <= 0.0) return;
                for (int nIndex = (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN; nIndex != (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_END; ++nIndex)
                {
                    if (m_strCreateCondition[1] == m_strTypeName[nIndex])
                    {
                        fValue *= m_fMagicBaseAr[nIndex - (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN];
                        break;
                    }
                }
                for (int nIndex = 1; nIndex != 7; ++nIndex)
                {
                    if (m_strItemTrinketCom[m_nSchoolRowOfCom, nIndex] == "1")
                    {
                        fBaseValue = 0.0;
                        InitItemEntry(ref m_ItemEntry[m_nItemEntryCount]);
                        m_ItemEntry[m_nItemEntryCount].nLevel = nLevel;
                        m_ItemEntry[m_nItemEntryCount].strType = m_strCreateCondition[0];
                        m_ItemEntry[m_nItemEntryCount].strSubType = m_strCreateCondition[1];
                        m_ItemEntry[m_nItemEntryCount].strQuality = m_strCreateCondition[2];
                        m_ItemEntry[m_nItemEntryCount].strSchool = m_strCreateCondition[3];
                        for (int nBase = 0; nBase != 3; ++nBase)
                        {
                            fBaseMagicValue[nBase] = double.Parse(CorrectNullStringToNumber(m_strItemTrinketCom[nBase + 1, nIndex])) * fValue;
                            FindBaseValue(nBase, fBaseMagicValue[nBase], ref fFactBaseMagicValue[nBase],
                                        ref m_ItemEntry[m_nItemEntryCount].ItemMagicSon[m_ItemEntry[m_nItemEntryCount].nItemBaseMagicCount],
                                        ref m_ItemEntry[m_nItemEntryCount].bItemMagicSige[m_ItemEntry[m_nItemEntryCount].nItemBaseMagicCount]);
                            fBaseValue += fFactBaseMagicValue[nBase];
                        }
                        m_ItemEntry[m_nItemEntryCount].nMagicTotalValue = (int)fValue;
                        m_ItemEntry[m_nItemEntryCount].nMagicBaseValue = (int)fBaseValue;
                        m_ItemEntry[m_nItemEntryCount].nMagicNowValue = (int)fBaseValue;
                        m_ItemEntry[m_nItemEntryCount].nRemainMagicValue = (int)(fValue - fBaseValue);
                        m_nItemEntryCount++;
                    }
                }
            }
            else
            {
                InitItemEntry(ref m_ItemEntry[m_nItemEntryCount]);
                m_ItemEntry[m_nItemEntryCount].nLevel = nLevel;
                m_ItemEntry[m_nItemEntryCount].strType = m_strCreateCondition[0];
                m_ItemEntry[m_nItemEntryCount].strSubType = m_strCreateCondition[1];
                m_ItemEntry[m_nItemEntryCount].strQuality = m_strCreateCondition[2];
                m_ItemEntry[m_nItemEntryCount].strSchool = m_strCreateCondition[3];
                m_nItemEntryCount++;
            }
        }

        private void CreateItemBaseWeapon(int nLevel)
        {
            int nPost = 0;
            double fHurt = 0.0;
            double fHurtMin = 0.0;
            double fHurtMax = 0.0;
            double fValue = 0.0;
            double fBaseValue = 0.0;
            double[] fBaseMagicValue = { 0.0, 0.0, 0.0 };
            double[] fFactBaseMagicValue = { 0.0, 0.0, 0.0 };

            for (nPost = (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN; nPost != (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_END; ++nPost)
                if (m_strCreateCondition[1] == m_strTypeName[nPost])
                    break;

            fHurt = m_fWGHurt[nLevel - 1, nPost - (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN + 1];
            for (int nIndex = (int)MainTypeName.QUAKITY_BEGIN; nIndex != (int)MainTypeName.QUAKITY_END; ++nIndex)
            {
                if (m_strCreateCondition[2] == m_strTypeName[nIndex])
                {
                    fHurt *= m_fQuality[nIndex - (int)MainTypeName.QUAKITY_BEGIN];
                    break;
                }
            }
            fHurtMin = fHurt * (1.0 - m_fWGHurtDrift[nPost - (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN]);
            fHurtMax = fHurt * (1.0 + m_fWGHurtDrift[nPost - (int)MainTypeName.SUB_ITEM_TYPE_WEAPON_BEGIN]);

            if (m_strCreateCondition[2] != "白色")
            {
                for (nPost = (int)MainTypeName.QUAKITY_BEGIN + 1; nPost != (int)MainTypeName.QUAKITY_END; ++nPost)
                    if (m_strCreateCondition[2] == m_strTypeName[nPost])
                        break;
                fValue = (double)m_nValue[nLevel - 1, nPost - (int)MainTypeName.QUAKITY_BEGIN];
                if (fValue <= 0.0) return;
                for (int nIndex = (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN; nIndex != (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_END; ++nIndex)
                {
                    if (m_strCreateCondition[1] == m_strTypeName[nIndex])
                    {
                        fValue *= m_fMagicBaseAr[nIndex - (int)MainTypeName.SUB_ITEM_TYPE_ARMOR_BEGIN];
                        break;
                    }
                }
                for (int nIndex = 1; nIndex != 6; ++nIndex)
                {
                    if (m_strItemWeaponCom[m_nSchoolRowOfCom, nIndex] == "1")
                    {
                        fBaseValue = 0.0;
                        InitItemEntry(ref m_ItemEntry[m_nItemEntryCount]);
                        m_ItemEntry[m_nItemEntryCount].nLevel = nLevel;
                        m_ItemEntry[m_nItemEntryCount].strType = m_strCreateCondition[0];
                        m_ItemEntry[m_nItemEntryCount].strSubType = m_strCreateCondition[1];
                        m_ItemEntry[m_nItemEntryCount].strQuality = m_strCreateCondition[2];
                        m_ItemEntry[m_nItemEntryCount].strSchool = m_strCreateCondition[3];
                        for (int nBase = 0; nBase != 3; ++nBase)
                        {
                            fBaseMagicValue[nBase] = double.Parse(CorrectNullStringToNumber(m_strItemWeaponCom[nBase + 1, nIndex])) * fValue;
                            FindBaseValue(nBase, fBaseMagicValue[nBase], ref fFactBaseMagicValue[nBase],
                                        ref m_ItemEntry[m_nItemEntryCount].ItemMagicSon[m_ItemEntry[m_nItemEntryCount].nItemBaseMagicCount],
                                        ref m_ItemEntry[m_nItemEntryCount].bItemMagicSige[m_ItemEntry[m_nItemEntryCount].nItemBaseMagicCount]);
                            fBaseValue += fFactBaseMagicValue[nBase];
                        }
                        m_ItemEntry[m_nItemEntryCount].nMagicTotalValue = (int)fValue;
                        m_ItemEntry[m_nItemEntryCount].nMagicBaseValue = (int)fBaseValue;
                        m_ItemEntry[m_nItemEntryCount].nMagicNowValue = (int)fBaseValue;
                        m_ItemEntry[m_nItemEntryCount].nRemainMagicValue = (int)(fValue - fBaseValue);
                        m_nItemEntryCount++;
                    }
                }
            }
            else
            {
                InitItemEntry(ref m_ItemEntry[m_nItemEntryCount]);
                m_ItemEntry[m_nItemEntryCount].nLevel = nLevel;
                m_ItemEntry[m_nItemEntryCount].strType = m_strCreateCondition[0];
                m_ItemEntry[m_nItemEntryCount].strSubType = m_strCreateCondition[1];
                m_ItemEntry[m_nItemEntryCount].strQuality = m_strCreateCondition[2];
                m_ItemEntry[m_nItemEntryCount].strSchool = m_strCreateCondition[3];
                m_ItemEntry[m_nItemEntryCount].nWGHurtMax = (int)fHurtMax;
                m_ItemEntry[m_nItemEntryCount].nWGHurtMin = (int)fHurtMin;
                m_nItemEntryCount++;
            }
        }

        private void CreateSpecial(int nPost)
        {
            int nValue = 0;
            int nCol = FindMagicTypeValueRow(m_strCreateCondition[3]);
            m_ItemEntry[nPost].nMagicSonCount = 0;

            for (int nIndex = 0; nIndex != 398; ++nIndex)
            {
                nValue = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 1]));
                if (m_strMagicTypeValue[nIndex, nCol] == "1" && nValue <= m_ItemEntry[nPost].nRemainMagicValue)
                {
                    InitItemMagicSon(ref m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount]);
                    m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].nMagicId = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 0]));
                    m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].nMagicValue = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 1]));
                    m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].nMagicMin = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 3]));
                    m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].nMagicMax = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 4]));
                    m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].strMagicType = m_strMagicTypeValue[nIndex, 2];
                    m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].strMagicString = FindMagicTypeName(m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].strMagicType)
                                                                                                         + " " + m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].nMagicMin.ToString()
                                                                                                         + " - " + m_ItemEntry[nPost].ItemSpeMagicSon[m_ItemEntry[nPost].nMagicSonCount].nMagicMax.ToString();
                    m_ItemEntry[nPost].nMagicSonCount++;
                }
            }
        }

        private int FindMagicTypeValueRow(string strSchool)
        {
            int nCol = 0;
            for (nCol = (int)MainTypeName.SCHOOL_BEGIN; nCol != (int)MainTypeName.SCHOOL_END; ++nCol)
                if (strSchool == m_strTypeName[nCol])
                    break;
            return nCol - (int)MainTypeName.SCHOOL_BEGIN + 7;
        }

        private void FindBaseValue(int nPost, double fValue, ref double fFactValue, ref MagicAttribute ItemBase, ref bool bSige)
        {
            bSige = false;
            fFactValue = 0.0;
            if (fValue <= 0.0)
                return;
            for (int nIndex = m_nMagicItemBeginToEnd[nPost, 1]; nIndex >= m_nMagicItemBeginToEnd[nPost, 0]; --nIndex)
            {
                int nValueTemp = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 1]));
                if (nValueTemp == -1)       return;
                if (fValue >= nValueTemp)
                {
                    bSige = true;
                    fFactValue = (double)nValueTemp;
                    ItemBase.nMagicId = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 0]));
                    ItemBase.nMagicValue = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 1]));
                    ItemBase.nMagicMin = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 3]));
                    ItemBase.nMagicMax = Int32.Parse(CorrectNullStringToNumber(m_strMagicTypeValue[nIndex, 4]));
                    ItemBase.strMagicType = m_strMagicTypeValue[nIndex, 2];
                    ItemBase.strMagicString = FindMagicTypeName(ItemBase.strMagicType)
                                            + " " + ItemBase.nMagicMin.ToString()
                                            + " - " + ItemBase.nMagicMax.ToString();
                    ItemBase.bSign = true;

                    m_ItemEntry[m_nItemEntryCount].bItemMagicSige[m_ItemEntry[m_nItemEntryCount].nItemBaseMagicCount] = true;
                    m_ItemEntry[m_nItemEntryCount].nItemMagicCount++;
                    m_ItemEntry[m_nItemEntryCount].nItemBaseMagicCount++;
                    return;
                }
            }
        }

        private void ShowItemEntryMagicByCreate(int nIndex)
        {
            this.DemValueText1.Text = "";
            this.DemValueText2.Text = "";
            this.DemValueText3.Text = "";
            this.DemValueText4.Text = "";
            this.DemValueText5.Text = "";
            this.DemValueText6.Text = "";
            this.DemValueText7.Text = "";
            this.DemValueText8.Text = "";

            if (m_ItemEntry[nIndex].bItemMagicSige[0])
                this.DemValueText1.Text = m_ItemEntry[nIndex].ItemMagicSon[0].strMagicString;
            if (m_ItemEntry[nIndex].bItemMagicSige[1])
                this.DemValueText2.Text = m_ItemEntry[nIndex].ItemMagicSon[1].strMagicString;
            if (m_ItemEntry[nIndex].bItemMagicSige[2])
                this.DemValueText3.Text = m_ItemEntry[nIndex].ItemMagicSon[2].strMagicString;
            if (m_ItemEntry[nIndex].bItemMagicSige[3])
                this.DemValueText4.Text = m_ItemEntry[nIndex].ItemMagicSon[3].strMagicString;
            if (m_ItemEntry[nIndex].bItemMagicSige[4])
                this.DemValueText5.Text = m_ItemEntry[nIndex].ItemMagicSon[4].strMagicString;
            if (m_ItemEntry[nIndex].bItemMagicSige[5])
                this.DemValueText6.Text = m_ItemEntry[nIndex].ItemMagicSon[5].strMagicString;
            if (m_ItemEntry[nIndex].bItemMagicSige[6])
                this.DemValueText7.Text = m_ItemEntry[nIndex].ItemMagicSon[6].strMagicString;
            if (m_ItemEntry[nIndex].bItemMagicSige[7])
                this.DemValueText8.Text = m_ItemEntry[nIndex].ItemMagicSon[7].strMagicString;
        }

        private void ShowItemEntryMagicByFind(int nId, string strType)
        {
            int nIndex = 0;
            int nPost = 0;
            switch (strType)
            {
                case "防具" :
                    nPost = 0;
                    break;
                case "饰品":
                    nPost = 1;
                    break;
                case "武器":
                    nPost = 2;
                    break;
            }
            for (nIndex = 0; nIndex != m_nCH_Info_Count[nPost]; ++nIndex)
                if (m_CH_Item[nPost, nIndex].nID == nId)
                    break;

            this.DemValueText1.Text = "";
            this.DemValueText2.Text = "";
            this.DemValueText3.Text = "";
            this.DemValueText4.Text = "";
            this.DemValueText5.Text = "";
            this.DemValueText6.Text = "";
            this.DemValueText7.Text = "";
            this.DemValueText8.Text = "";

            this.DemValueText1.Text = m_CH_Item[nPost, nIndex].strMagicType[0];
            this.DemValueText2.Text = m_CH_Item[nPost, nIndex].strMagicType[1];
            this.DemValueText3.Text = m_CH_Item[nPost, nIndex].strMagicType[2];
            this.DemValueText4.Text = m_CH_Item[nPost, nIndex].strMagicType[3];
            this.DemValueText5.Text = m_CH_Item[nPost, nIndex].strMagicType[4];
            this.DemValueText6.Text = m_CH_Item[nPost, nIndex].strMagicType[5];
            this.DemValueText7.Text = m_CH_Item[nPost, nIndex].strMagicType[6];
            this.DemValueText8.Text = m_CH_Item[nPost, nIndex].strMagicType[7];
        }
    }
}
