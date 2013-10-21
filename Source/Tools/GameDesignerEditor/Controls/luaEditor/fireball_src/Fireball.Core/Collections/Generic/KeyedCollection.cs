
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
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Runtime.InteropServices;

#endregion

namespace Fireball.Collections.Generic
{
    public class KeyedCollection<T> : IKeyedCollection<T>, IEnumerable
    {
        #region Nested Types

        [Serializable, StructLayout(LayoutKind.Sequential)]
        public struct Enumerator : IEnumerator<T>, IDisposable, IEnumerator
        {
            private KeyedCollection<T> m_KeyedColl;
            private int m_Index;
            private T m_Current;

            public Enumerator(KeyedCollection<T> coll)
            {
                this.m_KeyedColl = coll;
                this.m_Index = 0;
                m_Current = default(T);
            }

            #region IEnumerator<T> Members

            public T Current
            {
                get { return m_Current; }
            }

            #endregion

            #region IDisposable Members

            public void Dispose()
            {

            }

            #endregion

            #region IEnumerator Members

            object System.Collections.IEnumerator.Current
            {
                get { return m_Current; }
            }

            public bool MoveNext()
            {
                if (this.m_Index < this.m_KeyedColl.Count)
                {
                    this.m_Current = this.m_KeyedColl._items[this.m_Index];
                    this.m_Index++;
                    return true;
                }

                this.m_Index = this.m_KeyedColl.Count + 1;
                this.m_Current = default(T);
                return false;
            }

            public void Reset()
            {
                this.m_Index = 0;
                this.m_Current = default(T);
            }

            #endregion
        }

        public class KeyedCollectionAddEventArgs : EventArgs
        {
            private T m_Item;
            private int m_Index;
            private string m_Key;

            public T Item
            {
                get { return m_Item; }
                set { m_Item = value; }
            }

            public int Index
            {
                get { return m_Index; }
                set { m_Index = value; }
            }

            public string Key
            {
                get { return m_Key; }
                set { m_Key = value; }
            }

            public KeyedCollectionAddEventArgs(T item, int index, string key)
            {
                m_Item = item;
                m_Index = index;
                m_Key = key;
            }


        }
        public class KeyedCollectionRemoveEventArgs : EventArgs
        {
            private T m_Item;

            private string m_Key;

            private int m_Index;

            public string Key
            {
                get { return m_Key; }
                set { m_Key = value; }
            }

            public int Index
            {
                get { return m_Index; }
                set { m_Index = value; }
            }

            public T Item
            {
                get { return m_Item; }
                set { m_Item = value; }
            }

            public KeyedCollectionRemoveEventArgs(T removedItem, string key, int index)
            {
                m_Item = removedItem;
                m_Key = key;
                m_Index = index;
            }
        }
        public class KeyedCollectionMoveEventArgs : EventArgs
        {
            private T m_Item;
            private int m_NewIndex;
            private int m_OldIndex;
            private string m_Key;

            public string Key
            {
                get { return m_Key; }
                set { m_Key = value; }
            }

            public int OldIndex
            {
                get { return m_OldIndex; }
                set { m_OldIndex = value; }
            }

            public T Item
            {
                get { return m_Item; }
                set { m_Item = value; }
            }

            public int NewIndex
            {
                get { return m_NewIndex; }
                set { m_NewIndex = value; }
            }

            public KeyedCollectionMoveEventArgs(T item, int newIndex, int oldIndex, string key)
            {
                m_Item = item;
                m_NewIndex = newIndex;
                m_OldIndex = oldIndex;
                m_Key = key;
            }
        }
        public class KeyedCollectionAddRangeEventArgs : EventArgs
        {
            private T[] m_Items;
            private string[] m_Keys;
            private int m_StartIndex;

            public int StartIndex
            {
                get { return m_StartIndex; }
                set { m_StartIndex = value; }
            }

            public string[] Keys
            {
                get { return m_Keys; }
                set { m_Keys = value; }
            }

            public T[] Items
            {
                get { return m_Items; }
                set { m_Items = value; }
            }

