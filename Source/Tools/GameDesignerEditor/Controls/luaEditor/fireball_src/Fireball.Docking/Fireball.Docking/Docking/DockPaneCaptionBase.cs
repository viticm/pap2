#region Fireball License
//    Copyright (C) 2005  Sebastian Faltoni sebastian{at}dotnetfireball{dot}net
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

#endregion
#region Original License
// *****************************************************************************
// 
//  Copyright 2004, Weifen Luo
//  All rights reserved. The software and associated documentation 
//  supplied hereunder are the proprietary information of Weifen Luo
//  and are supplied subject to licence terms.
// 
//  WinFormsUI Library Version 1.0
// *****************************************************************************
#endregion

using System;
using System.Windows.Forms;
using System.Drawing;
using System.Runtime.InteropServices;

namespace Fireball.Docking
{
	/// <include file='CodeDoc/DockPaneCaptionBase.xml' path='//CodeDoc/Class[@name="DockPaneCaptionBase"]/ClassDef/*'/>
	public abstract class DockPaneCaptionBase : Control
	{
		/// <include file='CodeDoc/DockPaneCaptionBase.xml' path='//CodeDoc/Class[@name="DockPaneCaptionBase"]/Construct[@name="(DockPane)"]/*'/>
		protected internal DockPaneCaptionBase(DockPane pane)
		{
			m_dockPane = pane;

			#if FRAMEWORK_VER_2x
			SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
			#else
			SetStyle(ControlStyles.DoubleBuffer, true);
			#endif
			SetStyle(ControlStyles.ResizeRedraw, true);
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			SetStyle(ControlStyles.Selectable, false);
		}

		private DockPane m_dockPane;
		/// <include file='CodeDoc/DockPaneCaptionBase.xml' path='//CodeDoc/Class[@name="DockPaneCaptionBase"]/Property[@name="DockPane"]/*'/>
		protected DockPane DockPane
		{
			get	{	return m_dockPane;	}
		}

		/// <include file='CodeDoc/DockPaneCaptionBase.xml' path='//CodeDoc/Class[@name="DockPaneCaptionBase"]/Property[@name="Tabs"]/*'/>
		protected DockPaneTabCollection Tabs
		{
			get	{	return DockPane.Tabs;	}
		}

		/// <include file='CodeDoc/DockPaneCaptionBase.xml' path='//CodeDoc/Class[@name="DockPaneCaptionBase"]/Property[@name="Appearance"]/*'/>
		protected DockPane.AppearanceStyle Appearance
		{
			get	{	return DockPane.Appearance;	}
		}

		/// <exclude />
		protected override Size DefaultSize
		{
			get	{	return Size.Empty;	}
		}

		/// <exclude />
		protected override void WndProc(ref Message m)
		{
			if (m.Msg == (int)Win32.Msgs.WM_LBUTTONDOWN)
			{
				if (DockPane.DockPanel.AllowRedocking && DockPane.AllowRedocking &&
					!DockHelper.IsDockStateAutoHide(DockPane.DockState))
					DockPane.DockPanel.DragHandler.BeginDragPane(DockPane);
				else
					base.WndProc(ref m);
				return;
			}
			else if (m.Msg == (int)Win32.Msgs.WM_LBUTTONDBLCLK)
			{
				base.WndProc(ref m);

				if (DockHelper.IsDockStateAutoHide(DockPane.DockState))
				{
					DockPane.DockPanel.ActiveAutoHideContent = null;
					return;
				}

				IDockableWindow activeContent = DockPane.DockPanel.ActiveContent;
				for (int i=0; i<DockPane.Tabs.Count; i++)
				{
					IDockableWindow content = DockPane.Tabs[i].Content;
					if (DockPane.IsFloat)
						DockPane.RestoreToPanel();
					else
						DockPane.Float();
					if (activeContent != null)
						activeContent.DockHandler.Activate();
				}
				return;
			}
			else if (m.Msg == (int)Win32.Msgs.WM_WINDOWPOSCHANGING)
			{
				int offset = (int)Marshal.OffsetOf(typeof(Win32.WINDOWPOS), "flags");
				int flags = Marshal.ReadInt32(m.LParam, offset);
				Marshal.WriteInt32(m.LParam, offset, flags | (int)Win32.FlagsSetWindowPos.SWP_NOCOPYBITS);
			}

			base.WndProc(ref m);
		}

		internal void RefreshChanges()
		{
			OnRefreshChanges();
		}

		/// <include file='CodeDoc/DockPaneCaptionBase.xml' path='//CodeDoc/Class[@name="DockPaneCaptionBase"]/Method[@name="OnRefreshChanges()"]/*'/>
		protected virtual void OnRefreshChanges()
		{
		}

		/// <include file='CodeDoc/DockPaneCaptionBase.xml' path='//CodeDoc/Class[@name="DockPaneCaptionBase"]/Method[@name="MeasureHeight()"]/*'/>
		protected internal abstract int MeasureHeight();
	}
}
