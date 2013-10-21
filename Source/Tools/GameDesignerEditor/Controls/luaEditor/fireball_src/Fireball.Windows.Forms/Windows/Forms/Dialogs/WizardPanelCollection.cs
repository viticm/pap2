using System;
using System.Collections.Generic;
using System.Text;
using Fireball.Collections.Generic;

namespace Fireball.Windows.Forms.Dialogs
{
	public class WizardPanelCollection : IKeyedCollection<WizardPanel>
	{
		KeyedCollection<WizardPanel> m_Panels = null;

		internal delegate void AddPanelHandler(WizardPanel panel);
		internal delegate void AddPanelRangeHandler(WizardPanel[] panel);
		internal delegate void RemovePanelHandler(WizardPanel panel);
		internal delegate void InsertPanelHandler(WizardPanel panel, int index);

		internal event AddPanelHandler AddPanel;
		internal event AddPanelRangeHandler AddPanelRange;
		internal event RemovePanelHandler RemovePanel;
		internal event InsertPanelHandler InsertPanel;

		int m_Current = 0;

		public WizardPanelCollection()
		{
			m_Panels = new KeyedCollection<WizardPanel>();
		}

		#region IKeyedCollection<WizardPanel> Members

		public int Count
		{
			get
			{
				return m_Panels.Count;
			}
		}

		public WizardPanel this[int index]
		{
			get
			{
				return m_Panels[index];
			}
			set
			{
				m_Panels[index] = value;
			}
		}

		public WizardPanel this[string key]
		{
			get
			{
				return m_Panels[key];
			}
			set
			{
				m_Panels[key] = value;
			}
		}

		public string[] Keys
		{
			get {
				return m_Panels.Keys;
			}
		}

		public void Add(string key, WizardPanel item)
		{
			m_Panels.Add(key, item);
			if (AddPanel != null)
				AddPanel(item);
		}

		public void AddRange(string[] keys, WizardPanel[] items)
		{
			m_Panels.AddRange(keys, items);
			if (AddPanelRange != null)
				AddPanelRange(items);
		}

		public void Clear()
		{
			m_Panels.Clear();
		}

		public void Insert(int index, string key, WizardPanel item)
		{
			m_Panels.Insert(index, key, item);
			if (InsertPanel != null)
				InsertPanel(item,index);
		}

		public void RemoveAt(int index)
		{
			WizardPanel panel = m_Panels[index];

			m_Panels.Remove(panel);

			if (RemovePanel != null)
				RemovePanel(panel);
		}

		public void Remove(string key)
		{
			WizardPanel panel = m_Panels[key];

			m_Panels.Remove(panel);

			if (RemovePanel != null)
				RemovePanel(panel);
		}

		public void Remove(WizardPanel item)
		{

			m_Panels.Remove(item);

			if (RemovePanel != null)
				RemovePanel(item);
		}

        public bool ContainsKey(string key)
		{
			return m_Panels.ContainsKey(key);
		}

		public bool Contains(WizardPanel item)
		{
			return m_Panels.Contains(item);
		}

        public int IndexOfKey(string key)
		{
			return m_Panels.IndexOfKey(key);
		}

		public int IndexOf(WizardPanel item)
		{
			return m_Panels.IndexOf(item);
		}

		public WizardPanel[] GetItems()
		{
			return m_Panels.GetItems();
		}

		public WizardPanel[] GetItems(int startIndex)
		{
			return m_Panels.GetItems();
		}

		public WizardPanel[] GetItems(int startIndex, int finalIndex)
		{
			return m_Panels.GetItems(startIndex, finalIndex);
		}

		public void Sort()
		{
			m_Panels.Sort();
		}

		public WizardPanel Current
		{
			get
			{
				if (m_Panels.Count == 0)
					return null;

				return m_Panels[m_Current];
			}
            set
            {
                int index = m_Panels.IndexOf(value);
                if (index>=0)
                {
                    m_Current = index;
                    m_Panels[index].BringToFront();
                }
            }
		}

		public void Next()
		{
			if (m_Current >= m_Panels.Count - 1)
			{
				return;
			}
			m_Current++;

			this.Current.BringToFront();
		}

		public void Back()
		{
			if (m_Current <= 0)
			{
				return;
			}
			m_Current--;
			this.Current.BringToFront();
		}

		#endregion

        #region IKeyedCollection<WizardPanel> Members

        public string CreateFreeKey()
        {
            return m_Panels.CreateFreeKey();
        }

        public void Move(WizardPanel item, int newIndex)
        {
            m_Panels.Move(item, newIndex);
        }

        public void MoveByIndex(int index, int newIndex)
        {
            m_Panels.MoveByIndex(index, newIndex);
        }

        public void Reverse(int index, int length)
        {
            m_Panels.Reverse(index, length);
        }

        public void Reverse()
        {
            m_Panels.Reverse();
        }

        public void Swap(WizardPanel item1, WizardPanel item2)
        {
            m_Panels.Swap(item1, item2);
        }

        public void SwapByIndex(int index1, int index2)
        {
            m_Panels.SwapByIndex(index1, index2);
        }

        #endregion

        #region IKeyedCollection<WizardPanel> Members

        public bool TryGetItem(string key, out WizardPanel item)
        {
            return m_Panels.TryGetItem(key, out item);
        }

        public bool TryGetItem(int index, out WizardPanel item)
        {
            return m_Panels.TryGetItem(index, out item);
        }

        #endregion

        #region IKeyedCollection<WizardPanel> Membri di


        public void RemoveByKey(string key)
        {
            m_Panels.RemoveByKey(key);
        }


        public KeyedCollection<WizardPanel>.Enumerator GetEnumerator()
        {
            return m_Panels.GetEnumerator();
        }

        #endregion

        #region IKeyedCollection<WizardPanel> Members


        public void AddRange(KeyedCollection<WizardPanel> items)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public LightCollection<WizardPanel> Collection
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public event EventHandler CollectionChanged;

        public event EventHandler CollectionClear;

        public event KeyedCollection<WizardPanel>.KeyedCollectionAddHandler ItemAdd;

        public event KeyedCollection<WizardPanel>.KeyedCollectionAddRangeHandler ItemAddRange;

        public event KeyedCollection<WizardPanel>.KeyedCollectionMoveHandler ItemMove;

        public event KeyedCollection<WizardPanel>.KeyedCollectionRemoveHandler ItemRemove;

        public ReadOnlyKeyedCollection<WizardPanel> ReadOnlyCollection
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public void TrimToSize()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion
    }
}
