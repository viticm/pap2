
//    Copyright (C) 2006  Riccardo Marzi <riccardo@dotnetfireball.net>
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

namespace Fireball.Collections.Generic
{
    public class ReadOnlyCollection<T>
    {
        private LightCollection<T> _coll;

        public ReadOnlyCollection(T[] array)
        {
            _coll = new LightCollection<T>(array);
        }
        public ReadOnlyCollection(ILightCollection<T> collection)
        {
            _coll = (LightCollection<T>)collection; // new LightCollection<T>(collection);
        }

        internal LightCollection<T> InternalCollection
        {
            get
            {
                return _coll;
            }
        }

        public virtual T this[int index]
        {
            get
            {
                return _coll[index];
            }
        }
        public virtual int Capacity
        {
            get
            {
                return _coll.Capacity;
            }
        }
        public int Count
        {
            get
            {
                return _coll.Count;
            }
        }

        public virtual ReadOnlyCollection<T> Clone()
        {
            return new ReadOnlyCollection<T>(_coll.GetItems());
        }
        public virtual bool Contains(T item)
        {
            return _coll.Contains(item);
        }
        public virtual void CopyTo(Array array, int index)
        {
            _coll.CopyTo(array, index);
        }
        public virtual T Find(Predicate<T> match)
        {
            return _coll.Find(match);
        }
        public virtual LightCollection<T>.Enumerator GetEnumerator()
        {
            return _coll.GetEnumerator();
        }
        public virtual T[] GetItems()
        {
            return _coll.GetItems();
        }
        public virtual T[] GetItems(int startIndex)
        {
            return _coll.GetItems(startIndex);
        }
        public virtual T[] GetItems(int startIndex, int finalIndex)
        {
            return _coll.GetItems(startIndex,finalIndex);
        }
        public virtual int IndexOf(T item)
        {
            return _coll.IndexOf(item);
        }
        public virtual int IndexOf(T item, int index)
        {
            return _coll.IndexOf(item, index);
        }
        public virtual int IndexOf(T item, int index, int count)
        {
            return _coll.IndexOf(item, index, count);
        }
        public virtual void Reverse()
        {
            _coll.Reverse();
        }
        public virtual void Reverse(int index, int length)
        {
            _coll.Reverse(index, length);
        }
        public virtual void Sort()
        {
            _coll.Sort();
        }
        public virtual void Sort(Array keys)
        {
            _coll.Sort(keys);
        }
        public virtual void Sort(int index, int length)
        {
            _coll.Sort(index, length);
        }
        public virtual void Swap(T item1, T item2)
        {
            _coll.Swap(item1, item2);
        }
        public virtual void SwapByIndex(int index1, int index2)
        {
            _coll.SwapByIndex(index1, index2);
        }
        public virtual void TrimToSize()
        {
            _coll.TrimToSize();
        }
        public virtual bool TryGetItem(int index, out T item)
        {
            return _coll.TryGetItem(index, out item);
        }
    }
}
