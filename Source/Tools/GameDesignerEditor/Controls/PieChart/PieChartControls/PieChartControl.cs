using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Drawing.Design;
using System.ComponentModel.Design;
using System.Drawing.Imaging;
using System.Drawing.Printing;

namespace Nexus.Windows.Forms
{
  /// <summary>
  /// A control for displaying pie charts.
  /// </summary>
  [ToolboxBitmap(typeof(PieChart))]
  public partial class PieChart : Control
  {
    #region Constructor
    /// <summary>
    /// Constructs a new instance of a PieChart.
    /// </summary>
    public PieChart()
    {
      this.SetStyle(ControlStyles.UserPaint, true);
      this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      this.SetStyle(ControlStyles.DoubleBuffer, true);
      this.SetStyle(ControlStyles.ResizeRedraw, true);

      this.items = new ItemCollection(this);
      this.style = new PieChartStyle(this);
      this.itemStyle = new PieChartItemStyle(this);
      this.focusedItemStyle = new PieChartItemStyle(this);
      this.toolTip = new ToolTip();
    }
    #endregion

    #region Fields
    /// <summary>
    /// The collection which holds PieChartItems
    /// </summary>
    private ItemCollection items;

    /// <summary>
    /// The collection of styles that apply to this PieChart.
    /// </summary>
    private PieChartStyle style;

    /// <summary>
    /// The style for default (non-focused) items.
    /// </summary>
    private PieChartItemStyle itemStyle;

    /// <summary>
    /// The style for focused items.
    /// </summary>
    private PieChartItemStyle focusedItemStyle;

    /// <summary>
    /// The PieChartItem that has mouse focus.
    /// </summary>
    private PieChartItem focusedItem;

    /// <summary>
    /// True if the structure of the pie has changed and the layout needs to be recalculated.
    /// </summary>
    private bool isStructureChanged = true;

    /// <summary>
    /// True if the pie needs to be redrawn.
    /// </summary>
    private bool isVisualChanged = true;

    /// <summary>
    /// True if the underlying pens and brushes need to be recreated when the control is redrawn.
    /// </summary>
    private bool recreateGraphics = true;

    /// <summary>
    /// A reference counter for the number of change transactions that have been begun and not ended.
    /// </summary>
    private int transactionRef = 0;

    /// <summary>
    /// A list of DrawingMetrics objects that store calculated drawing data about each pie slice.
    /// </summary>
    private List<DrawingMetrics> drawingMetrics = new List<DrawingMetrics>();

    /// <summary>
    /// The ToolTip control that is used when hovering over pie slices.
    /// </summary>
    private ToolTip toolTip;

    /// <summary>
    /// The default ToolTip delay, which is stored when the delay is overwritten by this control.
    /// </summary>
    private int toolTipDefaultDelay;
    #endregion

    #region Properties
    /// <summary>
    /// The collection which holds PieChartItems
    /// </summary>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
    public ItemCollection Items
    {
      get
      {
        return items;
      }
    }

    /// <summary>
    /// The collection of styles that apply to this PieChart.
    /// </summary>
    [Browsable(false)]
    public PieChartStyle Style
    {
      get
      {
        return style;
      }
    }

    /// <summary>
    /// The collection of styles that apply to this PieChart.
    /// </summary>
    [Browsable(false)]
    public PieChartItemStyle ItemStyle
    {
      get
      {
        return itemStyle;
      }
    }

    /// <summary>
    /// The collection of styles that apply to this PieChart.
    /// </summary>
    [Browsable(false)]
    public PieChartItemStyle FocusedItemStyle
    {
      get
      {
        return focusedItemStyle;
      }
    }

    /// <summary>
    /// The PieChartItem that has mouse focus.
    /// </summary>
    [Browsable(false)]
    public PieChartItem FocusedItem
    {
      get
      {
        return focusedItem;
      }
    }

