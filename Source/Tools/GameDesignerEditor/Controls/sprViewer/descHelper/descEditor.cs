using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;

namespace descHelper
{
    public class descEditor
    {
        frmMain fm = new frmMain();

        public bool showEdit()
        {
            fm.ShowDialog();

            if (fm.DialogResult == System.Windows.Forms.DialogResult.OK)
            {
                return true;
            }

            return false;
        }
    }

    public static class itemProp
    {
        public static string iName = "";
        public static int iColor = 0;
        public static bool iCanStack = false;
        public static int iMax = 0;
        public static int tGenre = 0; 
        public static int tBindType = 0;                                    
        public static int maxExist = 0;                                     
        public static string desc = "";                                    
        public static bool canTrade = true; 
        public static int price = 0;                             
        public static int icoID = 0;
        public static IconSelector.Implement icoSelector = null;
        public static SqlConnection conn = null;
        public static string iconDir = "";
        public static int maxexisttime = 0;
        public static string requirement = "";
        public static int SoundID = 0;
    };
}
