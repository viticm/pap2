using System;
using System.Collections.Generic;
using System.Text;
using Fireball.Collections.Generic;
using Fireball.Plugins;
using FireEdit;

namespace Fireball.Plugin
{
    public class ColorDialogPlugin : FireEditPlugin
    {
        private static FormColor _formColor;
        private static ColorDialogPluginInfo _pluginInfo;

        public override bool Execute(Fireball.Plugins.PluginApplication application, params object[] parameters)
        {
            return true;
        }

        public override void OnMenuClick()
        {
            ShowForm(((FireEditApplication)FireEditApplication.Istance).MainForm);
        }

        private void ShowForm(System.Windows.Forms.IWin32Window owner)
        {
            if (_formColor == null || _formColor.Disposing) _formColor = new FormColor();
            //_formColor.Visible = false;
            _formColor.Show(owner);
        }

        public override bool Execute(Fireball.Plugins.PluginApplication application)
        {
            return this.Execute(application, new object[0]);
        }

        public override PluginInfo About
        {
            get
            {
                if (_pluginInfo == null) _pluginInfo = new ColorDialogPluginInfo();
                return _pluginInfo;
            }
        }
        public override string Name
        {
            get 
            {
                return "ColorDialog";
            }
        }
        public override bool ShowMenuItem
        {
            get 
            {
                return true; 
            }
        }

        public class ColorDialogPluginInfo : PluginInfo
        {
            public ColorDialogPluginInfo()
            {
                this.SetAuthor("Riccardo Marzi, dotnetfireball.org");
                this.SetCompany("DotNetFireball");
                this.SetEmail("riccardo@dotnetfireball.org");
                this.SetVersion(new Version(0, 1, 1));
                this.SetWebSite("http://www.dotnetfireball.org");
                
            }
            protected override void ShowAboutBox()
            {
                base.ShowAboutBox();
            }
        }
    }
}
