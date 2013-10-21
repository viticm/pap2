using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Data.SqlClient;
using System.Threading;

namespace icon_Sort
{
    public partial class Form1 : Form
    {
        private string strCopying = string.Empty;
        private int nCnt = 0;
        private int nFailedCnt = 0;
        private Thread t1 = null;

        public Form1()
        {
            InitializeComponent();
        }

        private void btnOpenFile_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(tbxSourcePath.Text))
            {
                fbdOpen.SelectedPath = tbxSourcePath.Text;
            }            
            if (fbdOpen.ShowDialog() == DialogResult.OK)
            {
                tbxSourcePath.Text = fbdOpen.SelectedPath;
            }            
        }

        private void btnSaveFile_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(tbxDescPath.Text))
            {
                fbdSave.SelectedPath = tbxDescPath.Text;
            }            
            if (fbdSave.ShowDialog() == DialogResult.OK)
            {
                tbxDescPath.Text = fbdSave.SelectedPath;
            }
        }

        private void btnCopy_Click(object sender, EventArgs e)
        {
            if (!CheckCopyable())
            {
                MessageBox.Show("目录不对", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            tbxCopying.Text = "";
            t1 = new Thread(new ThreadStart(SelectAndCopy));
            
            t1.Start();
            t1.Join();
            tbxCopying.Text = strCopying;
            lbCnt.Text = string.Format("{0}", nCnt);
            lbFailedCnt.Text = string.Format("{0}", nFailedCnt);
            
        }
        private bool CheckCopyable()
        {
            if (!Directory.Exists(tbxDescPath.Text) ||
                !Directory.Exists(tbxSourcePath.Text)
                )
            {
                return false;
            }
            return true;
        }
        private void SelectAndCopy()
        {
            nCnt = 0;
            nFailedCnt = 0;
            strCopying = string.Empty;
            SqlConnection conn = new SqlConnection("Server = 192.168.27.31; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;");

            string strSql = @"  SELECT *
                                FROM icon
                                WHERE (Kind = '防具') OR
                                      (Kind = '武器') OR
                                      (Kind = '饰品')
                                ORDER BY Kind, SubKind";

            SqlCommand cmd = new SqlCommand(strSql, conn);
            conn.Open();
            SqlDataReader dr = cmd.ExecuteReader();

            string strSourceFilePath = string.Empty;
            string strDescFilePath = string.Empty;
            string strFileName = string.Empty;
            string strKind = string.Empty;
            string strSubKind = string.Empty;
            
            while (dr.Read())
            {
                strFileName = dr["FileName"].ToString().Replace("UITex", "Tga");//后缀的大小写
                strKind = dr["Kind"].ToString();
                strSubKind = dr["SubKind"].ToString();

                strSourceFilePath = tbxSourcePath.Text + "\\" + strFileName;
                strDescFilePath = string.Format("{0}\\{1}", tbxDescPath.Text, strKind);
                if (Directory.Exists(strDescFilePath) == false)
                {
                    strCopying += strKind + "\r\n";
                    Directory.CreateDirectory(strDescFilePath);
                }
                strDescFilePath += "\\" + strSubKind;
                if (Directory.Exists(strDescFilePath) == false)
                {
                    strCopying += "    " + strSubKind + "\r\n";
                    Directory.CreateDirectory(strDescFilePath);
                }
                strDescFilePath += strFileName;
                strCopying += strKind +  "\\" + strSubKind + "\\" + strFileName + "\r\n";

                if (File.Exists(strSourceFilePath))
                {
                    File.Copy(strSourceFilePath, strDescFilePath, true);
                    nCnt++;
                }
                else
                {
                    strCopying += "错误：文件不存在" + strSourceFilePath + "\r\n";
                    nFailedCnt++;
                }

            };
            dr.Close();
            conn.Close();
        }


        
    }
}