//    Copyright (C) 2005  Sebastian Faltoni sebastian(dot)faltoni(at)gmail(dot)com
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
using System.Drawing.Drawing2D;

namespace Fireball.Windows.Forms
{
    public delegate void TabStripItemChangedHandler(TabStripItem tabItem);

    public delegate void TabStripItemClosingHandler(TabStripItemClosingEventArgs e);

    public class TabStrip : ContainerControl
    {

        private Rectangle _StripButtonRect = Rectangle.Empty;
        private TabStripItem _SelectedItem = null;
        private TabStripMenuGlyph _MenuGlyph = null;
        private ContextMenuStrip _ContextDocsList = null;
        private TabStripCloseButton _CloseButton = null;

        public event TabStripItemClosingHandler TabStripItemClosing;

        private int _VisibleCount = 0;

        public event TabStripItemChangedHandler TabStripItemChanged;

        public event EventHandler TabStripItemClosed;

        public TabStripItem SelectedItem
        {
            get { return _SelectedItem; }
            set
            {
                _SelectedItem = value;

                if (_SelectedItem == null)
                {
                    if (this.Controls.Count > 0)
                    {
                        _SelectedItem = (TabStripItem)this.Controls[this.Controls.Count - 1];
                        _SelectedItem.DrawState = DrawItemState.None;
                        this.Invalidate();
                        return;
                    }
                    return;
                }
                int index = this.Controls.GetChildIndex(_SelectedItem);


                this.SelectItem(_SelectedItem);

                if (_SelectedItem.StripRect.Right > _StripButtonRect.Right)
                    this.Controls.SetChildIndex(_SelectedItem, 0);
                

                for (int i = 0; i < this.Controls.Count; i++)
                {
                    TabStripItem item = (TabStripItem)this.Controls[i];
                    if (item == _SelectedItem)
                        continue;
                    this.UnSelectItem(item);
                }

                this.Invalidate();

                this.OnTabStripItemChanged(_SelectedItem);
            }
        }

        protected virtual void OnTabStripItemChanged(TabStripItem tabItem)
        {
            if (TabStripItemChanged != null)
                TabStripItemChanged(tabItem);
        }


        protected override void OnControlAdded(ControlEventArgs e)
        {
            if (!(e.Control is TabStripItem))
                throw new Exception("TabStrip accept only TabStripItem");

            base.OnControlAdded(e);

            if (this.SelectedItem == null)
                this.SelectedItem = (TabStripItem)e.Control;
            else
                e.Control.Visible = false;

            CalculateVisibility((TabStripItem)e.Control);

            this.Invalidate();
        }


        void CalculateVisibility(TabStripItem lastAdded)
        {
            Rectangle selectedButton = Rectangle.Empty;

            StringFormat sf = new StringFormat();

            sf.LineAlignment = StringAlignment.Center;
            sf.Trimming = StringTrimming.EllipsisCharacter;
            sf.FormatFlags |= StringFormatFlags.NoWrap;

            int right = 0;

            _VisibleCount = 0;

            using (Graphics _Gfx = this.CreateGraphics())
            {

                Control[] ctls = new Control[this.Controls.Count+1];

                this.Controls.CopyTo(ctls, 0);

                ctls[this.Controls.Count] = lastAdded;

                for (int i = 0; i < ctls.Length; i++)
                {
                    TabStripItem current = (TabStripItem)ctls[i];

                    Font currentFont = this.Font;

                    if ((current.DrawState &= DrawItemState.Selected) == DrawItemState.Selected)
                    {
                        currentFont = new Font(this.Font, FontStyle.Bold);
                    }

                    Size textSize = _Gfx.MeasureString(current.Text, currentFont, new SizeF(200, 10), sf).ToSize();

                    textSize.Width += 20;

                    Rectangle buttonRect = new Rectangle(right, 3, textSize.Width, 17);

                    current.StripRect = buttonRect;

                    right += textSize.Width;

                    if (right >= _StripButtonRect.Width)
                        break;

                    _VisibleCount = i;
                }
            }
           
        }

