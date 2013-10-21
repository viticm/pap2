using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.ComponentModel;
using System.Runtime.Serialization;

namespace Nexus.Windows.Forms
{
  /// <summary>
  /// Represents a single pie slice in a pie chart.
  /// </summary>
  [Serializable, TypeConverter(typeof(PieChartItem.ItemConverter)), DesignTimeVisible(false), DefaultProperty("Text")]
  public partial class PieChartItem : ICloneable, ISerializable, IComparable<PieChartItem>
  {
    #region Constructor
    /// <summary>
    /// Constructs a new instance.
    /// </summary>
    public PieChartItem()
      : this(10)
    {
    }

    /// <summary>
    /// Constructs a new instance.
    /// </summary>
    /// <param name="weight">The weight the item has.</param>
    public PieChartItem(double weight)
      : this(weight, Color.Gray)
    {
    }

    /// <summary>
    /// Constructs a new instance.
    /// </summary>
    /// <param name="weight">The weight the item has.</param>
    /// <param name="color">The fill color of the item.</param>
    public PieChartItem(double weight, Color color)
      : this(weight, color, "")
    {
    }

    /// <summary>
    /// Constructs a new instance.
    /// </summary>
    /// <param name="weight">The weight the item has.</param>
    /// <param name="color">The fill color of the item.</param>
    /// <param name="text">The textual representation of the item.</param>
    public PieChartItem(double weight, Color color, string text)
      : this(weight, color, text, "")
    {
    }

    /// <summary>
    /// Constructs a new instance.
    /// </summary>
    /// <param name="weight">The weight the item has.</param>
    /// <param name="color">The fill color of the item.</param>
    /// <param name="text">The textual representation of the item.</param>
    /// <param name="toolTipText">The tool tip text displayed when the mouse hovers over the item.</param>
    public PieChartItem(double weight, Color color, string text, string toolTipText)
      : this(weight, color, text, toolTipText, 0)
    {
    }

    /// <summary>
    /// Constructs a new instance.
    /// </summary>
    /// <param name="weight">The weight the item has.</param>
    /// <param name="color">The fill color of the item.</param>
    /// <param name="text">The textual representation of the item.</param>
    /// <param name="toolTipText">The tool tip text displayed when the mouse hovers over the item.</param>
    /// <param name="offset">The offset of the item from the center of the pie, in pixels.</param>
    public PieChartItem(double weight, Color color, string text, string toolTipText, float offset)
    {
      this.weight = weight;
      this.color = color;
      this.text = text;
      this.toolTipText = toolTipText;
      this.offset = offset;
    }

    /// <summary>
    /// Constructs a new instance.
    /// </summary>
    /// <param name="info">The serialization information for deserialization.</param>
    /// <param name="context">The context for deserialization.</param>
    protected PieChartItem(SerializationInfo info, StreamingContext context)
    {
      Deserialize(info, context);
    }
    #endregion

    #region Fields
    /// <summary>
    /// The textual representation of the item.
    /// </summary>
    private string text;

    /// <summary>
    /// The tool tip text displayed when the mouse hovers over the item.
    /// </summary>
    private string toolTipText;

    /// <summary>
    /// The weight the item has.
    /// </summary>
    private double weight;

    /// <summary>
    /// The offset of the item from the center of the pie, in pixels.
    /// </summary>
    private float offset;

    /// <summary>
    /// The fill color of the item.
    /// </summary>
    private Color color;

    /// <summary>
    /// A user-defined tag object
    /// </summary>
    private object tag;

    /// <summary>
    /// The control which contains this item.
    /// </summary>
    private PieChart owner;
    #endregion

    #region Properties
    /// <summary>
    /// Gets or sets the textual representation of the item.
    /// </summary>
    [Browsable(true)]
    [DefaultValue("")]
    [Description("The text that will be drawn on the control.")]
    public string Text
    {
      get
      {
        return text;
      }
      set
      {
        if (text != value)
        {
          text = value;
          MarkVisual();
        }
      }
    }

    /// <summary>
    /// Gets or sets the tool tip text displayed when the mouse hovers over the item.
    /// </summary>
    [Browsable(true)]
    [DefaultValue("")]
    [Description("The text that will be placed in a tooltip when this slice is hovered over.")]
    public string ToolTipText
    {
      get
      {
        return toolTipText;
      }
      set
      {
        if (toolTipText != value)
        {
          toolTipText = value;
          MarkVisual();
        }
      }
    }

    /// <summary>
    /// Gets or sets the weight the item has.  This weight is taken over the total weight of all items
    /// to determine how large of an angle this slice has.
    /// </summary>
    [Browsable(true)]
    [DefaultValue(10)]
    [Description("The weight of this slice.")]
    public double Weight
    {
      get
      {
        return weight;
      }
      set
      {
        if (weight != value)
        {
          UpdateWeight(value - weight);
          weight = value;
          MarkStructure();
        }
      }
    }

