using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing.Design;

namespace Nexus.Windows.Forms
{
  public partial class PieChart : Control
  {
    /// <summary>
    /// Stores a collection of PieChartItem objects associated with a PieChart.
    /// </summary>
    [Editor(typeof(ItemCollectionEditor), typeof(UITypeEditor))]
    public class ItemCollection : IList<PieChartItem>, IList
    {
      #region Constructor
      /// <summary>
      /// Constructs a new instance.
      /// </summary>
      /// <param name="container">The PieChart that this collection is associated with.</param>
      internal ItemCollection(PieChart container)
      {
        this.container = container;
        this.items = new List<PieChartItem>();
      }
      #endregion

      #region Fields
      /// <summary>
      /// The PieChart this collection is associated with.
      /// </summary>
      private PieChart container;

      /// <summary>
      /// The list of items stored in this control.
      /// </summary>
      private List<PieChartItem> items;

      /// <summary>
      /// The total weight of all items in this collection.
      /// </summary>
      private double totalWeight = 0;
      #endregion

      #region Properties
      public double TotalItemWeight
      {
        get
        {
          return totalWeight;
        }
      }
      #endregion

      #region Methods
      /// <summary>
      /// Sorts the items in the collection based on item weight.
      /// </summary>
      public void Sort()
      {
        items.Sort();
        container.MarkStructuralChange();
      }

      /// <summary>
      /// Sorts the items in the collection using the provided comparer.
      /// </summary>
      /// <param name="comparer">The comparer used to compare PieChartItems.</param>
      public void Sort(IComparer<PieChartItem> comparer)
      {
        items.Sort(comparer);
        container.MarkStructuralChange();
      }

      internal void ChangeItemWeight(double difference)
      {
        totalWeight += difference;
      }
      #endregion

      #region IEnumerable Members
      /// <summary>
      /// Returns an enumerator that iterates through a collection.
      /// </summary>
      /// <returns>An IEnumerator object that can be used to iterate through the collection.</returns>
      public IEnumerator<PieChartItem> GetEnumerator()
      {
        return items.GetEnumerator();
      }

      /// <summary>
      /// Returns an enumerator that iterates through a collection.
      /// </summary>
      /// <returns>An IEnumerator object that can be used to iterate through the collection.</returns>
      IEnumerator IEnumerable.GetEnumerator()
      {
        return items.GetEnumerator();
      }
      #endregion

      #region ICollection<T> Members
      /// <summary>
      /// Gets the number of elements contained in the collection.
      /// </summary>
      public int Count
      {
        get
        {
          return items.Count;
        }
      }

      /// <summary>
      /// Gets a value indicating whether the collection is read-only.
      /// </summary>
      bool ICollection<PieChartItem>.IsReadOnly
      {
        get
        {
          return false;
        }
      }

      /// <summary>
      /// Adds an item to the collection.
      /// </summary>
      /// <param name="item">The item to add to the collection.</param>
      public void Add(PieChartItem item)
      {
        items.Add(item);
        item.SetOwner(container);
        container.MarkStructuralChange();
      }

      /// <summary>
      /// Removes all items from the collection.
      /// </summary>
      public void Clear()
      {
        foreach (PieChartItem item in items)
          item.SetOwner(null);

        items.Clear();
        container.MarkStructuralChange();
      }

      /// <summary>
      /// Determines whether the collection contains a specific value.
      /// </summary>
      /// <param name="item">The object to locate in the collection.</param>
      /// <returns>True if the item is found in the collection, otherwise false.</returns>
      public bool Contains(PieChartItem item)
      {
        return items.Contains(item);
      }

      /// <summary>
      /// Copies the elements of the collection to an array, starting at a particular array index.
      /// </summary>
      /// <param name="array">The one-dimensional array that is the destination of the elements copied from the collection.
      /// The array must have zero-based indexing.</param>
      /// <param name="index">The zero-based index in array at which copying begins.</param>
      void ICollection<PieChartItem>.CopyTo(PieChartItem[] array, int index)
      {
        items.CopyTo(array, index);
      }

      /// <summary>
      /// Removes the first occurrence of a specific object from the collection.
      /// </summary>
      /// <param name="item">The object to remove from the collection.</param>
      /// <returns>True if the item was successfully removed from the colleection, otherwise false.  This method
      /// also returns false if the item is not found in the original collection.</returns>
      public bool Remove(PieChartItem item)
      {
        item.SetOwner(null);
        bool rval = items.Remove(item);
        container.MarkStructuralChange();
        return rval;
      }
      #endregion

