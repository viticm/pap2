
//    Copyright (C) 2005  Riccardo Marzi  <riccardo@dotnetfireball.net>
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace Fireball.Drawing.Shapes
{

	public class ShapeClosedCurve :ShapePolygon
    {
        #region Constructors

        internal ShapeClosedCurve()
		{

		}

        public ShapeClosedCurve(PointF[] points)
            : this(points, 0, null, null) { }
        public ShapeClosedCurve(PointF[] points, float rotation)
            : this(points, rotation, null, null) { }
        public ShapeClosedCurve(PointF[] points, Pen pen)
            : base(points, 0, pen) { }
        public ShapeClosedCurve(PointF[] points, float rotation, Pen pen)
            : base(points, rotation, pen) { }
        public ShapeClosedCurve(PointF[] points, float rotation, Brush brush)
            : this(points, rotation, null,brush) { }
        public ShapeClosedCurve(PointF[] points, float rotation, Pen pen, Brush brush)
            : base(points, rotation, pen)
        {
            this.Brush = brush;
        }

        #endregion

        #region Overrides

        public override Image GetThumb(ShapeThumbSize thumbSize)
        {
            throw new Exception("The method or operation is not implemented.");
        }
        public override string ToString()
        {
            return "{Fireball.Drawing.Shapes.ShapeClosedCurve}";
        }

        protected override void UpdatePath()
        {
            if (this.Points == null || this.Points.Length == 0) return;

            InternalPath = new GraphicsPath();
            InternalPath.AddClosedCurve(this.Points);

            Matrix mtx = new Matrix();
            mtx.RotateAt(this.Rotation, InternalRotationBasePoint);
            InternalPath.Transform(mtx);
        }

        #endregion      
	}
}