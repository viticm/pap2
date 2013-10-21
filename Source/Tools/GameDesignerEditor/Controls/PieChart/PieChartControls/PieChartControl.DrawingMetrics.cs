using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace Nexus.Windows.Forms
{
  public partial class PieChart : Control
  {
    /// <summary>
    /// Represents the drawing data for a single pie slice. This includes
    /// GraphicsPath items for each surface and edge of the slice, Regions for
    /// hit testing, and Pens and Brushes for each surface.
    /// </summary>
    internal class DrawingMetrics : IComparable<DrawingMetrics>
    {
      #region Constructor
      /// <summary>
      /// Constructs a new DrawingMetrics.
      /// </summary>
      /// <param name="control">The control this object is associated with.</param>
      /// <param name="item">The item this object is associated with.</param>
      /// <param name="startAngle">The start angle of this pie slice, in radians.</param>
      /// <param name="sweepAngle">The sweep angle of this pie slice, in radians.</param>
      public DrawingMetrics(PieChart control, PieChartItem item, Rectangle drawingBounds, float startAngle, float sweepAngle)
      {
        this.control = control;
        this.item = item;
        this.drawingBounds = drawingBounds;
        this.startAngle = (float)(startAngle % (2 * Math.PI));
        this.sweepAngle = sweepAngle;

        ConstructGraphics();
        ConstructPaths();
        ConstructRegions();
      }
      #endregion

      #region Fields
      private PieChart control;
      private PieChartItem item;
      private Rectangle drawingBounds;
      private float startAngle;
      private float sweepAngle;

      private Brush surfaceBrush;
      private Brush startBrush;
      private Brush endBrush;
      private Brush exteriorBrush;
      private Pen edgePen;

      private Region visibleRegion;
      private Region topRegion;
      private Region interiorRegion;
      private Region exteriorRegion;

      private GraphicsPath topFace;
      private GraphicsPath bottomFace;
      private GraphicsPath startFace;
      private GraphicsPath endFace;
      private GraphicsPath exteriorFace;
      private GraphicsPath startFaceEdges;
      private GraphicsPath endFaceEdges;
      private GraphicsPath exteriorFaceEdges;
      #endregion

      #region Properties
      /// <summary>
      /// The control this object is associated with.
      /// </summary>
      public PieChart Control
      {
        get
        {
          return control;
        }
      }

      /// <summary>
      /// The item this control is associated with.
      /// </summary>
      public PieChartItem Item
      {
        get
        {
          return item;
        }
      }

      /// <summary>
      /// Gets or sets the bounds which the whole pie fits in.
      /// </summary>
      public Rectangle DrawingBounds
      {
        get
        {
          return drawingBounds;
        }
        set
        {
          drawingBounds = value;
        }
      }

      /// <summary>
      /// The start angle of this control, in radians.
      /// </summary>
      public float StartAngle
      {
        get
        {
          return startAngle;
        }
      }

      /// <summary>
      /// The sweep angle of this control, in radians.
      /// </summary>
      public float SweepAngle
      {
        get
        {
          return sweepAngle;
        }
      }

      /// <summary>
      /// The end angle of this control, in radians.
      /// </summary>
      public float EndAngle
      {
        get
        {
          double result = StartAngle + SweepAngle;
          if (result > 2 * Math.PI)
            result -= 2 * Math.PI;
          return (float)result;
        }
      }

      /// <summary>
      /// The transformed start angle.  See the TransformAngle method for details about why the angles need to be transformed.
      /// </summary>
      public float TransformedStartAngle
      {
        get
        {
          return TransformAngle(StartAngle);
        }
      }

      /// <summary>
      /// The transformed end angle. See the TransformAngle method for details about why the angles need to be transformed.
      /// </summary>
      public float TransformedSweepAngle
      {
        get
        {
          // if the SweepAngle is sufficiently close to 2 * pi, return 2 * pi to avoid floating point error incurred in
          // transforming the angle.
          if (Math.Abs(SweepAngle - 2 * Math.PI) < 0.0001)
            return (float)(2 * Math.PI);

          // otherwise, use the transformed angle
          double result = TransformAngle(StartAngle + SweepAngle) - TransformedStartAngle;
          if (result < 0)
            result += 2 * Math.PI;
          return (float)result;
        }
      }

      /// <summary>
      /// Gets the offset size of the pie slice.  This offset is the real screen offset
      /// at which the pie slice should be drawn.
      /// </summary>
      public SizeF OffsetSize
      {
        get
        {
          double angle = StartAngle + SweepAngle / 2;
          double x = Item.Offset * Math.Cos(angle);
          double y = Item.Offset * Math.Sin(angle) * Control.Style.HeightWidthRatio;
          return new SizeF((float)x, (float)y);
        }
      }

      /// <summary>
      /// Gets whether or not the PieChartItem that this DrawingMetrics represents is the focused one.
      /// </summary>
      public bool IsFocused
      {
        get
        {
          return Control.FocusedItem == this.Item;
        }
      }
      #endregion

      #region Resource Methods
      /// <summary>
      /// Constructs all of the pens and brushes required for drawing the slice.
      /// </summary>
      private void ConstructGraphics()
      {
        this.surfaceBrush = CreateSurfaceBrush();
        this.edgePen = CreateEdgePen();

        startBrush = CreateInteriorBrush((float)(StartAngle - Math.PI));
        endBrush = CreateInteriorBrush(EndAngle);
        this.exteriorBrush = CreateExteriorBrush();
      }

      /// <summary>
      /// Disposes of all pens and brushes.
      /// </summary>
      private void DestroyGraphics()
      {
        surfaceBrush.Dispose();
        startBrush.Dispose();
        endBrush.Dispose();
        exteriorBrush.Dispose();
        edgePen.Dispose();
      }

      /// <summary>
      /// Recreates all of the pens and brushes required for drawing the slice.
      /// </summary>
      public void RecreateGraphics()
      {
        DestroyGraphics();
        ConstructGraphics();
      }

      /// <summary>
      /// Creates a brush for the exterior face.
      /// </summary>
      /// <returns>A brush used for filling the exterior face.</returns>
      private Brush CreateExteriorBrush()
      {
        // get the alpha-corrected surface color
        Color trueColor = GetAlphaTransparentSurfaceColor(Item.Color);

        // blend the color using the brightness factor
        ColorBlend blend = new ColorBlend();
        blend.Colors = new Color[] {
          ChangeColorBrightness(trueColor, Control.Style.ShadowBrightnessFactor / 2),
          trueColor,
          ChangeColorBrightness(trueColor, Control.Style.ShadowBrightnessFactor)
        };

        // set the blend positions
        blend.Positions = new float[] { 0, 0.1F, 1.0F };

        // create the brush and set the interpolation colors
        LinearGradientBrush brush = new LinearGradientBrush(new RectangleF(-DrawingBounds.Width / 2, -DrawingBounds.Height / 2, DrawingBounds.Width, DrawingBounds.Height), Color.Blue, Color.White, LinearGradientMode.Horizontal);
        brush.InterpolationColors = blend;
        return brush;
      }

      /// <summary>
      /// Creates a brush for the top surface of the slice.
      /// </summary>
      /// <returns>A brush used for filling the top face.</returns>
      private Brush CreateSurfaceBrush()
      {
        return new SolidBrush(GetAlphaTransparentSurfaceColor(Item.Color));
      }

      /// <summary>
      /// Creates a brush for drawing the interior edge, based on the angle the edge is shown at.
      /// </summary>
      /// <param name="angle">The angle of the interior edge.</param>
      /// <returns>A brush used for filling the interior face.</returns>
      private Brush CreateInteriorBrush(float angle)
      {
        return new SolidBrush(ChangeColorBrightness(GetAlphaTransparentSurfaceColor(Item.Color), (float)(Control.Style.ShadowBrightnessFactor * (1 - 0.8 * Math.Cos(angle)))));
      }

      /// <summary>
      /// Creates a brush for drawing the edges.
      /// </summary>
      /// <returns>A pen used for drawing the edges.</returns>
      private Pen CreateEdgePen()
      {
        float brightnessFactor = Control.ItemStyle.EdgeBrightnessFactor;
        if (IsFocused)
          brightnessFactor = Control.FocusedItemStyle.EdgeBrightnessFactor;
        return new Pen(ChangeColorBrightness(Item.Color, brightnessFactor), 1);
      }

      /// <summary>
      /// Gets the brush used for drawing the top surface.
      /// </summary>
      public Brush SurfaceBrush
      {
        get
        {
          return surfaceBrush;
        }
      }

      /// <summary>
      /// Gets the brush used for drawing the starting interior surface.
      /// </summary>
      public Brush StartBrush
      {
        get
        {
          return startBrush;
        }
      }

      /// <summary>
      /// Gets the brush used for drawing the ending interior surface.
      /// </summary>
      public Brush EndBrush
      {
        get
        {
          return endBrush;
        }
      }

      /// <summary>
      /// Gets the brush used for drawing the exterior surface.
      /// </summary>
      public Brush ExteriorBrush
      {
        get
        {
          return exteriorBrush;
        }
      }

      /// <summary>
      /// Gets the pen used for drawing the edges.
      /// </summary>
      public Pen EdgePen
      {
        get
        {
          return edgePen;
        }
      }

      /// <summary>
      /// Gets the GraphicsPath that represents the top surface.
      /// </summary>
      public GraphicsPath TopFace
      {
        get
        {
          return topFace;
        }
      }

      /// <summary>
      /// Gets the GraphicsPath that represents the bottom surface.
      /// </summary>
      public GraphicsPath BottomFace
      {
        get
        {
          return bottomFace;
        }
      }

      /// <summary>
      /// Gets the GraphicsPath that represents the starting interior surface.
      /// </summary>
      public GraphicsPath StartFace
      {
        get
        {
          return startFace;
        }
      }

      /// <summary>
      /// Gets the GraphicsPath that represents the ending interior surface.
      /// </summary>
      public GraphicsPath EndFace
      {
        get
        {
          return endFace;
        }
      }

      /// <summary>
      /// Gets the GraphicsPath that represents the exterior surface.
      /// </summary>
      public GraphicsPath ExteriorFace
      {
        get
        {
          return exteriorFace;
        }
      }

      /// <summary>
      /// Gets the GraphicsPath that represents the edges bordering the starting interior face.
      /// </summary>
      public GraphicsPath StartFaceEdges
      {
        get
        {
          return startFaceEdges;
        }
      }

      /// <summary>
      /// Gets the GraphicsPath that represents the edges bordering the ending interior face.
      /// </summary>
      public GraphicsPath EndFaceEdges
      {
        get
        {
          return endFaceEdges;
        }
      }

      /// <summary>
      /// Gets the GraphicsPath that represents the edges bordering the exterior face.
      /// </summary>
      public GraphicsPath ExteriorFaceEdges
      {
        get
        {
          return exteriorFaceEdges;
        }
      }

      /// <summary>
      /// Gets the Region which represents the entire surface.
      /// </summary>
      public Region VisibleRegion
      {
        get
        {
          return visibleRegion;
        }
      }

      /// <summary>
      /// Gets the Region which represents the top surface.
      /// </summary>
      public Region TopRegion
      {
        get
        {
          return topRegion;
        }
      }

      /// <summary>
      /// Gets the Region which represents the interior surface(s).
      /// </summary>
      public Region InteriorRegion
      {
        get
        {
          return interiorRegion;
        }
      }

      /// <summary>
      /// Gets the Region which represents the exterior surface(s).
      /// </summary>
      public Region ExteriorRegion
      {
        get
        {
          return exteriorRegion;
        }
      }

      /// <summary>
      /// Disposes of all graphics and regions.
      /// </summary>
      internal void DestroyResources()
      {
        DestroyGraphics();
        DestroyRegions();
      }

      /// <summary>
      /// Disposes of all regions.
      /// </summary>
      private void DestroyRegions()
      {
        visibleRegion.Dispose();
        topRegion.Dispose();
        interiorRegion.Dispose();
        exteriorRegion.Dispose();
      }

      /// <summary>
      /// Constructs regions which represent the boundaries of all faces of the pie slice.
      /// </summary>
      private void ConstructRegions()
      {
        // create the top region
        this.topRegion = new Region(TopFace);

        // create the exterior region
        this.exteriorRegion = new Region();
        this.exteriorRegion.MakeEmpty();
        if (ExteriorFace != null)
          this.exteriorRegion.Union(ExteriorFace);

        // create the interior region
        this.interiorRegion = new Region();
        this.interiorRegion.MakeEmpty();
        if (StartFace != null)
          this.interiorRegion.Union(StartFace);
        if (EndFace != null)
          this.interiorRegion.Union(EndFace);

        // create the visible region
        this.visibleRegion = new Region();
        this.visibleRegion.MakeEmpty();
        this.visibleRegion.Union(this.topRegion);
        this.visibleRegion.Union(this.exteriorRegion);
        this.visibleRegion.Union(this.interiorRegion);
      }
      #endregion

      #region Surface Methods
      /// <summary>
      /// Constructs all GraphicsPaths which represent every face of the slice.
      /// These GraphicsPaths are centered at (0,0), and a transform is applied to the Graphics
      /// object which draws the GraphicsPaths.
      /// </summary>
      private void ConstructPaths()
      {
        SizeF offset = OffsetSize;

        // create the bottom face
        bottomFace = new GraphicsPath();
        bottomFace.AddPie(offset.Width - Control.Style.EllipseWidth, offset.Height - Control.Style.EllipseHeight, 2 * Control.Style.EllipseWidth, 2 * Control.Style.EllipseHeight, (float)(TransformedStartAngle * 180 / Math.PI), (float)(TransformedSweepAngle * 180 / Math.PI));

        // create the top face
        topFace = new GraphicsPath();
        topFace.AddPie(offset.Width - Control.Style.EllipseWidth, -Control.Style.VisualThickness + offset.Height - Control.Style.EllipseHeight, 2 * Control.Style.EllipseWidth, 2 * Control.Style.EllipseHeight, (float)(TransformedStartAngle * 180 / Math.PI), (float)(TransformedSweepAngle * 180 / Math.PI));

        // create the interior start face if it is visible
        if (StartAngle >= Math.PI / 2 && StartAngle <= 3 * Math.PI / 2)
          startFace = CreateInteriorFace(StartAngle, out startFaceEdges);
        else
          startFace = null;

        // create the interior end face if it is visible
        if (EndAngle >= 3 * Math.PI / 2 || EndAngle <= Math.PI / 2)
          endFace = CreateInteriorFace(EndAngle, out endFaceEdges);
        else
          endFace = null;

        // create the exterior face
        exteriorFace = CreateExteriorFace(out exteriorFaceEdges);
      }

      /// <summary>
      /// Creates an interior face of a pie slice.
      /// </summary>
      /// <param name="angle">The angle of the face.</param>
      /// <param name="edgePath">The GraphicsPath which represents the edges of this face.</param>
      /// <returns>The GraphicsPath which represents the surface of the face.</returns>
      private GraphicsPath CreateInteriorFace(float angle, out GraphicsPath edgePath)
      {
        GraphicsPath path = new GraphicsPath();
        SizeF offset = OffsetSize;
        PointF[] points = new PointF[4];
        points[0] = new PointF(offset.Width, offset.Height - Control.Style.VisualThickness);
        points[1] = new PointF(offset.Width, offset.Height);
        points[2] = new PointF((float)(offset.Width + Control.Style.EllipseWidth * Math.Cos(angle)), (float)(offset.Height + Control.Style.EllipseHeight * Math.Sin(angle)));
        points[3] = new PointF(points[2].X, points[2].Y - Control.Style.VisualThickness);
        path.AddPolygon(points);

        edgePath = new GraphicsPath();
        edgePath.AddLine(points[0], points[1]);
        edgePath.StartFigure();
        edgePath.AddLine(points[2], points[3]);

        return path;
      }

      /// <summary>
      /// Creates an exterior face of a pie slice.
      /// </summary>
      /// <param name="edgePath">The GraphicsPath which represents the edges of this face.</param>
      /// <returns>The GraphicsPath which represents the surface of the face.</returns>
      private GraphicsPath CreateExteriorFace(out GraphicsPath edgePath)
      {
        SizeF offset = OffsetSize;

        // create an array to hold the possible angles to the parts of the external face
        float[] angles = new float[4];
        // create an array to hold the points on the exterior of the pie at each of the above angles
        PointF[] points = new PointF[4];

        // boolean values to determine which of the above angles an exterior face should be drawn between
        bool drawBetween01 = false;
        bool drawBetween12 = false;
        bool drawBetween23 = false;
        bool drawBetween03 = false;

        // start with angle 0 being 0 radians and angle 3 being pi radians (which represents the entire visible part of the pie)
        angles[0] = 0;
        angles[3] = (float)Math.PI;
        points[0] = new PointF(offset.Width + Control.Style.EllipseWidth, offset.Height);
        points[3] = new PointF(offset.Width - Control.Style.EllipseWidth, offset.Height);

        // test where the StartAngle and EndAngle are to determine which angles an exterior face needs to be drawn between
        if (StartAngle >= Math.PI && EndAngle >= Math.PI)
        {
          if (SweepAngle >= Math.PI)
          {
            // in this case, the whole front side of the graph contains the exterior of this slice
            drawBetween03 = true;
          }
          else
          {
            // in this case, the whole exterior is hidden on the backside of the pie, so return null
            edgePath = null;
            return null;
          }
        }
        else if (EndAngle >= Math.PI)
        {
          // the start angle is in visible range, but not the end angle
          drawBetween23 = true;
          angles[1] = 0;
          angles[2] = StartAngle;
        }
        else if (StartAngle >= Math.PI)
        {
          // the end angle is in visible range, but not the start angle
          drawBetween01 = true;
          angles[1] = EndAngle;
          angles[2] = (float)Math.PI;
        }
        else
        {
          if (SweepAngle < Math.PI)
          {
            // the exterior is only between the StartAngle and the EndAngle on the visible side of the pie
            drawBetween12 = true;
            angles[1] = StartAngle;
            angles[2] = EndAngle;
          }
          else
          {
            // the exterior starts on the visible side, wraps around the backside, and reappears on the visible side
            // two exterior pieces must be drawn
            drawBetween01 = true;
            drawBetween23 = true;
            angles[1] = StartAngle;
            angles[2] = EndAngle;
          }
        }

        // for the angles 1 and 2, calculate the endpoints of those edges, and then transform the angles into drawing-ready angles
        for (int i = 1; i <= 2; i++)
        {
          points[i] = new PointF((float)(offset.Width + Control.Style.EllipseWidth * Math.Cos(angles[i])), (float)(offset.Height + Control.Style.EllipseHeight * Math.Sin(angles[i])));
          angles[i] = TransformAngle(angles[i]);
        }

        // construct new GraphicsPaths for the surface and edges, and add the appropriate figures
        GraphicsPath path = new GraphicsPath();
        edgePath = new GraphicsPath();
        if (drawBetween01)
        {
          AddExteriorFigure(path, edgePath, points[0], points[1], angles[0], angles[1] - angles[0]);
        }
        if (drawBetween12)
        {
          AddExteriorFigure(path, edgePath, points[1], points[2], angles[1], angles[2] - angles[1]);
        }
        if (drawBetween23)
        {
          AddExteriorFigure(path, edgePath, points[2], points[3], angles[2], angles[3] - angles[2]);
        }
        if (drawBetween03)
        {
          AddExteriorFigure(path, edgePath, points[0], points[3], angles[0], angles[3] - angles[0]);
        }

        return path;
      }

      /// <summary>
      /// Adds an exterior face part to the graphics paths given.
      /// </summary>
      /// <param name="path">The GraphicsPath which represents the surface of the exterior.</param>
      /// <param name="edgePath">The GraphicsPath which represents the edges on the exterior.</param>
      /// <param name="rightPoint">The exterior point at the left of the exterior arc.</param>
      /// <param name="leftPoint">The exterior point at the right of the exterior arc.</param>
      /// <param name="startAngle">The start angle of the arc.</param>
      /// <param name="sweepAngle">The sweep angle of the arc.</param>
      private void AddExteriorFigure(GraphicsPath path, GraphicsPath edgePath, PointF rightPoint, PointF leftPoint, float startAngle, float sweepAngle)
      {
        SizeF offset = OffsetSize;

        // calculate the exterior points with the thickness of the slice taken into effect
        PointF rightPointThick = new PointF(rightPoint.X, rightPoint.Y - Control.Style.VisualThickness);
        PointF leftPointThick = new PointF(leftPoint.X, leftPoint.Y - Control.Style.VisualThickness);

        // draw the exterior figure
        path.StartFigure();
        path.AddLine(rightPoint, rightPointThick);
        path.AddArc(offset.Width - Control.Style.EllipseWidth, offset.Height - Control.Style.VisualThickness - Control.Style.EllipseHeight, 2 * Control.Style.EllipseWidth, 2 * Control.Style.EllipseHeight, (float)(startAngle * 180 / Math.PI), (float)(sweepAngle * 180 / Math.PI));
        path.AddLine(leftPointThick, leftPoint);
        path.AddArc(offset.Width - Control.Style.EllipseWidth, offset.Height - Control.Style.EllipseHeight, 2 * Control.Style.EllipseWidth, 2 * Control.Style.EllipseHeight, (float)((startAngle + sweepAngle) * 180 / Math.PI), (float)(-sweepAngle * 180 / Math.PI));

        // draw the exterior edges
        edgePath.StartFigure();
        edgePath.AddLine(rightPoint, rightPointThick);
        edgePath.StartFigure();
        edgePath.AddLine(leftPoint, leftPointThick);
      }
      #endregion

      #region Rendering Methods
      /// <summary>
      /// Draws the bottom edges of the slice.
      /// </summary>
      /// <param name="g">The Graphics on which the control is being rendered.</param>
      internal void RenderBottom(Graphics g)
      {
        if (Control.Style.ShowEdges)
          g.DrawPath(EdgePen, BottomFace);
      }

      /// <summary>
      /// Draws the top faces and edges of the slice.
      /// </summary>
      /// <param name="g">The Graphics on which the control is being rendered.</param>
      internal void RenderTop(Graphics g)
      {
        g.FillPath(SurfaceBrush, TopFace);
        if (Control.Style.ShowEdges)
          g.DrawPath(EdgePen, TopFace);
      }

      /// <summary>
      /// Draws the interior faces and edges of the slice.
      /// </summary>
      /// <param name="g">The Graphics on which the control is being rendered.</param>
      internal void RenderInterior(Graphics g)
      {
        if (StartFace != null)
        {
          g.FillPath(StartBrush, StartFace);
          if (Control.Style.ShowEdges)
            g.DrawPath(EdgePen, StartFaceEdges);
        }

        if (EndFace != null)
        {
          g.FillPath(EndBrush, EndFace);
          if (Control.Style.ShowEdges)
            g.DrawPath(EdgePen, EndFaceEdges);
        }
      }

      /// <summary>
      /// Draws the exterior faces and edges of the slice.
      /// </summary>
      /// <param name="g">The Graphics on which the control is being rendered.</param>
      internal void RenderExterior(Graphics g)
      {
        if (ExteriorFace != null)
        {
          g.FillPath(ExteriorBrush, ExteriorFace);
          if (Control.Style.ShowEdges)
            g.DrawPath(EdgePen, ExteriorFaceEdges);
        }
      }

      /// <summary>
      /// Gets the position at which the text of the object should be drawn.
      /// </summary>
      /// <remarks>This method returns the center point in the block where the text is drawn.</remarks>
      /// <returns>The point at which the text should be drawn.</returns>
      public PointF GetTextPosition()
      {
        // draw the text along the bisecting line of the control, 2/3 of the way out on the radius
        float angle = StartAngle + SweepAngle / 2;
        return new PointF((float)(OffsetSize.Width + Control.Style.EllipseWidth * 2 / 3 * Math.Cos(angle)), (float)(-Control.Style.VisualThickness + OffsetSize.Height + Control.Style.EllipseHeight * 2 / 3 * Math.Sin(angle)));
      }

      /// <summary>
      /// Draws the text of the slice.
      /// </summary>
      /// <param name="g">The Graphics on which the control is being rendered.</param>
      internal void RenderText(Graphics g)
      {
        if (!string.IsNullOrEmpty(Item.Text) && Control.Style.TextDisplayMode != TextDisplayTypes.Never)
        {
          StringFormat format = new StringFormat();
          format.Alignment = StringAlignment.Center;
          format.LineAlignment = StringAlignment.Center;
          PointF textPosition = GetTextPosition();
          if (Control.Style.TextDisplayMode == TextDisplayTypes.Always)
          {
            g.DrawString(Item.Text, Control.Font, SystemBrushes.ControlText, textPosition, format);
          }
          else
          {
            SizeF size = g.MeasureString(Item.Text, Control.Font, textPosition, format);
            Rectangle bounds = new Rectangle((int)Math.Round(textPosition.X - size.Width / 2), (int)Math.Round(textPosition.Y - size.Height / 2), (int)Math.Round(size.Width), (int)Math.Round(size.Height));
            if (TopRegion.IsVisible(bounds.Left, bounds.Top) && TopRegion.IsVisible(bounds.Left, bounds.Bottom) && TopRegion.IsVisible(bounds.Right, bounds.Top) && TopRegion.IsVisible(bounds.Right, bounds.Bottom))
              g.DrawString(Item.Text, Control.Font, SystemBrushes.ControlText, textPosition, format);
          }
        }
      }
      #endregion

      #region Methods
      /// <summary>
      /// Transforms the real angle of a pie slice into the angle that can be drawn at the given inclination.
      /// The angle must be transformed because when the circle representing the pie is drawn inclined,
      /// the angle on the circle must be made to match the angle on the corresponding ellipse.
      /// </summary>
      /// <param name="angle">The angle on a circle being transformed into an ellipse.</param>
      /// <returns>The angle as it appears on an ellipse.</returns>
      private float TransformAngle(float angle)
      {
        double x = Control.Style.EllipseWidth * Math.Cos(angle);
        double y = Control.Style.EllipseHeight * Math.Sin(angle);
        double result = Math.Atan2(y, x);
        if (result < 0)
          result += 2 * Math.PI;
        return (float)result;
      }

      /// <summary>
      /// Changes the brightness of a color by a correctionFactor, which is an integer between
      /// -1 and 1.  A correction factor of -1 will make the color black, while a correction
      /// factor of 1 will make the color white.
      /// </summary>
      /// <param name="color">The color to change the brightness of.</param>
      /// <param name="correctionFactor">The factor to change the color's brightness by.</param>
      /// <returns>The color with modified brightness.</returns>
      internal static Color ChangeColorBrightness(Color color, float correctionFactor)
      {
        if (correctionFactor == 0 || float.IsNaN(correctionFactor))
          return color;

        float red = (float)color.R;
        float green = (float)color.G;
        float blue = (float)color.B;
        if (correctionFactor < 0)
        {
          correctionFactor++;
          red *= correctionFactor;
          green *= correctionFactor;
          blue *= correctionFactor;
        }
        else
        {
          red += (255 - red) * correctionFactor;
          green += (255 - green) * correctionFactor;
          blue += (255 - blue) * correctionFactor;
        }
        return System.Drawing.Color.FromArgb(color.A, (int)red, (int)green, (int)blue);
      }

      /// <summary>
      /// Gets the correct alpha-transparent surface color from a given color.  This
      /// takes into effect the proper brightness correction as well, and differentiates
      /// between the focused and nonfocused state.
      /// </summary>
      /// <param name="color">The color to transform.</param>
      /// <returns>The transformed color.</returns>
      public Color GetAlphaTransparentSurfaceColor(Color color)
      {
        float alpha = Control.ItemStyle.SurfaceAlphaTransparency;
        float brightnessFactor = Control.ItemStyle.SurfaceBrightnessFactor;
        if (Control.FocusedItem == this.Item)
        {
          alpha = Control.FocusedItemStyle.SurfaceAlphaTransparency;
          brightnessFactor = Control.FocusedItemStyle.SurfaceBrightnessFactor;
        }

        return Color.FromArgb((int)(alpha * color.A), ChangeColorBrightness(color, brightnessFactor));
      }
      #endregion

      #region IComparable<T>
      /// <summary>
      /// Gets whether or not this slice is a "top" slice, meaning that the slice contains 3*pi / 2 in its range
      /// and is thus at the top of the chart on the screen.
      /// </summary>
      public bool IsTopItem
      {
        get
        {
          double start = StartAngle;
          double end = StartAngle + SweepAngle;
          if (start > 3 * Math.PI / 2)
          {
            start -= 2 * Math.PI;
            end -= 2 * Math.PI;
          }

          return start < 3 * Math.PI / 2 && end > 3 * Math.PI / 2;
        }
      }

      /// <summary>
      /// Gets whether or not this is a "left" slice, meaning that the slice is bound between
      /// pi / 2 and 3 * pi / 2 and is thus on the left of the chart on the screen.
      /// </summary>
      public bool IsLeftItem
      {
        get
        {
          return StartAngle > Math.PI / 2 && StartAngle < 3 * Math.PI / 2 && EndAngle > Math.PI / 2 && EndAngle < 3 * Math.PI / 2;
        }
      }

      /// <summary>
      /// Gets whether or not this is a "right" slice, meaning that the slice is bound
      /// between 3 * pi / 2 and 5 * pi / 2 and is thus on the right of the chart on the screen.
      /// </summary>
      public bool IsRightItem
      {
        get
        {
          return (StartAngle < Math.PI / 2 || StartAngle > 3 * Math.PI / 2) && (EndAngle < Math.PI / 2 || EndAngle > 3 * Math.PI / 2);
        }
      }

      /// <summary>
      /// Gets whether or not this slice is a "bottom" slice, meaning that the slice contains pi / 2 in its range
      /// and is thus at the bottom of the chart on the screen.
      /// </summary>
      public bool IsBottomItem
      {
        get
        {
          double start = StartAngle;
          double end = StartAngle + SweepAngle;
          if (start > Math.PI / 2)
          {
            start -= 2 * Math.PI;
            end -= 2 * Math.PI;
          }

          return start < Math.PI / 2 && end > Math.PI / 2;
        }
      }

      /// <summary>
      /// Compares two DrawingMetrics.  The criteria for comparison is what order
      /// the items should be drawn in.
      /// </summary>
      /// <param name="other">The other object being compared.</param>
      /// <returns>A positive value if this object is greater, 0 if the objects are equal, or a negative value if this object is less.</returns>
      public int CompareTo(DrawingMetrics other)
      {
        if (this == other)
          return 0;

        bool thisTop = this.IsTopItem;
        bool thisLeft = this.IsLeftItem;
        bool thisRight = this.IsRightItem;
        bool thisBottom = this.IsBottomItem;
        bool otherTop = other.IsTopItem;
        bool otherLeft = other.IsLeftItem;
        bool otherRight = other.IsRightItem;
        bool otherBottom = other.IsBottomItem;

        if (thisTop && otherTop)
          return 0; // if both are on top, they are equal
        else if (otherTop)
          return 1; // if the other is on top and this is not, other is greater because it should be drawn first
        else if (thisTop)
          return -1; // if this is on top and the other is not, other is less because this should be drwan first

        if (thisBottom && otherBottom)
          return 0; // if both are on bottom, they are equal
        else if (thisBottom)
          return 1; // if this is on the bottom and the other is not, other is less because this should be drawn first
        else if (otherBottom)
          return -1; // if other is on the bottom and the this is not, other is greater because it should be drawn first

        if (thisLeft && otherLeft)
          return -this.StartAngle.CompareTo(other.StartAngle); // if both on left, use opposite of the angle comparison
        else if (thisRight && otherRight)
        {
          // use the angle comparison
          double thisAngle = this.EndAngle < Math.PI ? this.EndAngle + Math.PI * 2 : this.EndAngle;
          double otherAngle = other.EndAngle < Math.PI ? other.EndAngle + Math.PI * 2 : other.EndAngle;
          return thisAngle.CompareTo(otherAngle);
        }
        else
          return thisLeft ? 1 : -1; // return the left ones greater than the right ones for consistent ordering
      }
      #endregion
    }
  }
}