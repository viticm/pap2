using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SymCrypt
{
    public partial class Form1 : Form
    {
        private SymmetricMethod _sm = new SymmetricMethod();
        private string strResult = string.Empty;

        public Form1()
        {
            InitializeComponent();

            MyInit();
        }
        private void MyInit()
        {
            textKey.Text = _sm.Key;
        }


        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            string strFile = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();
            textCurrentFile.Text = strFile;
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                e.Effect = DragDropEffects.Copy;
            else
                e.Effect = DragDropEffects.None; 
        }

        private void buttonEncrypt_Click(object sender, EventArgs e) // 加密
        {
            _sm.Key = textKey.Text;
            string fileContent = FileFolderHelper.FileToString(textCurrentFile.Text);
            strResult = _sm.Encrypto(fileContent);
            textResult.Text = strResult;
        }

        private void buttonDecrypt_Click(object sender, EventArgs e) // 解密
        {
            _sm.Key = textKey.Text;
            string fileContent = FileFolderHelper.FileToString(textCurrentFile.Text);
            strResult = _sm.Decrypto(fileContent);
            textResult.Text = strResult;
        }

        private void buttonCopy_Click(object sender, EventArgs e) // 复制到剪贴板
        {
            Clipboard.SetText(strResult);
        }

        private void button1_Click(object sender, EventArgs e) // 更改key
        {
            textKey.Enabled = true;
        }

        private void buttonClose_Click(object sender, EventArgs e) // 关闭
        {
            this.Dispose();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}