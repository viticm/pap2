//ORIGINAL LGPL SOURCE CODE FINDED ON COMPONA LGPL SOURCE CODE

using System;
using System.ComponentModel;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Fireball.Windows.Forms;
using System.Collections.Generic;

namespace Fireball.Windows.Forms
{
    /// <summary>
    /// Summary description for InfoTip.
    /// </summary>
    public class InfoTipForm : Form
    {
        [DllImport("user32.dll", EntryPoint = "SendMessage")]
        private static extern int SendMessage(IntPtr hWnd, int message, int _data, int _id);


        private int _SelectedIndex = 0;
        private int _Count = 1;

        private WeakReference _Control = null;

        private Control ParentControl
        {
            get
            {
                if (_Control != null)
                    return (Control)_Control.Target;
                else
                    return null;
            }
            set { _Control = new WeakReference(value); }
        }


        private Panel panel2;
        private FormatLabelControl InfoText;
        private PictureBox btnPrev;
        private PictureBox btnNext;
        private Panel pnlSelect;
        private Label lblIndex;
        private Panel pnlImage;
        private PictureBox picIcon;
        private Panel panel1;

        public event EventHandler SelectedIndexChanged = null;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private Container components = null;

        /// <summary>
        /// 
        /// </summary>
        public InfoTipForm()
        {
            InitializeComponent();


        }


        public int SelectedIndex
        {
            get { return _SelectedIndex; }
            set
            {
                if (value > _Count)
                    value = 1;
                if (value < 1)
                    value = _Count;

                _SelectedIndex = value;
                OnSelectedIndexChanged();
                SetPos();
            }
        }

