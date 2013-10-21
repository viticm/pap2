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

using System;
using System.Collections;

namespace Fireball.Syntax
{

	#region EventArgs and Delegate

	public class CollectionEventArgs : EventArgs
	{
		public CollectionEventArgs()
		{
		}

		public CollectionEventArgs(object item, int index)
		{
			this.Index = index;
			this.Item = item;
		}

		public object Item = null;
		public int Index = 0;
	}

	public delegate void CollectionEventHandler(object sender, CollectionEventArgs e);

	#endregion

	public abstract class BaseCollection : System.Collections.CollectionBase, IList
	{
        public BaseCollection()
		{
		}

		#region Events

		public event CollectionEventHandler ItemAdded = null;

		protected virtual void OnItemAdded(int index, object item)
		{
			if (this.ItemAdded != null)
			{
				CollectionEventArgs e = new CollectionEventArgs(item, index);

				this.ItemAdded(this, e);
			}
		}

		public event CollectionEventHandler ItemRemoved = null;

		protected virtual void OnItemRemoved(int index, object item)
		{
			if (this.ItemRemoved != null)
			{
				CollectionEventArgs e = new CollectionEventArgs(item, index);

				this.ItemRemoved(this, e);
			}
		}

		public event EventHandler ItemsCleared = null;

		protected virtual void OnItemsCleared()
		{
			if (this.ItemsCleared != null)
				this.ItemsCleared(this, EventArgs.Empty);
		}

		#endregion

		#region Overrides

		protected override void OnClearComplete()
		{
			base.OnClearComplete();
			this.OnItemsCleared();
		}

		protected override void OnRemoveComplete(int index, object value)
		{
			base.OnRemoveComplete(index, value);
			this.OnItemRemoved(index, value);
		}

		protected override void OnInsertComplete(int index, object value)
		{
			base.OnInsertComplete(index, value);
			this.OnItemAdded(index, value);
		}

		#endregion
	}
}