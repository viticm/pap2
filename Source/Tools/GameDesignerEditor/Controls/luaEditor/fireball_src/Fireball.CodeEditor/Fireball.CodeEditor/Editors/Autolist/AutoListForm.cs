//ORIGINAL LGPL SOURCE CODE FINDED ON COMPONA LGPL SOURCE CODE
using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Fireball.Win32;

namespace Fireball.Windows.Forms.CodeEditor
{
	/// <summary>
	/// Summary description for AutoListForm.
	/// </summary>
	[ToolboxItem(false)]
	public class AutoListForm : Form
	{
		[DllImport("user32.dll", EntryPoint="SendMessage")]
		private static extern int SendMessage(IntPtr hWnd, int message, int _data, int _id);

		private TabListBox LB;
		private ArrayList items = new ArrayList();
		private ToolTip tooltip;
		private IContainer components;
        private WeakReference _Control = null;
        public ToolStrip toolBox;
        private ToolStripButton toolStripButton1;
        private ToolStripButton toolStripButton2;
        private ToolStripButton toolStripButton3;

		private Control ParentControl
		{
			get
			{
				if (_Control != null)
					return (Control) _Control.Target;
				else
					return null;
			}
			set { _Control = new WeakReference(value); }
		}


		/// <summary>
		/// The imagelist that should be used by the AutoListForm
		/// </summary>
		public ImageList Images = null;

		/// <summary>
		/// Default AltoListControl constructor.
		/// </summary>
		public AutoListForm(Control Owner)
		{
			this.ParentControl = Owner;
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			//SetStyle(ControlStyles.ContainerControl  ,false);
			SetStyle(ControlStyles.Selectable, true);

			// TODO: Add any initialization after the InitForm call

		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			base.Dispose(disposing);
		}

		public void SendKey(int KeyCode)
		{
			SendMessage(LB.Handle, (int) WindowMessage.WM_KEYDOWN, KeyCode, 0);
		}