        public int Count
        {
            get { return _Count; }
            set { _Count = value; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parent"></param>
        public InfoTipForm(Control parent)
        {
            this.ParentControl = parent;
            this.CreateParams.ClassName = "tooltips_class32";
            //		//	this.CreateParams.Parent =ParentControl.Handle;
            //			this.RecreateHandle ();


            InitializeComponent();


        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(InfoTipForm));
            this.pnlSelect = new System.Windows.Forms.Panel();
            this.btnNext = new System.Windows.Forms.PictureBox();
            this.btnPrev = new System.Windows.Forms.PictureBox();
            this.lblIndex = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.InfoText = new Fireball.Windows.Forms.FormatLabelControl();
            this.pnlImage = new System.Windows.Forms.Panel();
            this.picIcon = new System.Windows.Forms.PictureBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.pnlSelect.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.btnNext)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.btnPrev)).BeginInit();
            this.panel2.SuspendLayout();
            this.pnlImage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picIcon)).BeginInit();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlSelect
            // 
            this.pnlSelect.Controls.Add(this.btnNext);
            this.pnlSelect.Controls.Add(this.btnPrev);
            this.pnlSelect.Controls.Add(this.lblIndex);
            this.pnlSelect.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlSelect.Location = new System.Drawing.Point(38, 0);
            this.pnlSelect.Name = "pnlSelect";
            this.pnlSelect.Padding = new System.Windows.Forms.Padding(4);
            this.pnlSelect.Size = new System.Drawing.Size(96, 35);
            this.pnlSelect.TabIndex = 0;
            // 
            // btnNext
            // 
            this.btnNext.BackColor = System.Drawing.SystemColors.Control;
            this.btnNext.Image = ((System.Drawing.Image)(resources.GetObject("btnNext.Image")));
            this.btnNext.Location = new System.Drawing.Point(82, 6);
            this.btnNext.Name = "btnNext";
            this.btnNext.Size = new System.Drawing.Size(10, 12);
            this.btnNext.TabIndex = 1;
            this.btnNext.TabStop = false;
            this.btnNext.DoubleClick += new System.EventHandler(this.btnNext_DoubleClick);
            this.btnNext.Click += new System.EventHandler(this.btnNext_Click);
            // 
            // btnPrev
            // 
            this.btnPrev.BackColor = System.Drawing.SystemColors.Control;
            this.btnPrev.Image = ((System.Drawing.Image)(resources.GetObject("btnPrev.Image")));
            this.btnPrev.Location = new System.Drawing.Point(5, 6);
            this.btnPrev.Name = "btnPrev";
            this.btnPrev.Size = new System.Drawing.Size(11, 12);
            this.btnPrev.TabIndex = 0;
            this.btnPrev.TabStop = false;
            this.btnPrev.DoubleClick += new System.EventHandler(this.btnPrev_DoubleClick);
            this.btnPrev.Click += new System.EventHandler(this.btnPrev_Click);
            // 
            // lblIndex
            // 
            this.lblIndex.Dock = System.Windows.Forms.DockStyle.Top;
            this.lblIndex.Location = new System.Drawing.Point(4, 4);
            this.lblIndex.Name = "lblIndex";
            this.lblIndex.Size = new System.Drawing.Size(88, 25);
            this.lblIndex.TabIndex = 2;
            this.lblIndex.Text = "20 of 20";
            this.lblIndex.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.InfoText);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel, ((byte)(134)));
            this.panel2.Location = new System.Drawing.Point(134, 0);
            this.panel2.Name = "panel2";
            this.panel2.Padding = new System.Windows.Forms.Padding(4);
            this.panel2.Size = new System.Drawing.Size(104, 35);
            this.panel2.TabIndex = 1;
            // 
            // InfoText
            // 
            this.InfoText.AutoSizeHorizontal = true;
            this.InfoText.AutoSizeVertical = true;
            this.InfoText.BackColor = System.Drawing.SystemColors.Info;
            this.InfoText.Font = new System.Drawing.Font("宋体", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.InfoText.ImageList = null;
            this.InfoText.LabelMargin = 0;
            this.InfoText.Link_Color = System.Drawing.Color.Blue;
            this.InfoText.Link_Color_Hover = System.Drawing.Color.Blue;
            this.InfoText.Link_UnderLine = false;
            this.InfoText.Link_UnderLine_Hover = true;
            this.InfoText.Location = new System.Drawing.Point(2, 4);
            this.InfoText.Name = "InfoText";
            this.InfoText.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.InfoText.Size = new System.Drawing.Size(117, 11);
            this.InfoText.TabIndex = 0;
            this.InfoText.Text = "ABC中文format <b>label</b>";
            this.InfoText.WordWrap = false;
            this.InfoText.Click += new System.EventHandler(this.InfoText_Click);
            this.InfoText.Resize += new System.EventHandler(this.InfoText_Resize);
            this.InfoText.Enter += new System.EventHandler(this.InfoText_Enter);
            // 
            // pnlImage
            // 
            this.pnlImage.Controls.Add(this.picIcon);
            this.pnlImage.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlImage.Location = new System.Drawing.Point(0, 0);
            this.pnlImage.Name = "pnlImage";
            this.pnlImage.Size = new System.Drawing.Size(38, 35);
            this.pnlImage.TabIndex = 2;
            this.pnlImage.Visible = false;
            // 
            // picIcon
            // 
            this.picIcon.Location = new System.Drawing.Point(6, 3);
            this.picIcon.Name = "picIcon";
            this.picIcon.Size = new System.Drawing.Size(23, 22);
            this.picIcon.TabIndex = 1;
            this.picIcon.TabStop = false;
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Controls.Add(this.pnlSelect);
            this.panel1.Controls.Add(this.pnlImage);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(240, 37);
            this.panel1.TabIndex = 3;
            // 
            // InfoTipForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.BackColor = System.Drawing.SystemColors.Info;
            this.ClientSize = new System.Drawing.Size(240, 37);
            this.ControlBox = false;
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "InfoTipForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Enter += new System.EventHandler(this.InfoTipForm_Enter);
            this.pnlSelect.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.btnNext)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.btnPrev)).EndInit();
            this.panel2.ResumeLayout(false);
            this.pnlImage.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.picIcon)).EndInit();
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="text"></param>
        //	public void ShowInfo(int x,int y,string text)
        //	{
        //		this.Show ();
        //		this.Location =new Point (x,y);
        //	}
        private void InfoText_Resize(object sender, EventArgs e)
        {
            DoResize();
        }

        private void DoResize()
        {
            int w = InfoText.Left + InfoText.Width + 8;
            if (Count > 1)
            {
                w += pnlSelect.Width;
            }
            if (picIcon.Image != null)
            {
                w += pnlImage.Width;
            }


            int h = InfoText.Top + InfoText.Height + 6;
            if (this.Image != null && this.Image.Height + this.picIcon.Top * 2 > h)
                h = this.Image.Height + this.picIcon.Top * 2;

            this.ClientSize = new Size(w, h);


        }

        /// <summary>
        /// 
        /// </summary>
        public void Init()
        {
            SelectedIndex = 1;
            SetPos();
        }

        public Image Image
        {
            get { return this.picIcon.Image; }
            set
            {
                this.picIcon.Image = value;
                if (value == null)
                {
                    pnlImage.Visible = false;
                }
                else
                {
                    pnlImage.Visible = true;
                    pnlImage.Width = this.Image.Width + 6;
                    picIcon.Size = Image.Size;
                }
                DoResize();
            }
        }

        private void btnNext_Click(object sender, EventArgs e)
        {
            SelectedIndex++;
            SetPos();

        }

        private void btnPrev_Click(object sender, EventArgs e)
        {
            SelectedIndex--;
            SetPos();
        }

        private void btnPrev_DoubleClick(object sender, EventArgs e)
        {
            SelectedIndex--;
            SetPos();
        }

        private void btnNext_DoubleClick(object sender, EventArgs e)
        {
            SelectedIndex++;
            SetPos();
        }

        private void SetPos()
        {
            if (Count == 1)
            {
                pnlSelect.Visible = false;
            }
            else
            {
                pnlSelect.Visible = true;
            }
            DoResize();

            lblIndex.Text = SelectedIndex.ToString() + " of " + Count.ToString();
            this.InfoText.Text = tlist[SelectedIndex-1];

            if (ParentControl != null)
                ParentControl.Focus();
        }

        private void InfoTipForm_Enter(object sender, EventArgs e)
        {
            ParentControl.Focus();
        }

        private void InfoText_Enter(object sender, EventArgs e)
        {
            ParentControl.Focus();
        }


        public List<string> tlist = new List<string>();
        public int tlist_pos = 0;

        public string Data
        {
            get { return InfoText.Text; }
            set { InfoText.Text = value; }
        }

        private void OnSelectedIndexChanged()
        {
            if (SelectedIndexChanged != null)
                SelectedIndexChanged(this, null);
        }

        private void InfoText_Click(object sender, EventArgs e)
        {

        }


    }
}