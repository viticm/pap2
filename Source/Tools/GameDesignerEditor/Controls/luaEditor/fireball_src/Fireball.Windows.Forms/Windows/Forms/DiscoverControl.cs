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
using System.Drawing;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Text;
using System.ComponentModel;

namespace Fireball.Windows.Forms
{
    [ToolboxBitmap(typeof(DiscoverControl), "DiscoverControl.bmp"), Designer(typeof(DiscoverControlDesigner)), DefaultEvent("DiscoverPaneChanged")]
    public class DiscoverControl : Panel
    {

        public event CancelEventHandler DiscoverPaneChanging;
        public event EventHandler DiscoverPaneChanged;
        public event EventHandler DiscoverShowOptions;


        public DiscoverPane SelectedPane
        {
            get { return _SelectedPane; }
            set
            {
                if (value != null &&  this.Controls.IndexOf(value) == -1)
                    throw new Exception("You need to add to Controls collection before set it as current pane");

                if (_SelectedPane != null)
                    _SelectedPane.Visible = false;
                _SelectedPane = value;
                if (_SelectedPane != null)
                    _SelectedPane.Visible = true;
            }
        }

        public DiscoverPane GetPaneAt(int x, int y)
        {
            return GetListedPaneAt(new Point(x, y));
        }
 

        protected override void OnPaint(PaintEventArgs e)
        {
            bool rtlEnabled = this.RightToLeft == RightToLeft.Yes;

            _CurrentRender.DrawBackground(e.Graphics, this.ClientRectangle, this.BackColor);

            if (this.SelectedPane != null)
            {
                _CurrentRender.DrawHeader(e.Graphics, this.HeaderRect,
                    this.SelectedPane.Text, _HeaderFont, null, rtlEnabled);
            }

            if (this.SwitchBarRect != Rectangle.Empty)
            {
                this._CurrentRender.DrawDiscoverContentPaneBackground(e.Graphics,
                    this.SwitchBarRect, DrawItemState.None);
            }

            foreach (Control currentControl in base.Controls)
            {
                DiscoverPane currentPane = (DiscoverPane)currentControl;
                if ((currentPane.Listed && (currentPane.ButtonRect != Rectangle.Empty))
                    && (currentPane.ButtonRect.X >= 0))
                {
                    DrawItemState paneState = DrawItemState.None;
                    if (this._SelectedPane == currentPane)
                    {
                        paneState |= DrawItemState.HotLight;
                    }
                    else if (_MouseOverPane == currentPane)
                    {
                        paneState |= DrawItemState.HotLight;
                    }
                    if (this._boo && (this._SelectedPane == currentPane))
                    {
                        paneState |= DrawItemState.Selected;
                    }
                    if (this.SelectedPane == currentPane)
                    {
                        paneState |= DrawItemState.Checked;
                    }
                    if (!currentPane.Enabled)
                    {
                        paneState |= DrawItemState.Disabled;
                    }
                    if (currentPane.ShowOnList)//BOOOO
                    {
                        this._CurrentRender.DrawDiscoverContentPaneBackground(e.Graphics, currentPane.ListButtonRect, paneState);
                        this._CurrentRender.DrawDiscoverContentPane(e.Graphics, currentPane.ListButtonRect, paneState,
                            currentPane, this.FooterFont, rtlEnabled);
                        continue;
                    }

                    if (CanShowButton(currentPane))
                    {
                        this._CurrentRender.DrawDiscoverFooterPaneBackground(e.Graphics, currentPane.ButtonRect, paneState);
                        this._CurrentRender.DrawDiscoverFooterPane(e.Graphics, currentPane.ButtonRect, paneState,
                            currentPane, this.FooterFont);
                    }
                }
            }

            if (_DrawGripper)
            {
                this._CurrentRender.DrawGripper(e.Graphics, this.GripperRect);
            }

            if (_DrawChewron)
            {
                this._CurrentRender.DrawChevron(e.Graphics, this.ChewronRect, this.ForeColor);
            }
        }

        private bool CanShowButton(DiscoverPane pane)
        {
            return this.SwitchBarRect.IntersectsWith(pane.ButtonRect);
        }

        private Rectangle HeaderRect
        {
            get
            {
                Rectangle rect;

                int h = this.HeaderFont.Height + 4;

                rect = new Rectangle(0, 0, this.ClientSize.Width - 1, h);

                return rect;
            }
        }

        public IDiscoverRenderer CurrentRender
        {
            get { return _CurrentRender; }
            set { _CurrentRender = value; }
        }

