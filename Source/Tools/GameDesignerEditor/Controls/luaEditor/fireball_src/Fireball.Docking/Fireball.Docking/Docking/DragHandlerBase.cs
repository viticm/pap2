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
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Fireball.Docking
{
	/// <summary>
	/// DragHandlerBase is the base class for drag handlers. The derived class should:
	///   1. Define its public method BeginDrag. From within this public BeginDrag method,
	///      DragHandlerBase.BeginDrag should be called to initialize the mouse capture
	///      and message filtering.
	///   2. Override the OnDragging and OnEndDrag methods.
	/// </summary>
	internal class DragHandlerBase : IMessageFilter
	{
		private Control m_dragControl = null;

		private IntPtr m_hWnd;
		private WndProcCallBack m_wndProcCallBack;
		private IntPtr m_prevWndFunc;
		private delegate IntPtr WndProcCallBack(IntPtr hwnd, int Msg, IntPtr wParam, IntPtr lParam);
		[DllImport("User32.dll")]
		private static extern IntPtr SetWindowLong(IntPtr hWnd, int nIndex, WndProcCallBack wndProcCallBack);
		[DllImport("User32.dll")]
		private static extern IntPtr SetWindowLong(IntPtr hWnd, int nIndex, IntPtr wndFunc);
		[DllImport("User32.dll")]
		private static extern IntPtr CallWindowProc(IntPtr prevWndFunc, IntPtr hWnd, int iMsg, IntPtr wParam, IntPtr lParam);

		public DragHandlerBase()
		{
			m_wndProcCallBack = new WndProcCallBack(this.WndProc);
		}

		private void AssignHandle(IntPtr hWnd)
		{
			m_hWnd = hWnd;
			m_prevWndFunc = SetWindowLong(hWnd, -4, m_wndProcCallBack);	// GWL_WNDPROC = -4
		}

		private void ReleaseHandle()
		{
			SetWindowLong(m_hWnd, -4, m_prevWndFunc);	// GWL_WNDPROC = -4
			m_hWnd = IntPtr.Zero;
			m_prevWndFunc = IntPtr.Zero;
		}

		internal Control DragControl
		{
			get	{	return m_dragControl;	}
		}

		private Point m_startMousePosition = Point.Empty;
		protected Point StartMousePosition
		{
			get	{	return m_startMousePosition;	}
		}

		protected bool BeginDrag(Control c)
		{
			// Avoid re-entrance;
			lock (this)
			{
				if (m_dragControl != null)
					return false;

				m_startMousePosition = Control.MousePosition;

				if (!User32.DragDetect(c.Handle, StartMousePosition))
					return false;

				m_dragControl = c;
				c.FindForm().Capture = true;
				AssignHandle(c.FindForm().Handle);
				Application.AddMessageFilter(this);
				return true;
			}
		}

		protected virtual void OnDragging()
		{
		}

		protected virtual void OnEndDrag(bool abort)
		{
		}

		private void EndDrag(bool abort)
		{
			ReleaseHandle();
			Application.RemoveMessageFilter(this);
			m_dragControl.FindForm().Capture = false;

			OnEndDrag(abort);

			m_dragControl = null;
		}

		bool IMessageFilter.PreFilterMessage(ref Message m)
		{
			if (m.Msg == (int)Win32.Msgs.WM_MOUSEMOVE)
				OnDragging();
			else if (m.Msg == (int)Win32.Msgs.WM_LBUTTONUP)
				EndDrag(false);
			else if (m.Msg == (int)Win32.Msgs.WM_CAPTURECHANGED)
				EndDrag(true);
			else if (m.Msg == (int)Win32.Msgs.WM_KEYDOWN && (int)m.WParam == (int)Keys.Escape)
				EndDrag(true);

			return OnPreFilterMessage(ref m);
		}

		protected virtual bool OnPreFilterMessage(ref Message m)
		{
			return false;
		}

		private IntPtr WndProc(IntPtr hWnd, int iMsg, IntPtr wParam, IntPtr lParam)
		{
			if (iMsg == (int)Win32.Msgs.WM_CANCELMODE || iMsg == (int)Win32.Msgs.WM_CAPTURECHANGED)
				EndDrag(true);

			return CallWindowProc(m_prevWndFunc, hWnd, iMsg, wParam, lParam);
		}
	}
}