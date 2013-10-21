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
using System.Collections;

namespace Fireball.Windows.Forms.TuxBar
{
	/// <summary>
	/// Descrizione di riepilogo per TuxBarItemCollection.
	/// </summary>	
	public sealed class TuxBarItemCollection
	{
		#region Declares
		public delegate void TuxBarItemCollectionHandler(TuxBarItem item);
        public delegate void TuxBarItemCollectionInsertHandler(TuxBarItem item, int index);
        public delegate void TuxBarItemCollectionClear(TuxBarItemCollection sender);

		private ArrayList m_List = null;
		public event TuxBarItemCollectionHandler AddItem;
		public event TuxBarItemCollectionHandler RemoveItem;
		public event TuxBarItemCollectionInsertHandler InsertItem;
        public event TuxBarItemCollectionClear ClearItems;
		public event EventHandler LayoutChanged;
		#endregion

		#region CTOR
		public TuxBarItemCollection()
		{
			m_List = new ArrayList();
		}
		#endregion

		#region IList

		public bool IsReadOnly
		{
			get
			{
				return m_List.IsReadOnly;
			}
		}

		public TuxBarItem this[int index]
		{
			get
			{
				return (TuxBarItem)m_List[index];
			}
			set
			{
				m_List[index] = value;
				if(LayoutChanged != null)
					LayoutChanged(value,new EventArgs());
			}
		}

		public void RemoveAt(int index)
		{
			TuxBarItem item = (TuxBarItem)m_List[index];
			Remove(item);
		}

		public void Insert(int index, TuxBarItem value)
		{
			m_List.Insert(index,value);
			if(InsertItem != null)
				InsertItem(value,index);
			if(LayoutChanged != null)
				LayoutChanged(value,new EventArgs());	
			value.Toogle+=new EventHandler(LayoutChanged);
		}

		public void Remove(TuxBarItem value)
		{
			if(RemoveItem != null)
				RemoveItem(value);
			m_List.Remove(value);
			if(LayoutChanged != null)
				LayoutChanged(value,new EventArgs());
		}

		public bool Contains(TuxBarItem value)
		{
			return m_List.Contains(value);
		}

		public void Clear()
		{
            if (ClearItems != null)
                ClearItems(this);
			m_List.Clear();            
		}

		public int IndexOf(TuxBarItem value)
		{
			return m_List.IndexOf(value);
		}

		public int Add(TuxBarItem value)
		{
			int i = m_List.Add(value);
			if(AddItem != null)
				AddItem(value);
			if(LayoutChanged != null)
				LayoutChanged(value,new EventArgs());
			value.Toogle+=new EventHandler(LayoutChanged);
			return i;
		}

		public bool IsFixedSize
		{
			get
			{
				return m_List.IsFixedSize;
			}
		}

		#endregion

		#region ICollection

		public bool IsSynchronized
		{
			get
			{
				return m_List.IsSynchronized;
			}
		}

		public int Count
		{
			get
			{
				return m_List.Count;
			}
		}

		public void CopyTo(Array array, int index)
		{
			m_List.CopyTo(array,index);
		}

		public object SyncRoot
		{
			get
			{
				return m_List.SyncRoot;
			}
		}

		#endregion

		#region IEnumerable

		public IEnumerator GetEnumerator()
		{
			return m_List.GetEnumerator();
		}

		#endregion
	}
}
