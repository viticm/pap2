using System;
using System.Collections.Generic;
using System.Text;

namespace Fireball.Collections.Generic
{
    public class FastQueue<T>
    {
        internal T[] _items;
        private int _count = 0;
        private int _capacity = 0;

        /// <summary>
        /// Initializes a new instance of the <see cref="T:FastQueue&lt;T&gt;"/> class.
        /// </summary>
        public FastQueue()
        {
            _items = new T[4];
        }

        /// <summary>
        /// Gets the count.
        /// </summary>
        /// <value>The count.</value>
        public int Count
        {
            get
            {
                return _count;
            }
        }

        /// <summary>
        /// Enqueues the specified item.
        /// </summary>
        /// <param name="item">The item.</param>
        public virtual void Enqueue(T item)
        {
            _count++;
            EnsureCapacity(_count);
            _items[_count - 1] = item;
        }
        /// <summary>
        /// Peeks this instance.
        /// </summary>
        /// <returns></returns>
        public virtual T Peek()
        {
            if (this._capacity == 0)
            {
                throw new InvalidOperationException("Empty Queue");
            }
            return this._items[_count - 1];
        }
        /// <summary>
        /// Dequeues this instance.
        /// </summary>
        /// <returns></returns>
        public virtual T Dequeue()
        {
            T item = this._items[_count - 1];

            T[] items = new T[_capacity];

            Array.Copy(_items, items, _count - 1);

            _items = items;

            return item;
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

                _capacity = newCapacity;

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
                        Array.Copy(this._items, 0, newArray, 0, this._count-1);
                    }

                    this._items = newArray;
                }
                else
                {
                    this._items = new T[4];
                }
            }
        }
    }
}
