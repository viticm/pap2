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
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Collections;

namespace Fireball.Windows.Forms
{

    public delegate void CheckChangedHandler(int index,string text,bool isChecked);

    public class SimpleCheckRadioList :  Widget
    {
        private ScrollableControl _Scroller = null;
        private FlowLayoutPanel _FlowPanel = null;
        private bool _MultiSelect = false;
        public event CheckChangedHandler CheckChanged;

        public bool MultiSelect
        {
            get { return _MultiSelect; }
            set
            {
                if (_FlowPanel.Controls.Count > 0)
                    throw new Exception("Can' t change selection type when elements exits");

                _MultiSelect = value;
            }
        }

        void stateChanged(object sender, EventArgs e)
        {
            bool check = false;

            if (sender is CheckBox)
                check = ((CheckBox)sender).Checked;
            else if(sender is RadioButton)
                check = ((RadioButton)sender).Checked;

            Control ctl = (Control)sender;

            if (CheckChanged != null)
                CheckChanged(_FlowPanel.Controls.IndexOf(ctl),
                    ctl.Text, check);
        }

        public void AddItem(string text)
        {
            AddItem(text, false);
        }

        public string[] CheckedItems
        {
            get
            {
                ArrayList list = new ArrayList();

                foreach (Control current in _FlowPanel.Controls)
                {
                    if (current is RadioButton)
                    {
                        if (((RadioButton)current).Checked)
                        {
                            list.Add(current.Text);
                        }
                    }
                    else if (current is CheckBox)
                    {
                        if (((CheckBox)current).Checked)
                        {
                            list.Add(current.Text);
                        }
                    }
                }

                string[] strs = new string[list.Count];

                list.CopyTo(strs);

                return strs;
            }
        }

        public string[] Items
        {
            get
            {
                string[] strs = new string[_FlowPanel.Controls.Count];

                for (int i = 0; i < _FlowPanel.Controls.Count; i++)
                {
                    strs[i] = _FlowPanel.Controls[i].Text;
                }

                return strs;
            }
        }

        public void AddItem(string text, bool selected)
        {
            if (_MultiSelect)
            {
                CheckBox check = new CheckBox();
                check.Text = text;
                check.CheckedChanged += new EventHandler(stateChanged);
                check.AutoSize = true;

                check.Checked = selected;

                _FlowPanel.Controls.Add(check);
            }
            else
            {
                RadioButton radio = new RadioButton();
                radio.Text = text;
                radio.AutoSize = true;
                radio.Checked = selected;
                radio.CheckedChanged += new EventHandler(stateChanged);

                _FlowPanel.Controls.Add(radio);
            }
        }

        public void Remove(string text)
        {
            for (int i = 0; i < _FlowPanel.Controls.Count; i++)
            {
                if (_FlowPanel.Controls[i].Text == text)
                {
                    _FlowPanel.Controls.RemoveAt(i);
                    break;
                }
            }
        }

        public void ClearItems()
        {
            _FlowPanel.Controls.Clear();
        }

        public SimpleCheckRadioList()
        {
            _Scroller = new ScrollableControl();

            _Scroller.AutoScroll = true;

            this.Controls.Add(_Scroller);

            _Scroller.Dock = DockStyle.Fill;

            _FlowPanel = new FlowLayoutPanel();

            _FlowPanel.AutoSize = true;

            _FlowPanel.FlowDirection = FlowDirection.TopDown;

            _Scroller.Controls.Add(_FlowPanel);

            this.BackColor = Color.White;
        }


        public override System.Drawing.Color BackColor
        {
            get
            {
                return base.BackColor;
            }
            set
            {
                base.BackColor = value;
                _Scroller.BackColor = value;

                _FlowPanel.BackColor = value;
            }
        }

    }
}
