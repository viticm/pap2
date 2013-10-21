using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Data;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Drawing.Drawing2D;
using System.Reflection;

/// <summary>
///	Mikael Wiberg, Copyright 2003, 2004
///		My mikwib address can be found on hotmail.com (usual HoTMaiL spam filters)
///		My mick address can be found on ar.com.au (heavy spam filters on, harldy anything gets through, START the subject with C# and it will probably go through)
///		My mik address can be found on netatonce.net (heavy spam filters on, harldy anything gets through, START the subject with C# and it will probably go through)
///	
///	Feel free to use the MWTreeView as you wish, as long as you do not take credit for it yourself (see next line).
///	However if it is used in commercial projects or applications it is NOT, I repeat NOT, free. Please contact me for further information.
///		Note that this differs from previous versions of the MWTreeView and I have made this decision in order to be duly compensated for my work
///			and time spent.
///	Use the MWTreeView at your own risk. I take no responsibility for ill effects happening directly or indirectly through the use of the MWTreeView.
/// </summary>
namespace MWControlSuite
{
	/// <summary>
	/// EditorHatchStyleUI is used in conjunction with the EditorHatchStyle UITypeEditor.
	/// </summary>
	public class EditorHatchStyleUI : System.Windows.Forms.UserControl
	{
		#region Variables

		private ITypeDescriptorContext itdc = null;
		private IWindowsFormsEditorService iwfes = null;
		private HatchStyle hsHatchStyle = HatchStyle.Weave;

		private Color cHatchStyleBackColor = Color.White;
		private Color cHatchStyleForeColor = Color.Black;
		private int iHatchStyleBoxWidth = 30;
		private int iHatchStyleBoxHeight = 30;
		private int iItemPadY = 1;
		private int iItemPadX = 6;

		bool bSmall = true;

		private System.Windows.Forms.ListBox lbHatchStyles;
		private System.Windows.Forms.Label lblHatchStyleNameLabel;
		private System.Windows.Forms.Label lblHatchStyleName;
		private System.Windows.Forms.Button btChangeSize;
		private System.Windows.Forms.Panel pLargeSample;
		private System.Windows.Forms.Button btAccept;
		private System.Windows.Forms.Button btCancel;
		private System.Windows.Forms.Label lblBackColor;
		private System.Windows.Forms.Label lblForeColor;
		private System.Windows.Forms.Panel pBackColor;
		private System.Windows.Forms.Panel pForeColor;
		private System.Windows.Forms.Panel pChangeColors;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		#endregion Variables



		#region Constructors and Dispose

		/// <summary>
		/// Standard constructor.
		/// </summary>
		public EditorHatchStyleUI()
		{
			InitializeComponent();

			AddHatchStylesToListBox();

			this.Width = 224;
		}

		/// <summary>
		/// Constructor that takes a HatchStyle, BackColor and ForeColor as arguments.
		/// </summary>
		/// <param name="hs">HatchStyle to be used in the instance created.</param>
		/// <param name="cBack">BackColor to be used in the instance created.</param>
		/// <param name="cFore">ForeColor to be used in the instance created.</param>
		public EditorHatchStyleUI(HatchStyle hs, Color cBack, Color cFore)
		{
			InitializeComponent();

			AddHatchStylesToListBox();

			hsHatchStyle = hs;

			cHatchStyleBackColor = cBack;
			cHatchStyleForeColor = cFore;

			this.Width = 224;
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if(disposing)
			{
				if(components != null)
				{
					components.Dispose();
				}
			}

			base.Dispose(disposing);
		}

