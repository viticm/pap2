using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;

namespace Fireball.Windows.Forms.Dialogs
{
	public partial class WizardForm : Form
	{
		private WizardPanelCollection m_Panels = null;

		public WizardForm()
		{
			InitializeComponent();

			m_Panels = new WizardPanelCollection();

			m_Panels.AddPanel += new WizardPanelCollection.AddPanelHandler(m_Panels_AddPanel);

			m_Panels.AddPanelRange += new WizardPanelCollection.AddPanelRangeHandler(m_Panels_AddPanelRange);

			m_Panels.InsertPanel += new WizardPanelCollection.InsertPanelHandler(m_Panels_InsertPanel);

			m_Panels.RemovePanel += new WizardPanelCollection.RemovePanelHandler(m_Panels_RemovePanel);
		}

		void m_Panels_RemovePanel(WizardPanel panel)
		{
			this.panelSteps.Controls.Remove(panel);
			panelSteps.Invalidate();
		}

		void m_Panels_InsertPanel(WizardPanel panel, int index)
		{
			panel.SetDock();
			this.panelSteps.Controls.Add(panel);
			panelSteps.Invalidate();
		}

		void m_Panels_AddPanelRange(WizardPanel[] panel)
		{
			for (int i = 0; i < panel.Length; i++)
				panel[i].SetDock();

			this.panelSteps.Controls.AddRange(panel);
			panelSteps.Invalidate();
		}

		void m_Panels_AddPanel(WizardPanel panel)
		{
			panel.SetDock();
			this.panelSteps.Controls.Add(panel);
			panelSteps.Invalidate();
		}

		private void btnAnnulla_Click(object sender, EventArgs e)
		{
			OnCancelButton(btnAnnulla, e);
		}

		private void btnBack_Click(object sender, EventArgs e)
		{
			OnBackButton(btnBack, e);
		}

		private void btnNext_Click(object sender, EventArgs e)
		{
			OnNextButton(btnNext, e);
		}

		protected virtual void OnCancelButton(Button sender, EventArgs e)
		{

		}

		protected virtual void OnBackButton(Button sender, EventArgs e)
		{

		}

		protected virtual void OnNextButton(Button sender, EventArgs e)
		{

		}

		public WizardPanelCollection Panels
		{
			get
			{
				return m_Panels;
			}
		}

		private void panel1_Paint(object sender, PaintEventArgs e)
		{
            Rectangle rect = new Rectangle(0, 0, panel1.Width, panel1.Height);
            LinearGradientBrush lgb = new LinearGradientBrush(rect, SystemColors.ControlLightLight, SystemColors.Control, LinearGradientMode.Vertical);
            e.Graphics.FillRectangle(lgb, rect);

			e.Graphics.DrawLine(SystemPens.ControlDark, 0, panel1.Height - 1, 
				panel1.Width, panel1.Height - 1);
		}

		private void leftPanel_Paint(object sender, PaintEventArgs e)
		{
			e.Graphics.DrawLine(Pens.Black, 0, leftPanel.Height - 1,
							leftPanel.Width, leftPanel.Height - 1);

			e.Graphics.DrawLine(Pens.Black, leftPanel.Width-1, 0, 
				leftPanel.Width-1, leftPanel.Height);
		}

		private void panelSteps_Paint(object sender, PaintEventArgs e)
		{
			e.Graphics.DrawLine(SystemPens.ControlDark, 0, panelSteps.Height - 1, panelSteps.Width - 1,
					panelSteps.Height - 1);
		}

		private void panelSteps_Resize(object sender, EventArgs e)
		{
			panelSteps.Invalidate();
		}

        private void panel3_Paint(object sender, PaintEventArgs e)
        {
            Rectangle rect = new Rectangle(0, 0, panel3.Width, panel3.Height);
            LinearGradientBrush lgb = new LinearGradientBrush(rect, SystemColors.ControlLightLight, SystemColors.Control, LinearGradientMode.Vertical);
            e.Graphics.FillRectangle(lgb, rect);
        }
	}
}