        public TabStrip()
        {
            this.SetStyle(ControlStyles.UserPaint, true);
            this.SetStyle(ControlStyles.ResizeRedraw, true);
            this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            this.SetStyle(ControlStyles.Selectable, true);
            _MenuGlyph = new TabStripMenuGlyph();

            _ContextDocsList = new ContextMenuStrip();

            _ContextDocsList.ItemClicked += new ToolStripItemClickedEventHandler(_ContextDocsList_ItemClicked);

            ((ToolStripProfessionalRenderer)_ContextDocsList.Renderer).ColorTable.UseSystemColors = true;

            _CloseButton = new TabStripCloseButton();

            this.Font = new Font(FontFamily.GenericSansSerif, 9);
        }

        public override Rectangle DisplayRectangle
        {
            get
            {
                Rectangle disp = base.DisplayRectangle;

                disp.Inflate(-2, -13);
                disp.Offset(1, 12);

                return disp;
            }
        }

        //private int CalculateVisibilityCount()
        //{
        //    for (int i = 0; i < this.Controls.Count; i++)
        //    {
        //    }
        //}

        protected override void OnPaint(PaintEventArgs e)
        {
            Rectangle borderRc = base.ClientRectangle;
            borderRc.Width--;
            borderRc.Height--;

            //ControlPaint.DrawVisualStyleBorder(e.Graphics, borderRc);
            e.Graphics.DrawRectangle(SystemPens.ControlDark, borderRc);

            e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;

            float right = 10;

            RectangleF selectedButton = Rectangle.Empty;

            StringFormat sf = new StringFormat();

           // sf.LineAlignment = StringAlignment.Center;
            sf.Trimming = StringTrimming.EllipsisCharacter;
            sf.FormatFlags |= StringFormatFlags.NoWrap;


            for (int i = 0; i < this.Controls.Count; i++)
            {
                TabStripItem current = (TabStripItem)this.Controls[i];

                Font currentFont = this.Font;

                if ((current.DrawState &= DrawItemState.Selected) == DrawItemState.Selected)
                {
                    currentFont = new Font(this.Font, FontStyle.Bold);
                }

                SizeF textSize = e.Graphics.MeasureString(current.Text, currentFont, new SizeF(200, 10), sf);

                textSize.Width += 20;

                RectangleF buttonRect = new RectangleF(right, 3, textSize.Width, 17);

                current.StripRect = buttonRect;

                right += textSize.Width;

                if (right > _StripButtonRect.Width)
                    break;

                DrawButton(e.Graphics, buttonRect, current.DrawState, this.Controls.GetChildIndex(current));

                PointF textLoc = new PointF(buttonRect.Left + buttonRect.Height - 2
                    , buttonRect.Top + (buttonRect.Height / 2)- (textSize.Height/2)-2);

                RectangleF textRect = buttonRect;

                textRect.Location = textLoc;

                textRect.X-=2;
                textRect.Y-=2;

                textRect.Width = (float)buttonRect.Width - (textRect.Left - buttonRect.Left);

                textRect.Height = textSize.Height + currentFont.Size/2;

                if ((current.DrawState &= DrawItemState.Selected) == DrawItemState.Selected)
                {

                    e.Graphics.DrawString(current.Text, currentFont, new SolidBrush(this.ForeColor), textRect, sf);
                }
                else
                {
                    e.Graphics.DrawString(current.Text, currentFont, new SolidBrush(this.ForeColor), textRect, sf);
                }

                if (current == _SelectedItem)
                    selectedButton = buttonRect;



                _VisibleCount = i;

                if (right > _StripButtonRect.Width)
                    break;
            }

            if (_SelectedItem != null)
            {
                if (this.Controls.GetChildIndex(_SelectedItem) != 0)
                {
                    e.Graphics.DrawLine(SystemPens.ControlDark, 0, _SelectedItem.StripRect.Bottom - 1,
                        _SelectedItem.StripRect.Left - 10, _SelectedItem.StripRect.Bottom - 1);
                }

                e.Graphics.DrawLine(SystemPens.ControlDark, _SelectedItem.StripRect.Right, _SelectedItem.StripRect.Bottom - 1,
                    this.ClientSize.Width, _SelectedItem.StripRect.Bottom - 1);

                e.Graphics.DrawLine(Pens.White, _SelectedItem.StripRect.Left - 9,
                    _SelectedItem.StripRect.Bottom - 1, _SelectedItem.StripRect.Right,
                    _SelectedItem.StripRect.Bottom - 1);
            }

            if (_SelectedItem == null) return;

            _CloseButton.Activated = _SelectedItem.CanClose;

            _MenuGlyph.DrawGlyph(e.Graphics);

            if (_SelectedItem.CanClose)
                _CloseButton.DrawCross(e.Graphics);
        }


