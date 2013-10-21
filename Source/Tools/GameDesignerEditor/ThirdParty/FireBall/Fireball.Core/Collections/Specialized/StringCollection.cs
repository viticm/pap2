
//    Copyright (C) 2005  Riccardo Marzi <riccardo@dotnetfireball.net>
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

using Fireball.Collections.Generic;

namespace Fireball.Collections.Specialized
{
    public class StringCollection : ILightCollection<string>
    {
        private LightCollection<string> _coll;

        public event LightCollection<string>.LightCollectionAddHandler ItemAdd;
        public event LightCollection<string>.LightCollectionRemoveHandler ItemRemove;
        public event LightCollection<string>.LightCollectionMoveHandler ItemMove;
        public event LightCollection<string>.LightCollectionAddRangeHandler ItemAddRange;
        public event EventHandler CollectionClear;

        /// <summary>
        /// Initializes a new instance of the <see cref="T:StringCollection"/> class.
        /// </summary>
        public StringCollection()
        {
            _coll = new LightCollection<string>();
        }
        /// <summary>
        /// Initializes a new instance of the <see cref="T:StringCollection"/> class.
        /// </summary>
        /// <param name="items">The items.</param>
        public StringCollection(string[] items)
        {
            _coll = new LightCollection<string>(items);
            _coll.ItemAdd += new LightCollection<string>.LightCollectionAddHandler(_coll_ItemAdd);
            _coll.ItemAddRange += new LightCollection<string>.LightCollectionAddRangeHandler(_coll_ItemAddRange);
            _coll.ItemMove += new LightCollection<string>.LightCollectionMoveHandler(_coll_ItemMove);
            _coll.ItemRemove += new LightCollection<string>.LightCollectionRemoveHandler(_coll_ItemRemove);
            _coll.CollectionClear += new EventHandler(_coll_CollectionClear);
        }

        #region Internal collection Events

        void _coll_ItemAdd(object sender, LightCollection<string>.LightCollectionAddEventArgs e)
        {
            if (ItemAdd != null)
            {
                ItemAdd(this, e);
            }
        }
        void _coll_ItemAddRange(object sender, LightCollection<string>.LightCollectionAddRangeEventArgs e)
        {
            if (ItemAddRange != null)
            {
                ItemAddRange(this, e);
            }
        }
        void _coll_ItemMove(object sender, LightCollection<string>.LightCollectionMoveEventArgs e)
        {
            if (ItemMove != null)
            {
                ItemMove(this, e);
            }
        }
        void _coll_ItemRemove(object sender, LightCollection<string>.LightCollectionRemoveEventArgs e)
        {
            if (ItemRemove != null)
            {
                ItemRemove(this, e);
            }
        }
        void _coll_CollectionClear(object sender, EventArgs e)
        {
            if (CollectionClear != null)
            {
                CollectionClear(this, e);
            }
        }

        #endregion

        /// <summary>
        /// Initializes a new instance of the <see cref="T:StringCollection"/> class.
        /// </summary>
        /// <param name="collection">The collection.</param>
        public StringCollection(ILightCollection<string> collection)
        {
            _coll = new LightCollection<string>(collection);
        }
        /// <summary>
        /// Initializes a new instance of the <see cref="T:StringCollection"/> class.
        /// </summary>
        /// <param name="startCapacity">The start capacity.</param>
        public StringCollection(int startCapacity)
        {
            _coll = new LightCollection<string>(startCapacity);
        }

        /// <summary>
        /// Gets or sets the capacity.
        /// </summary>
        /// <value>The capacity.</value>
        public virtual int Capacity
        {
            get
            {
                return _coll.Capacity;
            }
            set
            {
                _coll.Capacity = value;
            }
        }

        #region ILightCollection<string> Members