    /// <summary>
    /// Gets or sets the rotation of the pie chart.  This is represented in radians, with positive values indicating
    /// a rotation in the clockwise direction.
    /// </summary>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DefaultValue(0F)]
    [Description("The rotation around the center of the control, in radians.")]
    public float Rotation
    {
      get
      {
        return Style.Rotation;
      }
      set
      {
        Style.Rotation = value;
      }
    }

    /// <summary>
    /// Gets or sets the inclination of the control.  This is represented in radians, where an angle of 0
    /// represents looking at the edge of the control and an angle of pi represents looking
    /// straight down at the top of the pie.
    /// </summary>
    /// <remarks>
    /// The angle must be greater than 0 and less than or equal to pi radians.
    /// </remarks>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DefaultValue((float)(Math.PI / 6))]
    [Description("The inclination of the control, in radians.")]
    public float Inclination
    {
      get
      {
        return Style.Inclination;
      }
      set
      {
        Style.Inclination = value;
      }
    }

    /// <summary>
    /// Gets or sets thickness of the pie, in pixels.
    /// </summary>
    /// <remarks>This represents the three-dimensional thickness of the control.
    /// The actual visual thickness of the control depends on the inclination.  To determine what the apparent
    /// thickness of the control is, use the Style.VisualHeight property.  The thickness must be greater than or equal to 0.</remarks>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DefaultValue(10)]
    [Description("The thickness of the pie, in pixels.")]
    public float Thickness
    {
      get
      {
        return Style.Thickness;
      }
      set
      {
        Style.Thickness = value;
      }
    }

    /// <summary>
    /// Gets or sets radius of the control, in pixels.  If AutoSizePie is set to true, this value will be ignored.
    /// </summary>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DefaultValue(200)]
    [Description("The radius of the pie, in pixels.")]
    public float Radius
    {
      get
      {
        return Style.Radius;
      }
      set
      {
        Style.Radius = value;
      }
    }

    /// <summary>
    /// Gets or sets if the pie should be sized to fit the control.  If this property is true,
    /// the Radius property is ignored.
    /// </summary>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DefaultValue(false)]
    [Description("True if the control should size the pie to fit the control.")]
    public bool AutoSizePie
    {
      get
      {
        return Style.AutoSizePie;
      }
      set
      {
        Style.AutoSizePie = value;
      }
    }

    /// <summary>
    /// Gets or sets if edges should be drawn on pie slices.  If false, edges are not drawn.
    /// </summary>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DefaultValue(true)]
    [Description("True if the edges of pie slices should be drawn.")]
    public bool ShowEdges
    {
      get
      {
        return Style.ShowEdges;
      }
      set
      {
        Style.ShowEdges = value;
      }
    }

    /// <summary>
    /// Gets or sets if text should be drawn on pie slices.
    /// </summary>
    /// <remarks>
    /// This can have one of three values.  If TextDisplayTypes.Always, the text is always drawn.
    /// If TextDisplayTypes.FitOnly, the text is drawn only if it fits in the wedge.  If TextDisplayTypes.Never,
    /// the text is never drawn.
    /// </remarks>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DefaultValue(PieChart.TextDisplayTypes.FitOnly)]
    [Description("TextDisplayModeTypes.Always if text should always be drawn, TextDisplayModeTypes.Never if text should never be drawn, or TextDisplayModeTypes.FitOnly if text should be drawn only when it fits in the pie slice.")]
    public TextDisplayTypes TextDisplayMode
    {
      get
      {
        return Style.TextDisplayMode;
      }
      set
      {
        Style.TextDisplayMode = value;
      }
    }

    /// <summary>
    /// Gets or sets if tool tips should be shown when the mouse hovers over pie slices.  If false, tool tips are not shown.
    /// </summary>
    [Browsable(true)]
    [Category("Pie Chart")]
    [DefaultValue(true)]
    [Description("True if tool tips for pie slices should be drawn.")]
    public bool ShowToolTips
    {
      get
      {
        return Style.ShowToolTips;
      }
      set
      {
        Style.ShowToolTips = value;
      }
    }

