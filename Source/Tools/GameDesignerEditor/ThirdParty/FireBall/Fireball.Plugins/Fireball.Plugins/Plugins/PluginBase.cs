//    Copyright (C) 2005  Sebastian Faltoni
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

using System;
using System.Windows.Forms;

namespace Fireball.Plugins
{
    /// <summary>
    /// Abstract class for starting implementing your own plugin class
    /// </summary>
    public abstract class PluginBase : IPluginApplication
    {

        /// <summary>
        /// Use this method for the initialization of plugins on your software
        /// </summary>
        /// <param name="application">The Main application class</param>
        /// <returns></returns>
        public abstract bool Execute(PluginApplication application);

        /// <summary>
        /// Use this method for the initialization of plugins on your software
        /// </summary>
        /// <param name="application">The Main application class</param>
        /// <param name="parameters">if your application</param>
        /// <returns></returns>
        public abstract bool Execute(PluginApplication application, params object[] parameters);

        /// <summary>
        /// 
        /// </summary>
        public abstract PluginInfo About { get;}

        /// <summary>
        /// 
        /// </summary>
        public abstract bool ShowMenuItem { get;}

        /// <summary>
        /// 
        /// </summary>
        public abstract string Name { get;}

        /// <summary>
        /// 
        /// </summary>
        protected ToolStripMenuItem menuItem;

        /// <summary>
        /// 
        /// </summary>
        public virtual ToolStripMenuItem MenuItem
        {
            get
            {
                if (menuItem == null)
                {
                    menuItem = new ToolStripMenuItem(this.Name);

                    menuItem.Click += new EventHandler(menuItem_Click);
                }

                return menuItem;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void menuItem_Click(object sender, EventArgs e)
        {
            this.OnMenuClick();
        }

        /// <summary>
        /// 
        /// </summary>
        public virtual void OnMenuClick()
        {

        }

    }
}