        public int Count
        {
            get { return _coll.Count; }
        }
        public string this[int index]
        {
            get
            {
                return _coll[index];
            }
            set
            {
                _coll[index] = value;
            }
        }
        public int Add(string item)
        {
            return _coll.Add(item);
        }
        public void AddRange(string[] items)
        {
            _coll.AddRange(items);
        }
        public void Clear()
        {
            _coll.Clear();
        }
        public bool Contains(string item)
        {
            return _coll.Contains(item);
        }
        public void Insert(int index, string item)
        {
            _coll.Insert(index, item);
        }
        public bool Remove(string item)
        {
            return _coll.Remove(item);
        }
        public void RemoveAt(int index)
        {
            _coll.RemoveAt(index);
        }
        public string Find(Predicate<string> match)
        {
            return _coll.Find(match);
        }
        public int IndexOf(string item)
        {
            return _coll.IndexOf(item);
        }
        public int IndexOf(string item, int index)
        {
            return _coll.IndexOf(item, index);
        }
        public int IndexOf(string item, int index, int count)
        {
            return _coll.IndexOf(item, index, count);
        }
        public string[] GetItems()
        {
            return _coll.GetItems();
        }
        public string[] GetItems(int startIndex)
        {
            return _coll.GetItems(startIndex);
        }
        public string[] GetItems(int startIndex, int finalIndex)
        {
            return _coll.GetItems(startIndex, finalIndex);
        }
        public void CopyTo(Array array, int index)
        {
            _coll.CopyTo(array, index);
        }
        public LightCollection<string>.Enumerator GetEnumerator()
        {
            return _coll.GetEnumerator();
        }
        public void Reverse()
        {
            _coll.Reverse();
        }
        public void Reverse(int index, int length)
        {
            _coll.Reverse(index, length);
        }
        public void MoveByIndex(int index, int newIndex)
        {
            _coll.MoveByIndex(index, newIndex);
        }
        public void Move(string item, int newIndex)
        {
            _coll.Move(item, newIndex);
        }
        public void SwapByIndex(int index1, int index2)
        {
            _coll.SwapByIndex(index1, index2);
        }
        public void Swap(string item1, string item2)
        {
            _coll.Swap(item1, item2);
        }

        #endregion

        /// <summary>
        /// Joins the specified separator.
        /// </summary>
        /// <param name="separator">The separator.</param>
        /// <returns></returns>
        public string Join(string separator)
        {
            string[] items = _coll.GetItems();
            return string.Join(separator, items);
        }
        /// <summary>
        /// Joins the specified separator.
        /// </summary>
        /// <param name="separator">The separator.</param>
        /// <param name="startIndex">The start index.</param>
        /// <param name="count">The count.</param>
        /// <returns></returns>
        public string Join(string separator, int startIndex, int count)
        {
            string[] items = _coll.GetItems();
            return string.Join(separator, items, startIndex, count);
        }
        /// <summary>
        /// Toes the lower.
        /// </summary>
        public void ToLower()
        {
            for (int i = 0; i < _coll.Count; i++)
            {
                if (_coll[i] == null) continue;

                _coll[i] = _coll[i].ToLower();
            }
        }
        /// <summary>
        /// Toes the upper.
        /// </summary>
        public void ToUpper()
        {
            for (int i = 0; i < _coll.Count; i++)
            {
                if (_coll[i] == null) continue;

                _coll[i] = _coll[i].ToUpper();
            }
        }
        /// <summary>
        /// Reverses the strings.
        /// </summary>
        public void ReverseStrings()
        {
            for (int i = 0; i < _coll.Count; i++)
            {
                if (_coll[i] == null) continue;

                char[] chars = _coll[i].ToCharArray();
                Array.Reverse(chars);
                _coll[i] = new string(chars);
            }
        }

        #region ILightCollection<string> Members

        public bool TryGetItem(int index, out string item)
        {
            return _coll.TryGetItem(index, out item);
        }

        #endregion

        /// <summary>
        /// Sorts this instance.
        /// </summary>
        public virtual void Sort()
        {
            _coll.Sort();
        }
        /// <summary>
        /// Sorts the specified keys.
        /// </summary>
        /// <param name="keys">The keys.</param>
        public virtual void Sort(Array keys)
        {
            _coll.Sort(keys);
        }
        /// <summary>
        /// Sorts the specified index.
        /// </summary>
        /// <param name="index">The index.</param>
        /// <param name="length">The length.</param>
        public virtual void Sort(int index, int length)
        {
            _coll.Sort(index, length);
        }

        #region ILightCollection<string> Members

        public int Add(string item, bool sort)
        {
            return _coll.Add(item, sort);
        }

        public void AddRange(ILightCollection<string> items)
        {
            _coll.AddRange(items);
        }

        public LightCollection<string> Clone()
        {
            return _coll.Clone();
        }

        public void InsertRange(int index, ILightCollection<string> items)
        {
            _coll.InsertRange(index, items);
        }

        public void InsertRange(int index, string[] items)
        {
            _coll.InsertRange(index, items);
        }

        public ReadOnlyCollection<string> ReadOnlyCollection
        {
            get {
                return _coll.ReadOnlyCollection;
            }
        }

        public void TrimToSize()
        {
            _coll.TrimToSize();
        }

        #endregion
    }
}