    /// <summary>
    /// Determines how transparent surfaces of pie slices are.  A SurfaceAlphaTransparency of 1 means the surfaces
    /// are completely opaque, while a SurfaceAlphaTransparency of 0 means the surfaces are completely transparent.
    /// </summary>
    //[Browsable(true)]
    //[Category("Pie Chart")]
    //[DefaultValue(1F)]
    //[Description("The alpha transparency multiplier that should be applied to surface colors (between 0 and 1).")]
    //public float SurfaceAlphaTransparency
    //{
    //  get
    //  {
    //    return Style.SurfaceAlphaTransparency;
    //  }
    //  set
    //  {
    //    Style.SurfaceAlphaTransparency = value;
    //  }
    //}
    #endregion

    #region Methods
    public Size GetChartSize(Padding padding)
    {
      float maxOffset = GetMaximumItemOffset();
      int width = (int)(2 * (Radius + maxOffset) + padding.Horizontal);
      int height = (int)(2 * (Radius * Style.HeightWidthRatio + maxOffset) + Style.VisualThickness + padding.Vertical);
      return new Size(width, height);
    }

    /// <summary>
    /// Gets the maximum offset of all PieChartItems in the Items collection.
    /// </summary>
    /// <returns>The maximum offset of all items.</returns>
    private float GetMaximumItemOffset()
    {
      float max = 0;
      foreach (PieChartItem item in Items)
        max = Math.Max(max, item.Offset);
      return max;
    }

    /// <summary>
    /// Calculates the radius that will be used for autosizing the pie to fit the control.
    /// </summary>
    /// <returns>The radius that will fit the pie in the control bounds.</returns>
    private float GetAutoSizeRadius(Rectangle bounds, Padding padding)
    {
      float maxOffset = GetMaximumItemOffset();
      float widthHeightRatio = Style.HeightWidthRatio;
      float width = (bounds.Width - padding.Horizontal) / 2;
      float height = (bounds.Height - padding.Vertical - Style.VisualThickness) / 2;

      float radius = Math.Max(PieChartStyle.AutoSizeMinimumRadius, Math.Min(width - maxOffset, (height - maxOffset) / widthHeightRatio));

      return radius;
    }

    /// <summary>
    /// Constructs the array of DrawingMetrics, which store drawing information about each pie slice.
    /// </summary>
    private List<DrawingMetrics> ConstructDrawingMetrics(Rectangle bounds, Padding padding)
    {
      List<DrawingMetrics> results = new List<DrawingMetrics>();
      try
      {
        // increment the transaction reference counter so that any modifications in this method don't lead to a recursive redrawing of the control.
        transactionRef++;

        if (Items.TotalItemWeight == 0)
          return results;

        if (Style.AutoSizePie)
          Style.RadiusInternal = GetAutoSizeRadius(bounds, padding);

        float angleUsed = Style.Rotation;
        for (int i = 0; i < Items.Count; i++)
        {
          DrawingMetrics dm = new DrawingMetrics(this, Items[i], bounds, angleUsed, (float)(Items[i].Percent * Math.PI * 2));
          results.Add(dm);
          angleUsed += (float)dm.SweepAngle;
        }

        // sort the drawing metrics in the order they should be drawn
        results.Sort();
      }
      finally
      {
        // end our transaction
        transactionRef--;
      }

      return results;
    }

    /// <summary>
    /// Recreates all of the pens and brushes used by the DrawingMetrics that have been constructed.
    /// </summary>
    private void RecreateGraphics(List<DrawingMetrics> drawingMetrics, Rectangle bounds)
    {
      for (int i = 0; i < drawingMetrics.Count; i++)
      {
        drawingMetrics[i].DrawingBounds = bounds;
        drawingMetrics[i].RecreateGraphics();
      }
    }

    /// <summary>
    /// Destroys all of the DrawingMetrics currently in the array by releasing all of their resources.
    /// </summary>
    private void DestructDrawingMetrics(List<DrawingMetrics> drawingMetrics)
    {
      foreach (DrawingMetrics metric in drawingMetrics)
        metric.DestroyResources();

      drawingMetrics.Clear();
    }

