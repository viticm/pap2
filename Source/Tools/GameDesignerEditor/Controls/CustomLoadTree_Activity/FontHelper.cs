using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Drawing;
using System.Data;
using System.Drawing.Text;

namespace CustomLoadTree_Activity
{
    public class FontHelper
    {
        public FontHelper(string FontFile, string FontListFile, string ColorFile, string Root)
        {
            font = new CustomFont(FontFile);
            fontlist = new CustomFontList(FontListFile, Root);
            color = new CustomColor(ColorFile);
        }

        public string[] GetAllFontName()
        {
            return font.GetAllFontName();
        }

        public Font GetFontByID(int nID)
        {
            int nFontID = font.GetFontIDByID(nID);
            int nSize = fontlist.GetSizeByID(nFontID);
            FontCollection fontcollection = fontlist.GetFontCollectionByID(nFontID);
            Font ResultFont = font.GetFontByID(nID, nSize, fontcollection);
            
            return ResultFont;
        }

        

        public Color GetColorByID(int nID)
        {
            string ColorName = font.GetColorNameByID(nID);

            return color.GetColorByName(ColorName);
        }

        CustomFont font;
        CustomFontList fontlist;
        CustomColor color;
    };

    public class CustomFont
    {
        public CustomFont(string FontFile)
        {
            file = FontFile;
        }

        public string[] GetAllFontName()
        {
            ArrayList stringlist = new ArrayList();
            for (int i = 0; ; i++)
            {
                StringBuilder sb = new StringBuilder(128);
                FileFolderHelper.ReadIniFile(i.ToString(), "Name", "", sb, 128, file);

                if (sb.ToString() != "")
                {
                    sb.Append(string.Format("({0})", i));
                    stringlist.Add(sb.ToString());
                }
                else 
                {
                    break;
                }
            }
            string[] ResultStrings = new string[stringlist.Count];

            for (int i = 0; i < stringlist.Count; i++)
            {
                ResultStrings[i] = stringlist[i].ToString();
            }

            return ResultStrings;
        }

        public int GetFontIDByID(int nID)
        {
            StringBuilder sb = new StringBuilder(128);
            FileFolderHelper.ReadIniFile(nID.ToString(), "FontID", "1", sb, 128, file);
            return Int32.Parse(sb.ToString());
        }

        public Font GetFontByID(int nID, int nSize, FontCollection fc)
        {
            //StringBuilder sb = new StringBuilder(128);
            //FileFolderHelper.ReadIniFile(nID.ToString(), "Name", "", sb, 128, file);
            //string Name = sb.ToString();
            //FileFolderHelper.ReadIniFile(nID.ToString(), "Size", "", sb, 128, file);
            //float fSize = float.Parse(sb.ToString());
            FontFamily family = new FontFamily(fc.Families[0].Name, fc);
            Font CurFont = new Font(family, nSize);
            return CurFont;
        }

        public string GetColorNameByID(int nID)
        {
            StringBuilder sbColorName = new StringBuilder(128);
            FileFolderHelper.ReadIniFile(nID.ToString(), "Color", "", sbColorName, 128, file);

            return sbColorName.ToString();
        }

        string file;
    };

    public class CustomFontList
    {
        public CustomFontList(string FontListFile, string Root)
        {
            file = FontListFile;
            m_Root = Root;
        }

        public int GetSizeByID(int nID)
        {
            StringBuilder sb = new StringBuilder(128);
            FileFolderHelper.ReadIniFile(nID.ToString(), "Size", "1", sb, 128, file);
            return Int32.Parse(sb.ToString());
        }

        public FontCollection GetFontCollectionByID(int nID)
        {
            StringBuilder sb = new StringBuilder(128);
            FileFolderHelper.ReadIniFile(nID.ToString(), "File", "1", sb, 128, file);
            PrivateFontCollection pfc = new PrivateFontCollection();

            pfc.AddFontFile(m_Root.TrimEnd(new char[] { '\\'}) + sb.ToString());
            return pfc;
        }

        string file;
        string m_Root;
    };

    public class CustomColor
    {
        public CustomColor(string ColorFile)
        {
            file = ColorFile;

            string content = FileFolderHelper.FileToString(file);
            ColorTable = Helper.CreateDataTable(content);
            ColorTable.PrimaryKey = new DataColumn[] { ColorTable.Columns[0] };
        }

        public Color GetColorByName(string ColorName)
        {
            int R = 0;
            int G = 0;
            int B = 0;

            DataRow row = ColorTable.Rows.Find(new Object[] { ColorName });
            if (row != null)
            {
                R = Int32.Parse(row["r"].ToString());
                G = Int32.Parse(row["g"].ToString());
                B = Int32.Parse(row["b"].ToString());
            }

            return Color.FromArgb(R, G, B);
        }

        DataTable ColorTable;
        string file;
    };
}
