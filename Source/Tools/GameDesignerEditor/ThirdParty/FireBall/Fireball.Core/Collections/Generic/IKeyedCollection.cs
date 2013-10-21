using System;
namespace Fireball.Collections.Generic
{
    public interface IKeyedCollection<T>
    {
        void Add(string key, T item);
        void AddRange(string[] keys, T[] items);
        void AddRange(KeyedCollection<T> items);
        void Clear();
        LightCollection<T> Collection { get; }
        event EventHandler CollectionChanged;
        event EventHandler CollectionClear;
        bool Contains(T item);
        bool ContainsKey(string key);
        int Count { get; }
        string CreateFreeKey();
        KeyedCollection<T>.Enumerator GetEnumerator();
        T[] GetItems(int startIndex, int finalIndex);
        T[] GetItems(int startIndex);
        T[] GetItems();
        int IndexOf(T item);
        int IndexOfKey(string key);
        void Insert(int index, string key, T item);
        event KeyedCollection<T>.KeyedCollectionAddHandler ItemAdd;
        event KeyedCollection<T>.KeyedCollectionAddRangeHandler ItemAddRange;
        event KeyedCollection<T>.KeyedCollectionMoveHandler ItemMove;
        event KeyedCollection<T>.KeyedCollectionRemoveHandler ItemRemove;
        string[] Keys { get; }
        void Move(T item, int newIndex);
        void MoveByIndex(int index, int newIndex);
        ReadOnlyKeyedCollection<T> ReadOnlyCollection { get; }
        void Remove(T item);
        void RemoveAt(int index);
        void RemoveByKey(string key);
        void Reverse();
        void Reverse(int index, int length);
        void Sort();
        void Swap(T item1, T item2);
        void SwapByIndex(int index1, int index2);
        T this[int index] { get; set; }
        T this[string key] { get; set; }
        void TrimToSize();
        bool TryGetItem(string key, out T item);
        bool TryGetItem(int index, out T item);
    }
}