    /// <summary>
    /// Set the currently focused PieChartItem.
    /// </summary>
    /// <param name="item">The item that currently has mouse focus.</param>
    private void SetFocusedItem(PieChartItem item)
    {
      if (item != this.FocusedItem)
      {
        FireItemFocusChanging(this.FocusedItem, item);
        this.focusedItem = item;
        FireItemFocusChanged();

        MarkVisualChange(true);
      }

      // check to see if the item has a tool tip and if it should be displayed
      if (this.FocusedItem != null && this.FocusedItem.ToolTipText != null && this.Style.ShowToolTips)
      {
        toolTip.SetToolTip(this, this.FocusedItem.ToolTipText);
      }
      else
      {
        toolTip.RemoveAll();
      }
    }

    /// <summary>
    /// Performs a hit test to see which PieChartItem is under the current mouse position.
    /// </summary>
    /// <param name="screenPoint">The untranslated point given by the mouse move notification.</param>
    /// <returns>The DrawingMetrics of the item under the point, or null if no item is there.</returns>
    private DrawingMetrics HitTest(PointF controlPoint)
    {
      if (drawingMetrics.Count == 0)
        return null;

      // translated the point so the origin is at the center of the pie
      PointF transPoint = new PointF(controlPoint.X - Width / 2, controlPoint.Y - (Height + Style.VisualThickness) / 2);

      // if a single item is both the frontmost (bottom) and rearmost (top) item in the display, special hit testing is needed
      bool itemBottomTop = drawingMetrics[0].IsBottomItem && drawingMetrics[0].IsTopItem;
      if (itemBottomTop)
      {
        // check to see if the top surface or exterior surface of the control is hit, but not the interior surface
        if (drawingMetrics[0].TopRegion.IsVisible(transPoint) || drawingMetrics[0].ExteriorRegion.IsVisible(transPoint))
          return drawingMetrics[0];
      }

      // check surfaces of all controls in order, returning the first hit
      for (int i = drawingMetrics.Count - 1; i >= 0; i--)
      {
        if (drawingMetrics[i].VisibleRegion.IsVisible(transPoint))
          return drawingMetrics[i];
      }

      return null;
    }

    /// <summary>
    /// Handles the MouseEnter event.
    /// </summary>
    /// <param name="e">The event arguments.</param>
    protected override void OnMouseEnter(EventArgs e)
    {
      base.OnMouseEnter(e);

      toolTipDefaultDelay = toolTip.AutoPopDelay;
      toolTip.AutoPopDelay = int.MaxValue;
    }

    /// <summary>
    /// Handles the MouseLeave event.
    /// </summary>
    /// <param name="e">The event arguments.</param>
    protected override void OnMouseLeave(EventArgs e)
    {
      base.OnMouseLeave(e);

      toolTip.AutoPopDelay = toolTipDefaultDelay;
      SetFocusedItem(null);
    }

    /// <summary>
    /// Handles the MouseMove event.
    /// </summary>
    /// <param name="e">The event arguments.</param>
    protected override void OnMouseMove(MouseEventArgs e)
    {
      base.OnMouseMove(e);

      DrawingMetrics m = HitTest(new PointF(e.X, e.Y));
      SetFocusedItem(m == null ? null : m.Item);
    }

    /// <summary>
    /// Handles the MouseClick event.
    /// </summary>
    /// <param name="e">The event arguments.</param>
    protected override void OnMouseClick(MouseEventArgs e)
    {
      base.OnMouseClick(e);

      if (this.FocusedItem != null)
        FireItemClicked(this.FocusedItem);
    }

    /// <summary>
    /// Handles the DoubleClick event.
    /// </summary>
    /// <param name="e">The event arguments.</param>
    protected override void OnDoubleClick(EventArgs e)
    {
      base.OnDoubleClick(e);

      if (this.FocusedItem != null)
        FireItemDoubleClicked(this.FocusedItem);
    }

