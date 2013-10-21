
//    Copyright (C) 2004  Riccardo Marzi <riccardo@dotnetfireball.net>
//
//    Copyright (C) 2004  Sebastian Faltoni <sebastian@dotnetfireball.net>
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



#region Using directives

using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;

#endregion

namespace Fireball.Collections.Generic
{

    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class LightCollection<T> : ILightCollection<T>, ICloneable, IList
    {
        #region Static Members

        internal static string InternalGetResourceError(string key)
        {
            return string.Empty;
        }

        #endregion

        #region Nested Types

        public struct Enumerator : IEnumerator<T>, IEnumerator
        {
            private LightCollection<T> _list;
            private int _index;
            //private int _version;
            private T _current;

            internal Enumerator(LightCollection<T> list)
            {
                _list = list;
                _index = 0;
                _current = default(T);
            }

            public T Current
            {
                get { return _current; }
            }
            public void Dispose()
            {
            }
            object IEnumerator.Current
            {
                get
                {
                    if ((_index == 0) || (_index == (_list.Count + 1)))
                    {
                        throw new InvalidOperationException();
                    }
                    return this.Current;
                }
            }
            public bool MoveNext()
            {
                if (_index < _list.Count)
                {
                    _current = _list[_index];
                    _index++;
                    return true;
                }
                _index = _list.Count + 1;
                _current = default(T);
                return false;
            }
            public void Reset()
            {
                _index = 0;
                _current = default(T);
            }
        }

        public class LightCollectionAddEventArgs : EventArgs
        {
            private T _Item;
            private int _Index;

            public T Item
            {
                get
                {
                    return _Item;
                }
            }

            public int Index
            {
                get
                {
                    return _Index;
                }
            }

            public LightCollectionAddEventArgs(T item, int index)
            {
                _Item = item;
                _Index = index;
            }
        }
        public class LightCollectionRemoveEventArgs : EventArgs
        {
            private T _RemovedItem;
            private int _Index;

            public T RemovedItem
            {
                get
                {
                    return _RemovedItem;
                }
            }

            public int Index
            {
                get
                {
                    return _Index;
                }
            }

            public LightCollectionRemoveEventArgs(T removedItem, int index)
            {
                _RemovedItem = removedItem;
                _Index = index;
            }
        }
        public class LightCollectionMoveEventArgs : EventArgs
        {
            private T _Item;
            private int _NewIndex;
            private int _OldIndex;

            public T Item
            {
                get
                {
                    return _Item;
                }
            }

            public int NewIndex
            {
                get
                {
                    return _NewIndex;
                }
            }

            public int OldIndex
            {
                get
                {
                    return _OldIndex;
                }
            }


            public LightCollectionMoveEventArgs(T item, int newIndex, int oldIndex)
            {
                _Item = item;
                _NewIndex = newIndex;
                _OldIndex = oldIndex;
            }
        }
        public class LightCollectionAddRangeEventArgs : EventArgs
        {
            private T[] _Items;
            private int _StartIndex;

            public T[] Items
            {
                get
                {
                    return _Items;
                }
            }

            public int StartIndex
            {
                get
                {
                    return _StartIndex;
                }
            }

            public LightCollectionAddRangeEventArgs(T[] items, int startIndex)
            {
                _Items = items;
                _StartIndex = startIndex;
            }
        }

        #endregion

        #region Delegates

        public delegate void LightCollectionAddHandler(object sender, LightCollectionAddEventArgs e);
        public delegate void LightCollectionRemoveHandler(object sender, LightCollectionRemoveEventArgs e);
        public delegate void LightCollectionMoveHandler(object sender, LightCollectionMoveEventArgs e);
        public delegate void LightCollectionAddRangeHandler(object sender, LightCollectionAddRangeEventArgs e);

        #endregion

        #region Events

        public event LightCollectionAddHandler ItemAdd;
        public event LightCollectionRemoveHandler ItemRemove;
        public event LightCollectionMoveHandler ItemMove;
        public event LightCollectionAddRangeHandler ItemAddRange;
        public event EventHandler CollectionClear;

        #endregion

        #region Fields

        protected T[] _items;
        private int _count;
        private ReadOnlyCollection<T> _readonlyColl;

        #endregion

        #region Constructor

        /// <summary>
        /// Constructor of LightCollection
        /// </summary>
        public LightCollection()
        {
            Init(4);
        }
        /// <summary>
        /// Constructor of LightCollection with the initial capacity specified of the collection
        /// </summary>
        /// <param name="StartCapacity">The initial capacity of the collection</param>
        public LightCollection(int StartCapacity)
        {
            Init(StartCapacity);
        }
        /// <summary>
        /// Constructor of LightCollection
        /// </summary>
        /// <param name="coll">The collection from where copy items</param>
        public LightCollection(ILightCollection<T> coll)
        {
            Init(coll.Count);

            this.AddRange(coll.GetItems());
        }
        /// <summary>
        /// Constructor of LightCollection with array of T[] as argument
        /// </summary>
        /// <param name="array">Array of T[] as initial values</param>
        public LightCollection(T[] array)
        {

            if (array == null)
            {
                throw new ArgumentNullException("array");
            }
            _items = array;
            _count = array.Length;
        }

        #endregion

        #region Private Members

        private void Init(int capacity)
        {
            _items = new T[capacity];
            _count = 0;
            _readonlyColl = null;
        }
        private void EnsureCapacity(int min)
        {
            if (this._items.Length < min)
            {
                int newCapacity = (this._items.Length == 0) ? 4 : (this._items.Length * 2);

                if (newCapacity < min)
                {
                    newCapacity = min;
                }
                this.SetCollCapacity(newCapacity);
            }
        }
        private void SetCollCapacity(int value)
        {
            if (value != this._items.Length)
            {
                if (value < this._count)
                {
                    throw new ArgumentOutOfRangeException("value", "ArgumentOutOfRange SmallCapacity");
                }
                if (value > 0)
                {
                    T[] newArray = new T[value];

                    if (this._count > 0)
                    {
                        Array.Copy(this._items, 0, newArray, 0, this._count);
                    }

                    this._items = newArray;
                }
                else
                {
                    this._items = new T[4];
                }
            }
        }

        #endregion

        #region Public Properties

        /// <summary>
        /// Return a T element from the specified index
        /// </summary>
        /// <param name="index"></param>
        /// <returns></returns>
        public virtual T this[int index]
        {
            get
            {
                try
                {
                    return _items[index];
                }
                catch
                {
                    throw new IndexOutOfRangeException("Here is no item at index(" + index.ToString() + ")");
                }
            }
            set
            {
                try
                {
                    _items[index] = value;
                }
                catch
                {
                    throw new IndexOutOfRangeException("Here is no item at index(" + index.ToString() + ")");
                }
            }
        }
        /// <summary>
        /// Return the Count  of this collection
        /// </summary>
        public int Count
        {
            get
            {
                return _count;
            }
        }
        /// <summary>
        /// Return a read-only collection
        /// </summary>
        public virtual ReadOnlyCollection<T> ReadOnlyCollection
        {
            get
            {
                if (_readonlyColl == null) _readonlyColl = new ReadOnlyCollection<T>(this);
                return _readonlyColl;
            }
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// Add a T item to this collection
        /// </summary>
        /// <param name="item">Item to add to this collection</param>
        /// <returns>Return the Item index on this collection</returns>
        public virtual int Add(T item)
        {
            if (this._count == this._items.Length)
            {
                this.EnsureCapacity(this._count + 1);
            }

            this._items[this._count] = item;

            _count++; // non spostare

            if (ItemAdd != null) ItemAdd(this, new LightCollectionAddEventArgs(item, this._count));

            return _count-1;
        }
        /// <summary>
        /// Add a T item to this collection
        /// </summary>
        /// <param name="item">Item to add to this collection</param>
        /// <param name="sort">set to true for sort now the collection</param>
        /// <returns>Return the Item index on this collection</returns>
        public virtual int Add(T item, bool sort)
        {
            int i = Add(item);

            if (sort)
                this.Sort();

            return i;
        }
        /// <summary>
        /// Add a range of Items to this collection
        /// </summary>
        /// <param name="items">An ILightCollection<T> derived collection</param>
        public virtual void AddRange(ILightCollection<T> items)
        {
            InsertRange(this._count, items.GetItems());
        }
        /// <summary>
        /// Add a range of Items to this collection
        /// </summary>
        /// <param name="items">Array of T[] Items to add to this collection</param>
        public virtual void AddRange(T[] items)
        {
            InsertRange(this._count, items);
        }
        /// <summary>
        /// Get and Set the Collection Capacity
        /// </summary>
        public virtual int Capacity
        {
            get
            {
                return this._items.Length;
            }
            set
            {
                this.SetCollCapacity(value);
            }
        }
        /// <summary>
        /// Empty the collection
        /// </summary>
        public virtual void Clear()
        {
            _items = new T[] { };
            _count = 0;
            if (CollectionClear != null) CollectionClear(this, new EventArgs());
        }
        /// <summary>
        /// Create a cloned colletrion
        /// </summary>
        /// <returns>A copy of collection</returns>
        public LightCollection<T> Clone()
        {
            return new LightCollection<T>(this);
        }
        /// <summary>
        /// Check if a T item is contained on this collection
        /// </summary>
        /// <param name="item">The item to check</param>
        /// <returns>True if is contained otherwise False</returns>
        public virtual bool Contains(T item)
        {
            return (IndexOf(item, 0, _count) >= 0);
        }
        /// <summary>
        /// Copy collection to an array of T elements types
        /// </summary>
        /// <param name="array">An array of T elements types</param>
        /// <param name="index">Start copy index</param>
        public virtual void CopyTo(Array array, int index)
        {
            Array.Copy(_items, index, array, 0, _count);
        }
        /// <summary>
        /// Find an item of the collection using a predicate
        /// </summary>
        /// <param name="match"></param>
        /// <returns>Boolean Result</returns>
        public virtual T Find(Predicate<T> match)
        {

            if (match == null)
            {
                throw new ArgumentNullException("match");
            }
            for (int i = 0; i < _count; i++)
            {
                if (match(_items[i]))
                {
                    return _items[i];
                }
            }
            T type;
            type = default(T);
            return type;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public virtual LightCollection<T>.Enumerator GetEnumerator()
        {
            return new LightCollection<T>.Enumerator(this);
        }
        /// <summary>
        /// Get a copy of all collection items
        /// </summary>
        /// <returns>Array of T[]</returns>
        public virtual T[] GetItems()
        {
            if (_count <= 0) return new T[0];
            return GetItems(0, _count - 1);
        }
        /// <summary>
        /// Get a copy of all collection items
        /// </summary>
        /// <returns>Array of T[]</returns>
        public virtual T[] GetItems(int startIndex)
        {
            if (_count <= 0) return new T[0];
            return GetItems(startIndex, _count - 1);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="startIndex"></param>
        /// <param name="finalIndex"></param>
        /// <returns></returns>
        public virtual T[] GetItems(int startIndex, int finalIndex)
        {
            if (_count <= 0) return new T[0];
            if (finalIndex < startIndex)
            {
                throw new ArgumentOutOfRangeException("finalIndex", finalIndex, "finalIndex was out of range. Must be non-negative and less than the size of the collection.");
            }
            if ((startIndex < 0) || (startIndex > _count))
            {
                throw new ArgumentOutOfRangeException("startIndex", startIndex, "startIndex was out of range. Must be non-negative and less than the size of the collection.");
            }
            if (finalIndex > _count - 1)
            {
                throw new ArgumentOutOfRangeException("finalIndex", finalIndex, "finalIndex was out of range. Must be minor of the Count");
            }
            T[] newItems = new T[finalIndex - startIndex + 1];

            Array.Copy(_items, startIndex, newItems, 0, finalIndex + 1);

            return newItems;
        }
        /// <summary>
        /// Get The Index of a item on the collection
        /// </summary>
        /// <param name="item">The item to search</param>
        /// <returns>Index of T</returns>
        public virtual int IndexOf(T item)
        {
            return IndexOf(item, 0, _count);
        }
        /// <summary>
        /// Get The Index of a item on the collection
        /// </summary>
        /// <param name="item">The item to search</param>
        /// <param name="index">The index where start search</param>
        /// <returns>Index of T</returns>
        public virtual int IndexOf(T item, int index)
        {
            return IndexOf(item, index, _count - index);
        }
        /// <summary>
        /// Get The Index of a item on the collection
        /// </summary>
        /// <param name="item">The item to search</param>
        /// <param name="index">The index where start search</param>
        /// <param name="count">Number of max search to try</param>
        /// <returns>Index of T</returns>
        public virtual int IndexOf(T item, int index, int count)
        {
            return Array.IndexOf<T>(this._items, item, index, count);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="index"></param>
        /// <param name="items"></param>
        public virtual void InsertRange(int index, ILightCollection<T> items)
        {
            InsertRange(index, items.GetItems());
        }
        /// <summary>
        /// Insert a range of T[] elements to this collection starting from index
        /// </summary>
        /// <param name="index">The from where starting insert</param>
        /// <param name="items">T[] array of elements</param>
        public virtual void InsertRange(int index, T[] items)
        {
            if (items == null)
            {
                throw new ArgumentNullException("items", "Null Array");
            }
            if ((index < 0) || (index > this._count))
            {
                throw new ArgumentOutOfRangeException("index", "Index out of Range");
            }
            int num1 = items.Length;
            if (num1 > 0)
            {
                this.EnsureCapacity(this._count + num1);

                if (index < this._count)
                {
                    Array.Copy(this._items, index, this._items, (int)(index + num1), (int)(this._count - index));
                }
                items.CopyTo(this._items, index);
                this._count += num1;
            }

            if (ItemAddRange != null) ItemAddRange(this, new LightCollectionAddRangeEventArgs(items, index));
        }
        /// <summary>
        /// Insert a T Item at index
        /// </summary>
        /// <param name="index">The index where insert the Item</param>
        /// <param name="item">The item to insert</param>
        public virtual void Insert(int index, T item)
        {
            if ((index < 0) || (index > this._count))
            {
                throw new ArgumentOutOfRangeException("index", "Index must be within the bounds of the List.");
            }
            if (this._count == this._items.Length)
            {
                this.EnsureCapacity(this._count + 1);
            }
            if (index < this._count)
            {
                Array.Copy(this._items, index, this._items, (int)(index + 1), (int)(this._count - index));
            }
            this._items[index] = item;
            this._count++;

            if (ItemAdd != null) ItemAdd(this, new LightCollectionAddEventArgs(item, index));
        }
        /// <summary>
        /// Move an element from an index to another
        /// </summary>
        /// <param name="index">Source index</param>
        /// <param name="newIndex">Destination index</param>
        public virtual void MoveByIndex(int index, int newIndex)
        {
            if (index == newIndex) return;
            if (index < 0 || index >= _items.Length) throw new ArgumentOutOfRangeException("index");
            if (newIndex < 0 || newIndex >= _items.Length) throw new ArgumentOutOfRangeException("newIndex");

            int min = Math.Min(index, newIndex);
            T[] newArray = new T[_items.Length - min];

            int cp_index = index - min;
            int cp_newIndex = newIndex - min;

            Array.Copy(_items, min, newArray, 0, newArray.Length);

            if (index > newIndex)
            {
                int pos = newIndex + 1;

                _items[newIndex] = newArray[cp_index];

                for (int i = 0; i < newArray.Length; i++)
                {
                    if (i != (cp_index))
                    {
                        _items[pos] = newArray[cp_newIndex + i];
                        pos++;
                    }
                }
            }
            else // index < newIndex
            {
                int pos = index;

                _items[newIndex] = newArray[cp_index];

                for (int i = cp_index + 1; i < newArray.Length; i++)
                {
                    if (pos == (newIndex)) pos++;

                    _items[pos] = newArray[i];
                    pos++;
                }
            }

            if(ItemMove!=null) ItemMove(this, new LightCollectionMoveEventArgs(_items[newIndex], newIndex, index));

        }
        /// <summary>
        /// Move an element to a new index position
        /// </summary>
        /// <param name="item">Element to move</param>
        /// <param name="newIndex">Destination index</param>
        public virtual void Move(T item, int newIndex)
        {
            int index = this.IndexOf(item);
            if (index < 0) return;
            if(newIndex<0 || newIndex >= _count) return;
            if (index == newIndex) return;

            MoveByIndex(index, newIndex);
        }
        /// <summary>
        /// Remove a T item
        /// </summary>
        /// <param name="item"></param>
        /// <returns></returns>
        public virtual bool Remove(T item)
        {
            int i = this.IndexOf(item);
            if (i >= 0)
            {
                this.RemoveAt(i);
                return true;
            }
            return false;
        }
        /// <summary>
        /// Remove a T item at index
        /// </summary>
        /// <param name="index"></param>
        public virtual void RemoveAt(int index)
        {
            if ((index < 0) || (index >= _count))
            {
                throw new ArgumentOutOfRangeException("index", index, "Index was out of range. Must be non-negative and less than the size of the collection."); // InternalGetResourceFromDefault("ArgumentOutOfRange_Index"));
            }

            T removedItem = _items[index];

            Array.Copy(_items, index + 1, _items, index, _items.Length - index - 1);


            Array.Resize<T>(ref _items, _count - 1);

            _count--;

            if (ItemRemove != null) ItemRemove(this, new LightCollectionRemoveEventArgs(removedItem, index));
        }
        /// <summary>
        /// Reverse the order of the collection content
        /// </summary>
        public virtual void Reverse()
        {
            Array.Reverse(_items, 0, _count);
        }
        /// <summary>
        /// Reverse the order of the collection content
        /// </summary>
        /// <param name="index">Start index</param>
        /// <param name="length">Number of elements</param>
        public virtual void Reverse(int index, int length)
        {
            if ((index + length) >= _count) throw new ArgumentOutOfRangeException((index >= _count) ? "index" : "length");
            if (length <= 1) return;
            Array.Reverse(_items, index, length);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="index"></param>
        /// <param name="length"></param>
        public virtual void Sort(int index, int length)
        {
            Array.Sort<T>(_items, index, length);
        }
        /// <summary>
        /// 
        /// </summary>
        public virtual void Sort()
        {
            Sort(0, _count);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="keys"></param>
        public virtual void Sort(Array keys)
        {
            if (keys.Length != this.Count  /*_items.Length*/)
            {
                throw new Exception("The Length of \"keys\" does not match with \"items\" lenght");
            }
            Array.Sort(keys, _items);
        }
        /// <summary>
        /// Swap two elements
        /// </summary>
        /// <param name="index1">First element index</param>
        /// <param name="index2">Second element index</param>
        public virtual void SwapByIndex(int index1, int index2)
        {
            if (index1 == index2) return;
            if (index1 < 0 || index1 >= _items.Length) throw new ArgumentOutOfRangeException("index1");
            if (index2 < 0 || index2 >= _items.Length) throw new ArgumentOutOfRangeException("index2");


            T temp = _items[index2];
            _items[index2] = _items[index1];
            _items[index1] = temp;
        }
        /// <summary>
        /// Swap two elements
        /// </summary>
        /// <param name="item1">First element</param>
        /// <param name="item2">First element</param>
        public virtual void Swap(T item1, T item2)
        {
            if (item1.Equals(item2)) return;

            int index1 = this.IndexOf(item1);
            int index2 = this.IndexOf(item2);

            SwapByIndex(index1, index2);
        }
        /// <summary>
        /// Reduce the collection buffer to real count size
        /// </summary>
        public virtual void TrimToSize()
        {
            this.Capacity = this._count;
        }
        /// <summary>
        /// Try to get an item if exist
        /// </summary>
        /// <param name="index">Index of the item</param>
        /// <param name="item">Return the item if exist, otherwise null</param>
        /// <returns></returns>
        public virtual bool TryGetItem(int index, out T item)
        {
            item = default(T);
            if (index < 0 || index >= _items.Length) return false;
            item = _items[index];
            return true;
        }

        #endregion

        #region ICloneable Members

        object ICloneable.Clone()
        {
            return this.Clone();
        }

        #endregion

        #region IList Members

        int IList.Add(object value)
        {
            if (!value.GetType().Equals(typeof(T))) throw new ArgumentException("Invalid type " + value.GetType().ToString());
            return this.Add((T)value);
        }
        void IList.Clear()
        {
            this.Clear();
        }
        bool IList.Contains(object value)
        {
            if (!value.GetType().Equals(typeof(T))) throw new ArgumentException("Invalid type " + value.GetType().ToString());
            return this.Contains((T)value);
        }
        int IList.IndexOf(object value)
        {
            if (!value.GetType().Equals(typeof(T))) throw new ArgumentException("Invalid type " + value.GetType().ToString());
            return this.IndexOf((T)value);
        }
        void IList.Insert(int index, object value)
        {
            if (!value.GetType().Equals(typeof(T))) throw new ArgumentException("Invalid type " + value.GetType().ToString());
            this.Insert(index, (T)value);
        }
        bool IList.IsFixedSize
        {
            get { return false; }
        }
        bool IList.IsReadOnly
        {
            get { return false; }
        }
        void IList.Remove(object value)
        {
            if (!value.GetType().Equals(typeof(T))) throw new ArgumentException("Invalid type " + value.GetType().ToString());
            this.Remove((T)value);
        }
        void IList.RemoveAt(int index)
        {
            this.RemoveAt(index);
        }
        object IList.this[int index]
        {
            get
            {
                return this[index];
            }
            set
            {
                if (!value.GetType().Equals(typeof(T))) throw new ArgumentException("Invalid type " + value.GetType().ToString());
                this[index] = (T)value;
            }
        }

        #endregion

        #region ICollection Members

        void ICollection.CopyTo(Array array, int index)
        {
            this.CopyTo(array, index);
        }
        int ICollection.Count
        {
            get
            {
                return this.Count;
            }
        }
        bool ICollection.IsSynchronized
        {
            get
            {
                return false;
            }
        }
        object ICollection.SyncRoot
        {
            get
            {
                return null;
            }
        }

        #endregion

        #region IEnumerable Members

        IEnumerator IEnumerable.GetEnumerator()
        {
            return this.GetEnumerator();
        }

        #endregion

    }
}
