using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Windows.Forms;

namespace SkillScriptControl
{
    public class SSControl
    {
        //编辑脚本，返回值为新值，或者旧值
        public string SkillDataEdit(string strCode)
        {
            FrmScriptEdit fse = new FrmScriptEdit(strCode);

            GetColList(strCode);

            if( fse.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                return fse.GetScriptCode();
            }
            else
            {
                return strCode;
            }
        }

        public string ShowComboBoxList(string strCode, string strText)
        {
            FrmComboBoxList fcb = new FrmComboBoxList(strCode, strText);
            if(fcb.ShowDialog() == DialogResult.OK)
            {
                return fcb.strResult;
            }
            else
            {
                return strText;
            }
        }

        //给定脚本字符串，返回列名们
        public static ArrayList GetColList(string strCode)
        {
            ArrayList al_ret = new ArrayList();
            strCode = strCode.TrimStart(new char[] { '\t', '\r', '\n', ' '});
            if (strCode == "") return al_ret;
            string str1st = strCode.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries)[0];
            str1st = str1st.TrimStart(new char[] { '\t', '{', ',', ' ' });
            str1st = str1st.TrimEnd(new char[] { '\t', '}', ',', ' ' });
            String[] astr_cols = str1st.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);
            foreach(string strLine in astr_cols)
            {
                string strCol = strLine.Split(new char[] { '=' })[0].Trim();
                al_ret.Add(strCol);
            }
            return al_ret;
        }
    }
}
