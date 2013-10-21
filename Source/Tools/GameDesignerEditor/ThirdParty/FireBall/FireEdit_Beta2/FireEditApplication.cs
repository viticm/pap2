#region License

//    Copyright (C) 2004  Sebastian Faltoni sebastian(at)dotnetfireball(dot)net
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#endregion License

using System;
using System.Collections.Generic;
using System.Text;
using Fireball.Plugins;
using System.Windows.Forms;
using System.IO;

namespace FireEdit
{
    public class FireEditApplication : PluginApplication
    {
        private FormMain _FormMain = null;

        //private static FireEditApplication _FireEditApplication = null;

        private FireEditPluginManager _FireEditPluginManager = null;

        private string PluginFile = null;

        static FireEditApplication()
        {
            FireEditApplication.istance = new FireEditApplication();
        }

        private FireEditApplication()
        {
            _FireEditPluginManager = new FireEditPluginManager();

            _FireEditPluginManager.AssemblySearchPath =
                Path.Combine(Application.StartupPath, "plugins");

            if (!Directory.Exists(_FireEditPluginManager.AssemblySearchPath))
                Directory.CreateDirectory(
                    _FireEditPluginManager.AssemblySearchPath);

            PluginFile = Path.Combine(Application.StartupPath, "plugin.conf");

            if (File.Exists(PluginFile) == false)
            {
                File.WriteAllText(PluginFile, "<plugins />");
            }            
        }

        public override void Run()
        {
            Application.EnableVisualStyles();
            Application.Run();
        }

        public void Run(FormMain mainForm)
        {
            _FormMain = mainForm;
            Application.EnableVisualStyles();
            _FireEditPluginManager.LoadFromXml(PluginFile);

            foreach (FireEditPlugin plugin in _FireEditPluginManager.PluginsLoaded)
            {
                plugin.Execute(this);
            }

            Application.Run(mainForm);
        }


        /// <summary>
        /// The istance of the FormMain Class
        /// </summary>
        public FormMain MainForm
        {
            get
            {
                return _FormMain;
            }
        }

        ///// <summary>
        ///// Static Istance of FireEdit Application
        ///// </summary>
        //public static FireEditApplication Istance
        //{
        //    get
        //    {
        //        if (_FireEditApplication == null)
        //            _FireEditApplication = new FireEditApplication();

        //        return _FireEditApplication;
        //    }
        //}

        /// <summary>
        /// Static Shortcut to FireEditApplication Run method
        /// </summary>
        /// <param name="mainForm"></param>
        public static void RunMain(FormMain mainForm)
        {
            ((FireEditApplication)FireEditApplication.Istance).Run(mainForm);
        }

        public override IPluginManager Manager
        {
            get
            {
                return (IPluginManager)this._FireEditPluginManager;
            }
        }
    }
}