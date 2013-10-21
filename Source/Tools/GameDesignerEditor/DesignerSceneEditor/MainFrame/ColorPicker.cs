using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace DesignerSceneEditor
{
	/// <summary>
	/// Summary description for ColorPicker.
	/// </summary>
	public class ColorPicker : System.Windows.Forms.UserControl
	{

		private Color[] m_CustomColors=new Color[48];
		private Rectangle[] m_CustomColorsPos=new Rectangle[48];
		private System.Windows.Forms.ListBox listWeb;
		private System.Windows.Forms.ListBox listSystem;
		private System.ComponentModel.IContainer components;
		internal DevComponents.DotNetBar.TabStrip tabStrip1;
		private DevComponents.DotNetBar.TabItem tabCustom;
		private DevComponents.DotNetBar.TabItem tabWeb;
		private DevComponents.DotNetBar.TabItem tabSystem;
		private System.Windows.Forms.Panel colorPanel;

		public Color SelectedColor=Color.Empty;

		public ColorPicker()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			InitCustomColors();
			InitOtherColors();
		}

		private void InitCustomColors()
		{
			m_CustomColors[0]=Color.FromArgb(255,255,255);
			m_CustomColors[1]=Color.FromArgb(255,195,198);
			m_CustomColors[2]=Color.FromArgb(255,227,198);
			m_CustomColors[3]=Color.FromArgb(255,255,198);
			m_CustomColors[4]=Color.FromArgb(198,255,198);
			m_CustomColors[5]=Color.FromArgb(198,255,255);
			m_CustomColors[6]=Color.FromArgb(198,195,255);
			m_CustomColors[7]=Color.FromArgb(255,195,255);

			m_CustomColors[8]=Color.FromArgb(231,227,231);
			m_CustomColors[9]=Color.FromArgb(255,130,132);
			m_CustomColors[10]=Color.FromArgb(255,195,132);
			m_CustomColors[11]=Color.FromArgb(255,255,132);
			m_CustomColors[12]=Color.FromArgb(132,255,132);
			m_CustomColors[13]=Color.FromArgb(132,255,255);
			m_CustomColors[14]=Color.FromArgb(132,130,255);
			m_CustomColors[15]=Color.FromArgb(255,130,255);

			m_CustomColors[16]=Color.FromArgb(198,195,198);
			m_CustomColors[17]=Color.FromArgb(255,0,0);
			m_CustomColors[18]=Color.FromArgb(255,130,0);
			m_CustomColors[19]=Color.FromArgb(255,255,0);
			m_CustomColors[20]=Color.FromArgb(0,255,0);
			m_CustomColors[21]=Color.FromArgb(0,255,255);
			m_CustomColors[22]=Color.FromArgb(0,0,255);
			m_CustomColors[23]=Color.FromArgb(255,0,255);

			m_CustomColors[24]=Color.FromArgb(132,130,132);
			m_CustomColors[25]=Color.FromArgb(198,0,0);
			m_CustomColors[26]=Color.FromArgb(198,65,0);
			m_CustomColors[27]=Color.FromArgb(198,195,0);
			m_CustomColors[28]=Color.FromArgb(0,195,0);
			m_CustomColors[29]=Color.FromArgb(0,195,198);
			m_CustomColors[30]=Color.FromArgb(0,0,198);
			m_CustomColors[31]=Color.FromArgb(198,0,198);

			m_CustomColors[32]=Color.FromArgb(66,65,66);
			m_CustomColors[33]=Color.FromArgb(132,0,0);
			m_CustomColors[34]=Color.FromArgb(132,65,0);
			m_CustomColors[35]=Color.FromArgb(132,130,0);
			m_CustomColors[36]=Color.FromArgb(0,130,0);
			m_CustomColors[37]=Color.FromArgb(0,130,132);
			m_CustomColors[38]=Color.FromArgb(0,0,132);
			m_CustomColors[39]=Color.FromArgb(132,0,132);

			m_CustomColors[40]=Color.FromArgb(0,0,0);
			m_CustomColors[41]=Color.FromArgb(66,0,0);
			m_CustomColors[42]=Color.FromArgb(132,65,66);
			m_CustomColors[43]=Color.FromArgb(66,65,0);
			m_CustomColors[44]=Color.FromArgb(0,65,0);
			m_CustomColors[45]=Color.FromArgb(0,65,66);
			m_CustomColors[46]=Color.FromArgb(0,0,66);
			m_CustomColors[47]=Color.FromArgb(66,0,66);
		}

		private void InitOtherColors()
		{
			listWeb.BeginUpdate();
			listWeb.Items.Clear();
			Type type = typeof(Color);
			System.Reflection.PropertyInfo[] fields=type.GetProperties(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static);
			Color clr=new Color();
			foreach(System.Reflection.PropertyInfo pi in fields)
				listWeb.Items.Add(pi.GetValue(clr,null));
			listWeb.EndUpdate();

			listSystem.BeginUpdate();
			listSystem.Items.Clear();
            type=typeof(SystemColors);
			fields=type.GetProperties(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static);
			foreach(System.Reflection.PropertyInfo pi in fields)
				listSystem.Items.Add(pi.GetValue(clr,null));
			listSystem.EndUpdate();
		}


		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.listSystem = new System.Windows.Forms.ListBox();
			this.listWeb = new System.Windows.Forms.ListBox();
			this.tabStrip1 = new DevComponents.DotNetBar.TabStrip();
			this.tabSystem = new DevComponents.DotNetBar.TabItem(this.components);
			this.tabCustom = new DevComponents.DotNetBar.TabItem(this.components);
			this.colorPanel = new System.Windows.Forms.Panel();
			this.tabWeb = new DevComponents.DotNetBar.TabItem(this.components);
			this.SuspendLayout();
			// 
			// listSystem
			// 
			this.listSystem.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listSystem.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
			this.listSystem.IntegralHeight = false;
			this.listSystem.Name = "listSystem";
			this.listSystem.Size = new System.Drawing.Size(200, 158);
			this.listSystem.TabIndex = 0;
			this.listSystem.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.DrawSystemItem);
			this.listSystem.SelectedIndexChanged += new System.EventHandler(this.SystemSelectionChange);
			// 
			// listWeb
			// 
			this.listWeb.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listWeb.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
			this.listWeb.IntegralHeight = false;
			this.listWeb.Name = "listWeb";
			this.listWeb.Size = new System.Drawing.Size(200, 158);
			this.listWeb.TabIndex = 0;
			this.listWeb.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.DrawWebItem);
			this.listWeb.SelectedIndexChanged += new System.EventHandler(this.WebSelectionChange);
			// 
			// tabStrip1
			// 
			this.tabStrip1.CanReorderTabs = true;
			this.tabStrip1.CloseButtonVisible = true;
			this.tabStrip1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.tabStrip1.ForeColor = System.Drawing.SystemColors.ControlDark;
			this.tabStrip1.Location = new System.Drawing.Point(0, 158);
			this.tabStrip1.Name = "tabStrip1";
			this.tabStrip1.SelectedTab = this.tabCustom;
			this.tabStrip1.Size = new System.Drawing.Size(200, 24);
			this.tabStrip1.TabIndex = 1;
			this.tabStrip1.Tabs.Add(this.tabCustom);
			this.tabStrip1.Tabs.Add(this.tabWeb);
			this.tabStrip1.Tabs.Add(this.tabSystem);
			this.tabStrip1.Text = "tabStrip1";
			// 
			// tabSystem
			// 
			this.tabSystem.AttachedControl = this.listSystem;
			this.tabSystem.Name = "tabSystem";
			this.tabSystem.PredefinedColor = DevComponents.DotNetBar.eTabItemColor.Default;
			this.tabSystem.Text = "System";
			// 
			// tabCustom
			// 
			this.tabCustom.AttachedControl = this.colorPanel;
			this.tabCustom.Name = "tabCustom";
			this.tabCustom.PredefinedColor = DevComponents.DotNetBar.eTabItemColor.Default;
			this.tabCustom.Text = "Custom";
			// 
			// colorPanel
			// 
			this.colorPanel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.colorPanel.Name = "colorPanel";
			this.colorPanel.Size = new System.Drawing.Size(200, 158);
			this.colorPanel.TabIndex = 2;
			this.colorPanel.MouseUp += new System.Windows.Forms.MouseEventHandler(this.CustomColorMouseUp);
			this.colorPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.PaintCustomColors);
			// 
			// tabWeb
			// 
			this.tabWeb.AttachedControl = this.listWeb;
			this.tabWeb.Name = "tabWeb";
			this.tabWeb.PredefinedColor = DevComponents.DotNetBar.eTabItemColor.Default;
			this.tabWeb.Text = "Web";
			// 
			// ColorPicker
			// 
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.listWeb,
																		  this.listSystem,
																		  this.colorPanel,
																		  this.tabStrip1});
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "ColorPicker";
			this.Size = new System.Drawing.Size(200, 182);
			this.ResumeLayout(false);

		}
		#endregion

		private void PaintCustomColors(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			Rectangle r=Rectangle.Empty;
			int x=6, y=6;
			Graphics g=e.Graphics;
			Border3DSide side=(Border3DSide.Left | Border3DSide.Right | Border3DSide.Top | Border3DSide.Bottom);
			int width=colorPanel.ClientRectangle.Width;
			int iIndex=0;
			foreach(Color clr in m_CustomColors)
			{
				r=new Rectangle(x,y,21,21);
				if(r.Right>width)
				{
					y+=25;
					x=6;
					r.X=x;
					r.Y=y;
				}
				ControlPaint.DrawBorder3D(g,x,y,21,21,Border3DStyle.Sunken,side);
				r.Inflate(-2,-2);
				g.FillRectangle(new SolidBrush(clr),r);

				m_CustomColorsPos[iIndex]=r;
				iIndex++;

				x+=24;
			}
		}

		private void DrawWebItem(object sender, System.Windows.Forms.DrawItemEventArgs e)
		{
            Rectangle r=e.Bounds;
			Rectangle rClr=new Rectangle(r.X+1,r.Y+2,24,r.Height-4);
			
			Brush textbrush=SystemBrushes.ControlText;
			if((e.State & DrawItemState.Selected)!=0)
			{
				textbrush=SystemBrushes.HighlightText;
				e.Graphics.FillRectangle(SystemBrushes.Highlight,e.Bounds);
			}
			else
				e.Graphics.FillRectangle(SystemBrushes.Window,e.Bounds);

			Color clr=(Color)listWeb.Items[e.Index];
            e.Graphics.FillRectangle(new SolidBrush(clr),rClr);
			e.Graphics.DrawRectangle(SystemPens.ControlText,rClr);
			r.Offset(30,0);
			r.Width-=30;
			e.Graphics.DrawString(clr.Name,listWeb.Font,textbrush,r,StringFormat.GenericTypographic);
		}

		private void DrawSystemItem(object sender, System.Windows.Forms.DrawItemEventArgs e)
		{
			Rectangle r=e.Bounds;
			Rectangle rClr=new Rectangle(r.X+1,r.Y+2,24,r.Height-4);
			
			Brush textbrush=SystemBrushes.ControlText;
			if((e.State & DrawItemState.Selected)!=0)
			{
				textbrush=SystemBrushes.HighlightText;
				e.Graphics.FillRectangle(SystemBrushes.Highlight,e.Bounds);
			}
			else
                e.Graphics.FillRectangle(SystemBrushes.Window,e.Bounds);

			Color clr=(Color)listSystem.Items[e.Index];
			e.Graphics.FillRectangle(new SolidBrush(clr),rClr);
			e.Graphics.DrawRectangle(SystemPens.ControlText,rClr);
			r.Offset(30,0);
			r.Width-=30;
			e.Graphics.DrawString(clr.Name,listWeb.Font,textbrush,r,StringFormat.GenericTypographic);
		}

		private void CustomColorMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			for(int i=0;i<48;i++)
			{
				if(m_CustomColorsPos[i].Contains(e.X,e.Y))
				{
					SelectedColor=m_CustomColors[i];
					ColorSelected();
                    break;					
				}
			}
		}
        
		private void ColorSelected()
		{
			DevComponents.DotNetBar.PopupContainerControl ctrl=this.Parent as DevComponents.DotNetBar.PopupContainerControl;
			if(ctrl==null)
				return;

			// Change the Parent Item image to indicate which color was selected last
			// Assumes that Image with ImageIndex 21 is used on button
			DevComponents.DotNetBar.ButtonItem btn=ctrl.ParentItem as DevComponents.DotNetBar.ButtonItem;
			Bitmap bmp=new Bitmap(btn.ImageList.Images[21],btn.ImageList.ImageSize);
			Graphics g=Graphics.FromImage(bmp);
			g.DrawImageUnscaled(btn.ImageList.Images[21],0,0);
			using(SolidBrush brush=new SolidBrush(this.SelectedColor))
				g.FillRectangle(brush,0,12,16,4);
			g.Dispose();
			bmp.MakeTransparent(btn.ImageList.TransparentColor);
			btn.ImageIndex=-1;
			DevComponents.DotNetBar.DotNetBarManager manager=null;
			if(btn.ContainerControl is DevComponents.DotNetBar.Bar)
				manager=((DevComponents.DotNetBar.Bar)btn.ContainerControl).Owner as DevComponents.DotNetBar.DotNetBarManager;
			else if(btn.ContainerControl is DevComponents.DotNetBar.MenuPanel)
                manager=((DevComponents.DotNetBar.MenuPanel)btn.ContainerControl).Owner as DevComponents.DotNetBar.DotNetBarManager;
			if(manager!=null && btn.Name!="")
			{
				ArrayList items=manager.GetItems(btn.Name,true);
				foreach(DevComponents.DotNetBar.ButtonItem item in items)
					item.Image=bmp;
			}
			else
				btn.Image=bmp;

			DevComponents.DotNetBar.BaseItem topItem=ctrl.ParentItem;
			while(topItem.Parent is DevComponents.DotNetBar.ButtonItem)
				topItem=topItem.Parent;
			topItem.Expanded=false;
			if(topItem.Parent!=null)
				topItem.Parent.AutoExpand=false;
		}

		private void WebSelectionChange(object sender, System.EventArgs e)
		{
			if(listWeb.SelectedItem!=null)
			{
				SelectedColor=(Color)listWeb.SelectedItem;
				ColorSelected();
			}
		}

		private void SystemSelectionChange(object sender, System.EventArgs e)
		{
			if(listSystem.SelectedItem!=null)
			{
				SelectedColor=(Color)listSystem.SelectedItem;
				ColorSelected();
			}
		}

		protected override void OnBackColorChanged(EventArgs e)
		{
			base.OnBackColorChanged(e);
			colorPanel.BackColor=this.BackColor;
		}
	}
}