            public KeyedCollectionAddRangeEventArgs(T[] items, string[] keys, int startIndex)
            {
                m_Items = items;
                m_Keys = keys;
                m_StartIndex = startIndex;
            }
        }

        #endregion

        #region Delegates

        public delegate void KeyedCollectionAddHandler(object sender, KeyedCollectionAddEventArgs e);
        public delegate void KeyedCollectionRemoveHandler(object sender, KeyedCollectionRemoveEventArgs e);
        public delegate void KeyedCollectionMoveHandler(object sender, KeyedCollectionMoveEventArgs e);
        public delegate void KeyedCollectionAddRangeHandler(object sender, KeyedCollectionAddRangeEventArgs e);

        #endregion

        #region Events

        public event KeyedCollectionAddHandler ItemAdd;
        public event KeyedCollectionRemoveHandler ItemRemove;
        public event KeyedCollectionMoveHandler ItemMove;
        public event KeyedCollectionAddRangeHandler ItemAddRange;
        public event EventHandler CollectionClear;
        public event EventHandler CollectionChanged;

        #endregion

        #region Private Fields

        private LightCollection<string> _keys;
        private LightCollection<T> _items;
        private ReadOnlyKeyedCollection<T> _readonlyColl;

        #endregion

        #region Constructor

        public KeyedCollection()
            : this(4)
        {
        }
        public KeyedCollection(int initialSize)
        {
            _keys = new LightCollection<string>(initialSize);
            _items = new LightCollection<T>(initialSize);
            _readonlyColl = null;
        }
        public KeyedCollection(string[] keys, T[] items)
        {
            _keys = new LightCollection<string>(keys);
            _items = new LightCollection<T>(items);
            _readonlyColl = null;
        }

        #endregion

        #region Public Properties

        public T this[int index]
        {
            get
            {
                return _items[index];
            }
            set
            {
                _items[index] = value;
            }
        }
        public T this[string key]
        {
            get
            {
                return _items[_keys.IndexOf(key)];
            }
            set
            {
                _items[_keys.IndexOf(key)] = value;
            }
        }

        public LightCollection<T> Collection
        {
            get
            {
                return _items;
            }
        }
        public int Count
        {
            get
            {
                return _items.Count;
            }
        }
        public string[] Keys
        {
            get
            {
                return _keys.GetItems();
            }
        }
        public virtual ReadOnlyKeyedCollection<T> ReadOnlyCollection
        {
            get
            {
                if (_readonlyColl == null) _readonlyColl = new ReadOnlyKeyedCollection<T>(this);
                return _readonlyColl;
            }
        }

        #endregion

        #region Public Methods