        public void AddTab(TabStripItem tabItem)
        {
            this.Controls.Add(tabItem);
        }

        public void RemoveTab(TabStripItem tabItem)
        {
            this.Controls.Remove(tabItem);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            if (e.Button != MouseButtons.Left)
                return;



            if (_MenuGlyph.GlyphRect.Contains(e.Location))
            {
                loadMenuItems();
                if (_ContextDocsList.Visible == false)
                    _ContextDocsList.Show(this, new Point(e.X, _MenuGlyph.GlyphRect.Bottom));
            }

            if (_CloseButton.CrossRect.Contains(e.Location) && this.SelectedItem.CanClose )
            {
                TabStripItemClosingEventArgs evt = new TabStripItemClosingEventArgs(_SelectedItem);

                OnTabStripItemClosing(evt);

                if (!evt.Cancel)
                {
                    this.Controls.Remove(_SelectedItem);

                    this.OnTabStripItemClosing(EventArgs.Empty);
                }
            }

            this.Invalidate();

            TabStripItem selected = null;

            for (int i = 0; i < this.Controls.Count; i++)
            {
                TabStripItem current = (TabStripItem)this.Controls[i];

                if (current.StripRect.Contains(e.Location))
                {
                    current.DrawState |= DrawItemState.Selected;

                    if (current.StripRect.Right > _StripButtonRect.Right)
                    {
                        break;
                    }
                    else
                    {
                        selected = current;
                        break;
                    }
                }
            }

            if (selected == null)
                return;

            this.SelectedItem = selected;

            this.Invalidate();
        }

        protected override void OnControlRemoved(ControlEventArgs e)
        {
            base.OnControlRemoved(e);
            if (this.Controls.Count > 0)
                this.SelectedItem = (TabStripItem)this.Controls[this.Controls.Count - 1];
            else
                this.SelectedItem = null;
        }

        public virtual void OnTabStripItemClosing(TabStripItemClosingEventArgs e)
        {
            if (TabStripItemClosing != null)
            {
                TabStripItemClosing(e);
            }
        }

        public virtual void OnTabStripItemClosing(EventArgs e)
        {
            if (TabStripItemClosed != null)
            {
                TabStripItemClosed(this,e);
            }
        }

        void SelectItem(TabStripItem tabItem)
        {
            tabItem.Visible = true;
            tabItem.DrawState |= DrawItemState.Selected;
        }

        void UnSelectItem(TabStripItem tabItem)
        {
            tabItem.Visible = false;
            tabItem.DrawState = DrawItemState.None;
        }

        void loadMenuItems()
        {
            _ContextDocsList.Items.Clear();


            for (int i = 0; i < this.Controls.Count; i++)
            {
                TabStripItem item = (TabStripItem)this.Controls[i];

                ToolStripMenuItem tItem = new ToolStripMenuItem(item.Text);

                tItem.Tag = item;

                _ContextDocsList.Items.Add(tItem);
            }
        }

