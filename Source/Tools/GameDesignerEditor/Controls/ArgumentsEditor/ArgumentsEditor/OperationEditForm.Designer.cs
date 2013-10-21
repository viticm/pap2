namespace ArgumentsEditor
{
    partial class OperationEditForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
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
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.bFunction = new DevComponents.DotNetBar.ButtonItem();
            this.bEditFunction = new DevComponents.DotNetBar.ButtonItem();
            this.bNotFunction = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteFunction = new DevComponents.DotNetBar.ButtonItem();
            this.bExpression = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteExpression = new DevComponents.DotNetBar.ButtonItem();
            this.bEditAnd = new DevComponents.DotNetBar.ButtonItem();
            this.bEditOr = new DevComponents.DotNetBar.ButtonItem();
            this.bAll = new DevComponents.DotNetBar.ButtonItem();
            this.bAddAnd = new DevComponents.DotNetBar.ButtonItem();
            this.bAddOr = new DevComponents.DotNetBar.ButtonItem();
            this.bAddFunction = new DevComponents.DotNetBar.ButtonItem();
            this.bExpressionEnd = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteEndExpression = new DevComponents.DotNetBar.ButtonItem();
            this.bEditAndEnd = new DevComponents.DotNetBar.ButtonItem();
            this.bEditOrEnd = new DevComponents.DotNetBar.ButtonItem();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.bAddFunctionAtEnd = new DevComponents.DotNetBar.ButtonItem();
            this.panelEx1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.contextMenuBar1);
            this.panelEx1.Controls.Add(this.buttonX2);
            this.panelEx1.Controls.Add(this.buttonX1);
            this.panelEx1.Controls.Add(this.richTextBox1);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(532, 135);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bFunction,
            this.bExpression,
            this.bAll,
            this.bExpressionEnd});
            this.contextMenuBar1.Location = new System.Drawing.Point(12, 12);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(95, 91);
            this.contextMenuBar1.Stretch = true;
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.contextMenuBar1.TabIndex = 3;
            this.contextMenuBar1.TabStop = false;
            this.contextMenuBar1.Text = "contextMenuBar1";
            // 
            // bFunction
            // 
            this.bFunction.AutoExpandOnClick = true;
            this.bFunction.ImagePaddingHorizontal = 8;
            this.bFunction.Name = "bFunction";
            this.bFunction.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bEditFunction,
            this.bNotFunction,
            this.bDeleteFunction});
            this.bFunction.Text = "bFunction";
            // 
            // bEditFunction
            // 
            this.bEditFunction.ImagePaddingHorizontal = 8;
            this.bEditFunction.Name = "bEditFunction";
            this.bEditFunction.Text = "±à¼­";
            this.bEditFunction.Click += new System.EventHandler(this.EditFunction);
            // 
            // bNotFunction
            // 
            this.bNotFunction.ImagePaddingHorizontal = 8;
            this.bNotFunction.Name = "bNotFunction";
            this.bNotFunction.Text = "È¡·´";
            this.bNotFunction.Click += new System.EventHandler(this.bNotFunction_Click);
            // 
            // bDeleteFunction
            // 
            this.bDeleteFunction.ImagePaddingHorizontal = 8;
            this.bDeleteFunction.Name = "bDeleteFunction";
            this.bDeleteFunction.Text = "É¾³ý";
            this.bDeleteFunction.Click += new System.EventHandler(this.bDeleteFunction_Click);
            // 
            // bExpression
            // 
            this.bExpression.AutoExpandOnClick = true;
            this.bExpression.ImagePaddingHorizontal = 8;
            this.bExpression.Name = "bExpression";
            this.bExpression.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bDeleteExpression,
            this.bEditAnd,
            this.bEditOr});
            this.bExpression.Text = "bExpression";
            // 
            // bDeleteExpression
            // 
            this.bDeleteExpression.ImagePaddingHorizontal = 8;
            this.bDeleteExpression.Name = "bDeleteExpression";
            this.bDeleteExpression.Text = "É¾³ý";
            this.bDeleteExpression.Click += new System.EventHandler(this.bDeleteExpression_Click);
            // 
            // bEditAnd
            // 
            this.bEditAnd.ImagePaddingHorizontal = 8;
            this.bEditAnd.Name = "bEditAnd";
            this.bEditAnd.Text = "±à¼­ &&";
            this.bEditAnd.Click += new System.EventHandler(this.bAddAnd_Click);
            // 
            // bEditOr
            // 
            this.bEditOr.ImagePaddingHorizontal = 8;
            this.bEditOr.Name = "bEditOr";
            this.bEditOr.Text = "±à¼­ |";
            this.bEditOr.Click += new System.EventHandler(this.bAddOr_Click);
            // 
            // bAll
            // 
            this.bAll.AutoExpandOnClick = true;
            this.bAll.ImagePaddingHorizontal = 8;
            this.bAll.Name = "bAll";
            this.bAll.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bAddAnd,
            this.bAddOr,
            this.bAddFunction});
            this.bAll.Text = "bAll";
            // 
            // bAddAnd
            // 
            this.bAddAnd.ImagePaddingHorizontal = 8;
            this.bAddAnd.Name = "bAddAnd";
            this.bAddAnd.Text = "Ìí¼Ó &&";
            this.bAddAnd.Click += new System.EventHandler(this.bAddAnd_Click);
            // 
            // bAddOr
            // 
            this.bAddOr.ImagePaddingHorizontal = 8;
            this.bAddOr.Name = "bAddOr";
            this.bAddOr.Text = "Ìí¼Ó |";
            this.bAddOr.Click += new System.EventHandler(this.bAddOr_Click);
            // 
            // bAddFunction
            // 
            this.bAddFunction.BeginGroup = true;
            this.bAddFunction.ImagePaddingHorizontal = 8;
            this.bAddFunction.Name = "bAddFunction";
            this.bAddFunction.Text = "Ìí¼Óº¯Êý";
            this.bAddFunction.Click += new System.EventHandler(this.EditFunction);
            // 
            // bExpressionEnd
            // 
            this.bExpressionEnd.AutoExpandOnClick = true;
            this.bExpressionEnd.ImagePaddingHorizontal = 8;
            this.bExpressionEnd.Name = "bExpressionEnd";
            this.bExpressionEnd.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bDeleteEndExpression,
            this.bEditAndEnd,
            this.bEditOrEnd,
            this.bAddFunctionAtEnd});
            this.bExpressionEnd.Text = "bExpressionEnd";
            // 
            // bDeleteEndExpression
            // 
            this.bDeleteEndExpression.ImagePaddingHorizontal = 8;
            this.bDeleteEndExpression.Name = "bDeleteEndExpression";
            this.bDeleteEndExpression.Text = "É¾³ý";
            this.bDeleteEndExpression.Click += new System.EventHandler(this.bDeleteExpression_Click);
            // 
            // bEditAndEnd
            // 
            this.bEditAndEnd.ImagePaddingHorizontal = 8;
            this.bEditAndEnd.Name = "bEditAndEnd";
            this.bEditAndEnd.Text = "±à¼­ &&";
            this.bEditAndEnd.Click += new System.EventHandler(this.bAddAnd_Click);
            // 
            // bEditOrEnd
            // 
            this.bEditOrEnd.ImagePaddingHorizontal = 8;
            this.bEditOrEnd.Name = "bEditOrEnd";
            this.bEditOrEnd.Text = "±à¼­ |";
            this.bEditOrEnd.Click += new System.EventHandler(this.bAddOr_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(450, 103);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 2;
            this.buttonX2.Text = "È¡Ïû";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(362, 103);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 1;
            this.buttonX1.Text = "È·¶¨";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // richTextBox1
            // 
            this.richTextBox1.Location = new System.Drawing.Point(12, 12);
            this.richTextBox1.Multiline = false;
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(508, 74);
            this.richTextBox1.TabIndex = 0;
            this.richTextBox1.Text = "";
            this.richTextBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.richTextBox1_MouseDown);
            this.richTextBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.richTextBox1_KeyPress);
            // 
            // bAddFunctionAtEnd
            // 
            this.bAddFunctionAtEnd.BeginGroup = true;
            this.bAddFunctionAtEnd.ImagePaddingHorizontal = 8;
            this.bAddFunctionAtEnd.Name = "bAddFunctionAtEnd";
            this.bAddFunctionAtEnd.Text = "Ä©Î²Ìí¼Óº¯Êý";
            this.bAddFunctionAtEnd.Click += new System.EventHandler(this.AddFunctionAtEnd);
            // 
            // OperationEditForm
            // 
            this.AcceptButton = this.buttonX1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonX2;
            this.ClientSize = new System.Drawing.Size(532, 135);
            this.Controls.Add(this.panelEx1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "OperationEditForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "²Ù×÷·û±à¼­";
            this.panelEx1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem bFunction;
        private DevComponents.DotNetBar.ButtonItem bDeleteFunction;
        private DevComponents.DotNetBar.ButtonItem bExpression;
        private DevComponents.DotNetBar.ButtonItem bDeleteExpression;
        private DevComponents.DotNetBar.ButtonItem bEditAnd;
        private DevComponents.DotNetBar.ButtonItem bEditOr;
        private DevComponents.DotNetBar.ButtonItem bAll;
        private DevComponents.DotNetBar.ButtonItem bAddAnd;
        private DevComponents.DotNetBar.ButtonItem bAddOr;
        private DevComponents.DotNetBar.ButtonItem bExpressionEnd;
        private DevComponents.DotNetBar.ButtonItem bDeleteEndExpression;
        private DevComponents.DotNetBar.ButtonItem bEditAndEnd;
        private DevComponents.DotNetBar.ButtonItem bEditOrEnd;
        private DevComponents.DotNetBar.ButtonItem bNotFunction;
        private DevComponents.DotNetBar.ButtonItem bEditFunction;
        private DevComponents.DotNetBar.ButtonItem bAddFunction;
        private DevComponents.DotNetBar.ButtonItem bAddFunctionAtEnd;
    }
}