using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace Fireball.Windows.Forms.Dialogs
{
	public class WizardPanel : UserControl
	{
        public virtual event EventHandler PanelStateChange;

		public WizardPanel()
		{
			this.Dock = DockStyle.Fill;
		}

		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);
			
		}

		public override DockStyle Dock
		{
			get
			{
				return base.Dock;
			}
			set
			{
				//base.Dock = DockStyle.Fill;
			}
		}

		private void InitializeComponent()
		{
			this.SuspendLayout();
			// 
			// WizardPanel
			// 
			this.Name = "WizardPanel";
			this.Size = new System.Drawing.Size(187, 158);
			this.ResumeLayout(false);

		}

		internal void SetDock()
		{
			base.Dock = DockStyle.Fill;
		}

        public virtual bool CanGoNext
        {
            get
            {
                return true;
            }
        }

        public virtual bool CanGoBack
        {
            get
            {
                return true;
            }
        }
	}
}