        public Font HeaderFont
        {
            get { return _HeaderFont; }
            set { _HeaderFont = value; }
        }

        internal int ListButtonHeight
        {
            get
            {
                return 26;
            }
        }

        internal Rectangle ChewronRect
        {
            get
            {
                if (!_DrawChewron)
                    return Rectangle.Empty;

                Rectangle rect = new Rectangle(this.ClientSize.Width - 23,
                    this.SwitchBarRect.Top, 22, this.SwitchBarRect.Height);

                return rect;
            }
        }

        private Rectangle GripperRect
        {
            get
            {
                int y = this.ClientSize.Height - 30 - 8;

                y -= this.ListButtonHeight * _ShowPanes;

                _GripperRect = new Rectangle(1, y,
                      this.ClientSize.Width - 2, 8);

                return _GripperRect;
            }
        }

        internal Rectangle SwitchBarRect
        {
            get
            {
                return new Rectangle(1, this.ClientSize.Height - 30,
                    this.ClientSize.Width - 2, 29);
            }
        }

        public Font FooterFont
        {
            get
            {
                return this.Font;
            }
        }

        private DiscoverPane GetListedPaneAt(Point loc)
        {
            if (this.ShowPanes == 0)
                return null;

            foreach (DiscoverPane pane in base.Controls)
            {
                if (pane.ShowOnList && pane.ListButtonRect.Contains(loc))
                {
                    return pane;
                }
            }
            return null;
        }

        private DiscoverPane GetButtonPaneAt(Point loc)
        {
            foreach (DiscoverPane pane in base.Controls)
            {
                if (!pane.ShowOnList && pane.ButtonRect.Contains(loc))
                {
                    return pane;
                }
            }
            return null;
        }

        private int _ShowPanes = 0;

        public int ShowPanes
        {
            get
            {
                return _ShowPanes;
            }
            set
            {
                _ShowPanes = value;

                if (_ShowPanes < 0)
                    _ShowPanes = 0;

                if (_ShowPanes > this.ListedCount())
                {
                    _ShowPanes = this.ListedCount();
                }

                

                this.PerformLayout();
            }
        }

        protected virtual void OnDiscoverPaneChanging(CancelEventArgs e)
        {
            if (DiscoverPaneChanging != null)
                DiscoverPaneChanging(this, e);
        }

        protected virtual void OnDiscoverPaneChanged(EventArgs e)
        {
            if (DiscoverPaneChanged != null)
                DiscoverPaneChanged(this, e);
        }

        protected override void OnLayout(LayoutEventArgs levent)
        {
            base.OnLayout(levent);

            if (this.IsHandleCreated)
            {
                int h = this.GripperRect.Top;
                h -= this.HeaderRect.Bottom;

                Rectangle rect = new Rectangle(1, this.HeaderRect.Bottom,
                    this.ClientSize.Width - 2, h);

                int i = 0;

                int count = 0;

                foreach (Control current in base.Controls)
                {
                    current.ClientSize = rect.Size;
                    current.Location = rect.Location;

                    DiscoverPane pane = (DiscoverPane)current;

                    if (count < this.ShowPanes && this.ShowPanes > 0)
                    {
                        pane.ShowOnList = true;
                    }
                    else
                    {
                        pane.ShowOnList = false;
                    }

                    if (pane.ShowOnList)
                    {
                        int top = this.ListButtonHeight * (i + 1);

                        top = this.SwitchBarRect.Top - top;

                        Rectangle rc = new Rectangle(1, top,
                            this.ClientSize.Width - 2, this.ListButtonHeight);

                        pane.ListButtonRect = rc;

                        i++;

                    }
                    count++;
                }

                this.Invalidate();
            }
        }

        private Point old_location;

        private bool grip_is_ondrag = false;

        private DiscoverPane _MouseOverPane = null;

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            if (this.GripperRect.Contains(e.Location))
            {
                grip_is_ondrag = true;
                old_location = e.Location;
            }
        }

