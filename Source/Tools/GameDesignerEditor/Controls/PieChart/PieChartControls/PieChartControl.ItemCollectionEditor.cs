using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Drawing.Design;
using System.ComponentModel.Design;

namespace Nexus.Windows.Forms
{
  public partial class PieChart : Control
  {
    /// <summary>
    /// Class used to edit an ItemCollection in design time.
    /// </summary>
    /// <remarks>
    /// The designer uses the default CollectionEditor implementation, and overrides
    /// the display text for items in the item list.
    /// </remarks>
    internal class ItemCollectionEditor : CollectionEditor
    {
      #region Constructor
      public ItemCollectionEditor(Type type)
        : base(type)
      {
      }
      #endregion

      #region Overrides
      /// <summary>
      /// Gets the display text for a PieChartItem in the collection list.
      /// </summary>
      /// <param name="value">The PieChartItem to get the display text for.</param>
      /// <returns>The string that will be displayed for the PieChartItem.</returns>
      protected override string GetDisplayText(object value)
      {
        if (value is PieChartItem)
        {
          PieChartItem item = (PieChartItem)value;
          if (!string.IsNullOrEmpty(item.Text))
            return string.Format("{0} [weight {1:f3}]", item.Text, item.Weight);
          else
            return string.Format("[weight {0:f3}]", item.Weight);
        }
        return value.GetType().Name;
      }
      #endregion
    }
  }
}