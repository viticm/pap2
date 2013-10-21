using System;
namespace Fireball.Collections.Generic
{
    public interface ILightCollection<T>
    {
        int Add(T item, bool sort);
        int Add(T item);
        void AddRange(ILightCollection<T> items);
        void AddRange(T[] items);
        int Capacity { get; set; }
        void Clear();
        //LightCollection<T> Clone();
        event EventHandler CollectionClear;
        bool Contains(T item);
        void CopyTo(Array array, int index);
        int Count { get; }
        T Find(Predicate<T> match);
        LightCollection<T>.Enumerator GetEnumerator();
        T[] GetItems(int startIndex, int finalIndex);
        T[] GetItems(int startIndex);
        T[] GetItems();
        int IndexOf(T item, int index, int count);
        int IndexOf(T item, int index);
        int IndexOf(T item);
        void Insert(int index, T item);
        void InsertRange(int index, ILightCollection<T> items);
        void InsertRange(int index, T[] items);
        event LightCollection<T>.LightCollectionAddHandler ItemAdd;
        event LightCollection<T>.LightCollectionAddRangeHandler ItemAddRange;
        event LightCollection<T>.LightCollectionMoveHandler ItemMove;
        event LightCollection<T>.LightCollectionRemoveHandler ItemRemove;
        void Move(T item, int newIndex);
        void MoveByIndex(int index, int newIndex);
        ReadOnlyCollection<T> ReadOnlyCollection { get; }
        bool Remove(T item);
        void RemoveAt(int index);
        void Reverse(int index, int length);
        void Reverse();
        void Sort();
        //void Sort(int index, int length);
        //void Sort(Array keys);
        void Swap(T item1, T item2);
        void SwapByIndex(int index1, int index2);
        T this[int index] { get; set; }
        void TrimToSize();
        bool TryGetItem(int index, out T item);
    }
}
