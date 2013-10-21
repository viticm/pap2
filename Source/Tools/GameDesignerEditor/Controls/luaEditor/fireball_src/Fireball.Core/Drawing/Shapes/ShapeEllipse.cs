
//    Copyright (C) 2005  Riccardo Marzi  <riccardo@dotnetfireball.net>
//
//    Copyright (C) 2005  Sebastian Faltoni  <sebastian@dotnetfireball.net>
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

	public class ShapeEllipse : ShapeRectangle
	{

		#region Constructors

		public ShapeEllipse(float left, float top, float width, float height)
			: base(new RectangleF(left, top, width, height), 0) { }
		public ShapeEllipse(float left, float top, float width, float height, float rotation)
			: base(new RectangleF(left, top, width, height), rotation) { }
		public ShapeEllipse(RectangleF bounds)
			: base(bounds, 0) { }
		public ShapeEllipse(RectangleF bounds, float rotation)
			: base(bounds, rotation) { }
		public ShapeEllipse(PointF firstCorner, PointF secondCorner)
			: base(firstCorner, secondCorner, 0) { }
		public ShapeEllipse(PointF firstCorner, PointF secondCorner, float rotation)
			: base(firstCorner, secondCorner, rotation) { }


		public ShapeEllipse(float left, float top, float width, float height, Pen pen)
			: base(new RectangleF(left, top, width, height), 0, pen) { }
		public ShapeEllipse(float left, float top, float width, float height, float rotation, Pen pen)
			: base(new RectangleF(left, top, width, height), rotation, pen) { }
		public ShapeEllipse(RectangleF bounds, Pen pen)
			: base(bounds, 0, pen) { }
		public ShapeEllipse(RectangleF bounds, float rotation, Pen pen)
			: base(bounds, rotation, pen) { }
		public ShapeEllipse(PointF firstCorner, PointF secondCorner, Pen pen)
			: base(firstCorner, secondCorner, 0, pen) { }
		public ShapeEllipse(PointF firstCorner, PointF secondCorner, float rotation, Pen pen)
			: base(firstCorner, secondCorner, rotation, pen) { }

		public ShapeEllipse(float left, float top, float width, float height, Brush brush)
			: base(new RectangleF(left, top, width, height), 0, brush) { }
		public ShapeEllipse(float left, float top, float width, float height, float rotation, Brush brush)
			: base(new RectangleF(left, top, width, height), rotation, brush) { }
		public ShapeEllipse(RectangleF bounds, Brush brush)
			: base(bounds, 0, brush) { }
		public ShapeEllipse(RectangleF bounds, float rotation, Brush brush)
			: base(bounds, rotation, brush) { }
		public ShapeEllipse(PointF firstCorner, PointF secondCorner, Brush brush)
			: base(firstCorner, secondCorner, 0, brush) { }
		public ShapeEllipse(PointF firstCorner, PointF secondCorner, float rotation, Brush brush)
			: base(firstCorner, secondCorner, rotation, brush) { }

		public ShapeEllipse(float left, float top, float width, float height, Pen pen, Brush brush)
			: base(new RectangleF(left, top, width, height), 0, pen, brush) { }
		public ShapeEllipse(float left, float top, float width, float height, float rotation, Pen pen, Brush brush)
			: base(new RectangleF(left, top, width, height), 0, pen, brush) { }
		public ShapeEllipse(RectangleF bounds, Pen pen, Brush brush)
			: base(bounds, 0, pen, brush) { }
		public ShapeEllipse(RectangleF bounds, float rotation, Pen pen, Brush brush)
			: base(bounds, rotation, pen, brush) { }
		public ShapeEllipse(PointF firstCorner, PointF secondCorner, Pen pen, Brush brush)
			: base(firstCorner, secondCorner, 0, pen, brush) { }
		public ShapeEllipse(PointF firstCorner, PointF secondCorner, float rotation, Pen pen, Brush brush)
			: base(firstCorner, secondCorner, rotation, pen, brush) { }

		#endregion

		#region Overrides

        public override Image GetThumb(ShapeThumbSize thumbSize)
        {
            throw new Exception("The method or operation is not implemented.");
        }
        public override string ToString()
        {
            return "{Fireball.Drawing.Shapes.ShapeEllipse}";
        }

        protected override void UpdatePath()
		{
			InternalPath = new GraphicsPath(FillMode.Winding);
			InternalPath.AddEllipse(this.Bounds);

			Matrix mtx = new Matrix();
			mtx.RotateAt(this.Rotation, this.Location);
			InternalPath.Transform(mtx);
		}

		#endregion

	}
}