        void _ContextDocsList_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            this.SelectedItem = (TabStripItem)e.ClickedItem.Tag;
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (_MenuGlyph.GlyphRect.Contains(e.Location))
            {
                _MenuGlyph.IsMouseOver = true;
                this.Invalidate(_MenuGlyph.GlyphRect);
            }
            else
            {
                if (_MenuGlyph.IsMouseOver != false)
                {
                    _MenuGlyph.IsMouseOver = false;
                    this.Invalidate(_MenuGlyph.GlyphRect);
                }
            }

            if (_CloseButton.CrossRect.Contains(e.Location))
            {
                _CloseButton.IsMouseOver = true;
                this.Invalidate(_CloseButton.CrossRect);
            }
            else
            {
                if (_CloseButton.IsMouseOver != false)
                {
                    _CloseButton.IsMouseOver = false;
                    this.Invalidate(_CloseButton.CrossRect);
                }
            }
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            base.OnMouseLeave(e);
            _MenuGlyph.IsMouseOver = false;
            this.Invalidate(_MenuGlyph.GlyphRect);

            _CloseButton.IsMouseOver = false;
            this.Invalidate(_CloseButton.CrossRect);
        }

        void DrawButton(Graphics gfx, RectangleF buttonRect, DrawItemState state, int buttonIndex)
        {
            GraphicsPath _Path = new GraphicsPath();

            int mtop = 3;

            if ((state &= DrawItemState.Selected) == DrawItemState.Selected || buttonIndex == 0)
                _Path.AddLine(buttonRect.Left - 10, buttonRect.Bottom - 1, buttonRect.Left + (buttonRect.Height / 2) - 4, mtop + 4);
            else
            {
                _Path.AddLine(buttonRect.Left, buttonRect.Bottom - 1, buttonRect.Left,
                    buttonRect.Bottom - (buttonRect.Height / 2) - 2);

                _Path.AddLine(buttonRect.Left, buttonRect.Bottom - (buttonRect.Height / 2) - 3,
                    buttonRect.Left + (buttonRect.Height / 2) - 4, mtop + 3);
            }

            _Path.AddLine(buttonRect.Left + (buttonRect.Height / 2) + 2,
                mtop, buttonRect.Right - 3, mtop);

            _Path.AddLine(buttonRect.Right, mtop + 2, buttonRect.Right,
                buttonRect.Bottom - 1);

            _Path.AddLine(buttonRect.Right - 4, buttonRect.Bottom - 1, buttonRect.Left,
                buttonRect.Bottom - 1);

            _Path.CloseFigure();

            LinearGradientBrush _brush = null;

            if ((state &= DrawItemState.Selected) == DrawItemState.Selected)
            {
                
                _brush = new LinearGradientBrush(buttonRect,
                SystemColors.ControlLightLight, SystemColors.Window,
                LinearGradientMode.Vertical);
            }
            else
            {
                _brush = new LinearGradientBrush(buttonRect,
                    SystemColors.ControlLightLight, SystemColors.Control,
                    LinearGradientMode.Vertical);
            }

            gfx.FillPath(_brush, _Path);

            gfx.DrawPath(SystemPens.ControlDark, _Path);
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            _StripButtonRect = new Rectangle(0, 0, this.ClientSize.Width - 40, 10);

            _MenuGlyph.GlyphRect = new Rectangle(this.ClientSize.Width - 39, 2, 16, 16);

            _CloseButton.CrossRect = new Rectangle(this.ClientSize.Width - 20,2, 16, 16);

            if (_SelectedItem != null)
            {
                if (_SelectedItem.StripRect.Right > _StripButtonRect.Right)
                {
                    this.Controls.SetChildIndex(_SelectedItem, 0);
                }
            }
        }
    }
}
