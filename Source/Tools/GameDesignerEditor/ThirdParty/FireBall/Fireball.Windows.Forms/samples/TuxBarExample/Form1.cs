//    Copyright (C) 2005  Sebastian Faltoni
//	  Copyright (C) 2005  Riccardo Marzi
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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Fireball.Windows.Forms.TuxBar;

namespace TuxBarExample
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        int index = 0;

        private void button1_Click(object sender, EventArgs e)
        {
            TuxBarItem item = null;

            if ((index % 2) != 0)
            {
                item = new MyTuxBarItem1();
            }
            else
            {
                item = new MyTuxBarItem2();
            }


            item.Text = "Item Number " + index.ToString();
            tuxBarContainer1.Items.Add(item);

            index++;
        }

        public class MyTuxBarItem1 : TuxBarItem
        {
            private RichTextBox m_RichTextBox = null;

            public MyTuxBarItem1()
            {
                m_RichTextBox = new RichTextBox();

                this.ChildControls.Add(m_RichTextBox);

                m_RichTextBox.Dock = DockStyle.Fill;

                m_RichTextBox.BorderStyle = BorderStyle.None;

                this.Icon = Properties.Resources.garbage;

                m_RichTextBox.DetectUrls = true;

                m_RichTextBox.Text = @"This a very simple sample :P
 http://www.dotnetfireball.org";
            }
        }

        public class MyTuxBarItem2 : TuxBarItem
        {
            private WebBrowser m_WebBrowser = null;

            public MyTuxBarItem2()
            {
                m_WebBrowser = new WebBrowser();

                this.ChildControls.Add(m_WebBrowser);

                m_WebBrowser.Dock = DockStyle.Fill;

                m_WebBrowser.Navigate("http://www.dotnetfireball.org");

                this.Icon = Properties.Resources.lightbulb;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (tuxBarContainer1.Items.Count > 0)
                tuxBarContainer1.Items.RemoveAt(tuxBarContainer1.Items.Count - 1);
        }
    }
}