    /// <summary>
    /// Gets the percent of the control occupied by this item.
    /// </summary>
    [Browsable(false)]
    public double Percent
    {
      get
      {
        // not calculatable if the item is not in a control.
        if (owner == null)
        {
          throw new InvalidOperationException("The item percent cannot be calculated because the item does not belong to a pie chart.");
        }

        if (owner.Items.TotalItemWeight == 0)
          return 0;

        return Weight / owner.Items.TotalItemWeight;
      }
    }

    /// <summary>
    /// Gets or sets the offset of the item from the center of the pie, in pixels.
    /// </summary>
    [Browsable(true)]
    [DefaultValue(0)]
    [Description("The offset of this slice from the center of the pie, in pixels.")]
    public float Offset
    {
      get
      {
        return offset;
      }
      set
      {
        if (this.offset != value)
        {
          offset = value;
          MarkStructure();
        }
      }
    }

    /// <summary>
    /// Gets or sets the fill color of the item.
    /// </summary>
    [Browsable(true)]
    [DefaultValue(typeof(Color), "Color.Gray")]
    [Description("The color of the slice.")]
    public Color Color
    {
      get
      {
        return color;
      }
      set
      {
        if (color != value)
        {
          color = value;
          MarkVisual(true);
        }
      }
    }

    /// <summary>
    /// Gets or sets a user-defined tag value.
    /// </summary>
    [Browsable(false)]
    public object Tag
    {
      get
      {
        return tag;
      }
      set
      {
        tag = value;
      }
    }
    #endregion

    #region Methods
    /// <summary>
    /// Shortcut method to inform the control that the structure has changed.
    /// </summary>
    private void MarkStructure()
    {
      if (owner != null)
        owner.MarkStructuralChange();
    }

    /// <summary>
    /// Shortcut method to inform the control that the control needs refreshing.
    /// </summary>
    private void MarkVisual()
    {
      MarkVisual(false);
    }

    /// <summary>
    /// Shortcut method to inform the control that the control needs refreshing.
    /// </summary>
    /// <param name="recreateGraphics">True if the underlying pens and brushes need to be recreated.</param>
    private void MarkVisual(bool recreateGraphics)
    {
      if (owner != null)
        owner.MarkVisualChange(recreateGraphics);
    }

    /// <summary>
    /// Sets the owning control.
    /// </summary>
    /// <param name="control">The control that contains this item.</param>
    internal void SetOwner(PieChart control)
    {
      if (owner != control && owner != null && control != null)
        throw new InvalidOperationException("A pie chart item cannot belong to two different controls.");

      UpdateWeight(-this.Weight);
      this.owner = control;
      UpdateWeight(this.Weight);
    }

    private void UpdateWeight(double difference)
    {
      if (this.owner != null)
        this.owner.Items.ChangeItemWeight(difference);
    }
    #endregion

    #region Serialization
    /// <summary>
    /// Populates a SerializationInfo with the data needed to serialize the target object.
    /// </summary>
    /// <param name="info">The SerializationInfo to populate with data.</param>
    /// <param name="context">The destination for this serialization.</param>
    void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
    {
      Serialize(info, context);
    }

    /// <summary>
    /// Populates a SerializationInfo with the data needed to serialize the target object.
    /// </summary>
    /// <param name="info">The SerializationInfo to populate with data.</param>
    /// <param name="context">The destination for this serialization.</param>
    protected virtual void Serialize(SerializationInfo info, StreamingContext context)
    {
      if (!string.IsNullOrEmpty(Text))
        info.AddValue("Text", Text);
      if (!string.IsNullOrEmpty(ToolTipText))
        info.AddValue("ToolTipText", ToolTipText);

      info.AddValue("Weight", Weight);
      info.AddValue("Offset", Offset);
      info.AddValue("Color", Color);
    }

    /// <summary>
    /// Loads the state of the item from the specified SerializationInfo. 
    /// </summary>
    /// <param name="info">The SerializationInfo that describes the item.</param>
    /// <param name="context">The state of the stream during deserialization.</param>
    protected virtual void Deserialize(SerializationInfo info, StreamingContext context)
    {
      foreach (SerializationEntry entry in info)
      {
        switch (entry.Name)
        {
          case "Text":
            this.Text = info.GetString(entry.Name);
            break;
          case "ToolTipText":
            this.ToolTipText = info.GetString(entry.Name);
            break;
          case "Weight":
            this.Weight = info.GetDouble(entry.Name);
            break;
          case "Offset":
            this.Offset = info.GetSingle(entry.Name);
            break;
          case "Color":
            this.Color = (Color)entry.Value;
            break;
        }
      }
    }

    /// <summary>
    /// Makes an exact copy of this item.
    /// </summary>
    /// <returns>A new replica of this item.</returns>
    public virtual object Clone()
    {
      return new PieChartItem(Weight, Color, Text, ToolTipText, Offset);
    }
    #endregion

    #region IComparable Members
    public int CompareTo(PieChartItem other)
    {
      return this.Weight.CompareTo(other.Weight);
    }
    #endregion
  }
}
