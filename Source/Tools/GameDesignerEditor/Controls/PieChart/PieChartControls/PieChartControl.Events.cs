using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace Nexus.Windows.Forms
{
  public delegate void PieChartItemEventHandler(object sender, PieChart.PieChartItemEventArgs e);
  public delegate void PieChartItemFocusEventHandler(object sender, PieChart.PieChartItemFocusEventArgs e);

  public partial class PieChart : Control
  {
    #region Events
    /// <summary>
    /// Fired when an item is clicked.
    /// </summary>
    public event PieChartItemEventHandler ItemClicked;

    /// <summary>
    /// Fired when an item is double-clicked.
    /// </summary>
    public event PieChartItemEventHandler ItemDoubleClicked;
    
    /// <summary>
    /// Fired when the focus is changing from one item to another.
    /// </summary>
    public event PieChartItemFocusEventHandler ItemFocusChanging;

    /// <summary>
    /// Fired when the focus has changed to another item.
    /// </summary>
    public event EventHandler ItemFocusChanged;

    /// <summary>
    /// Fired when the AutoSizePie property has changed.
    /// </summary>
    public event EventHandler AutoSizePieChanged;

    /// <summary>
    /// Fired when the radius of the control has changed.
    /// </summary>
    public event EventHandler RadiusChanged;

    /// <summary>
    /// Fired when the inclination of the control has changed.
    /// </summary>
    public event EventHandler InclinationChanged;

    /// <summary>
    /// Fired when the rotation has changed.
    /// </summary>
    public event EventHandler RotationChanged;

    /// <summary>
    /// Fired when the thickness of the control has changed.
    /// </summary>
    public event EventHandler ThicknessChanged;

    /// <summary>
    /// Fired when the ShowEdges property has changed.
    /// </summary>
    public event EventHandler ShowEdgesChanged;

    /// <summary>
    /// Fired when the TextDisplayMode property has changed.
    /// </summary>
    public event EventHandler TextDisplayModeChanged;

    /// <summary>
    /// Fired when the ShowToolTips property has changed.
    /// </summary>
    public event EventHandler ShowToolTipsChanged;

    /// <summary>
    /// Called to fire an ItemClicked event.
    /// </summary>
    /// <param name="item">The event arguments.</param>
    private void FireItemClicked(PieChartItem item)
    {
      if (ItemClicked != null)
        ItemClicked(this, new PieChartItemEventArgs(item));
    }

    /// <summary>
    /// Called to fire an ItemDoubleClicked event.
    /// </summary>
    /// <param name="item">The event arguments.</param>
    private void FireItemDoubleClicked(PieChartItem item)
    {
      if (ItemDoubleClicked != null)
        ItemDoubleClicked(this, new PieChartItemEventArgs(item));
    }

    /// <summary>
    /// Called to fire an ItemFocusChanging event.
    /// </summary>
    /// <param name="oldItem">The item that was previously focused.</param>
    /// <param name="newItem">The item that is gaining focus.</param>
    private void FireItemFocusChanging(PieChartItem oldItem, PieChartItem newItem)
    {
      if (ItemFocusChanging != null)
        ItemFocusChanging(this, new PieChartItemFocusEventArgs(oldItem, newItem));
    }

    /// <summary>
    /// Called to fire an ItemFocusChanged event.
    /// </summary>
    /// <param name="item">The event arguments.</param>
    private void FireItemFocusChanged()
    {
      if (ItemFocusChanged != null)
        ItemFocusChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Called to fire an AutoSizePieChanged event.
    /// </summary>
    internal void FireAutoSizePieChanged()
    {
      if (AutoSizePieChanged != null)
        AutoSizePieChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Called to fire an RadiusChanged event.
    /// </summary>
    internal void FireRadiusChanged()
    {
      if (RadiusChanged != null)
        RadiusChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Called to fire an InclinationChanged event.
    /// </summary>
    internal void FireInclinationChanged()
    {
      if (InclinationChanged != null)
        InclinationChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Called to fire an RotationChanged event.
    /// </summary>
    internal void FireRotationChanged()
    {
      if (RotationChanged != null)
        RotationChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Called to fire an ThicknessChanged event.
    /// </summary>
    internal void FireThicknessChanged()
    {
      if (ThicknessChanged != null)
        ThicknessChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Called to fire an ShowEdgesChanged event.
    /// </summary>
    internal void FireShowEdgesChanged()
    {
      if (ShowEdgesChanged != null)
        ShowEdgesChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Called to fire an TextDisplayModeChanged event.
    /// </summary>
    internal void FireTextDisplayModeChanged()
    {
      if (TextDisplayModeChanged != null)
        TextDisplayModeChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Called to fire an ShowToolTipsChanged event.
    /// </summary>
    internal void FireShowToolTipsChanged()
    {
      if (ShowToolTipsChanged != null)
        ShowToolTipsChanged(this, EventArgs.Empty);
    }
    #endregion

    #region PieChartItemEventArgs
    /// <summary>
    /// Stores a PieChartItem that is involved with an event.
    /// </summary>
    public class PieChartItemEventArgs : EventArgs
    {
      #region Constructor
      /// <summary>
      /// Constructs a new instance.
      /// </summary>
      /// <param name="item">The item involved with an event.</param>
      public PieChartItemEventArgs(PieChartItem item)
      {
        this.item = item;
      }
      #endregion

      #region Fields
      /// <summary>
      /// The item involved with the event.
      /// </summary>
      private PieChartItem item;
      #endregion

      #region Methods
      /// <summary>
      /// Gets the item involved with the event.
      /// </summary>
      public PieChartItem Item
      {
        get
        {
          return item;
        }
      }
      #endregion
    }
    #endregion

    #region PieChartFocusEventArgs
    /// <summary>
    /// Stores the PieChartItems that are involved with an focus changing event.
    /// </summary>
    public class PieChartItemFocusEventArgs : EventArgs
    {
      #region Constructor
      /// <summary>
      /// Constructs a new instance.
      /// </summary>
      /// <param name="oldItem">The item that is losing focus.</param>
      /// <param name="newItem">The item that is gaining focus.</param>
      public PieChartItemFocusEventArgs(PieChartItem oldItem, PieChartItem newItem)
      {
        this.oldItem = oldItem;
        this.newItem = newItem;
      }
      #endregion

      #region Fields
      /// <summary>
      /// The item that is losing focus.
      /// </summary>
      private PieChartItem oldItem;

      /// <summary>
      /// The item that is gaining focus.
      /// </summary>
      private PieChartItem newItem;
      #endregion

      #region Methods
      /// <summary>
      /// Gets the item that is losing focus.
      /// </summary>
      public PieChartItem OldItem
      {
        get
        {
          return oldItem;
        }
      }

      /// <summary>
      /// Gets the item that is gaining focus.
      /// </summary>
      public PieChartItem NewItem
      {
        get
        {
          return newItem;
        }
      }
      #endregion
    }
    #endregion
  }
}