        private void CalculateGripPosition()
        {

        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

  

            if (grip_is_ondrag)
            {
                if (this.ShowPanes < 0)
                {
                    this.ShowPanes = 0;
                    return;
                }
                int m = this.GripperRect.Bottom - this.GripperRect.Top;

                m = m / 2;

                Rectangle rcUp = new Rectangle(1, this.GripperRect.Top - this.ListButtonHeight,
                    this.ClientSize.Width - 2, m + this.ListButtonHeight);

                Rectangle rcDown = new Rectangle(1, this.GripperRect.Bottom + this.ListButtonHeight,
                     this.ClientSize.Width - 2, m + this.ListButtonHeight);

                if (rcUp.Contains(e.Location)
                    && (this.ShowPanes < ListedCount()))
                {
                    this.ShowPanes++;
                }
                else if (rcDown.Contains(e.Location))
                {
                    if (this.ShowPanes > 0)
                    {
                        this.ShowPanes--;
                    }
                }

                old_location = new Point(e.Location.X, this.GripperRect.Top + m);

                this.PerformLayout();
                return;
            }
            if (this.GripperRect.Contains(e.Location))
            {
                this.Cursor = Cursors.SizeNS;
                return;
            }

            DiscoverPane listedPane = this.GetListedPaneAt(e.Location);

            if (listedPane != null)
            {
                if (listedPane.ListButtonRect.Contains(e.Location))
                {
                    this.Cursor = Cursors.Hand;
                }
                else
                {

                    this.Cursor = Cursors.Default;
                }
            }
            else if (this.ChewronRect.Contains(e.Location))
            {
                this.Cursor = Cursors.Hand;
            }
            else
            {
                this.Cursor = Cursors.Default;

                DiscoverPane buttonpane = this.GetButtonPaneAt(e.Location);

                if (buttonpane != null)
                {
                    this.Cursor = Cursors.Hand;                    
                }

                _MouseOverPane = buttonpane;

                this.Invalidate(this.SwitchBarRect);
            }
        }

        private int ListedCount()
        {
            int count = 0;

            foreach (DiscoverPane pane in this.Controls)
            {
                if (pane.Listed)
                {
                    count++;
                }
            }
            return count;
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            if (grip_is_ondrag == true)
                this.Cursor = Cursors.Default;
            grip_is_ondrag = false;

            

            DiscoverPane listedPane = this.GetListedPaneAt(e.Location);

            if (listedPane != null)
            {
                CancelEventArgs evt = new CancelEventArgs();

                evt.Cancel = false;

                OnDiscoverPaneChanging(evt);

                if (!evt.Cancel)
                {
                    this.SelectedPane = listedPane;

                    OnDiscoverPaneChanged(new EventArgs());
                }

                return;

            }

            DiscoverPane buttonPane = this.GetButtonPaneAt(e.Location);

            if (buttonPane != null && buttonPane.Listed)
            {
                CancelEventArgs evt = new CancelEventArgs();

                evt.Cancel = false;

                OnDiscoverPaneChanging(evt);

                if (!evt.Cancel)
                {
                    this.SelectedPane = buttonPane;

                    OnDiscoverPaneChanged(new EventArgs());
                }
            }

            if (this.ChewronRect.Contains(e.Location))
            {
                _ContextMenu.Show(this, e.Location);
            }

            this.PerformLayout();
        }

        public ToolStripRenderer MenuItemsRenderer
        {
            get { return _ContextMenu.Renderer; }
            set { _ContextMenu.Renderer = value; }
        }

        protected override void OnControlAdded(ControlEventArgs e)
        {
            e.Control.Visible = false;
            base.OnControlAdded(e);           
        }

        protected override void OnControlRemoved(ControlEventArgs e)
        {
            if (e.Control != null)
            {
                DiscoverPane pane = (DiscoverPane)e.Control;

                if (pane.Listed)
                {
                    if (this.ShowPanes > 0)
                        this.ShowPanes--;
                }
            }
            base.OnControlRemoved(e);
            this.PerformLayout();
            this.Invalidate();
        }

        public DiscoverControl()
        {
            this.SetStyle(ControlStyles.AllPaintingInWmPaint
                | ControlStyles.OptimizedDoubleBuffer
                | ControlStyles.ResizeRedraw
                | ControlStyles.UserPaint
                , true);

            _CurrentRender = new DiscoverProfessionalRender();


            _AddRemoveButtonsMenuItem = new DiscoverMenuItem(this);
            _ConfigureButtonsMenuItem = new DiscoverMenuItem(this);
            _FewerButtonsMenuItem = new DiscoverMenuItem(this);
            _MoreButtonsMenuItem = new DiscoverMenuItem(this);
            _PaneOptionsMenuItem = new DiscoverMenuItem(this);


            _FewerButtonsMenuItem.Click += new EventHandler(_FewerButtonsMenuItem_Click);
            _MoreButtonsMenuItem.Click += new EventHandler(_MoreButtonsMenuItem_Click);
            _ConfigureButtonsMenuItem.Click += new EventHandler(_ConfigureButtonsMenuItem_Click);

            _ConfigureButtonsMenuItem.Visible = false;



            _FewerButtonsMenuItem.Image = Properties.Resources.show_fever;
            _MoreButtonsMenuItem.Image = Properties.Resources.show_more;

            _ContextMenu = new ContextMenuStrip();

            _ContextMenu.Items.AddRange(
                new ToolStripItem[] {
                    _MoreButtonsMenuItem,
                    _FewerButtonsMenuItem,
                    _PaneOptionsMenuItem,     
                    _ConfigureButtonsMenuItem,
                    _AddRemoveButtonsMenuItem 
                   }
                );

            _ContextMenu.Opening += new System.ComponentModel.CancelEventHandler(_ContextMenu_Opening);            
        }