		#endregion Constructors and Dispose



		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.lbHatchStyles = new System.Windows.Forms.ListBox();
			this.lblHatchStyleNameLabel = new System.Windows.Forms.Label();
			this.lblHatchStyleName = new System.Windows.Forms.Label();
			this.btChangeSize = new System.Windows.Forms.Button();
			this.pLargeSample = new System.Windows.Forms.Panel();
			this.btAccept = new System.Windows.Forms.Button();
			this.btCancel = new System.Windows.Forms.Button();
			this.lblBackColor = new System.Windows.Forms.Label();
			this.lblForeColor = new System.Windows.Forms.Label();
			this.pBackColor = new System.Windows.Forms.Panel();
			this.pForeColor = new System.Windows.Forms.Panel();
			this.pChangeColors = new System.Windows.Forms.Panel();
			this.pChangeColors.SuspendLayout();
			this.SuspendLayout();
			// 
			// lbHatchStyles
			// 
			this.lbHatchStyles.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left)));
			this.lbHatchStyles.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
			this.lbHatchStyles.IntegralHeight = false;
			this.lbHatchStyles.Location = new System.Drawing.Point(0, 0);
			this.lbHatchStyles.Name = "lbHatchStyles";
			this.lbHatchStyles.Size = new System.Drawing.Size(208, 248);
			this.lbHatchStyles.TabIndex = 0;
			this.lbHatchStyles.MeasureItem += new System.Windows.Forms.MeasureItemEventHandler(this.lbHatchStyles_MeasureItem);
			this.lbHatchStyles.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.lbHatchStyles_DrawItem);
			this.lbHatchStyles.SelectedIndexChanged += new System.EventHandler(this.lbHatchStyles_SelectedIndexChanged);
			// 
			// lblHatchStyleNameLabel
			// 
			this.lblHatchStyleNameLabel.Location = new System.Drawing.Point(216, 0);
			this.lblHatchStyleNameLabel.Name = "lblHatchStyleNameLabel";
			this.lblHatchStyleNameLabel.Size = new System.Drawing.Size(144, 16);
			this.lblHatchStyleNameLabel.TabIndex = 1;
			this.lblHatchStyleNameLabel.Text = "HatchStyle Name:";
			this.lblHatchStyleNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// lblHatchStyleName
			// 
			this.lblHatchStyleName.Location = new System.Drawing.Point(208, 16);
			this.lblHatchStyleName.Name = "lblHatchStyleName";
			this.lblHatchStyleName.Size = new System.Drawing.Size(160, 16);
			this.lblHatchStyleName.TabIndex = 2;
			this.lblHatchStyleName.Text = "Dashed Downward Diagonal";
			this.lblHatchStyleName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblHatchStyleName.Visible = false;
			// 
			// btChangeSize
			// 
			this.btChangeSize.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.btChangeSize.BackColor = System.Drawing.SystemColors.Control;
			this.btChangeSize.Location = new System.Drawing.Point(352, 0);
			this.btChangeSize.Name = "btChangeSize";
			this.btChangeSize.Size = new System.Drawing.Size(16, 16);
			this.btChangeSize.TabIndex = 3;
			this.btChangeSize.Click += new System.EventHandler(this.btChangeSize_Click);
			this.btChangeSize.Paint += new System.Windows.Forms.PaintEventHandler(this.btChangeSize_Paint);
			// 
			// pLargeSample
			// 
			this.pLargeSample.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pLargeSample.Location = new System.Drawing.Point(224, 56);
			this.pLargeSample.Name = "pLargeSample";
			this.pLargeSample.Size = new System.Drawing.Size(128, 128);
			this.pLargeSample.TabIndex = 4;
			this.pLargeSample.Paint += new System.Windows.Forms.PaintEventHandler(this.pLargeSample_Paint);
			// 
			// btAccept
			// 
			this.btAccept.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.btAccept.Location = new System.Drawing.Point(224, 32);
			this.btAccept.Name = "btAccept";
			this.btAccept.Size = new System.Drawing.Size(56, 23);
			this.btAccept.TabIndex = 5;
			this.btAccept.Text = "Accept";
			this.btAccept.Click += new System.EventHandler(this.btAccept_Click);
			// 
			// btCancel
			// 
			this.btCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.btCancel.Location = new System.Drawing.Point(296, 32);
			this.btCancel.Name = "btCancel";
			this.btCancel.Size = new System.Drawing.Size(56, 23);
			this.btCancel.TabIndex = 5;
			this.btCancel.Text = "Cancel";
			this.btCancel.Click += new System.EventHandler(this.btCancel_Click);
			// 
			// lblBackColor
			// 
			this.lblBackColor.Location = new System.Drawing.Point(0, 0);
			this.lblBackColor.Name = "lblBackColor";
			this.lblBackColor.Size = new System.Drawing.Size(64, 24);
			this.lblBackColor.TabIndex = 6;
			this.lblBackColor.Text = "Back Color";
			this.lblBackColor.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblForeColor
			// 
			this.lblForeColor.Location = new System.Drawing.Point(0, 24);
			this.lblForeColor.Name = "lblForeColor";
			this.lblForeColor.Size = new System.Drawing.Size(64, 24);
			this.lblForeColor.TabIndex = 7;
			this.lblForeColor.Text = "Fore Color";
			this.lblForeColor.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// pBackColor
			// 
			this.pBackColor.BackColor = System.Drawing.Color.Black;
			this.pBackColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pBackColor.Location = new System.Drawing.Point(64, 0);
			this.pBackColor.Name = "pBackColor";
			this.pBackColor.Size = new System.Drawing.Size(24, 24);
			this.pBackColor.TabIndex = 8;
			this.pBackColor.Click += new System.EventHandler(this.pBackColor_Click);
			// 
			// pForeColor
			// 
			this.pForeColor.BackColor = System.Drawing.Color.White;
			this.pForeColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pForeColor.Location = new System.Drawing.Point(64, 24);
			this.pForeColor.Name = "pForeColor";
			this.pForeColor.Size = new System.Drawing.Size(24, 24);
			this.pForeColor.TabIndex = 9;
			this.pForeColor.Click += new System.EventHandler(this.pForeColor_Click);
			// 
			// pChangeColors
			// 
			this.pChangeColors.Controls.Add(this.lblForeColor);
			this.pChangeColors.Controls.Add(this.lblBackColor);
			this.pChangeColors.Controls.Add(this.pBackColor);
			this.pChangeColors.Controls.Add(this.pForeColor);
			this.pChangeColors.Location = new System.Drawing.Point(240, 192);
			this.pChangeColors.Name = "pChangeColors";
			this.pChangeColors.Size = new System.Drawing.Size(96, 48);
			this.pChangeColors.TabIndex = 10;
			// 
			// EditorHatchStyleUI
			// 
			this.Controls.Add(this.pChangeColors);
			this.Controls.Add(this.btAccept);
			this.Controls.Add(this.pLargeSample);
			this.Controls.Add(this.btChangeSize);
			this.Controls.Add(this.lblHatchStyleName);
			this.Controls.Add(this.lblHatchStyleNameLabel);
			this.Controls.Add(this.lbHatchStyles);
			this.Controls.Add(this.btCancel);
			this.Name = "EditorHatchStyleUI";
			this.Size = new System.Drawing.Size(368, 248);
			this.Resize += new System.EventHandler(this.EditorHatchStyleUI_Resize);
			this.Load += new System.EventHandler(this.EditorHatchStyleUI_Load);
			this.pChangeColors.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion Component Designer generated code



		#region Properties and EventHandlers

		#region HatchStyle

		/// <summary>
		/// Changes the TextDir and sets the new value for this ITDC.
		/// </summary>
		[
		Browsable(false),
		Category("Appearance"),
		Description("HatchStyle"),
		DefaultValue(HatchStyle.Weave),
		Editor(typeof(EditorHatchStyle), typeof(UITypeEditor))
		]
		public HatchStyle HatchStyle
		{
			get
			{
				return hsHatchStyle;
			}
			set
			{
				HatchStyle hsHatchStyleOld = hsHatchStyle;

				hsHatchStyle = value;
				lblHatchStyleName.Text = GetSpaceSeparatedString(hsHatchStyle.ToString());
				lbHatchStyles.SelectedItem = hsHatchStyle;

				//Call the EventHandler so that EditorHatchStyle's HatchStyle EventHandler will pick it up and update.
				OnHatchStyleChanged(new MWHatchStyleEventArgs(hsHatchStyleOld, hsHatchStyle));

				this.Refresh();

				if(this.ITDC != null)
				{
					this.ITDC.PropertyDescriptor.SetValue(this.ITDC.Instance, hsHatchStyle);
				}
			}
		}

		/// <summary>
		/// A delegate for event MWHatchStyleEventHandler.
		/// </summary>
		public delegate void MWHatchStyleEventHandler(object sender, MWHatchStyleEventArgs e);

		/// <summary>
		/// Occurs when the HatchStyle property changes.
		/// </summary>
		[
		Browsable(true),
		Category("HatchStyle"),
		Description("Occurs when the HatchStyle property changes.")
		]
		public event MWHatchStyleEventHandler HatchStyleChanged;

		protected virtual void OnHatchStyleChanged(MWHatchStyleEventArgs e)
		{
			if(HatchStyleChanged != null)
			{
				HatchStyleChanged(this, e);
			}
		}

		#endregion HatchStyle



		#region ITDC

		/// <summary>
		/// The ITypeDescriptorContext of this Control.
		/// Used at design time.
		/// </summary>
		[
		Browsable(false),
		Category("Design Time"),
		Description("ITypeDescriptorContext of this Control."),
		DefaultValue(null)
		]
		public ITypeDescriptorContext ITDC
		{
			get
			{
				return itdc;
			}
			set
			{
				itdc = value;
			}
		}

		#endregion ITDC



		#region IWFES

		/// <summary>
		/// The IWindowsFormsEditorService of this Control.
		/// Used at design time.
		/// </summary>
		[
		Browsable(false),
		Category("Design Time"),
		Description("IWindowsFormsEditorService of this Control."),
		DefaultValue(null)
		]
		public IWindowsFormsEditorService IWFES
		{
			get
			{
				return iwfes;
			}
			set
			{
				iwfes = value;
			}
		}

		#endregion IWFES

		#endregion Properties and EventHandlers



		#region EventHandlers

		#region EditorHatchStyleUI

		/// <summary>
		/// Standard Load EventHandler for this EditorHatchStyleUI.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void EditorHatchStyleUI_Load(object sender, System.EventArgs e)
		{
			//It doesn't seem as if it is possible to open a ColorDialog when a UITypeEditor is open.
			//	Therefore make the color changing options invisible.
			if(this.IWFES != null)
			{
				pChangeColors.Visible = false;
			}
		}

		/// <summary>
		/// Standard Resize EventHandler for this EditorHatchStyleUI.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void EditorHatchStyleUI_Resize(object sender, System.EventArgs e)
		{
			if(bSmall)
			{
				this.Width = 224;
			}
			else
			{
				this.Width = 368;
			}

			if(pChangeColors.Visible)
			{
				this.Height = 248;
			}
			else
			{
				this.Height = 196;
			}

			this.Refresh();
		}

		#endregion EditorHatchStyleUI



		#region ListBox

		/// <summary>
		/// Standard MeasureItem EventHandler for ListBox lbHatchStyles.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard MeasureItemEventArgs object.</param>
		private void lbHatchStyles_MeasureItem(object sender, System.Windows.Forms.MeasureItemEventArgs e)
		{
			e.ItemHeight = iHatchStyleBoxWidth + 2 * iItemPadY;
			e.ItemWidth = lbHatchStyles.ClientRectangle.Width;
		}

		/// <summary>
		/// Standard DrawItem EventHandler for ListBox lbHatchStyles.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard DrawItemEventArgs object.</param>
		private void lbHatchStyles_DrawItem(object sender, System.Windows.Forms.DrawItemEventArgs e)
		{
			SolidBrush brshText = new SolidBrush(e.ForeColor);
			string str = lbHatchStyles.Items[e.Index].ToString();
			SizeF szfString = e.Graphics.MeasureString(str, e.Font);
			int iYPos = e.Bounds.Top + (e.Bounds.Height - (int)szfString.Height) / 2;
			int iXPos = iItemPadX + iHatchStyleBoxWidth + iItemPadX;

			e.DrawBackground();
			e.Graphics.DrawString(str, e.Font, brshText, iXPos, iYPos);

			PaintHatchStyle(e, (HatchStyle)lbHatchStyles.Items[e.Index]);

			if(lbHatchStyles.SelectedIndex == e.Index)
			{
				e.DrawFocusRectangle();
			}
		}

		/// <summary>
		/// Standard SelectedIndexChanged EventHandler for ListBox lbHatchStyles.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void lbHatchStyles_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			HatchStyle = (HatchStyle)lbHatchStyles.SelectedItem;

			if(bSmall && this.IWFES != null)
			{
				this.IWFES.CloseDropDown();
			}
		}

		#endregion ListBox



		#region Panel pLargeSample

		/// <summary>
		/// Standard Paint EventHandler for Panel pLargeSample.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard PaintEventArgs object.</param>
		private void pLargeSample_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			if(!bSmall)
			{
				Brush brsh;

				if(lbHatchStyles.SelectedIndex > 0)
				{
					brsh = new HatchBrush((HatchStyle)lbHatchStyles.SelectedItem, cHatchStyleForeColor, cHatchStyleBackColor);
				}
				else
				{
					brsh = new SolidBrush(cHatchStyleBackColor);
				}

				e.Graphics.FillRectangle(brsh, pLargeSample.ClientRectangle);
			}
		}

		#endregion Panel pLargeSample



		#region Panels pBackColor and pForeColor

		/// <summary>
		/// Standard Click EventHandler for Panel pBackColor.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void pBackColor_Click(object sender, System.EventArgs e)
		{
			ColorDialog cd = new ColorDialog();
			cd.AllowFullOpen = true;
			cd.AnyColor = true;
			cd.FullOpen = true;
			cd.Color = cHatchStyleBackColor;
			DialogResult dr = cd.ShowDialog();
			if(dr == DialogResult.OK)
			{
				pBackColor.BackColor = cd.Color;
				cHatchStyleBackColor = cd.Color;

				lbHatchStyles.Refresh();
			}
		}

		/// <summary>
		/// Standard Click EventHandler for Panel pForeColor.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void pForeColor_Click(object sender, System.EventArgs e)
		{
			ColorDialog cd = new ColorDialog();
			cd.AllowFullOpen = true;
			cd.AnyColor = true;
			cd.FullOpen = true;
			cd.Color = cHatchStyleBackColor;
			DialogResult dr = cd.ShowDialog();
			if(dr == DialogResult.OK)
			{
				pForeColor.BackColor = cd.Color;
				cHatchStyleForeColor = cd.Color;

				lbHatchStyles.Refresh();
			}
		}

		#endregion Panels pBackColor and pForeColor



		#region Buttons

		#region Button btChangeSize

		/// <summary>
		/// Standard Click EventHandler for Button btChangeSize.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void btChangeSize_Click(object sender, System.EventArgs e)
		{
			bSmall = !bSmall;

			if(bSmall)
			{
				this.Width = 224;

				lblHatchStyleName.Visible = false;
			}
			else
			{
				this.Width = 368;

				lblHatchStyleName.Visible = true;
			}
		}

		/// <summary>
		/// Standard Paint EventHandler for Button btChangeSize.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard PaintEventArgs object.</param>
		private void btChangeSize_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			SolidBrush brshBlack = new SolidBrush(Color.Black);

			//Shape of a tringle or arrow.
			Point[] apt =	{
								new Point(btChangeSize.Width / 4, btChangeSize.Height / 2 - 1),
								new Point(btChangeSize.Width * 3 / 4, btChangeSize.Height * 1 / 4 - 1),
								new Point(btChangeSize.Width * 3 / 4, btChangeSize.Height * 3 / 4 - 1)
							};

			//The arrow is painted pointing different directions depending on the size of the window (if bSmall is true or false).
			//The arrow also reverses direction if this Control is not used as a UITypeEditor.
			if(	bSmall && this.IWFES != null ||
				!bSmall && this.IWFES == null)
			{
				e.Graphics.FillPolygon(brshBlack, apt);
			}
			else
			{
				e.Graphics.RotateTransform(180);
				e.Graphics.TranslateTransform(-16, -14);
				e.Graphics.FillPolygon(brshBlack, apt);
			}
		}

		#endregion Button btChangeSize



		#region Button btAccept

		/// <summary>
		/// Standard Click EventHandler for Button btAccept.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void btAccept_Click(object sender, System.EventArgs e)
		{
			if(this.IWFES != null)
			{
				this.IWFES.CloseDropDown();
			}
		}

		#endregion Button btAccept

		#endregion Buttons

		#endregion EventHandlers



		#region Methods

		#region AddHatchStylesToListBox

		/// <summary>
		/// Add all HatchStyles to ListBox lbHatchStyles except for duplicates.
		/// </summary>
		private void AddHatchStylesToListBox()
		{
			//Since there are duplicates in the HatchStyle enum this could not be a straight Enum.GetValues.

			string[] astr = Enum.GetNames(typeof(HatchStyle));

			int i = 0;
			foreach(object o in Enum.GetValues(typeof(HatchStyle)))
			{
				if(o.ToString() == astr[i].ToString())
				{
					lbHatchStyles.Items.Add(o);
				}

				i++;
			}
		}

		#endregion AddHatchStylesToListBox



		#region PaintHatchStyle

		/// <summary>
		/// Paint the HatchStyle supplied using the DrawItemEventArgs supplied.
		/// </summary>
		/// <param name="e">DrawItemEventArgs that holds the graphics and bounds objects used when painting.</param>
		/// <param name="hs">HatchStyle that is to be painted.</param>
		private void PaintHatchStyle(DrawItemEventArgs e, HatchStyle hs)
		{
			Pen penBlack = new Pen(Color.Black, 0);
			HatchBrush brshHatch = null;

			brshHatch = new HatchBrush(hs, cHatchStyleForeColor, cHatchStyleBackColor);
			e.Graphics.FillRectangle(brshHatch, iItemPadX, e.Bounds.Top + iItemPadY, iHatchStyleBoxWidth, iHatchStyleBoxHeight);

			e.Graphics.DrawRectangle(penBlack, iItemPadX, e.Bounds.Top + iItemPadY, iHatchStyleBoxWidth - 1, iHatchStyleBoxHeight - 1);
		}

		#endregion PaintHatchStyle

		#endregion Methods



		#region Help Methods

		/// <summary>
		/// Takes a string and returns a more readable form.
		/// </summary>
		/// <param name="str">String to modify.</param>
		/// <returns>String in a more readable form.</returns>
		private string GetSpaceSeparatedString(string str)
		{
			string strRet = string.Empty;

			for(int i = 0; i < str.Length; i++)
			{
				if(	!IsDigit(str[i]) && str[i].ToString().ToUpper() == str[i].ToString() ||
					i > 0 && !IsDigit(str[i - 1]) && IsDigit(str[i]))
				{
					strRet += " ";
				}
				strRet += str[i].ToString();
			}

			return strRet.Trim();
		}

		/// <summary>
		/// Check if the supplied char is a digit or not.
		/// </summary>
		/// <param name="ch">Char to check if it is a digit or not.</param>
		/// <returns>True if the supplied char is a digit or false otherwise.</returns>
		private bool IsDigit(char ch)
		{
			int iCh = (int)ch;

			if(iCh >= 48 && iCh <= 57)
			{
				return true;
			}

			return false;
		}

		#endregion Help Methods



		/// <summary>
		/// Standard Click EventHandler for Button btCancel.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void btCancel_Click(object sender, System.EventArgs e)
		{
			//Figure out how to close the dropdown and revert back to old value.
		}

	}
}