    /// <summary>
    /// Handles the SizeChanged event.
    /// </summary>
    /// <param name="e">The event arguments.</param>
    protected override void OnSizeChanged(EventArgs e)
    {
      base.OnSizeChanged(e);

      if (this.AutoSizePie)
        MarkStructuralChange();
    }

    /// <summary>
    /// Handles the PaddingChanged event.
    /// </summary>
    /// <param name="e">The event arguments.</param>
    protected override void OnPaddingChanged(EventArgs e)
    {
      base.OnPaddingChanged(e);

      if (this.AutoSizePie)
        MarkStructuralChange();
    }

    /// <summary>
    /// Renders the given DrawingMetrics, which are calculated using ConstructDrawingMetrics.
    /// </summary>
    /// <param name="g">The graphics surface on which the chart is being rendered.</param>
    /// <param name="drawingMetrics">The drawing metrics to render.</param>
    private void Render(Graphics g, List<DrawingMetrics> drawingMetrics, Rectangle bounds, Padding padding)
    {
      // use a high-quality smoothing mode
      g.SmoothingMode = SmoothingMode.AntiAlias;
      g.TranslateTransform(bounds.Left + padding.Left + (bounds.Width - padding.Horizontal) / 2, bounds.Top + padding.Top + (bounds.Height - padding.Vertical + Style.VisualThickness) / 2);

      // don't draw anything if there's nothing to draw
      if (drawingMetrics.Count == 0)
        return;

      // if there is an item that is both at the bottom and top of the pie, special drawing considerations
      // are needed
      bool itemBottomTop = drawingMetrics[0].IsBottomItem && drawingMetrics[0].IsTopItem;
      if (itemBottomTop)
      {
        drawingMetrics[0].RenderBottom(g);
        drawingMetrics[0].RenderInterior(g);
      }
      else
      {
        drawingMetrics[0].RenderBottom(g);
        drawingMetrics[0].RenderInterior(g);
        drawingMetrics[0].RenderExterior(g);
      }

      for (int i = 1; i < drawingMetrics.Count; i++)
      {
        drawingMetrics[i].RenderBottom(g);
        drawingMetrics[i].RenderInterior(g);
        drawingMetrics[i].RenderExterior(g);
        drawingMetrics[i].RenderTop(g);
      }

      if (itemBottomTop)
        drawingMetrics[0].RenderExterior(g);
      drawingMetrics[0].RenderTop(g);

      foreach (DrawingMetrics metric in this.drawingMetrics)
      {
        metric.RenderText(g);
      }
    }

    /// <summary>
    /// Save the chart as an image.
    /// </summary>
    /// <param name="fileName">The path to the file where the image will be saved.</param>
    /// <param name="format">The format to save the image in.</param>
    /// <param name="sizeInPixels">The size of the image, in pixels.</param>
    public void SaveAs(string fileName, ImageFormat format, Size sizeInPixels)
    {
      SaveAs(fileName, format, sizeInPixels, Padding.Empty);
    }

    /// <summary>
    /// Saves the chart as an image.
    /// </summary>
    /// <param name="fileName">The path to the file where the image will be saved.</param>
    /// <param name="format">The format to save the image in.</param>
    /// <param name="sizeInPixels">The size of the image, in pixels.</param>
    /// <param name="padding">The padding which defines the border of the image.</param>
    public void SaveAs(string fileName, ImageFormat format, Size sizeInPixels, Padding padding)
    {
      List<DrawingMetrics> metrics = ConstructDrawingMetrics(new Rectangle(Point.Empty, sizeInPixels), padding);
      using (Bitmap bitmap = new Bitmap(sizeInPixels.Width, sizeInPixels.Height))
      using (Graphics g = Graphics.FromImage(bitmap))
      {
        // fill in the background
        g.FillRectangle(Brushes.White, 0, 0, sizeInPixels.Width, sizeInPixels.Height);

        // render the chart
        Render(g, metrics, new Rectangle(Point.Empty, sizeInPixels), padding);

        // save the image
        bitmap.Save(fileName, format);
      }

      DestructDrawingMetrics(metrics);
    }