		#region Component Designer generated code
        public void OnHandleDestroyed(object sender, EventArgs e)
        {
        }
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AutoListForm));
            this.tooltip = new System.Windows.Forms.ToolTip(this.components);
            this.toolBox = new System.Windows.Forms.ToolStrip();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton2 = new System.Windows.Forms.ToolStripButton();
            this.LB = new Fireball.Windows.Forms.CodeEditor.TabListBox();
            this.toolStripButton3 = new System.Windows.Forms.ToolStripButton();
            this.toolBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // tooltip
            // 
            this.tooltip.AutoPopDelay = 5000;
            this.tooltip.InitialDelay = 100;
            this.tooltip.ReshowDelay = 100;
            this.tooltip.Popup += new System.Windows.Forms.PopupEventHandler(this.tooltip_Popup);
            // 
            // toolBox
            // 
            this.toolBox.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.toolBox.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolBox.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1,
            this.toolStripButton2,
            this.toolStripButton3});
            this.toolBox.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.Flow;
            this.toolBox.Location = new System.Drawing.Point(0, 239);
            this.toolBox.Name = "toolBox";
            this.toolBox.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional;
            this.toolBox.Size = new System.Drawing.Size(221, 23);
            this.toolBox.TabIndex = 1;
            this.toolBox.Text = "toolStrip1";
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(23, 20);
            this.toolStripButton1.Text = "toolStripButton1";
            // 
            // toolStripButton2
            // 
            this.toolStripButton2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton2.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton2.Image")));
            this.toolStripButton2.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton2.Name = "toolStripButton2";
            this.toolStripButton2.Size = new System.Drawing.Size(23, 20);
            this.toolStripButton2.Text = "toolStripButton2";
            // 
            // LB
            // 
            this.LB.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.LB.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.LB.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LB.IntegralHeight = false;
            this.LB.ItemHeight = 16;
            this.LB.Location = new System.Drawing.Point(5, 4);
            this.LB.Name = "LB";
            this.LB.Size = new System.Drawing.Size(213, 212);
            this.LB.Sorted = true;
            this.LB.TabIndex = 0;
            this.LB.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.LB_DrawItem);
            this.LB.SelectedIndexChanged += new System.EventHandler(this.LB_SelectedIndexChanged);
            this.LB.DoubleClick += new System.EventHandler(this.LB_DoubleClick);
            this.LB.MouseMove += new System.Windows.Forms.MouseEventHandler(this.LB_MouseMove);
            this.LB.MouseDown += new System.Windows.Forms.MouseEventHandler(this.LB_MouseDown);
            this.LB.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.LB_KeyPress);
            this.LB.KeyUp += new System.Windows.Forms.KeyEventHandler(this.LB_KeyUp);
            this.LB.KeyDown += new System.Windows.Forms.KeyEventHandler(this.LB_KeyDown);
            this.LB.HandleDestroyed += new System.EventHandler(this.OnHandleDestroyed);
            // 
            // toolStripButton3
            // 
            this.toolStripButton3.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton3.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton3.Image")));
            this.toolStripButton3.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton3.Name = "toolStripButton3";
            this.toolStripButton3.Size = new System.Drawing.Size(23, 20);
            this.toolStripButton3.Text = "toolStripButton3";
            // 
            // AutoListForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.ClientSize = new System.Drawing.Size(221, 262);
            this.Controls.Add(this.toolBox);
            this.Controls.Add(this.LB);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "AutoListForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.VisibleChanged += new System.EventHandler(this.AutoListForm_VisibleChanged);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.AutoListForm_FormClosing);
            this.Resize += new System.EventHandler(this.AutoListForm_Resize);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.AutoListForm_KeyDown);
            this.toolBox.ResumeLayout(false);
            this.toolBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		/// <summary>		
		/// </summary>
		/// <param name="e"></param>
		protected override void OnPaint(PaintEventArgs e)
		{
			e.Graphics.Clear(SystemColors.Control);
			ControlPaint.DrawBorder3D(e.Graphics, 0, 0, this.Width, this.Height, Border3DStyle.Raised);

		}

		public void SelectItem(string Text)
		{
			Text = Text.ToLower();

			for (int i = 0; i < LB.Items.Count; i++)
			{
				ListItem li = (ListItem) LB.Items[i];
				string lis = li.Text.ToLower();
				if (lis.StartsWith(Text))
				{
					LB.SelectedIndex = i;
					break;
				}
			}
		}

		private void LB_KeyDown(object sender, KeyEventArgs e)
		{
			this.OnKeyDown(e);
			//	e.Handled =true;
		}

		private void LB_KeyPress(object sender, KeyPressEventArgs e)
		{
			this.OnKeyPress(e);
			//	e.Handled =true;
		}

		private void LB_KeyUp(object sender, KeyEventArgs e)
		{
			this.OnKeyUp(e);
			//	e.Handled =true;
		}

		/// <summary>
		/// For public use only.
		/// </summary>
		/// <param name="keyData"></param>
		/// <returns></returns>
		protected override bool IsInputKey(Keys keyData)
		{
			return true;
		}

		/// <summary>
		/// For public use only.
		/// </summary>
		/// <param name="charCode"></param>
		/// <returns></returns>
		protected override bool IsInputChar(char charCode)
		{
			return true;
		}

		/// <summary>
		/// Adds a new ListItem to the AutoListForm.
		/// </summary>
		/// <param name="Text">Text of the new ListItem</param>
		/// <param name="ImageIndex">Image index that should be assigned to the new ListItem</param>
		/// <returns></returns>
		public ListItem Add(string Text, int ImageIndex)
		{
			return this.Add(Text, Text, ImageIndex);
		}

		/// <summary>
		/// Adds a new ListItem to the AutoListForm.
		/// </summary>
		/// <param name="Text">Text of the new ListItem</param>
		/// <param name="InsertText">Text to insert when this item is selected</param>
		/// <param name="ImageIndex">Image index that should be assigned to the new ListItem</param>
		/// <returns></returns>
		public ListItem Add(string Text, string InsertText, int ImageIndex)
		{
			ListItem li = new ListItem(Text, ImageIndex, "", InsertText);
			this.LB.Items.Add(li);


			//this.LB.Sorted =true;
			return li;
		}

		public ListItem Add(string Text, string InsertText, string ToolTip, int ImageIndex)
		{
			ListItem li = new ListItem(Text, ImageIndex, "", InsertText);
			this.LB.Items.Add(li);
			li.ToolTip = ToolTip;
			//this.LB.Sorted =true;
			return li;
		}

		/// <summary>
		/// Clears the content of the AutoList.
		/// </summary>
		public void Clear()
		{
			this.LB.Items.Clear();
		}

		private void LB_DrawItem(object sender, DrawItemEventArgs e)
		{
			bool selected = (e.State & DrawItemState.Selected) == DrawItemState.Selected;

			if (e.Index == -1)
				return;

			int Offset = 24;

			ListItem li = (ListItem) LB.Items[e.Index];
			string text = li.Text;
			Brush bg, fg;

			if (selected)
			{
				bg = SystemBrushes.Highlight;
				fg = SystemBrushes.HighlightText;
				//fg=Brushes.Black;
			}
			else
			{
				bg = SystemBrushes.Window;
				fg = SystemBrushes.WindowText;
				//bg=Brushes.White;
				//fg=Brushes.Black;
			}

			if (!selected)
			{
				e.Graphics.FillRectangle(bg, 0, e.Bounds.Top, e.Bounds.Width, LB.ItemHeight);
				//e.Graphics.FillRectangle (SystemBrushes.Highlight,0,e.Bounds.Top,27 ,LB.ItemHeight); 
			}
			else
			{
				e.Graphics.FillRectangle(SystemBrushes.Window, Offset, e.Bounds.Top, e.Bounds.Width - Offset, LB.ItemHeight);
				e.Graphics.FillRectangle(SystemBrushes.Highlight, new Rectangle(Offset + 1, e.Bounds.Top + 1, e.Bounds.Width - Offset - 2, LB.ItemHeight - 2));


				//e.Graphics.FillRectangle (SystemBrushes.Highlight,27,e.Bounds.Top,e.Bounds.Width-27 ,LB.ItemHeight); 
				//e.Graphics.FillRectangle (new SolidBrush(Color.FromArgb (182,189,210)),new Rectangle (1+27,e.Bounds.Top+1,e.Bounds.Width-2- ,LB.ItemHeight-2));


				ControlPaint.DrawFocusRectangle(e.Graphics, new Rectangle(Offset, e.Bounds.Top, e.Bounds.Width - Offset, LB.ItemHeight));
			}


			e.Graphics.DrawString(text, e.Font, fg, Offset + 2, e.Bounds.Top + 1);


			if (Images != null)
				e.Graphics.DrawImage(Images.Images[li.Type], 6, e.Bounds.Top + 0);

		}

		/// <summary>
		/// Gets the "insert text" from the selected item.
		/// </summary>
		public string SelectedText
		{
			get
			{
				if (LB.SelectedItem == null)
					return "";

				ListItem li = (ListItem) LB.SelectedItem;
				return li.InsertText;
			}
		}

		/// <summary>
		/// Gets the "insert full text" from the selected item.
		/// </summary>
		public string SelectedFullText
		{
			get
			{
				if (LB.SelectedItem == null)
					return "";

				ListItem li = (ListItem) LB.SelectedItem;
				return li.Text;
			}
		}

		private void LB_DoubleClick(object sender, EventArgs e)
		{
			this.OnDoubleClick(e);
		}

		public void BeginLoad()
		{
			this.LB.Sorted = false;
			this.LB.DrawMode = DrawMode.Normal;
			this.LB.SuspendLayout();
		}

        private void EndLoadPre()
        {
			this.LB.ResumeLayout();
			this.LB.Sorted = true;
			this.LB.DrawMode = DrawMode.OwnerDrawFixed;

			//set height
			this.Height = 0;
			if (this.LB.Items.Count > 10)
			{
				this.Height = this.LB.ItemHeight*11 + 12;
			}
			else
			{
				this.Height = this.LB.ItemHeight*(this.LB.Items.Count) + 12;
			}
            this.Height += this.toolBox.Height;
			int max = 0;
			Graphics g = LB.CreateGraphics();
			foreach (ListItem li in LB.Items)
			{
				int w = (int) g.MeasureString(li.Text, LB.Font).Width + 45;
				if (w > max)
					max = w;
			}
            if (max < toolBox.Items.Count * 20)
            {
                max = toolBox.Items.Count * 20;
            }
			this.Width = max + SystemInformation.VerticalScrollBarWidth;
            this.Refresh();
            g.Dispose();
        }

		public void EndLoad()
		{
            //初始化
            this.ToolBoxDock = toolBox.Dock;
            toolBox.Items.Clear();

            //插入一个“全部”的按钮
            ToolStripItem tsiAll = toolBox.Items.Add(Images.Images[6]);
            tsiAll.Tag = "";
            tsiAll.ToolTipText = "显示全部";
            tsiAll.Click += new EventHandler(tsi_Click);
            tsiAll.Text = "0";
            tsiAll.DisplayStyle = ToolStripItemDisplayStyle.Image;
            (tsiAll as ToolStripButton).CheckState = CheckState.Checked;
            
            //设置分类按钮相关
            int nPos = 1;
            foreach(ListItem li in LB.Items)
            {
                foreach(ToolStripItem tsiItem in toolBox.Items)
                {
                    if (tsiItem.Tag.ToString() == li.Type.ToString())
                        goto label1;
                }
                ToolStripItem tsi = toolBox.Items.Add(Images.Images[li.Type]);
                tsi.Tag = li.Type.ToString();
                tsi.Click += new EventHandler(tsi_Click);
                tsi.ToolTipText = li.Type.ToString();
                tsi.DisplayStyle = ToolStripItemDisplayStyle.Image;
                tsi.Text = nPos.ToString();
                nPos++;
            label1: ;
            }


            //复制一份LB.Item
            ArrayList ali = new ArrayList();
            foreach (ListItem li in LB.Items)
            {
                ali.Add(new string[] { li.InsertText, li.Text, li.ToolTip, li.Type.ToString() });
            }
            LB.Tag = ali;

            EndLoadPre();
		}
        
        //当前选中的索引

        private int m_currSelectedIndex = 0;
        private void tsi_Click(object sender, EventArgs e)
        {
            this.LB.Sorted = false;
            this.LB.DrawMode = DrawMode.Normal;
            this.LB.SuspendLayout();
            this.LB.Items.Clear();

            foreach(ToolStripButton tsb in toolBox.Items)
            {
                tsb.CheckState = CheckState.Unchecked;
            }
            ToolStripItem tsi = sender as ToolStripItem;
            (tsi as ToolStripButton).CheckState = CheckState.Indeterminate;
            m_currSelectedIndex = Int32.Parse(tsi.Text);
            foreach (string[] asItem in LB.Tag as ArrayList)
            {
                if (tsi.Tag.ToString() == asItem[3] || tsi.Tag.ToString() == "")
                {
                    Add(asItem[1], asItem[0], asItem[2], Int32.Parse(asItem[3]));
                }
            }

            EndLoadPre();
        }

        //设置ToolBox在上面还是在下面
        public DockStyle ToolBoxDock
        {
            get { return toolBox.Dock; }
            set
            {
                toolBox.Dock = value;
                if(value == DockStyle.Bottom)
                {
                    LB.Top = 4;
                }
                else
                {
                    LB.Top = toolBox.Height + 4;
                }
            }
        }


		private void AutoListForm_Resize(object sender, EventArgs e)
		{
            LB.Size = new Size(this.Width - 8, this.Height - 8 - this.toolBox.Height);
            ListItem li = (ListItem)LB.SelectedItem;
            if(li != null)  showToolTip(li.ToolTip);
		}

		private void LB_SelectedIndexChanged(object sender, EventArgs e)
		{
			ListItem li = (ListItem) LB.SelectedItem;
            showToolTip(li.ToolTip);
		}

        //显示ToolTip窗口
        private void showToolTip(string strText)
        {
            if (strText == "" || strText==null)
            {
                tooltip.Hide(this.ParentControl);
                return;
            }
            tooltip.ShowAlways = true;
            EditViewControl evc = this.ParentControl as EditViewControl;
            Point p = evc.PointToClient(this.Location);
            p.X += this.Width;
            tooltip.Show(strText, this.ParentControl, p);
            tooltip.InitialDelay = 2;
            tooltip.Active = true;
        }

		private void LB_MouseDown(object sender, MouseEventArgs e)
		{
			SelectItem(e.X, e.Y);
		}

		private void SelectItem(int x, int y)
		{
			Point p = new Point(x, y);
			int r = (p.Y/LB.ItemHeight) + LB.TopIndex;
			if (r != LB.SelectedIndex)
			{
				if (r < LB.Items.Count && r >= 0)
				{
					LB.SelectedIndex = r;

				}
			}

		}

		private void LB_MouseMove(object sender, MouseEventArgs e)
		{
			if (e.Button != 0)
			{
				SelectItem(e.X, e.Y);
			}
		}

        private void tooltip_Popup(object sender, PopupEventArgs e)
        {

        }

        private void AutoListForm_FormClosing(object sender, FormClosingEventArgs e)
        {

        }

        private void AutoListForm_VisibleChanged(object sender, EventArgs e)
        {
            //强制让ToolTip跟随AotuListForm的显示显示，隐藏隐藏。
            if(this.Visible == true)
            {
                ListItem li = (ListItem)LB.SelectedItem;
                if (li != null) showToolTip(li.ToolTip);
            }
            else
            {
                showToolTip("");
            }
        }

        private void AutoListForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode != Keys.Right && e.KeyCode != Keys.Left)
                return;
            int nNewIndex = m_currSelectedIndex;
            if(e.KeyCode == Keys.Left)
            {
                nNewIndex--;
            }
            else if(e.KeyCode == Keys.Right)
            {
                nNewIndex++;
            }
            if (nNewIndex > toolBox.Items.Count - 1) nNewIndex = 0;
            if (nNewIndex < 0) nNewIndex = toolBox.Items.Count - 1;
            this.tsi_Click(toolBox.Items[nNewIndex], new EventArgs());
        }
	}
}