      #region ICollection Members
      /// <summary>
      /// Copies the elements of the collection to an array, starting at a particular array index.
      /// </summary>
      /// <param name="array">The one-dimensional array that is the destination of the elements copied from the collection.
      /// The array must have zero-based indexing.</param>
      /// <param name="index">The zero-based index in array at which copying begins</param>
      void ICollection.CopyTo(Array array, int index)
      {
        ((ICollection)items).CopyTo(array, index);
      }

      /// <summary>
      /// Gets an object that can be used to synchronize access to the collection.
      /// </summary>
      object ICollection.SyncRoot
      {
        get
        {
          return this;
        }
      }

      /// <summary>
      /// Gets a value indicating whether access to the collection is synchronized (thread safe).
      /// </summary>
      bool ICollection.IsSynchronized
      {
        get
        {
          return false;
        }
      }
      #endregion

      #region IList<T> Members
      /// <summary>
      /// Gets or sets the element at the specified index.
      /// </summary>
      /// <param name="index">The zero-based index of the element to get or set.</param>
      /// <returns>The element at the specified index.</returns>
      public PieChartItem this[int index]
      {
        get
        {
          return items[index];
        }
        set
        {
          if (items[index] != value)
          {
            items[index].SetOwner(null);
            items[index] = value;
            items[index].SetOwner(container);
            container.MarkStructuralChange();
          }
        }
      }

      /// <summary>
      /// Determines the index of a specific item in the list.
      /// </summary>
      /// <param name="item">The object to locate in the list.</param>
      /// <returns>The index of the item if found in the list, otherwise -1.</returns>
      public int IndexOf(PieChartItem item)
      {
        return items.IndexOf(item);
      }

      /// <summary>
      /// Inserts an item to the list at the specified index.
      /// </summary>
      /// <param name="index">The zero-based index at which item should be inserted.</param>
      /// <param name="item">The object to insert into the list.</param>
      public void Insert(int index, PieChartItem item)
      {
        item.SetOwner(container);
        items.Insert(index, item);
        container.MarkStructuralChange();
      }

      /// <summary>
      /// Removes the item at the specified index.
      /// </summary>
      /// <param name="index">The zero-based index of the item to remove.</param>
      public void RemoveAt(int index)
      {
        items[index].SetOwner(null);
        items.RemoveAt(index);
        container.MarkStructuralChange();
      }
      #endregion

      #region IList Members
      /// <summary>
      /// Adds an item to the list.
      /// </summary>
      /// <param name="obj">The item to add to the list.</param>
      /// <returns>The position at which the item was inserted.</returns>
      int IList.Add(object obj)
      {
        this.Add((PieChartItem)obj);
        return this.Count - 1;
      }

      /// <summary>
      /// Determines whether the list contains a specific value.
      /// </summary>
      /// <param name="obj">The object to locate in the list.</param>
      /// <returns>True if an instance of the item was found in the list, otherwise false.</returns>
      bool IList.Contains(object obj)
      {
        return this.Contains((PieChartItem)obj);
      }

      /// <summary>
      /// Determines the index of a specific item in the list.
      /// </summary>
      /// <param name="obj">The object to locate in the list.</param>
      /// <returns>The index of the item if found in the list, otherwise -1.</returns>
      int IList.IndexOf(object obj)
      {
        return this.IndexOf((PieChartItem)obj);
      }

      /// <summary>
      /// Inserts an item to the list at the specified index.
      /// </summary>
      /// <param name="index">The zero-based index at which item should be inserted.</param>
      /// <param name="obj">The object to insert into the list.</param>
      void IList.Insert(int index, object obj)
      {
        this.Insert(index, (PieChartItem)obj);
      }

      /// <summary>
      /// Removes the first occurrence of a specific object from the collection.
      /// </summary>
      /// <param name="item">The object to remove from the collection.</param>
      void IList.Remove(object obj)
      {
        this.Remove((PieChartItem)obj);
      }

      /// <summary>
      /// Gets or sets the element at the specified index.
      /// </summary>
      /// <param name="index">The zero-based index of the element to get or set.</param>
      /// <returns>The element at the specified index.</returns>
      object IList.this[int index]
      {
        get
        {
          return this[index];
        }
        set
        {
          this[index] = (PieChartItem)value;
        }
      }

      /// <summary>
      /// Gets a value indicating whether the list is read-only.
      /// </summary>
      bool IList.IsReadOnly
      {
        get
        {
          return false;
        }
      }

      /// <summary>
      /// Gets a value indicating whether the list has a fixed size.
      /// </summary>
      bool IList.IsFixedSize
      {
        get
        {
          return false;
        }
      }
      #endregion
    }
  }
}