    /// <summary>
    /// Registers a PrintDocument to print this pie chart.
    /// </summary>
    /// <param name="doc">The PrintDocument to register.</param>
    public void AttachPrintDocument(PrintDocument doc)
    {
      doc.PrintPage += new PrintPageEventHandler(OnPrintPage);
    }

    /// <summary>
    /// Called by a registered PrintDocument to control printing of the chart.
    /// </summary>
    /// <param name="sender">The sender.</param>
    /// <param name="e">The event arguments.</param>
    void OnPrintPage(object sender, PrintPageEventArgs e)
    {
      List<DrawingMetrics> metrics = ConstructDrawingMetrics(e.MarginBounds, Padding.Empty);

      e.Graphics.SetClip(e.MarginBounds);
      Render(e.Graphics, metrics, e.MarginBounds, Padding.Empty);
      e.HasMorePages = false;

      DestructDrawingMetrics(metrics);
    }

    /// <summary>
    /// Handles the painting of the control.
    /// </summary>
    /// <param name="pe">The paint event arguments.</param>
    protected override void OnPaint(PaintEventArgs pe)
    {
      // check to see if the structure has changed and if we're not in the middle of a transaction
      if (isStructureChanged && transactionRef == 0)
      {
        DestructDrawingMetrics(this.drawingMetrics);
        this.drawingMetrics = ConstructDrawingMetrics(this.ClientRectangle, this.Padding);
      }
      else if (isVisualChanged && recreateGraphics && transactionRef == 0)
      {
        RecreateGraphics(this.drawingMetrics, this.ClientRectangle);
      }

      // clear any change markings
      isStructureChanged = false;
      isVisualChanged = false;
      recreateGraphics = false;

      Render(pe.Graphics, this.drawingMetrics, ClientRectangle, Padding);
    }
    #endregion

    #region Transaction Methods
    /// <summary>
    /// Starts a modification transaction.  As long as any modification trasactions are open,
    /// the changes made to the control will not be reflected.  It is necessary to call
    /// EndModification for each call to BeginModification; otherwise, the control will
    /// never redraw.
    /// </summary>
    public void BeginModification()
    {
      this.transactionRef++;
    }

    /// <summary>
    /// Ends a modification transaction.  As long as any modification trasactions are open,
    /// the changes made to the control will not be reflected.  It is necessary to call
    /// EndModification for each call to BeginModification; otherwise, the control will
    /// never redraw.
    /// </summary>
    public void EndModification()
    {
      this.transactionRef = Math.Max(0, this.transactionRef - 1);
      if (this.transactionRef == 0 && this.isVisualChanged)
        this.Invalidate();
    }

    /// <summary>
    /// Sets a flag that indicates the control has changed structurally, and that DrawingMetrics
    /// will need to be completely recreated.
    /// </summary>
    internal void MarkStructuralChange()
    {
      this.isStructureChanged = true;
      this.isVisualChanged = true;
      if (this.transactionRef == 0)
        this.Invalidate();
      else
        Console.WriteLine("Pie chart not invalidated");
    }

    /// <summary>
    /// Sets a flag that indicates that the control needs to be refreshed, but that no structural
    /// or resource (pen/brush) altering changes were made.
    /// </summary>
    internal void MarkVisualChange()
    {
      MarkVisualChange(false);
    }

    /// <summary>
    /// Sets a flag that indicates the control needs to be refreshed.  If recreateGraphics is true,
    /// then pens and brushes will be recreated.
    /// </summary>
    /// <param name="recreateGraphics">True if pens and brushes should be recreated.</param>
    internal void MarkVisualChange(bool recreateGraphics)
    {
      this.isVisualChanged = true;
      this.recreateGraphics = this.recreateGraphics || recreateGraphics;
      if (this.transactionRef == 0)
        this.Invalidate();
    }
    #endregion
  }
}
