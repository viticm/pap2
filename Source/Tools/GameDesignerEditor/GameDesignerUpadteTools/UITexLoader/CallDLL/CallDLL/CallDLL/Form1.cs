using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;


namespace CallDLL
{
    public partial class Form1 : Form
    {
        private string curFilename;
        private Rectangle IconRect;
        public Form1()
        {
            InitializeComponent();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }
        
       // System.IntPtr Handle ;
        private void 打开图片ToolStripMenuItem_Click(object sender, EventArgs e)
        {
          //  Form1.Release();
            if (this.openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                curFilename = this.openFileDialog1.FileName;

                Form1.Setup(this.pictureBox1.Handle, curFilename);
                Form1.Render(this.pictureBox1.Handle);
               
                //System.Timers.Timer t = new System.Timers.Timer(1000);
                //t.Elapsed += new System.Timers.ElapsedEventHandler(theout);//到达时间的时候执行事件； 
                //t.AutoReset = true;//设置是执行一次（false）还是一直执行(true)； 
                //t.Enabled = true;//是否执行System.Timers.Timer.Elapsed事件； 
               // this.pictureBox1.Refresh();

            } 
        }
         
        private void Form1_Load(object sender, EventArgs e)
        {
          
        }
       
      
        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            bool a = Form1.Render(this.pictureBox1.Handle);
        }
        int IconID;
        private void pictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            IconRect = GetCurrenIconRect();
            IconID = GetCurrenIconID();
            label1.Text = IconID.ToString();
            label2.Text = IconRect.ToString();
        }




    }
}