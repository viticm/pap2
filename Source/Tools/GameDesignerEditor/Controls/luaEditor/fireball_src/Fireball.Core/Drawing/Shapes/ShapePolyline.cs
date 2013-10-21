
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

	public class ShapePolyline :ShapeLine
	{

		private PointF[] _points;

		#region Constructors

		internal ShapePolyline()
		{
		}

		public ShapePolyline(PointF[] points)
			: this(points, 0, null) { }
		public ShapePolyline(PointF[] points, float rotation)
			: this(points, rotation, null) { }
		public ShapePolyline(PointF[] points, Pen pen)
			: this(points, 0, pen) { }
		public ShapePolyline(PointF[] points, float rotation, Pen pen)
		{
			_points = points;

			this.Rotation = rotation;
			this.Pen = pen;

			this.UpdatePath();
		}

		#endregion

		#region Overrides

		public override RectangleF Bounds
		{
			get
			{
				float minX = float.MaxValue;
				float minY = minX;
				float maxX = float.MinValue;
				float maxY = maxX;

				for (int i = 0; i < _points.Length; i++)
				{
					PointF current = _points[i];
					minX = Math.Min(minX, current.X);
					minY = Math.Min(minY, current.Y);
					maxX = Math.Max(maxX, current.X);
					maxY = Math.Max(maxY, current.Y);
				}

				return new RectangleF(minX, minY, maxX - minX, maxY - minY);
			}
		}
        public override PointF StartPoint
        {
            get
            {
                return _points[0];
            }
            set
            {
                _points[0] = value;
                UpdatePath();
            }
        }
        public override PointF EndPoint
        {
            get
            {
                return _points[_points.Length-1];
            }
            set
            {
                _points[_points.Length - 1] = value;
                UpdatePath();
            }
        }

		public override void Translate(float dx, float dy)
		{
			for (int i = 0; i < _points.Length; i++)
			{
				_points[i].X += dx;
				_points[i].Y += dy;
			}
			UpdatePath();
		}
        public override Image GetThumb(ShapeThumbSize thumbSize)
        {
            throw new Exception("The method or operation is not implemented.");
        }
        public override string ToString()
        {
            return "{Fireball.Drawing.Shapes.ShapePolyline}";
        }

        protected override void UpdatePath()
		{
            if (_points==null || _points.Length == 0) return;

			InternalPath = new GraphicsPath();
			InternalPath.AddLines(_points);

			Matrix mtx = new Matrix();
			mtx.RotateAt(this.Rotation, InternalRotationBasePoint);
			InternalPath.Transform(mtx);
		}

		#endregion

		public virtual PointF[] Points
		{
			get
			{
				return _points;
			}
			set
			{
				_points = value;
			}
		}

	}
}