        void _ConfigureButtonsMenuItem_Click(object sender, EventArgs e)
        {
            this.OnDiscoverShowOptions(new EventArgs());
        }

        protected virtual void OnDiscoverShowOptions(EventArgs e)
        {
            if (DiscoverShowOptions != null)
                DiscoverShowOptions(this, new EventArgs());
        }

        void _MoreButtonsMenuItem_Click(object sender, EventArgs e)
        {
            this.ShowPanes++;
        }

        void _FewerButtonsMenuItem_Click(object sender, EventArgs e)
        {
            this.ShowPanes--;
        }

        void _ContextMenu_Opening(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _AddRemoveButtonsMenuItem.Text = DiscoverLanguage.AddRemoveButtons;
            _ConfigureButtonsMenuItem.Text = DiscoverLanguage.ConfigureButtons;
            _FewerButtonsMenuItem.Text = DiscoverLanguage.FewerButtons;
            _MoreButtonsMenuItem.Text = DiscoverLanguage.MoreButtons;
            _PaneOptionsMenuItem.Text = DiscoverLanguage.PaneOptions;

            if (this.ShowPanes == 0)
            {
                _FewerButtonsMenuItem.Enabled = false;
            }
            else
            {
                _FewerButtonsMenuItem.Enabled = true;
            }

            if (this.Controls.Count > 0 && this.ShowPanes < this.Controls.Count)
            {
                _MoreButtonsMenuItem.Enabled = true;
            }
            else
            {
                _MoreButtonsMenuItem.Enabled = false;
            }

            _AddRemoveButtonsMenuItem.DropDownItems.Clear();

            foreach (DiscoverPane pane in this.Controls)
            {                
                ToolStripMenuItem menuPane = new ToolStripMenuItem(pane.Text,
                    pane.SmallImage);

                menuPane.Tag = pane;

                menuPane.Checked = pane.Listed;

                menuPane.CheckOnClick = true;

                menuPane.CheckedChanged += new EventHandler(addRemoveButtonCheckChanged);

                _AddRemoveButtonsMenuItem.DropDownItems.Add(menuPane);
            }
        }

        private void addRemoveButtonCheckChanged(object sender, EventArgs e)
        {
            ToolStripMenuItem menuItem = (ToolStripMenuItem)sender;

            DiscoverPane pane = (DiscoverPane)menuItem.Tag;

            if (this.ShowPanes > 0 && pane.ShowOnList)
                this.ShowPanes--;

            pane.Listed = menuItem.Checked;

            this.PerformLayout();
        }

        private IDiscoverRenderer _CurrentRender = null;

        private DiscoverPane _SelectedPane = null;

        private Font _HeaderFont = new Font("Tahoma", 12f, FontStyle.Bold, GraphicsUnit.Point);

        private bool _boo = false;

        private bool _DrawChewron = true;

        private bool _DrawGripper = true;

        private Rectangle _GripperRect;

        private DiscoverMenuItem _AddRemoveButtonsMenuItem = null;

        private DiscoverMenuItem _ConfigureButtonsMenuItem = null;

        private DiscoverMenuItem _FewerButtonsMenuItem = null;

        private DiscoverMenuItem _MoreButtonsMenuItem = null;

        private DiscoverMenuItem _PaneOptionsMenuItem = null;

        private ContextMenuStrip _ContextMenu = null;



        internal class DiscoverMenuItem : ToolStripMenuItem
        {

            private DiscoverControl _DiscoverControl = null;

            public DiscoverMenuItem(DiscoverControl parentDiscover)
                : this(parentDiscover, string.Empty)
            {

            }

            public DiscoverMenuItem(DiscoverControl parentDiscover, string text)
            {
                this._DiscoverControl = parentDiscover;
                this.Text = text;
            }

            public DiscoverControl DiscoverControl
            {
                get { return _DiscoverControl; }
            }
        }

    }
}