        public virtual void Add(string key, T item)
        {
            if (_keys.Contains(key))
            {
                throw new Exception("Key already exist");
            }
            _keys.Add(key);
            int index = _items.Add(item);

            if (ItemAdd != null) ItemAdd(this, new KeyedCollectionAddEventArgs(_items[index], index, _keys[index]));
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void AddRange(KeyedCollection<T> items)
        {
            if (items == null) return;

            AddRange(items.Keys, items.GetItems());
        }
        public virtual void AddRange(string[] keys, T[] items)
        {
            if (keys.Length != items.Length)
            {
                throw new Exception(" ");
            }

            int startIndex = _items.Count;

            _keys.AddRange(keys);
            _items.AddRange(items);

            if (ItemAddRange != null) ItemAddRange(this, new KeyedCollectionAddRangeEventArgs(items, keys, startIndex));
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void Clear()
        {
            _items.Clear();
            _keys.Clear();

            if (CollectionClear != null) CollectionClear(this, new EventArgs());
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual bool Contains(T item)
        {
            return _items.Contains(item);
        }
        public virtual bool ContainsKey(string key)
        {
            return _keys.Contains(key);
        }
        public virtual string CreateFreeKey()
        {
            string key = "";
            do
            {
                key = Guid.NewGuid().ToString().Substring(0, 8);
            }
            while (_keys.Contains(key));
            return key;
        }
        public KeyedCollection<T>.Enumerator GetEnumerator()
        {
            return new KeyedCollection<T>.Enumerator(this);
        }
        //public IEnumerator<T> GetEnumerator()
        //{
        //    return new KeyedCollection<T>.Enumerator(this);
        //}
        public virtual T[] GetItems()
        {
            if (_items.Count <= 0) return new T[0];
            return _items.GetItems();
        }
        public virtual T[] GetItems(int startIndex)
        {
            if (_items.Count <= 0) return new T[0];
            return _items.GetItems(startIndex);
        }
        public virtual T[] GetItems(int startIndex, int finalIndex)
        {
            if (_items.Count <= 0) return new T[0];
            return _items.GetItems(startIndex, finalIndex);
        }
        public virtual int IndexOfKey(string key)
        {
            return _keys.IndexOf(key);
        }
        public virtual int IndexOf(T item)
        {
            return _items.IndexOf(item);
        }
        public virtual void Insert(int index, string key, T item)
        {
            if (_keys.Contains(key))
            {
                throw new Exception("Key already exist");
            }
            _keys.Insert(index, key);
            _items.Insert(index, item);

            if (ItemAdd != null) ItemAdd(this, new KeyedCollectionAddEventArgs(_items[index], index, _keys[index]));
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void MoveByIndex(int index, int newIndex)
        {
            _items.MoveByIndex(index, newIndex);
            _keys.MoveByIndex(index, newIndex);

            if (ItemMove != null) ItemMove(this, new KeyedCollectionMoveEventArgs(_items[newIndex], newIndex, index, _keys[newIndex]));
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());

        }
        public virtual void Move(T item, int newIndex)
        {
            int index = this.IndexOf(item);
            MoveByIndex(index, newIndex);
        }
        public virtual void RemoveAt(int index)
        {
            T removedItem = _items[index];
            string removedKey = _keys[index];

            _items.RemoveAt(index);
            _keys.RemoveAt(index);

            if (ItemRemove != null) ItemRemove(this, new KeyedCollectionRemoveEventArgs(removedItem, removedKey, index));
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void RemoveByKey(string key)
        {
            int index = _keys.IndexOf(key);
            T removedItem = _items[index];
            _items.RemoveAt(index);
            _keys.RemoveAt(index);

            if (ItemRemove != null) ItemRemove(this, new KeyedCollectionRemoveEventArgs(removedItem, key, index));
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void Remove(T item)
        {
            int index = _items.IndexOf(item);
            T removedItem = _items[index];
            string removedKey = _keys[index];
            _items.RemoveAt(index);
            _keys.RemoveAt(index);

            if (ItemRemove != null) ItemRemove(this, new KeyedCollectionRemoveEventArgs(removedItem, removedKey, index));
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void Sort()
        {
            _items.Sort(_keys.GetItems());
            _keys.Sort(_keys.GetItems());
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void SwapByIndex(int index1, int index2)
        {
            _items.SwapByIndex(index1, index2);
            _keys.SwapByIndex(index1, index2);
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void Swap(T item1, T item2)
        {
            int index1 = _items.IndexOf(item1);
            int index2 = _items.IndexOf(item2);

            this.SwapByIndex(index1, index2);
        }
        public virtual void Reverse()
        {
            _items.Reverse();
            _keys.Reverse();

            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void Reverse(int index, int length)
        {
            _items.Reverse(index, length);
            _keys.Reverse(index, length);
            if (CollectionChanged != null) CollectionChanged(this, new EventArgs());
        }
        public virtual void TrimToSize()
        {
            _items.TrimToSize();
            _keys.TrimToSize();
        }
        public bool TryGetItem(int index, out T item)
        {
            return _items.TryGetItem(index, out item);
        }
        public bool TryGetItem(string key, out T item)
        {
            item = default(T);
            if (!_keys.Contains(key)) return false;
            item = this[key];
            return true;
        }

        #endregion

        #region IEnumerable Members

        //IEnumerator IKeyedCollection<T>.GetEnumerator()
        //{
        //    return this.GetEnumerator();
        //}
        IEnumerator IEnumerable.GetEnumerator()
        {
            return this.GetEnumerator();
        }

        #endregion

    }
}
